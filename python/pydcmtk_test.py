import os
import subprocess as sp
program_name = "C:\dev\dcmtk\build\install\bin\dcmdump.exe"
filename = "c:\temp\testct.dcm"

if filename:
    # os.execv("C:\dev\dcmtk\build\install\bin\dcmdump.exe", filename)
    sp.Popen([program_name, filename])

print(filename)
