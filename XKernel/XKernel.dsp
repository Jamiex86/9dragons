# Microsoft Developer Studio Project File - Name="XKernel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=XKernel - Win32 KR_Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XKernel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XKernel.mak" CFG="XKernel - Win32 KR_Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XKernel - Win32 KR_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 KR_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 KR_Release_debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 KR_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 KR_Test_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 KR_Test_Release_debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 KR_Test_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 VN_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 VN_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 VN_Release_debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 VN_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 TW_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 TW_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 TW_Release_debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 TW_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 US_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 US_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 US_Release_debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 US_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 CN_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 CN_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 CN_Release_debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 CN_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 JP_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 JP_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 JP_Release_debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 JP_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 RS_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 RS_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 RS_Release_debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 RS_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 TH_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 TH_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 TH_Release_debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XKernel - Win32 TH_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/EmperorOfDragons/XKernel", LXVAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XKernel - Win32 KR_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/release/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/KR/release/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/release/XKernel"
# PROP Intermediate_Dir "../Compiled/KR/release/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XKOREAN" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/KR/XKernel.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 KR_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/KR/debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/KR/debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/KR/debug/XKernel"
# PROP Intermediate_Dir "../Compiled/KR/debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "_DEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XKOREAN" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/KR/XKernel_D.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 KR_Release_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/release_debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/KR/release_debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/release_debug/XKernel"
# PROP Intermediate_Dir "../Compiled/KR/release_debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./../XGamebase/Include" /I "./../XGamebase/Include/Template" /I "./../Utility" /I "./../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XKOREAN" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/KR/XKernel_RD.lib"
# ADD LIB32 /nologo /out:"../Library/KR/XKernel_RD.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 KR_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/release_gm/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/KR/release_gm/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/release_gm/XKernel"
# PROP Intermediate_Dir "../Compiled/KR/release_gm/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XKOREAN" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/KR/XKernel_GM.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 KR_Test_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/test_release/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/KR/test_release/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/test_release/XKernel"
# PROP Intermediate_Dir "../Compiled/KR/test_release/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XTESTSERVER" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XTESTSERVER" /D "_XKOREAN" /D "_XUSEAPEXMODULEXXXXX" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/KR/tsXKernel.lib"
# ADD LIB32 /nologo /out:"../Library/KR/tsXKernel.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 KR_Test_Release_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/test_release_debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/KR/test_release_debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/test_release_debug/XKernel"
# PROP Intermediate_Dir "../Compiled/KR/test_release_debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /FR /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XTESTSERVER" /D "_XKOREAN" /D "_XUSEAPEXMODULE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/KR/XKernel_RD.lib"
# ADD LIB32 /nologo /out:"../Library/KR/tsXKernel_RD.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 KR_Test_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/test_release_gm/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/KR/test_release_gm/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/test_release_gm/XKernel"
# PROP Intermediate_Dir "../Compiled/KR/test_release_gm/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XTESTSERVER" /D "_XKOREAN" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/KR/tsXKernel_GM.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 VN_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/VN/release/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/VN/release/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/VN/release/XKernel"
# PROP Intermediate_Dir "../Compiled/VN/release/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XVIETNAMESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/VN/XKernel.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 VN_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/VN/debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/VN/debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/VN/debug/XKernel"
# PROP Intermediate_Dir "../Compiled/VN/debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "_DEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XVIETNAMESE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/VN/XKernel_D.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 VN_Release_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/VN/release_debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/VN/release_debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/VN/release_debug/XKernel"
# PROP Intermediate_Dir "../Compiled/VN/release_debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./../XGamebase/Include" /I "./../XGamebase/Include/Template" /I "./../Utility" /I "./../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XVIETNAMESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/VN/XKernel_RD.lib"
# ADD LIB32 /nologo /out:"../Library/VN/XKernel_RD.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 VN_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/VN/release_gm/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/VN/release_gm/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/VN/release_gm/XKernel"
# PROP Intermediate_Dir "../Compiled/VN/release_gm/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XVIETNAMESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/VN/XKernel_GM.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 TW_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TW/release/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/TW/release/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TW/release/XKernel"
# PROP Intermediate_Dir "../Compiled/TW/release/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XTAIWANESE" /D "_XTW_CSRELEASEVERSION" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TW/XKernel.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 TW_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/TW/debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/TW/debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/TW/debug/XKernel"
# PROP Intermediate_Dir "../Compiled/TW/debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "_DEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XTAIWANESE" /D "_XTW_CSRELEASEVERSION" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TW/XKernel_D.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 TW_Release_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TW/release_debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/TW/release_debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TW/release_debug/XKernel"
# PROP Intermediate_Dir "../Compiled/TW/release_debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./../XGamebase/Include" /I "./../XGamebase/Include/Template" /I "./../Utility" /I "./../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XTAIWANESE" /D "_XTW_CSRELEASEVERSION" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/TW/XKernel_RD.lib"
# ADD LIB32 /nologo /out:"../Library/TW/XKernel_RD.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 TW_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TW/release_gm/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/TW/release_gm/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TW/release_gm/XKernel"
# PROP Intermediate_Dir "../Compiled/TW/release_gm/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XTAIWANESE" /D "_XTW_CSRELEASEVERSION" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TW/XKernel_GM.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 US_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/US/release/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/US/release/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/US/release/XKernel"
# PROP Intermediate_Dir "../Compiled/US/release/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_ACCLAIM_VERSION" /D "_XENGLISH" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/US/XKernel.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 US_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/US/debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/US/debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/US/debug/XKernel"
# PROP Intermediate_Dir "../Compiled/US/debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "_DEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_ACCLAIM_VERSION" /D "_XENGLISH" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/US/XKernel_D.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 US_Release_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/US/release_debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/US/release_debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/US/release_debug/XKernel"
# PROP Intermediate_Dir "../Compiled/US/release_debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./../XGamebase/Include" /I "./../XGamebase/Include/Template" /I "./../Utility" /I "./../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_ACCLAIM_VERSION" /D "_XENGLISH" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/US/XKernel_RD.lib"
# ADD LIB32 /nologo /out:"../Library/US/XKernel_RD.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 US_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/US/release_gm/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/US/release_gm/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/US/release_gm/XKernel"
# PROP Intermediate_Dir "../Compiled/US/release_gm/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_ACCLAIM_VERSION" /D "_XENGLISH" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/US/XKernel_GM.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 CN_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/CN/release/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/CN/release/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/CN/release/XKernel"
# PROP Intermediate_Dir "../Compiled/CN/release/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XCHINESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/CN/XKernel.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 CN_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/CN/debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/CN/debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/CN/debug/XKernel"
# PROP Intermediate_Dir "../Compiled/CN/debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "_DEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XCHINESE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/CN/XKernel_D.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 CN_Release_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/CN/release_debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/CN/release_debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/CN/release_debug/XKernel"
# PROP Intermediate_Dir "../Compiled/CN/release_debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./../XGamebase/Include" /I "./../XGamebase/Include/Template" /I "./../Utility" /I "./../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XCHINESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/CN/XKernel_RD.lib"
# ADD LIB32 /nologo /out:"../Library/CN/XKernel_RD.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 CN_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/CN/release_gm/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/CN/release_gm/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/CN/release_gm/XKernel"
# PROP Intermediate_Dir "../Compiled/CN/release_gm/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XCHINESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/CN/XKernel_GM.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 JP_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/JP/release/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/JP/release/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/JP/release/XKernel"
# PROP Intermediate_Dir "../Compiled/JP/release/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XJAPANESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/JP/XKernel.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 JP_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/JP/debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/JP/debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/JP/debug/XKernel"
# PROP Intermediate_Dir "../Compiled/JP/debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "_DEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XJAPANESE" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/JP/XKernel_D.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 JP_Release_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/JP/release_debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/JP/release_debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/JP/release_debug/XKernel"
# PROP Intermediate_Dir "../Compiled/JP/release_debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./../XGamebase/Include" /I "./../XGamebase/Include/Template" /I "./../Utility" /I "./../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XJAPANESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/JP/XKernel_RD.lib"
# ADD LIB32 /nologo /out:"../Library/JP/XKernel_RD.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 JP_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/JP/release_gm/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/JP/release_gm/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/JP/release_gm/XKernel"
# PROP Intermediate_Dir "../Compiled/JP/release_gm/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XJAPANESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/JP/XKernel_GM.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 RS_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/RS/release/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/RS/release/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/RS/release/XKernel"
# PROP Intermediate_Dir "../Compiled/RS/release/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XRUSSIAN" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/RS/XKernel.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 RS_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/RS/debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/RS/debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/RS/debug/XKernel"
# PROP Intermediate_Dir "../Compiled/RS/debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "_DEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XRUSSIAN" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/RS/XKernel_D.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 RS_Release_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/RS/release_debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/RS/release_debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/RS/release_debug/XKernel"
# PROP Intermediate_Dir "../Compiled/RS/release_debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./../XGamebase/Include" /I "./../XGamebase/Include/Template" /I "./../Utility" /I "./../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XRUSSIAN" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/RS/XKernel_RD.lib"
# ADD LIB32 /nologo /out:"../Library/RS/XKernel_RD.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 RS_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/RS/release_gm/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/RS/release_gm/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/RS/release_gm/XKernel"
# PROP Intermediate_Dir "../Compiled/RS/release_gm/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XRUSSIAN" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/RS/XKernel_GM.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 TH_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TH/release/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/TH/release/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TH/release/XKernel"
# PROP Intermediate_Dir "../Compiled/TH/release/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XTHAI" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TH/XKernel.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 TH_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/TH/debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/TH/debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/TH/debug/XKernel"
# PROP Intermediate_Dir "../Compiled/TH/debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "_DEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XTHAI" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TH/XKernel_D.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 TH_Release_debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TH/release_debug/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/TH/release_debug/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TH/release_debug/XKernel"
# PROP Intermediate_Dir "../Compiled/TH/release_debug/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./../XGamebase/Include" /I "./../XGamebase/Include/Template" /I "./../Utility" /I "./../" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XTHAI" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/TH/XKernel_RD.lib"
# ADD LIB32 /nologo /out:"../Library/TH/XKernel_RD.lib"

!ELSEIF  "$(CFG)" == "XKernel - Win32 TH_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TH/release_gm/XKernel"
# PROP BASE Intermediate_Dir "../Compiled/TH/release_gm/XKernel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TH/release_gm/XKernel"
# PROP Intermediate_Dir "../Compiled/TH/release_gm/XKernel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./" /I ".././" /I "./../XGamebase/Include" /I "./../XGamebase/src/libs/jpeg" /I "./../XGamebase/Include/Template" /I "../HackDetector/HackShield" /I "../HackDetector/GameGuard" /I "../HackDetector/XTrap" /I "../MeshControl" /I "../XParticleCore" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XTHAI" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TH/XKernel_GM.lib"

!ENDIF 

# Begin Target

# Name "XKernel - Win32 KR_Release"
# Name "XKernel - Win32 KR_Debug"
# Name "XKernel - Win32 KR_Release_debug"
# Name "XKernel - Win32 KR_Release_GM"
# Name "XKernel - Win32 KR_Test_Release"
# Name "XKernel - Win32 KR_Test_Release_debug"
# Name "XKernel - Win32 KR_Test_Release_GM"
# Name "XKernel - Win32 VN_Release"
# Name "XKernel - Win32 VN_Debug"
# Name "XKernel - Win32 VN_Release_debug"
# Name "XKernel - Win32 VN_Release_GM"
# Name "XKernel - Win32 TW_Release"
# Name "XKernel - Win32 TW_Debug"
# Name "XKernel - Win32 TW_Release_debug"
# Name "XKernel - Win32 TW_Release_GM"
# Name "XKernel - Win32 US_Release"
# Name "XKernel - Win32 US_Debug"
# Name "XKernel - Win32 US_Release_debug"
# Name "XKernel - Win32 US_Release_GM"
# Name "XKernel - Win32 CN_Release"
# Name "XKernel - Win32 CN_Debug"
# Name "XKernel - Win32 CN_Release_debug"
# Name "XKernel - Win32 CN_Release_GM"
# Name "XKernel - Win32 JP_Release"
# Name "XKernel - Win32 JP_Debug"
# Name "XKernel - Win32 JP_Release_debug"
# Name "XKernel - Win32 JP_Release_GM"
# Name "XKernel - Win32 RS_Release"
# Name "XKernel - Win32 RS_Debug"
# Name "XKernel - Win32 RS_Release_debug"
# Name "XKernel - Win32 RS_Release_GM"
# Name "XKernel - Win32 TH_Release"
# Name "XKernel - Win32 TH_Debug"
# Name "XKernel - Win32 TH_Release_debug"
# Name "XKernel - Win32 TH_Release_GM"
# Begin Group "TripleX Define"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XBaseDefs.h
# End Source File
# Begin Source File

SOURCE=.\XExternDefs.h
# End Source File
# Begin Source File

SOURCE=.\XExternValues.h
# End Source File
# End Group
# Begin Group "TripleX Core"

# PROP Default_Filter ""
# Begin Group "Application"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XApplication\X3DApp.cpp
# End Source File
# Begin Source File

SOURCE=.\X3DApp.h
# End Source File
# Begin Source File

SOURCE=.\XApplication\X3DSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\X3DSettings.h
# End Source File
# Begin Source File

SOURCE=.\XApplication\XDeviceEnum.cpp
# End Source File
# Begin Source File

SOURCE=.\XDeviceEnum.h
# End Source File
# Begin Source File

SOURCE=.\XApplication\XDialogs.cpp
# End Source File
# Begin Source File

SOURCE=.\XDialogs.h
# End Source File
# Begin Source File

SOURCE=.\XApplication\XKernel.cpp
# End Source File
# Begin Source File

SOURCE=.\XKernel.h
# End Source File
# Begin Source File

SOURCE=.\XApplication\XProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\XProcess.h
# End Source File
# End Group
# Begin Group "Camera"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XCamera\XCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\XCamera.h
# End Source File
# End Group
# Begin Group "Collide system"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XCollideSystem\XCollide.cpp
# End Source File
# Begin Source File

SOURCE=.\XCollide.h
# End Source File
# End Group
# Begin Group "Data manager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XDataManager\XItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Xitem.h
# End Source File
# Begin Source File

SOURCE=.\XDataManager\XList.cpp
# End Source File
# Begin Source File

SOURCE=.\Xlist.h
# End Source File
# End Group
# Begin Group "Error check"

# PROP Default_Filter ""
# Begin Group "XExceptionReporter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XExceptionReporter\XExceptionReporter.cpp
# End Source File
# Begin Source File

SOURCE=.\XExceptionReporter.h
# End Source File
# Begin Source File

SOURCE=.\XFunctionStackTrace.h
# End Source File
# Begin Source File

SOURCE=.\XExceptionReporter\XGetWinVer.cpp
# End Source File
# Begin Source File

SOURCE=.\XGetWinVer.h
# End Source File
# Begin Source File

SOURCE=.\XExceptionReporter\XMiniVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\XMiniVersion.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\dbghelp.h
# End Source File
# Begin Source File

SOURCE=.\XErrorCheck\XError.cpp
# End Source File
# Begin Source File

SOURCE=.\XError.h
# End Source File
# Begin Source File

SOURCE=.\XErrorCheck\XException.cpp
# End Source File
# Begin Source File

SOURCE=.\XException.h
# End Source File
# Begin Source File

SOURCE=.\XErrorCheck\XExceptionHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\XExceptionHandler.h
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XMath\XCollideMath.cpp
# End Source File
# Begin Source File

SOURCE=.\XCollideMath.h
# End Source File
# Begin Source File

SOURCE=.\XCollideSystem\XEigen.cpp
# End Source File
# Begin Source File

SOURCE=.\XEigen.h
# End Source File
# Begin Source File

SOURCE=.\XUtilities\XFastCode.cpp
# End Source File
# Begin Source File

SOURCE=.\XFastCode.h
# End Source File
# End Group
# Begin Group "Mesh control"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XMeshControl\XBillboard.cpp
# End Source File
# Begin Source File

SOURCE=.\XBillboard.h
# End Source File
# Begin Source File

SOURCE=.\XMath\XMeshMath.cpp
# End Source File
# Begin Source File

SOURCE=.\XMeshMath.h
# End Source File
# Begin Source File

SOURCE=.\XModelContainerDef.h
# End Source File
# Begin Source File

SOURCE=.\XMeshControl\XObb.cpp
# End Source File
# Begin Source File

SOURCE=.\XObb.h
# End Source File
# End Group
# Begin Group "Shader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XVertexShaders.h
# End Source File
# End Group
# Begin Group "Texture control"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LoadThread.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadThread.h
# End Source File
# Begin Source File

SOURCE=.\XDataPool.h
# End Source File
# Begin Source File

SOURCE=.\XMemoryPool.h
# End Source File
# Begin Source File

SOURCE=.\XReference.h
# End Source File
# Begin Source File

SOURCE=.\XTexture\XRenderStyleManager.cpp
# End Source File
# Begin Source File

SOURCE=.\XRenderStyleManager.h
# End Source File
# Begin Source File

SOURCE=.\XTexture\XTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\XTexture.h
# End Source File
# Begin Source File

SOURCE=.\XTextureController.cpp
# End Source File
# Begin Source File

SOURCE=.\XTextureController.h
# End Source File
# Begin Source File

SOURCE=.\XTexture\XTextureManager.cpp
# End Source File
# Begin Source File

SOURCE=.\XTextureManager.h
# End Source File
# Begin Source File

SOURCE=.\XTextureObjectController.cpp
# End Source File
# Begin Source File

SOURCE=.\XTextureObjectController.h
# End Source File
# End Group
# Begin Group "Timer control"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XTimerControl\XTimeCounter.cpp
# End Source File
# Begin Source File

SOURCE=.\XTimeCounter.h
# End Source File
# Begin Source File

SOURCE=.\XTimerControl\XTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\XTimer.h
# End Source File
# End Group
# End Group
# Begin Group "TripleX Utility"

# PROP Default_Filter ""
# Begin Group "D3D utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XD3DUtilities\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dfont.h
# End Source File
# Begin Source File

SOURCE=.\XD3DUtilities\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\XD3DUtilities\dsutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dsutil.h
# End Source File
# Begin Source File

SOURCE=.\XD3DUtilities\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dxutil.h
# End Source File
# End Group
# Begin Group "Security"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XSecurity\XCrypto.Cpp
# End Source File
# Begin Source File

SOURCE=.\XCrypto.h
# End Source File
# Begin Source File

SOURCE=.\XEncryptdata.h
# End Source File
# Begin Source File

SOURCE=.\XSecurity\XEncryptor.cpp
# End Source File
# Begin Source File

SOURCE=.\XEncryptor.h
# End Source File
# End Group
# Begin Group "Utility"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XUtilities\XCompress.cpp
# End Source File
# Begin Source File

SOURCE=.\XCompress.h
# End Source File
# Begin Source File

SOURCE=.\XUtilities\XPackage.cpp
# End Source File
# Begin Source File

SOURCE=.\XPackage.h
# End Source File
# Begin Source File

SOURCE=.\XUtilities\XRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\XRegistry.h
# End Source File
# Begin Source File

SOURCE=.\XUtilities\XStringItem.cpp
# End Source File
# Begin Source File

SOURCE=.\XStringItem.h
# End Source File
# Begin Source File

SOURCE=.\XUtilities\XUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\XUtils.h
# End Source File
# End Group
# End Group
# Begin Group "XGUI Control"

# PROP Default_Filter ""
# Begin Group "GUI control"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XGUI\XBallGauge.cpp
# End Source File
# Begin Source File

SOURCE=.\XBallGauge.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XButton.cpp
# End Source File
# Begin Source File

SOURCE=.\XButton.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XCheckButton.cpp
# End Source File
# Begin Source File

SOURCE=.\XCheckButton.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XGUIControlCenter.cpp
# End Source File
# Begin Source File

SOURCE=.\XGUIControlCenter.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XGUIObject.cpp
# End Source File
# Begin Source File

SOURCE=.\XGUIObject.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XHSlider.cpp
# End Source File
# Begin Source File

SOURCE=.\XHSlider.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XImageStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\XImageStatic.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\XListBox.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XMString.cpp
# End Source File
# Begin Source File

SOURCE=.\XMString.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\XScrollBar.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\XStatic.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XStickGauge.cpp
# End Source File
# Begin Source File

SOURCE=.\XStickGauge.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XToolTip.cpp
# End Source File
# Begin Source File

SOURCE=.\XToolTip.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XTreeBox.cpp
# End Source File
# Begin Source File

SOURCE=.\XTreeBox.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XVSlider.cpp
# End Source File
# Begin Source File

SOURCE=.\XVSlider.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\XWindow.h
# End Source File
# Begin Source File

SOURCE=.\XInputSystem\XWindow_IME.cpp
# End Source File
# Begin Source File

SOURCE=.\XWindow_IME.h
# End Source File
# Begin Source File

SOURCE=.\XGUI\XWindowManager.cpp
# End Source File
# Begin Source File

SOURCE=.\XWindowManager.h
# End Source File
# End Group
# Begin Group "Font system"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XFontSystem\X3DUFont.cpp
# End Source File
# Begin Source File

SOURCE=.\X3DUFont.h
# End Source File
# Begin Source File

SOURCE=.\XFontSystem\XFont.cpp
# End Source File
# Begin Source File

SOURCE=.\XFont.h
# End Source File
# Begin Source File

SOURCE=.\XFontSystem\XStringDataBase.cpp
# End Source File
# Begin Source File

SOURCE=.\XStringDataBase.h
# End Source File
# Begin Source File

SOURCE=.\XFontSystem\XUnicodeConv.cpp
# End Source File
# Begin Source File

SOURCE=.\XUnicodeConv.h
# End Source File
# Begin Source File

SOURCE=.\XFontSystem\XUnicodeFont.cpp
# End Source File
# Begin Source File

SOURCE=.\XUnicodeFont.h
# End Source File
# Begin Source File

SOURCE=.\XVSPrintf.cpp
# End Source File
# End Group
# Begin Group "Input system"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XInputSystem\XIME.cpp
# End Source File
# Begin Source File

SOURCE=.\XIME.h
# End Source File
# Begin Source File

SOURCE=.\XInputSystem\XInput.cpp
# End Source File
# Begin Source File

SOURCE=.\XInput.h
# End Source File
# End Group
# End Group
# End Target
# End Project
