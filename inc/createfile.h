#ifndef CREATE_FILE_H_
#define CREATE_FILE_H_
	extern enum FileExtension;
	extern bool getElfInfoAndConfig(std::ifstream& File, uint32_t& entry_point, uint32_t& len);
	int LatticeCreateHexFile(const char* src_file, const char* dst_file);
	int AlteraCreateMifFile(const char* src_file, const char* dst_file);
#endif /* CREATE_FILE_H_ */