#include "MemoryInitialisationFile.h"
static bool replaceExtension(std::string& targetStr, const char* targetExt, const char* desiredExtension);
MemoryInitialisationFile::MemoryInitialisationFile()
{

}

MemoryInitialisationFile::~MemoryFileInitialisation()
{

}

bool MemoryInitialisationFile::LatticeCreateMif(ProgramFile& ProgFile)
{
	if (!ProgFile.isOpen())
		return false;
	_FileName = ProgFile.getFileName();
	switch (ProgFile.getFileType)
	{
	case ProgramFile::FileType_t::ELF:
		replaceExtension(_FileName, "elf", "mem");
		break;
	case ProgramFile::FileType_t::BIN:
		replaceExtension(_FileName, "bin", "mem");
		break;
	default:
		return false;
		break;
	}
	_File.open(_FileName.c_str(), std::ios::binary);
	uint32_t data = 0;
	int tmp;
	char hex_format[9];
	for (size_t i = 0; i < ProgFile.getLength(); i += 4)
	{
		for (int j = 0; j < 4; j++)
		{
			tmp = ProgFile.getNextByte();
			if (tmp != -1)
				data = (data >> 8) | ((tmp & 0xFF) << 24);
			else
				data = (data >> 8);
		}
		std::snprintf(hex_format, 9, "%08X", data);
		_File << hex_format;
		if (tmp == -1)
			break;
	}
	_File.close();
	return true;
}

bool MemoryInitialisationFile::CreateGenericMif(ProgramFile& ProgFile)
{
	if (!ProgFile.isOpen())
		return false;
	_FileName = ProgFile.getFileName();
	switch (ProgFile.getFileType)
	{
	case ProgramFile::FileType_t::ELF:
		replaceExtension(_FileName, "elf", "mif");
		break;
	case ProgramFile::FileType_t::BIN:
		replaceExtension(_FileName, "bin", "mif");
		break;
	default:
		return false;
		break;
	}
	uint32_t data;
	_File.open(_FileName.c_str(), std::ios::binary);
	data = (data + data % 4) / 4;
	data = ProgFile.getLength() / 4 + ProgFile.getLength() % 4;
	_File << "DEPTH=" << data << ";\n";
	_File << "ADDRESS_RADIX = UNS;\n";
	_File << "DATA_RADIX = UNS;\n";
	_File << "CONTENT BEGIN\n";
	int address = 0;	
	for (size_t i = 0; i < ProgFile.getLength(); i += 4)
	{
		for (int j = 0; j < 4; j++)
		{
			tmp = ProgFile.getNextByte();
			if (tmp != -1)
				data = (data >> 8) | ((tmp & 0xFF) << 24);
			else
				data = (data >> 8);
		}
		_File << "\t" << address << "\t:\t" << data << ";\n";
		address++;
		if (tmp == -1)
			break;
	}
	_File << "END;\n";
	_File.close();
}

static bool replaceExtension(std::string& targetStr, const char *targetExt, const char* desiredExtension)
{
	size_t pos = target.rfind(targetExt);
	if (pos != std::string::npos)
		targetStr.replace(pos + 1, 3, desiredExtension);
	else
		return false;
}
