#include "Riscduinov.h"

RiscDuinoV::RiscDuinoV()
{
}

RiscDuinoV::RiscDuinoV(const char* serial_port)
{
	setPort(serial_port);
}

RiscDuinoV::~RiscDuinoV()
{
}

bool RiscDuinoV::sendFile(ProgramFile& ProgFile, bool verbose)
{
	bool ret = false;
	_Serial.open(_Port.c_str(), 115200);
	if (!_Serial.isOpen())
	{
		std::cerr << "Cannot open " << _Port << "\n";
		return false;
	}
	if (!resetCPU())
		return false;
	switch (ProgFile.getFileType())
	{
	case ProgramFile::FileType_t::ELF:
		ret = sendElf(ProgFile, verbose);
		break;
	case ProgramFile::FileType_t::S19:
		ret = sendS19(ProgFile, verbose);
		break:
	default:
		break;
	}
	_Serial.close();
	return ret;
}

void RiscDuinoV::setPort(const char* serial_port)
{
	_Port = serial_port;
}

void RiscDuinoV::setAckTimeout(int AckTimeout)
{
	_AckTimeout = AckTimeout;
}

void RiscDuinoV::setSendToFlash(bool sendToFlash)
{
	_SendToFlash = sendToFlash;
}

bool RiscDuinoV::sendElf(ProgramFile& ProgFile, bool verbose)
{
	int byte;
	if (verbose)
	{
		std::printf("Entry point = 0x%08X, Size = %d bytes\n", ProgFile.getEntryPoint(), ProgFile.getLength());
	}
	for (size_t i = 0; i < ProgFile.getLength(); i += 16)
	{
		std::printf("0x%08X : ", ProgFile.getEntryPoint() + i);
		for (int j = 0; j < 16; j++)
		{
			byte = ProgFile.getNextByte();
			if (i + j < ProgFile.getLength())
			{
				if (byte == -1)
				{
					std::cerr << "Error while sending elf...\n";
					std::cerr << "Reseting...\n";
					resetCPU();
					return false;
				}
				else
				{
					_Serial.write(byte & 0xFF);
					if (verbose)
					{
						std::printf("%02X ", byte & 0xFF);
					}
				}
			}

		}
	}
	return false;
}

bool RiscDuinoV::sendS19(ProgramFile& ProgFile, bool verbose)
{
	int byte;
	while (!ProgFile.eof())
	{
		byte = ProgFile.getNextByte();
		if (byte == -1)
			break;
		else
		{
			_Serial.write(byte & 0xFF);
			if (verbose)
			{
				std::cout << (char)(byte & 0xFF);
			}
		}
	}
	return true;
}

bool RiscDuinoV::resetCPU()
{
	char Ack[10];
	std::memset(Ack, 0, sizeof(Ack));
	bool CPUAck = false;
	for (int i = 0; i < _AckTimeout; i++)
	{
		if (_SendToFlash == true)
		{
			RiscV.write("+++1");
		}
		else
		{
			RiscV.write("+++2");
		}
		if (RiscV.read(Ack, 8))
			break;
#ifdef DEBUG
		std::cout << "Ack_Timeout = " << i << "\n";
#endif
		if (std::strcmp(Ack, "RISC-V\n\r"))
		{
			std::cerr << "Wrong or no response from the CPU...\n";
			return false;
		}
	}
	return true;
}
