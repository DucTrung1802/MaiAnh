@ECHO OFF
SET TARGET_CHIP=HT32F67051
SET PACK_FILE=..\..\..\..\Holtek.HT32_DFP.latest.pack

start Project_67051.emProject

REM pyocd erase --chip -t %TARGET_CHIP% --pack=%PACK_FILE%
pyocd gdbserver -t %TARGET_CHIP% --pack=%PACK_FILE%