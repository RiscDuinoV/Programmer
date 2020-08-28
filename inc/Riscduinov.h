#ifndef RISCDUINOV_H_
#include <iostream>
#include <string>
#include "ProgramFile.h"
#include "serial.h"
class RiscDuinoV
{
public:
	RiscDuinoV();
	RiscDuinoV(const char *serial_port);
	~RiscDuinoV();
	bool sendFile(ProgramFile &ProgFile, bool verbose = false);
	void setPort(const char* serial_port);
	void setAckTimeout(int AckTimeout);
	void setSendToFlash(bool sendToFlash);
private:
	bool sendElf(ProgramFile& ProgFile, bool verbose = false);
	bool sendS19(ProgramFile& ProgFile, bool verbose = false);
	bool resetCPU();
	Serial _Serial;
	std::string _Port;
	bool _SendToFlash = false;
	int _AckTimeout = 5;
};
#endif // !RISCDUINOV_H_
