@echo off

if not exist "%~dp0local-paths.cmd" (
  echo ERROR: Copy local-paths.example.cmd to local-paths.cmd and edit it.
  exit /b 2
)

call "%~dp0local-paths.cmd"

if not exist "%VC6_ROOT%\VC98\Bin\CL.EXE" (
  echo ERROR: VC6 compiler not found under VC6_ROOT.
  exit /b 3
)

if not exist "%VC6_ROOT%\VC98\Bin\LINK.EXE" (
  echo ERROR: VC6 linker not found under VC6_ROOT.
  exit /b 3
)

if not exist "%PSDK_ROOT%\Include\Windows.h" (
  echo ERROR: February 2003 Platform SDK headers not found.
  exit /b 4
)

if not exist "%DXSDK_ROOT%\Include\d3dx9.h" (
  echo ERROR: October 2004 DirectX headers not found.
  exit /b 5
)

if not exist "%DXSDK_ROOT%\Lib\d3dx9dt.lib" (
  echo ERROR: October 2004 VC6 Extras d3dx9dt.lib not found.
  exit /b 5
)

if not exist "%EOD_SOURCE%\EmperorOfDragons.dsp" (
  echo ERROR: Source root is invalid.
  exit /b 6
)

set PATH=%VC6_ROOT%\Common\MSDev98\Bin;%VC6_ROOT%\VC98\Bin;%PSDK_ROOT%\Bin;%PATH%
set INCLUDE=%DXSDK_ROOT%\Include;%PSDK_ROOT%\Include;%VC6_ROOT%\VC98\Include
set LIB=%DXSDK_ROOT%\Lib;%PSDK_ROOT%\Lib;%VC6_ROOT%\VC98\Lib

exit /b 0
