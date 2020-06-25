#include <iostream>
#include <string>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#endif

#ifdef  __linux__
#include <errno.h>
#include <fcntl.h> 
#include <termios.h>
#include <unistd.h>
#endif

class Serial
{
private:
    static const int _BUFSIZE = 1024;
    char _buf[_BUFSIZE];
    std::string _Port; 
    bool _isOpen = false;
    #if _WIN32
        HANDLE _serialHandle;
        DCB _serialParams;
        COMMTIMEOUTS _timeOut;
    #endif
    #ifdef __linux__
        struct termios _tty;
        int _fd;
    #endif
public:
    Serial();
    ~Serial();
    int open(const char *port, int baudrate);
    void close();
    bool isOpen();
    int write(const char *data, int quantity);
    int write(const char *str);
    int write(const char c);
    int read(char &c);
    int read(char *data, int quantity);
    void flush();
};