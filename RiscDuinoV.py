from serial import Serial
from ProgramFile import ProgramFile
import time
class RiscDuinoV:
    def __init__(self, port_name = "NONE", baudrate = 115200, timeout = 5, sendToFlash = False, delay_inter_byte = 0, verbose = False):
        self.__port_name = port_name
        self.__baudrate = baudrate
        self.__serial_handler = Serial()
        self.__Timeout = timeout
        self.__SendToFlash = sendToFlash
        self.__delay_inter_byte = delay_inter_byte
        self.__verbose = verbose

    def open(self, port_name = "NONE", baudrate = 115200):
        if (port_name != "NONE"):
            self.__port_name = port_name
            self.__baudrate = baudrate
        try:
            self.__serial_handler.port = self.__port_name
            self.__serial_handler.baudrate = self.__baudrate
            self.__serial_handler.inter_byte_timeout
            self.__serial_handler.open()
        except:
            pass
    def isOpen(self):
        return self.__serial_handler.is_open

    def sendFile(self, ProgFile : ProgramFile):
        if self.isOpen() == False:
            return -1
        # if self.__ResetCpu() == False:
        #     return -1
        print("Sending...")
        if ProgFile.file_extension[1] == ".elf":
            segments = ProgFile.GetElfSegments()
            self.sendElf(segments[0])
        elif ProgFile.file_extension[1] == ".s19" or ProgFile.file_extension[1] == ".srec":
            self.sendS19(ProgFile.GetS19())
        print("Done!")

    def sendElf(self, segment):
        if self.__verbose == True:
            print("Entry point = 0x%08X, size = %d" %(segment.addr, len(segment.data)))
        init_sequence = [int(0xCAFECAFE), int(segment.addr), int(len(segment.data))]
        for word in init_sequence:
            for byte in word.to_bytes(4, "little"):
                self.__serial_handler.write(int(byte).to_bytes(1, "little"))
                time.sleep(self.__delay_inter_byte / 1000)
        i = 0
        address = segment.addr
        for byte in segment.data:
            self.__serial_handler.write(int(byte & 0xFF).to_bytes(1, "little"))
            time.sleep(self.__delay_inter_byte / 1000)
            if self.__verbose == True:
                if i % 16 == 0:
                    print("\n0x%08X : " %(address + i), end='')
                print("%02X " %(byte), end='')
                i += 1
        if self.__verbose:
            print()
    def sendS19(self, data):
        for byte in data:
            self.__serial_handler.write(int(byte).to_bytes(1, "little"))
            time.sleep(self.__delay_inter_byte / 1000)
            if self.__verbose == True:
                print(byte)

    def __ResetCpu(self):
        i = self.__Timeout
        attempt = 1
        while True:
            if self.__SendToFlash == True:
                self.__serial_handler.write(b"+++1")
            else:
                self.__serial_handler.write(b"+++2")
            rsp = self.__serial_handler.read(len(b"RISC-V\n\r"))
            if rsp != b"RISC-V\n\r":
                i -= 1
                attempt += 1
                print("No response from the CPU...")
                if i != 0:
                    print("Attempt n°%d..." %(attempt), end=' ')
                    time.sleep(1)
                else:
                    print("Aborting...")
                    return False
            else:
                return True
        
    def close(self):
        self.__serial_handler.close()