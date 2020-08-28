#include <iostream>
#include <fstream>
#include "createfile.h"

//int LatticeCreateHexFile(const char* src_file, const char *dst_file)
static void replaceFileExtension(const char* file_name, const char* extension, std::string &dst_file_name)
{
	std::string src_file_name = file_name;
	size_t pos = src_file_name.rfind(".");
	dst_file_name = file_name;
	if (pos != std::string::npos)
	{
		dst_file_name.replace(pos, dst_file_name.length - pos, extension);
	}
	else
	{
		dst_file_name.append(extension);
	}
}
int LatticeCreateHexFile(const char* src_file_name)
{
	std::string dst_file_name;
	replaceFileExtension(src_file_name, ".mem", dst_file_name);
	std::ifstream src_file;
	std::ofstream dst_file;
	if (!src_file.is_open())
	{
		src_file.close();
		std::cerr << "Cannot open " << src_file_name << "\n";
		return -1;
	}
	if (getElfInfoAndConfig(src_file, ))
	dst_file.open(dst_file_name.c_str());
	if (!dst_file.is_open())
	{
		dst_file.close();
		std::cerr << "Cannot open " << dst_file_name.c_str() << "\n";
		return -1;
	}
	uint32_t data;
	{
		std::ifstream in(src_file_name, std::ifstream::ate | std::ifstream::binary);
		data = in.tellg();
	}
	data = (data + data % 4) / 4;
	char tmpvar;
	char hex_format[9];
	bool exit = true;
	dst_file << "#Format=Hex\n";
	dst_file << "#Depth=" << data << "\n";
	//mem_file << "#Depth=8192\n";
	dst_file << "#Width=32\n";
	dst_file << "#AddrRadix=2\n";
	dst_file << "#DataRadix=3\n";
	dst_file << "#Data\n";
	while (!src_file.eof())
	{
		data = 0;
		for (int i = 0; i < 4; i++)
		{
			src_file.get(tmpvar);
			if (!src_file.eof())
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
		dst_file << hex_format << "\n";
	}
	src_file.close();
	dst_file.close();
	return 0;
}
int AlteraCreateMifFile(const char* src_file_name)
{
	std::string dst_file_name;
	std::ifstream src_file;
	std::ofstream dst_file;
	replaceFileExtension(src_file_name, ".mif", dst_file_name);
	src_file.open(src_file_name, std::ios::binary);
	if (!src_file.is_open())
	{
		src_file.close();
		std::cerr << "Cannot open " << src_file_name << "\n";
		return -1;
	}
	
	dst_file.open(dst_file_name.c_str());
	if (!dst_file.is_open())
	{
		dst_file.close();
		std::cerr << "Cannot open " << dst_file_name.c_str() << "\n";
		return -1;
	}
	dst_file << "WIDTH=32;\n";
	uint32_t data;
	{
		std::ifstream in(src_file_name, std::ifstream::ate | std::ifstream::binary);
		data = in.tellg();
	}
	data = (data + data % 4) / 4;
	dst_file << "DEPTH=" << data << ";\n";
	dst_file << "ADDRESS_RADIX = UNS;\n";
	dst_file << "DATA_RADIX = UNS;\n";
	dst_file << "CONTENT BEGIN\n";
	int address = 0;
	char tmpvar;
	bool exit = true;
	while (!src_file.eof())
	{
		data = 0;
		for (int i = 0; i < 4; i++)
		{
			src_file.get(tmpvar);
			if (!src_file.eof())
				data |= (tmpvar & 0xFF) << (8 * i);
			else
			{
				exit = false;
				break;
			}
				
		}
		if (exit == false)
			break;
		dst_file << "\t" <<address << "\t:\t" << data <<  ";\n";
		address++;
	}
	dst_file << "END;\n";
	src_file.close();
	dst_file.close();
	return 0;
}