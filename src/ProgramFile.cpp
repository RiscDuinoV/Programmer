#include "ProgramFile.h"

ProgramFile::ProgramFile()
{
	Opened = false;
}

ProgramFile::ProgramFile(const char* file_name)
{
	_FileName = file_name;
	Opened = false;
}
ProgramFile::~ProgramFile()
{

}
void ProgramFile::open()
{
	if (!_FileName.empty())
	{
		open(_FileName.c_str());
	}
}
void ProgramFile::open(const char* file_name)
{
	if (_FileName.empty())
		_FileName = file_name;
	_File.open(_FileName.c_str(), std::ios::binary);
	if (isOpen())
	{
		parseFileType();
		configureFile();
	}
}
void ProgramFile::close()
{
	_File.close();
}
bool ProgramFile::isOpen() const
{
	return _File.is_open();
}
int ProgramFile::getNextByte()
{
	int byte = -1;
	if (_File.is_open())
		byte = _File.get();
	return byte;
}
bool ProgramFile::eof() const
{
	return _File.eof();
}
void ProgramFile::setIndex(int index)
{
	_File.seekg((size_t)_EntryPoint + index);
}
ProgramFile::FileType_t ProgramFile::getFileType() const
{
	return _FileType;
}
const char* ProgramFile::getFileName() const
{
	return _FileName.c_str();
}

uint32_t ProgramFile::getEntryPoint() const
{
	return _EntryPoint;
}

uint32_t ProgramFile::getLength() const
{
	return _Len;
}

void ProgramFile::parseFileType()
{
	char tmp[] = { (char)(_File.get() & 0xFF), (char)(_File.get() & 0xFF), (char)(_File.get() & 0xFF), (char)(_File.get() & 0xFF)};
	if (tmp[0] == 0x7F && tmp[1] == 0x45 && tmp[2] == 0x4C && tmp[3] == 0x46)
	{
		_FileType = FileType_t::ELF;
	}
	else if (tmp[0] == 'S' && tmp[1] == '0')
	{
		_FileType = FileType_t::S19;
	}
	else
	{
		_FileType = FileType_t::BIN;
	}
	_File.seekg(0);
}

void ProgramFile::configureFile()
{
	switch (_FileType)
	{
	case FileType_t::ELF:
		_File.seekg(0x18);
		_File.get(reinterpret_cast<char*>(&_EntryPoint), 4);
		_File.seekg(0x34 + 0x10);
		_File.get(reinterpret_cast<char*>(&_Len), 4);
		_File.seekg(_EntryPoint);
		_Len -= _EntryPoint;
		break;
	default:
		_EntryPoint = 0;
		break;
	}
}
