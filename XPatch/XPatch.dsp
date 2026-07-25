# Microsoft Developer Studio Project File - Name="XPatch" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=XPatch - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XPatch.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XPatch.mak" CFG="XPatch - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XPatch - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "XPatch - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/EmperorOfDragons/XPatch", DYEBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XPatch - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 imagehlp.lib Zip/Zip.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBC" /out:"..\Game\XPatch.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "XPatch - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 imagehlp.lib Zip/ZipD.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBC" /out:"..\Game\XPatch.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "XPatch - Win32 Release"
# Name "XPatch - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SeverMessageDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\XPatch.cpp
# End Source File
# Begin Source File

SOURCE=.\XPatch.rc
# End Source File
# Begin Source File

SOURCE=.\XPatchDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SeverMessageDialog.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\XPatch.h
# End Source File
# Begin Source File

SOURCE=.\XPatchDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Border.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Close.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\M0Q1ILL5.BMP
# End Source File
# Begin Source File

SOURCE=.\res\Minimize.bmp
# End Source File
# Begin Source File

SOURCE=.\res\minimize.ico
# End Source File
# Begin Source File

SOURCE=.\res\Open.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Upgrade.bmp
# End Source File
# Begin Source File

SOURCE=.\res\XPactchMainIcon.ico
# End Source File
# Begin Source File

SOURCE=.\res\XPatch.ico
# End Source File
# Begin Source File

SOURCE=.\res\XPatch.rc2
# End Source File
# End Group
# Begin Group "Registry"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\RegStringDefine.h
# End Source File
# Begin Source File

SOURCE=.\XRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\XRegistry.h
# End Source File
# End Group
# Begin Group "Package"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Package.cpp
# End Source File
# Begin Source File

SOURCE=.\Package.h
# End Source File
# End Group
# Begin Group "MFC Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BitmapDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapDialog.h
# End Source File
# Begin Source File

SOURCE=.\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\Dib.cpp
# End Source File
# Begin Source File

SOURCE=.\DIB.h
# End Source File
# Begin Source File

SOURCE=.\Label.cpp
# End Source File
# Begin Source File

SOURCE=.\Label.h
# End Source File
# Begin Source File

SOURCE=.\ShadeButtonST.cpp
# End Source File
# Begin Source File

SOURCE=.\ShadeButtonST.h
# End Source File
# Begin Source File

SOURCE=.\xDib.cpp
# End Source File
# Begin Source File

SOURCE=.\xDib.h
# End Source File
# End Group
# Begin Group "Ziplib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\WWIIStarter_FTP\Zip\AutoBuffer.h
# End Source File
# Begin Source File

SOURCE=..\WWIIStarter_FTP\Zip\BigFile.h
# End Source File
# Begin Source File

SOURCE=..\WWIIStarter_FTP\Zip\CentralDir.h
# End Source File
# Begin Source File

SOURCE=..\WWIIStarter_FTP\Zip\FileHeader.h
# End Source File
# Begin Source File

SOURCE=..\WWIIStarter_FTP\Zip\zconf.h
# End Source File
# Begin Source File

SOURCE=..\WWIIStarter_FTP\Zip\ZipArchive.h
# End Source File
# Begin Source File

SOURCE=..\WWIIStarter_FTP\Zip\ZipException.h
# End Source File
# Begin Source File

SOURCE=..\WWIIStarter_FTP\Zip\ZipInternalInfo.h
# End Source File
# Begin Source File

SOURCE=..\WWIIStarter_FTP\Zip\ZipStorage.h
# End Source File
# Begin Source File

SOURCE=..\WWIIStarter_FTP\Zip\zlib.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\Border.rgn
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
