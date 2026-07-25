@echo off

call "%~dp0Set-Preservation-Environment.cmd"
if errorlevel 1 exit /b %errorlevel%

echo Checking recovered source...
if not exist "%EOD_SOURCE%\XKernel\XSecurity\XCrypto.Cpp" (
  echo ERROR: XCrypto.Cpp is missing.
  exit /b 10
)

echo Checking original VC6 projects...
if not exist "%EOD_SOURCE%\XGamebase\XGamebase.dsp" exit /b 11
if not exist "%EOD_SOURCE%\XKernel\XKernel.dsp" exit /b 11
if not exist "%EOD_SOURCE%\XFControl\XFControl.dsp" exit /b 11
if not exist "%EOD_SOURCE%\EmperorOfDragons.dsp" exit /b 11

echo Checking D3DX generation...
findstr /C:"#define D3DX_SDK_VERSION 22" "%DXSDK_ROOT%\Include\d3dx9core.h" >nul
if errorlevel 1 (
  echo ERROR: DirectX include root is not the required D3DX v22 family.
  exit /b 12
)

echo Checking forbidden D3DX v43 resolution...
findstr /C:"#define D3DX_SDK_VERSION 43" "%DXSDK_ROOT%\Include\d3dx9core.h" >nul
if not errorlevel 1 (
  echo ERROR: D3DX v43 is present in the preservation include root.
  exit /b 13
)

findstr /C:"#define D3DX_SDK_VERSION 43" "%EOD_SOURCE%\XKernel\d3dx9core.h" >nul
if not errorlevel 1 (
  echo ERROR: Source-local D3DX v43 headers still override the preservation SDK.
  echo Apply and review the complete v22 header-family placement before build.
  exit /b 13
)

findstr /C:"#define D3DX_SDK_VERSION 22" "%EOD_SOURCE%\XKernel\d3dx9core.h" >nul
if errorlevel 1 (
  echo ERROR: Source-local D3DX headers are not the reviewed v22 family.
  exit /b 13
)

echo Checking tool discovery...
if not exist "%VC6_ROOT%\VC98\Bin\CL.EXE" exit /b 14
if not exist "%VC6_ROOT%\VC98\Bin\LINK.EXE" exit /b 14
if not exist "%VC6_ROOT%\VC98\Bin\LIB.EXE" exit /b 14
if not exist "%VC6_ROOT%\VC98\Bin\NMAKE.EXE" exit /b 14
if not exist "%VC6_ROOT%\Common\MSDev98\Bin\MSDEV.EXE" exit /b 14

echo.
echo PREFLIGHT PASSED.
echo No compiler, linker, installer, project, or executable was run.
exit /b 0
