import os 
from elftools.elf.elffile import ELFFile

class ElfSegments:
    def __init__(self, addr, data):
        self.addr = addr
        self.data = data

class ProgramFile:
    def __init__(self, file_name = "NONE"):
        self.__file_name = file_name
        self.__file_extension = os.path.splitext(self.__file_name)
        self.__isOpen = False

    def open(self, file_name = "NONE"):
        if (file_name != "NONE"):
            self.__file_name = file_name
            self.__file_extension = os.path.splitext(self.__file_name)
        try:
            self.__file_handler = open(self.__file_name, "rb")
            self.__isOpen = True
        except:
            self.__isOpen = False

    def isOpen(self):
        return self.__isOpen
        
    def GetElfSegments(self):
        if self.isOpen() == False:
            return b""
        ret = []
        elffile = ELFFile(self.__file_handler)
        for segment in elffile.iter_segments():
            if segment['p_type'] == 'PT_LOAD':
                addr = segment['p_paddr']
                if addr == 0 and elffile["e_entry"] != 0:
                    addr = elffile["e_entry"]
                    ret.append(ElfSegments(addr, segment.data()[addr : len(segment.data())]))
                else:
                    ret.append(ElfSegments(addr, segment.data()))
        self.__file_handler.seek(0)
        return ret
    
    def GetS19(self):
        if self.isOpen() == False:
            return b""
        ret = self.__file_handler.read()
        self.__file_handler.seek(0)
        return ret

    def GetBinary(self):
        if self.isOpen() == False:
            return b""
        ret = self.__file_handler.read()
        self.__file_handler.seek(0)
        return ret

    def close(self):
        self.__file_handler.close()

    def __FileExtension(self):
        return self.__file_extension

    file_extension = property(__FileExtension)