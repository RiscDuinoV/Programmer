from ProgramFile import ProgramFile
class InitialisationFile:
    @staticmethod
    def Lattice(ProgFile : ProgramFile):
        target_file = ProgFile.file_extension[0] + ".mem"
        target_file_handler = open(target_file, "w")
        i = 0
        word = 0
        for segment in ProgFile.GetElfSegments():
            for byte in segment.data:
                word = (word >> 8) | ((byte & 0xFF) << 24)
                i += 1
                if i % 4 == 0:
                    target_file_handler.write("%08X\n" %(word))

    @staticmethod
    def Altera(ProgFile : ProgramFile):
        target_file = ProgFile.file_extension[0] + ".mif"
        target_file_handler = open(target_file, "w")
        segments = ProgFile.GetElfSegments()
        target_file_handler.write("WIDTH=32;\n")
        target_file_handler.write("DEPTH=%d;\n" %(len(segments[0].data) / 4))
        target_file_handler.write("ADDRESS_RADIX = UNS\n")
        target_file_handler.write("DATA_RADIX = UNS;\n")
        target_file_handler.write("CONTENT BEGIN\n")
        word = 0
        i = 0
        for segment in segments:
            address_offset = 0
            for byte in segment.data:
                word = (word >> 8) | ((byte & 0xFF) << 24)
                i += 1
                if i % 4 == 0:
                    target_file_handler.write("%d\t:\t%d;\n" %(address_offset, word))
                    address_offset += 1