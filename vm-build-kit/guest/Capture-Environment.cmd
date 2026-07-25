@echo off
setlocal

call "%~dp0Set-Preservation-Environment.cmd"
if errorlevel 1 exit /b %errorlevel%

set EVIDENCE=%~dp0evidence
if not exist "%EVIDENCE%" mkdir "%EVIDENCE%"

echo %DATE% %TIME%> "%EVIDENCE%\capture-time.txt"
ver> "%EVIDENCE%\windows-version.txt"
set> "%EVIDENCE%\environment.txt"
echo %VC6_ROOT%\VC98\Bin\CL.EXE> "%EVIDENCE%\tool-paths.txt"
echo %VC6_ROOT%\VC98\Bin\LINK.EXE>> "%EVIDENCE%\tool-paths.txt"
echo %VC6_ROOT%\VC98\Bin\LIB.EXE>> "%EVIDENCE%\tool-paths.txt"
echo %PSDK_ROOT%\Bin\RC.EXE>> "%EVIDENCE%\tool-paths.txt"
echo %VC6_ROOT%\VC98\Bin\NMAKE.EXE>> "%EVIDENCE%\tool-paths.txt"
echo %VC6_ROOT%\Common\MSDev98\Bin\MSDEV.EXE>> "%EVIDENCE%\tool-paths.txt"

cl.exe 2> "%EVIDENCE%\cl-version.txt"
link.exe 2> "%EVIDENCE%\link-version.txt"
lib.exe 2> "%EVIDENCE%\lib-version.txt"
rc.exe /? > "%EVIDENCE%\rc-version.txt" 2>&1
nmake.exe /? > "%EVIDENCE%\nmake-version.txt" 2>&1

echo Environment evidence written to:
echo %EVIDENCE%
echo Return this directory to the host for hashing before any build.
exit /b 0
