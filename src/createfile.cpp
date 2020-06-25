#include <iostream>
#include <fstream>
#include "createfile.h"
int LatticeCreateHexFile(const char* src_file, const char *dst_file)
{
	std::ifstream file;
	file.open(src_file, std::ios::binary);
	if (!file.is_open())
	{
		file.close();
		std::cerr << "Cannot open " << src_file << "\n";
		return -1;
	}
	std::ofstream mem_file;
	mem_file.open(dst_file);
	if (!mem_file.is_open())
	{
		mem_file.close();
		std::cerr << "Cannot open " << dst_file << "\n";
		return -1;
	}
	uint32_t data;
	{
		std::ifstream in(src_file, std::ifstream::ate | std::ifstream::binary);
		data = in.tellg();
	}
	data = (data + data % 4) / 4;
	char tmpvar;
	char hex_format[9];
	bool exit = true;
	mem_file << "#Format=Hex\n";
	mem_file << "#Depth=" << data << "\n";
	//mem_file << "#Depth=8192\n";
	mem_file << "#Width=32\n";
	mem_file << "#AddrRadix=2\n";
	mem_file << "#DataRadix=3\n";
	mem_file << "#Data\n";
	while (!file.eof())
	{
		data = 0;
		for (int i = 0; i < 4; i++)
		{
			file.get(tmpvar);
			if (!file.eof())
				data |= (tmpvar & 0xFF) << (8 * i);
			else
			{
				exit = false;
				break;
			}
		}
		//std::printf("0x%08X\n", data);
		if (!exit)
			break;
		std::snprintf(hex_format, 9, "%08X", data);
		mem_file << hex_format << "\n";
	}
	file.close();
	mem_file.close();
	return 0;
}
int AlteraCreateMifFile(const char* src_file, const char* dst_file)
{
	std::ifstream file;
	file.open(src_file, std::ios::binary);
	if (!file.is_open())
	{
		file.close();
		std::cerr << "Cannot open " << src_file << "\n";
		return -1;
	}
	std::ofstream mem_file;
	mem_file.open(dst_file);
	if (!mem_file.is_open())
	{
		mem_file.close();
		std::cerr << "Cannot open " << dst_file << "\n";
		return -1;
	}
	mem_file << "WIDTH=32;\n";
	uint32_t data;
	{
		std::ifstream in(src_file, std::ifstream::ate | std::ifstream::binary);
		data = in.tellg();
	}
	data = (data + data % 4) / 4;
	mem_file << "DEPTH=" << data << ";\n";
	mem_file << "ADDRESS_RADIX = UNS;\n";
	mem_file << "DATA_RADIX = UNS;\n";
	mem_file << "CONTENT BEGIN\n";
	int address = 0;
	char tmpvar;
	bool exit = true;
	while (!file.eof())
	{
		data = 0;
		for (int i = 0; i < 4; i++)
		{
			file.get(tmpvar);
			if (!file.eof())
				data |= (tmpvar & 0xFF) << (8 * i);
			else
			{
				exit = false;
				break;
			}
				
		}
		if (exit == false)
			break;
		mem_file << "\t" <<address << "\t:\t" << data <<  ";\n";
		address++;
	}
	mem_file << "END;\n";
	file.close();
	mem_file.close();
	return 0;
}