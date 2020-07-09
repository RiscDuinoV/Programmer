#include <iostream>
#include <string>
#include <serial.h>
#include <vector>
#include <fstream>
#include <cstdlib>
#include "createfile.h"


#ifdef _WIN32
#include <windows.h>
#define delay(ms)	Sleep(ms)
#endif

#ifdef  __linux__
#include <unistd.h>
#define delay(ms)	usleep(ms * 1000)
#endif


enum Config_States { ST_IDLE, ST_BAUDRATE, ST_SERIAL_PORT, ST_FILE, ST_DELAY };
enum OutputFileFormat { NONE = 0, LATTICE_HEX, ALTERA_MIF };
typedef struct
{
    uint32_t baudrate = 0;
    
	std::string port = "";
    
	std::string fileName;
   
	bool SendToFlash = true;
    
	int Ack_Timeout = 5; // Number of times that we'll try to communicate with the RISC-V
	
	enum OutputFileFormat OutputFileFormat = NONE;
	
	bool Verbose = false;

	uint32_t delay_ms = 0;
}ConfigStruct;



void PrintMenu(void)
{
    std::cout << "RiscDuinoV Programmer v0.4\n";
    std::cout << "Valid options :\n";
    std::cout << " -b BAUDRATE\tSet baudrate (default : 115200)\n";
    std::cout << " -p PORT\tSelect Serial Port\n";
    std::cout << " -a FILE\tSelect the file to send to RISC-V\n";
    std::cout << " -r\t\tCode will be written only in RAM\n";
	std::cout << " -LHex\tConvert input file into a Hex Lattice Memory Initialisation file\n";
	std::cout << " -AMif\tConvert input file into a Altera or Generic Memory Initialisation File\n";
	std::cout << " -v\t\tVerbose\n";
	std::cout << " -D DELAY\tDelay transmission of each byte by DELAY ms\n";
}
ConfigStruct GetConfig(std::vector<std::string> &InputArgs)
{
    ConfigStruct Config = {115200, "", "", true, 5};
    enum Config_States Config_ST = ST_IDLE;
    for (int i = 0; i < InputArgs.size(); i++)
    {
        switch (Config_ST)
        {
            case ST_IDLE:
                if (!InputArgs[i].compare("-b"))
                {
                    Config_ST = ST_BAUDRATE;
                }
                else if (!InputArgs[i].compare("-p"))
                {
                    Config_ST = ST_SERIAL_PORT;
                }
                else if (!InputArgs[i].compare("-a"))
                {
                    Config_ST = ST_FILE;
                }
                else if (!InputArgs[i].compare("-r"))
                {
                    Config.SendToFlash = false;
                }
				else if (!InputArgs[i].compare("-LHex"))
				{
					Config.OutputFileFormat = LATTICE_HEX;
				}
				else if (!InputArgs[i].compare("-AMif"))
				{
					Config.OutputFileFormat = ALTERA_MIF;
				}
				else if (!InputArgs[i].compare("-v"))
				{
					Config.Verbose = true;
				}
				else if (!InputArgs[i].compare("-D"))
				{
					Config_ST = ST_DELAY;
				}
                break;
            case ST_BAUDRATE:
                Config.baudrate = std::stoul(InputArgs[i]);
                Config_ST = ST_IDLE;
                break;
            case ST_SERIAL_PORT:
                Config.port = InputArgs[i];
                Config_ST = ST_IDLE;
                break;
            case ST_FILE:
                Config.fileName = InputArgs[i];
                Config_ST = ST_IDLE;
                break;
			case ST_DELAY:
				Config.delay_ms = std::stoul(InputArgs[i]);
				Config_ST = ST_IDLE;
				break;
        default:
            break;
        }
    }
    return Config;
}
void SendFile(std::ifstream &File, Serial &RiscV, const ConfigStruct &Config)
{
    char byte;
    char Ack[10];
    std::memset(Ack, 0, sizeof(Ack));
    RiscV.flush();
	bool CPUAck = false;
	for (int i = 0; i < Config.Ack_Timeout; i++)
	{
		if (Config.SendToFlash == true)
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
			return;
		}
	}
    while (!File.eof())
    {
        byte = File.get();
        RiscV.write(byte);
		delay(Config.delay_ms);
		if (Config.Verbose == true)
		{
			std::cout << byte;
		}
    }
}
int ConvOutputFormat(const ConfigStruct& Config)
{
	size_t pos = Config.fileName.rfind(".bin");
	if (pos != std::string::npos)
	{
		std::string outputFileName = Config.fileName;
		
		switch (Config.OutputFileFormat)
		{
			case LATTICE_HEX:
				outputFileName.replace(pos, strlen(".bin"), ".mem");
				return LatticeCreateHexFile(Config.fileName.c_str(), outputFileName.c_str());
			case ALTERA_MIF:
				outputFileName.replace(pos, strlen(".bin"), ".mif");
				return AlteraCreateMifFile(Config.fileName.c_str(), outputFileName.c_str());
			default:
			break;
		}
		return 0;
	}
	else
	{
		std::cerr << "Cannot convert a non binary file!\n";
		return -1;
	}
}
int main(int argc, const char *argv[])
{
    std::vector<std::string> InputArgs;
    ConfigStruct Config;
    std::ifstream File;
    Serial RiscV;
    // int TIME_EACH_BYTE = 5; // Number of times that we'll try to communicate with the RISC-V
    for (int i = 0; i < argc; i++)
    {
        InputArgs.push_back(argv[i]);
    }
    Config = GetConfig(InputArgs);
    if (argc <= 1)
    {
        PrintMenu();
        return 0;
    }
	if (Config.OutputFileFormat != NONE && Config.fileName.empty() == false)
	{
		ConvOutputFormat(Config);
	}
	if (Config.fileName.empty() == false && Config.port.empty() == false)
	{
		File.open(Config.fileName);

		if (!File.is_open())
		{
			std::cerr << "Cannot open " << Config.fileName << "\n";
			return -1;
		}
		RiscV.open(Config.port.c_str(), Config.baudrate);
		if (!RiscV.isOpen())
		{
			std::cerr << "Cannot open " << Config.port << "\n";
			return -2;
		}
		SendFile(File, RiscV, Config);


		RiscV.close();
		File.close();
	}

    return 0;
}
