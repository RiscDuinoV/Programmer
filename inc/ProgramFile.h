#ifndef PROGRAMFILE_H_
#define PROGRAMFILE_H_
#include <fstream>
class ProgramFile
{
	
public:
	enum class FileType_t { UNDEFINED = 0, ELF, BIN, S19, iHEX };
	ProgramFile();
	ProgramFile(const char *file_name);
	virtual ~ProgramFile();
	void open();
	void open(const char* file_name);
	void close();
	bool isOpen() const;
	int getNextByte();
	bool eof() const;
	void setIndex(int index);
	enum FileType_t getFileType() const;
	const char* getFileName() const;
	uint32_t getEntryPoint() const;
	uint32_t getLength() const;
private:
	
	enum class FileType_t _FileType = FileType_t::UNDEFINED;
	std::string _FileName;
	std::ifstream _File;
	bool Opened;
	void parseFileType();
	void configureFile();
	uint32_t _EntryPoint = 0;
	uint32_t _Len = 0;
};
#endif // !PROGRAMFILE_H_
