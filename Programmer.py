#!/usr/bin/python
__version__ = 0.1
import sys
from ProgramFile import ProgramFile
from InitialisationFile import InitialisationFile
from RiscDuinoV import RiscDuinoV
from enum import Enum
class ConfigStates(Enum):
    ST_IDLE = 0
    ST_BAUDRATE = 1
    ST_SERIAL_PORT = 2
    ST_FILE = 3
    ST_DELAY = 4

class OutputFileFormat(Enum):
    NONE = 0
    LATTICE_HEX = 1
    ALTERA_MIF = 2

class ConfigStruct:
    baudrate = 115200
    port = ""
    fileName = ""
    SendToFlash = True
    Ack_Timeout = 5 # Number of times that we'll try to communicate with the RISC-V
    OutputFileFormat = OutputFileFormat.NONE
    Verbose = False
    delay_ms = 0
        

def PrintMenu():
    print ("RiscDuinoV Programmer v%.1f" %(__version__))
    print ("Valid options :")
    print (" -b BAUDRATE\tSet baudrate (default : 115200)")
    print (" -p PORT\tSelect Serial Port")
    print (" -a FILE\tSelect the file to send to RISC-V")
    print (" -r\t\tCode will be written only in RAM")
    print (" -LHex\tConvert input file into a Hex Lattice Memory Initialisation file")
    print (" -AMif\tConvert input file into a Altera or Generic Memory Initialisation File")
    print (" -v\t\tVerbose")
    print (" -D DELAY\tDelay transmission of each byte by DELAY ms")

def GetConfig(args):
    Config_ST = ConfigStates.ST_IDLE
    Config = ConfigStruct()
    for arg in args:
        if (Config_ST == ConfigStates.ST_IDLE):
            if (arg == "-b"):
                Config_ST = ConfigStates.ST_BAUDRATE
            elif (arg == "-p"):
                Config_ST = ConfigStates.ST_SERIAL_PORT
            elif (arg == "-a"):
                Config_ST = ConfigStates.ST_FILE
            elif (arg == "-r"):
                Config.SendToFlash = False
            elif (arg == "-LHex"):
                Config.OutputFileFormat = OutputFileFormat.LATTICE_HEX
            elif (arg == "-AMif"):
                Config.OutputFileFormat = OutputFileFormat.ALTERA_MIF
            elif (arg == "-v"):
                Config.Verbose = True
            elif (arg == "-D"):
                Config_ST = ConfigStates.ST_DELAY
        elif (Config_ST == ConfigStates.ST_BAUDRATE):
            Config.baudrate = int(arg)
            Config_ST = ConfigStates.ST_IDLE
        elif (Config_ST == ConfigStates.ST_SERIAL_PORT):
            Config.port = arg
            Config_ST = ConfigStates.ST_IDLE
        elif (Config_ST == ConfigStates.ST_FILE):
            Config.fileName = arg
            Config_ST = ConfigStates.ST_IDLE
        elif (Config_ST == ConfigStates.ST_DELAY):
            Config.delay_ms = int(arg)
            Config_ST = ConfigStates.ST_IDLE
    return Config
def main(args):
    if (len(args) <= 1):
        PrintMenu()
        return 0
    Config = GetConfig(args)
    ProgFile = ProgramFile(Config.fileName)
    ProgFile.open()
    if ProgFile.isOpen() == False:
        print("Cannot open %s" %(Config.fileName))
        return -1
    if Config.OutputFileFormat != OutputFileFormat.NONE:
        if Config.OutputFileFormat == OutputFileFormat.LATTICE_HEX:
            InitialisationFile.Lattice(ProgFile)
        elif Config.OutputFileFormat == OutputFileFormat.ALTERA_MIF:
            InitialisationFile.Altera(ProgFile)
    if len(Config.port) == 0:
        return 0
    riscduinov = RiscDuinoV(Config.port, Config.baudrate, Config.Ack_Timeout, Config.SendToFlash, Config.delay_ms, Config.Verbose)
    riscduinov.open()
    if riscduinov.isOpen() == False:
        print("Cannot open %s" %(Config.port))
        return -1
    riscduinov.sendFile(ProgFile)
    riscduinov.close()
    ProgFile.close()
if __name__ == "__main__":
    argv = ["-p", "COM5", "-a", "D:/Programmation/RISCV/RiscV_Projects/Project_template/bin/Sortie.elf", "-v", "-r"]
    main(argv)