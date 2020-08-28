#ifndef MEMORYINITIALISATIONFILE
#include <fstream>
#include "ProgramFile.h"
class MemoryInitialisationFile
{
public:
	MemoryInitialisationFile();
	virtual ~MemoryInitialisationFile();
	bool LatticeCreateMif(ProgramFile &ProgFile);
	bool CreateGenericMif(ProgramFile& ProgFile);
private:
	std::ofstream _File;
	std::string _FileName;
};
#endif // !MEMORYINITIALISATIONFILE
