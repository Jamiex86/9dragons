# Microsoft Developer Studio Project File - Name="XGamebase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=XGamebase - Win32 KR_Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XGamebase.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XGamebase.mak" CFG="XGamebase - Win32 KR_Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XGamebase - Win32 KR_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 KR_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 KR_Release_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 KR_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 KR_Test_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 KR_Test_Release_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 KR_Test_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 VN_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 VN_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 VN_Release_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 VN_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 TW_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 TW_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 TW_Release_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 TW_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 US_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 US_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 US_Release_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 US_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 CN_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 CN_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 CN_Release_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 CN_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 JP_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 JP_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 JP_Release_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 JP_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 RS_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 RS_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 RS_Release_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 RS_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 TH_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 TH_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 TH_Release_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "XGamebase - Win32 TH_Release_GM" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/EmperorOfDragons/XGamebase", VKEBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XGamebase - Win32 KR_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/release/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/KR/release/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/release/XGamebase"
# PROP Intermediate_Dir "../Compiled/KR/release/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/KR/XGamebase.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 KR_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/KR/debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/KR/debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/KR/debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/KR/debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/KR/XGamebase_D.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 KR_Release_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/release_debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/KR/release_debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/release_debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/KR/release_debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/KR/XGamebase_RD.lib"
# ADD LIB32 /nologo /out:"../Library/KR/XGamebase_RD.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 KR_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/release_gm/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/KR/release_gm/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/release_gm/XGamebase"
# PROP Intermediate_Dir "../Compiled/KR/release_gm/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/KR/XGamebase_GM.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 KR_Test_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/test_release/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/KR/test_release/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/test_release/XGamebase"
# PROP Intermediate_Dir "../Compiled/KR/test_release/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XTESTSERVER" /FR /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XTESTSERVER" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/KR/tsXGamebase.lib"
# ADD LIB32 /nologo /out:"../Library/KR/tsXGamebase.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 KR_Test_Release_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/test_release_debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/KR/test_release_debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/test_release_debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/KR/test_release_debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GX /Zi /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /FR /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XTESTSERVER" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/KR/tsXGamebase_RD.lib"
# ADD LIB32 /nologo /out:"../Library/KR/tsXGamebase_RD.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 KR_Test_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/KR/test_release_gm/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/KR/test_release_gm/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/KR/test_release_gm/XGamebase"
# PROP Intermediate_Dir "../Compiled/KR/test_release_gm/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XTESTSERVER" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/KR/tsXGamebase_GM.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 VN_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/VN/release/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/VN/release/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/VN/release/XGamebase"
# PROP Intermediate_Dir "../Compiled/VN/release/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XVIETNAMESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/VN/XGamebase.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 VN_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/VN/debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/VN/debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/VN/debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/VN/debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XVIETNAMESE" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/VN/XGamebase_D.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 VN_Release_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/VN/release_debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/VN/release_debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/VN/release_debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/VN/release_debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XVIETNAMESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/VN/XGamebase_RD.lib"
# ADD LIB32 /nologo /out:"../Library/VN/XGamebase_RD.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 VN_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/VN/release_gm/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/VN/release_gm/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/VN/release_gm/XGamebase"
# PROP Intermediate_Dir "../Compiled/VN/release_gm/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XVIETNAMESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/VN/XGamebase_GM.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 TW_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TW/release/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/TW/release/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TW/release/XGamebase"
# PROP Intermediate_Dir "../Compiled/TW/release/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XTAIWANESE" /D "_XTW_CSRELEASEVERSION" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TW/XGamebase.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 TW_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/TW/debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/TW/debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/TW/debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/TW/debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XTAIWANESE" /D "_XTW_CSRELEASEVERSION" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TW/XGamebase_D.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 TW_Release_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TW/release_debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/TW/release_debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TW/release_debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/TW/release_debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XTAIWANESE" /D "_XTW_CSRELEASEVERSION" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/TW/XGamebase_RD.lib"
# ADD LIB32 /nologo /out:"../Library/TW/XGamebase_RD.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 TW_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TW/release_gm/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/TW/release_gm/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TW/release_gm/XGamebase"
# PROP Intermediate_Dir "../Compiled/TW/release_gm/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XTAIWANESE" /D "_XTW_CSRELEASEVERSION" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TW/XGamebase_GM.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 US_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/US/release/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/US/release/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/US/release/XGamebase"
# PROP Intermediate_Dir "../Compiled/US/release/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_ACCLAIM_VERSION" /D "_XENGLISH" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/US/XGamebase.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 US_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/US/debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/US/debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/US/debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/US/debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_ACCLAIM_VERSION" /D "_XENGLISH" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/US/XGamebase_D.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 US_Release_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/US/release_debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/US/release_debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/US/release_debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/US/release_debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_ACCLAIM_VERSION" /D "_XENGLISH" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/US/XGamebase_RD.lib"
# ADD LIB32 /nologo /out:"../Library/US/XGamebase_RD.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 US_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/US/release_gm/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/US/release_gm/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/US/release_gm/XGamebase"
# PROP Intermediate_Dir "../Compiled/US/release_gm/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_ACCLAIM_VERSION" /D "_XENGLISH" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/US/XGamebase_GM.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 CN_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/CN/release/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/CN/release/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/CN/release/XGamebase"
# PROP Intermediate_Dir "../Compiled/CN/release/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XCHINESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/CN/XGamebase.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 CN_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/CN/debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/CN/debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/CN/debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/CN/debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XCHINESE" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/CN/XGamebase_D.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 CN_Release_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/CN/release_debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/CN/release_debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/CN/release_debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/CN/release_debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XCHINESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/CN/XGamebase_RD.lib"
# ADD LIB32 /nologo /out:"../Library/CN/XGamebase_RD.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 CN_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/CN/release_gm/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/CN/release_gm/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/CN/release_gm/XGamebase"
# PROP Intermediate_Dir "../Compiled/CN/release_gm/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XCHINESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/CN/XGamebase_GM.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 JP_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/JP/release/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/JP/release/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/JP/release/XGamebase"
# PROP Intermediate_Dir "../Compiled/JP/release/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XJAPANESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/JP/XGamebase.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 JP_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/JP/debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/JP/debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/JP/debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/JP/debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XJAPANESE" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/JP/XGamebase_D.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 JP_Release_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/JP/release_debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/JP/release_debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/JP/release_debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/JP/release_debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XJAPANESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/JP/XGamebase_RD.lib"
# ADD LIB32 /nologo /out:"../Library/JP/XGamebase_RD.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 JP_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/JP/release_gm/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/JP/release_gm/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/JP/release_gm/XGamebase"
# PROP Intermediate_Dir "../Compiled/JP/release_gm/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XJAPANESE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/JP/XGamebase_GM.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 RS_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/RS/release/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/RS/release/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/RS/release/XGamebase"
# PROP Intermediate_Dir "../Compiled/RS/release/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XRUSSIAN" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/RS/XGamebase.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 RS_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/RS/debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/RS/debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/RS/debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/RS/debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XRUSSIAN" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/RS/XGamebase_D.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 RS_Release_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/RS/release_debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/RS/release_debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/RS/release_debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/RS/release_debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XRUSSIAN" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/RS/XGamebase_RD.lib"
# ADD LIB32 /nologo /out:"../Library/RS/XGamebase_RD.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 RS_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/RS/release_gm/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/RS/release_gm/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/RS/release_gm/XGamebase"
# PROP Intermediate_Dir "../Compiled/RS/release_gm/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XRUSSIAN" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/RS/XGamebase_GM.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 TH_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TH/release/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/TH/release/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TH/release/XGamebase"
# PROP Intermediate_Dir "../Compiled/TH/release/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XTHAI" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TH/XGamebase.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 TH_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../Compiled/TH/debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/TH/debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Compiled/TH/debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/TH/debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XDWDEBUG" /D "_XADMINISTRATORMODE" /D "_XTHAI" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TH/XGamebase_D.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 TH_Release_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TH/release_debug/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/TH/release_debug/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TH/release_debug/XGamebase"
# PROP Intermediate_Dir "../Compiled/TH/release_debug/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "_XDWDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XADMINISTRATORMODE" /D "_XTHAI" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../Library/TH/XGamebase_RD.lib"
# ADD LIB32 /nologo /out:"../Library/TH/XGamebase_RD.lib"

!ELSEIF  "$(CFG)" == "XGamebase - Win32 TH_Release_GM"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../Compiled/TH/release_gm/XGamebase"
# PROP BASE Intermediate_Dir "../Compiled/TH/release_gm/XGamebase"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Compiled/TH/release_gm/XGamebase"
# PROP Intermediate_Dir "../Compiled/TH/release_gm/XGamebase"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "./Include" /I "./Include/template" /I "./src/gfx" /I "./src/libs/jpeg" /I "./src/libs/png" /I "./src/libs/zlib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_XGMCLIENT" /D "_XTHAI" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Library/TH/XGamebase_GM.lib"

!ENDIF 

# Begin Target

# Name "XGamebase - Win32 KR_Release"
# Name "XGamebase - Win32 KR_Debug"
# Name "XGamebase - Win32 KR_Release_Debug"
# Name "XGamebase - Win32 KR_Release_GM"
# Name "XGamebase - Win32 KR_Test_Release"
# Name "XGamebase - Win32 KR_Test_Release_Debug"
# Name "XGamebase - Win32 KR_Test_Release_GM"
# Name "XGamebase - Win32 VN_Release"
# Name "XGamebase - Win32 VN_Debug"
# Name "XGamebase - Win32 VN_Release_Debug"
# Name "XGamebase - Win32 VN_Release_GM"
# Name "XGamebase - Win32 TW_Release"
# Name "XGamebase - Win32 TW_Debug"
# Name "XGamebase - Win32 TW_Release_Debug"
# Name "XGamebase - Win32 TW_Release_GM"
# Name "XGamebase - Win32 US_Release"
# Name "XGamebase - Win32 US_Debug"
# Name "XGamebase - Win32 US_Release_Debug"
# Name "XGamebase - Win32 US_Release_GM"
# Name "XGamebase - Win32 CN_Release"
# Name "XGamebase - Win32 CN_Debug"
# Name "XGamebase - Win32 CN_Release_Debug"
# Name "XGamebase - Win32 CN_Release_GM"
# Name "XGamebase - Win32 JP_Release"
# Name "XGamebase - Win32 JP_Debug"
# Name "XGamebase - Win32 JP_Release_Debug"
# Name "XGamebase - Win32 JP_Release_GM"
# Name "XGamebase - Win32 RS_Release"
# Name "XGamebase - Win32 RS_Debug"
# Name "XGamebase - Win32 RS_Release_Debug"
# Name "XGamebase - Win32 RS_Release_GM"
# Name "XGamebase - Win32 TH_Release"
# Name "XGamebase - Win32 TH_Debug"
# Name "XGamebase - Win32 TH_Release_Debug"
# Name "XGamebase - Win32 TH_Release_GM"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "filetype"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\filetype\bmp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\filetype\bmpfiles.cpp
# End Source File
# Begin Source File

SOURCE=.\src\filetype\gif.cpp
# End Source File
# Begin Source File

SOURCE=.\src\filetype\jpg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\filetype\pal.cpp
# End Source File
# Begin Source File

SOURCE=.\src\filetype\pcx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\filetype\png.cpp
# End Source File
# Begin Source File

SOURCE=.\src\filetype\tga.cpp
# End Source File
# End Group
# Begin Group "gfx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\gfx\baseprim.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gfx\baseprim.h
# End Source File
# Begin Source File

SOURCE=.\src\gfx\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gfx\blit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gfx\blit.h
# End Source File
# Begin Source File

SOURCE=.\src\gfx\blitmain.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gfx\bmpconv.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gfx\bmputils.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gfx\bmpvtbl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gfx\colorpal.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gfx\macro12.h
# End Source File
# Begin Source File

SOURCE=.\src\gfx\macro15.h
# End Source File
# Begin Source File

SOURCE=.\src\gfx\macro16.h
# End Source File
# Begin Source File

SOURCE=.\src\gfx\macro24.h
# End Source File
# Begin Source File

SOURCE=.\src\gfx\macro32.h
# End Source File
# Begin Source File

SOURCE=.\src\gfx\macro8.h
# End Source File
# Begin Source File

SOURCE=.\src\gfx\prim.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gfx\quantize.cpp
# End Source File
# End Group
# Begin Group "rwops"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\rwops\rw_mem.cpp
# End Source File
# Begin Source File

SOURCE=.\src\rwops\rw_stdio.cpp
# End Source File
# Begin Source File

SOURCE=.\src\rwops\rwops.cpp
# End Source File
# End Group
# Begin Group "system"

# PROP Default_Filter ""
# Begin Group "win_system"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\system\xwfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xwsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xwtimer.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\system\xconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xerror.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xgamebase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xlibc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xlog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xmisc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xos.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xrandom.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xtimer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\xunicode.cpp
# End Source File
# End Group
# Begin Group "libs"

# PROP Default_Filter ""
# Begin Group "png"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\libs\png\libpng.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\png.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngasmrd.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngconf.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngerror.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pnggccrd.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngget.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngmem.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngpread.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngread.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngrio.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngrtran.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngrutil.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngset.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngtrans.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngvcrd.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngwio.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngwrite.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngwtran.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\png\pngwutil.c
# End Source File
# End Group
# Begin Group "jpeg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\libs\jpeg\jcapimin.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jcapistd.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jccoefct.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jccolor.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jchuff.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jchuff.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jcinit.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jcmainct.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jcmarker.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jcmaster.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jcomapi.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jconfig.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jcparam.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jcphuff.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jcprepct.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jcsample.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jctrans.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdapimin.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdapistd.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdatadst.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdatasrc.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdcolor.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdct.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdhuff.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdhuff.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdinput.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdmainct.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdmarker.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdmaster.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdmerge.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdphuff.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdpostct.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdsample.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jdtrans.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jerror.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jerror.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jfdctint.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jidctflt.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jidctfst.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jidctint.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jidctred.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jinclude.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jmemnobs.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jmemsys.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jpegint.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jquant1.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jquant2.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jutils.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\jpeg\jversion.h
# End Source File
# End Group
# Begin Group "zlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\libs\zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\compress.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\deflate.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\infblock.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\infblock.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\infcodes.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\infcodes.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\inffast.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\infutil.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\infutil.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\trees.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\trees.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\zutil.c
# End Source File
# Begin Source File

SOURCE=.\src\libs\zlib\zutil.h
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Template"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Include\template\list.tlh
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Include\template\stack.tlh
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\Include\xconfig.h
# End Source File
# Begin Source File

SOURCE=.\Include\xfiletype.h
# End Source File
# Begin Source File

SOURCE=.\Include\xgamebase.h
# End Source File
# Begin Source File

SOURCE=.\Include\xgfx.h
# End Source File
# Begin Source File

SOURCE=.\Include\xinline.h
# End Source File
# Begin Source File

SOURCE=.\Include\xintern.h
# End Source File
# Begin Source File

SOURCE=.\Include\xstring.h
# End Source File
# Begin Source File

SOURCE=.\Include\xsystem.h
# End Source File
# Begin Source File

SOURCE=.\Include\xunicode.h
# End Source File
# End Group
# End Target
# End Project
