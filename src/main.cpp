#include <iostream>
#include <string>
#include <serial.h>
#include <vector>
#include <fstream>
#include <cstdlib>
//#include "createfile.h"
#include "Riscduinov.h"
#include "ProgramFile.h"
#include "MemoryInitialisationFile.h"

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
enum FileExtension { ELF = 0, iHEX, S19, BIN };
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
    enum Config_States Config_ST = Config_States::ST_IDLE;
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
bool getElfInfoAndConfig(std::ifstream &File, uint32_t &entry_point, uint32_t &len)
{
	if (File.get() != 0x7F || File.get() != 0x45 || File.get() != 0x4C || File.get() != 0x46)
		return false;
	File.seekg(0x18);
	File.get(reinterpret_cast<char*>(&entry_point), 4);
	File.seekg(0x34 + 0x10);
	File.get(reinterpret_cast<char *>(&len), 4);
	File.seekg(entry_point);
	len -= entry_point;
	return true;
}

int main1(void)
{
	ProgramFile file;
	return 0;
}
//int main2(void)
//{
//	std::ifstream File;
//	File.open("C:/Users/Pedro/Documents/Programmation/RISCV/RiscV_Projects/Project_template/bin/Sortie.elf", std::ifstream::binary);
//	uint32_t entry_point;
//	uint32_t len;
//	uint8_t byte;
//	if (!File.is_open())
//	{
//		std::cerr << "Cannot open File \n";
//		File.close();
//		return -1;
//	}
//	if (!getElfInfo(File, entry_point, len))
//	{
//		std::cerr << "Error in .elf\n";
//		File.close();
//		return -1;
//	}
//	File.seekg(entry_point);
//	for (size_t i = 0; i < len; i += 16)
//	{
//		std::printf("0x%08X : ", entry_point + i);
//		for (int j = 0; j < 16; j++)
//		{
//			byte = File.get();
//			if (File.eof() || i + j >= len)
//				break;
//			std::printf("%02X ", byte);
//		}
//		std::printf("\n");
//		if (File.eof())
//			break;
//	}
//	File.close();
//	return 0;
//}
int main(int argc, const char *argv[])
{
    std::vector<std::string> InputArgs;
    ConfigStruct Config;
	RiscDuinoV Riscduinov;
	ProgramFile File;
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
	if (Config.fileName.empty() == false)
	{
		File.open(Config.fileName.c_str());
		if (!File.isOpen())
		{
			std::cerr << "Cannot open " << Config.fileName << "\n";
			return -1;
		}
	}
	if (Config.OutputFileFormat != NONE)
	{
		MemoryInitialisationFile MifObj;
		bool ret;
		switch (Config.OutputFileFormat)
		{
		case OutputFileFormat::LATTICE_HEX:
			ret = MifObj.LatticeCreateMif(File);
			break;
		case OutputFileFormat::ALTERA_MIF:
			ret = MifObj.CreateGenericMif(File);
			break;
		default:
			ret = false;
			break;
		}
	}
	if (Config.port.empty() == false)
	{
		Riscduinov.setPort(Config.port.c_str());
		Riscduinov.setSendToFlash(Config.SendToFlash);
		Riscduinov.setAckTimeout(Config.Ack_Timeout);
		if (!Riscduinov.sendFile(File, Config.Verbose))
			return -1;
	}
	File.close();
    return 0;
}
