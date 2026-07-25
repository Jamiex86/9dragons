@echo off
setlocal

if not "%EOD_BUILD_AUTHORIZED%"=="YES" (
  echo ERROR: Build is disabled.
  echo Set EOD_BUILD_AUTHORIZED=YES only after the reviewed VC6 project diff,
  echo dependency-promotion gates, and environment evidence are approved.
  exit /b 20
)

call "%~dp0Preflight.cmd"
if errorlevel 1 exit /b %errorlevel%

set LOGROOT=%~dp0build-logs
if not exist "%LOGROOT%" mkdir "%LOGROOT%"

echo Building XGamebase...
msdev.exe "%EOD_SOURCE%\XGamebase\XGamebase.dsp" /MAKE "XGamebase - Win32 US_Release" /REBUILD > "%LOGROOT%\01-XGamebase.log" 2>&1
if errorlevel 1 exit /b 31

echo Building XKernel...
msdev.exe "%EOD_SOURCE%\XKernel\XKernel.dsp" /MAKE "XKernel - Win32 US_Release" /REBUILD > "%LOGROOT%\02-XKernel.log" 2>&1
if errorlevel 1 exit /b 32

echo Building XFControl...
msdev.exe "%EOD_SOURCE%\XFControl\XFControl.dsp" /MAKE "XFControl - Win32 US_Release" /REBUILD > "%LOGROOT%\03-XFControl.log" 2>&1
if errorlevel 1 exit /b 33

echo Building NineDragons...
msdev.exe "%EOD_SOURCE%\EmperorOfDragons.dsp" /MAKE "EmperorOfDragons - Win32 US_Release" /REBUILD > "%LOGROOT%\04-NineDragons.log" 2>&1
if errorlevel 1 exit /b 34

echo Build commands completed.
echo Do not execute the output. Return logs and artifacts to host quarantine.
exit /b 0
