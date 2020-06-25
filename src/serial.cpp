#include "serial.h"

Serial::Serial()
{
	memset(_buf, 0, _BUFSIZE);
	#ifdef WIN32
	memset(&_serialHandle, 0, sizeof(_serialHandle));
	memset(&_serialParams, 0, sizeof(_serialParams));
	memset(&_timeOut, 0, sizeof(_timeOut));
	#endif
    _Port.clear();
}
Serial::~Serial()
{
	if (_isOpen)
		close();
}
// "\\\\.\\COM1"
int Serial::open(const char *port, int baudrate)
{
    if (_isOpen)
        close();
    #ifdef _WIN32
        _Port = "\\\\.\\";
        _Port.append(port);
        _serialHandle = CreateFile(_Port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (_serialHandle == INVALID_HANDLE_VALUE)
        {
            return -1;
        }
        std::memset(&_serialParams, 0, sizeof(_serialParams));
        _serialParams.BaudRate = baudrate;
        _serialParams.ByteSize = 8;
        _serialParams.StopBits = ONESTOPBIT; 
        _serialParams.Parity = NOPARITY;
        if (!SetCommState(_serialHandle, &_serialParams))
        {
            return -1;
        }

        std::memset(&_timeOut, 0, sizeof(_timeOut));
        _timeOut.ReadIntervalTimeout = 50;
        _timeOut.ReadTotalTimeoutConstant = 50;
        _timeOut.ReadTotalTimeoutMultiplier = 50;
        _timeOut.WriteTotalTimeoutConstant = 50;
        _timeOut.WriteTotalTimeoutMultiplier = 10;
        if (!SetCommTimeouts(_serialHandle, &_timeOut))
        {
            return -1;
        }
        PurgeComm(_serialHandle, PURGE_RXCLEAR | PURGE_TXCLEAR);
    #endif
    #ifdef __linux__
        _Port = port;
        _fd = ::open(_Port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (_fd < 0)
        {
            return - 1;
        }
        if (tcgetattr (_fd, &_tty) != 0)
        {
            close();
            return -1;
        }
        switch (baudrate)
        {
            case 4800:
                baudrate = B4800;
                break;
            case 9600:
                baudrate = B9600;
                break;
            case 19200:
                baudrate = B19200;
                break;
            case 38400:
                baudrate = B38400;
                break;
            case 57600:
                baudrate = B57600;
                break;
            case 115200:
                baudrate = B115200;
                break;
            case 230400:
                baudrate = B230400;
                break;
            case 460800:
                baudrate = B460800;
                break;
            default:
                baudrate = B115200;
        }
        cfsetospeed (&_tty, baudrate);
        cfsetispeed (&_tty, baudrate);
        cfmakeraw(&_tty);
        _tty.c_cc[VMIN] = 0;
        _tty.c_cc[VTIME] = 0; // Does not work!
        if (tcsetattr(_fd, TCSANOW, &_tty) < 0)
        {
            close();
            return -1;
        }
        tcflush(_fd, TCIOFLUSH);
    #endif
    _isOpen = true;
    return 0;
}
void Serial::close()
{
    #ifdef _WIN32
        CloseHandle(_serialHandle);
    #endif    
    _isOpen = false;
}
bool Serial::isOpen()
{
    return _isOpen;
}
int Serial::write(const char *data, int quantity)
{
    unsigned long bytesWritten;
    #ifdef _WIN32
        if (!WriteFile(_serialHandle, data, quantity, &bytesWritten, NULL))
        {
            return -1;
        }
        else
        {
            return static_cast<int>(bytesWritten);
        }
    #endif
    #ifdef __linux__
        bytesWritten = ::write(_fd, data, quantity);
        return bytesWritten;
    #endif
}
int Serial::write(const char *str)
{
    return write(str, static_cast<int>(std::strlen(str)));
}
int Serial::write(const char c)
{
    return write(&c, 1);
}
int Serial::read(char &c)
{
    return read(&c, 1);
}
int Serial::read(char *data, int quantity)
{
    unsigned long bytesRead;
    #ifdef _WIN32
        if (!ReadFile(_serialHandle, data, quantity, &bytesRead, NULL))
        {
            return -1;
        }
        else
        {
            return static_cast<int>(bytesRead);
        }
    #endif
    #ifdef __linux__
        usleep(500000); // Little sleep to be sure to receive data from Serial port
        bytesRead = ::read(_fd, data, quantity);
        return bytesRead;
    #endif
}
void Serial::flush()
{
    #ifdef __linux__
        tcflush(_fd, TCIFLUSH);   
    #endif
}