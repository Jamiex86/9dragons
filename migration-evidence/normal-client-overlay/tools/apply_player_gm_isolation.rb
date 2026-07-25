#!/usr/bin/env ruby
# Applies the review-only normal-player GM isolation to an assembled source tree.

root = File.expand_path(ARGV.fetch(0))

def transform(root, relative)
  path = File.join(root, relative)
  original = File.binread(path)
  changed = yield(original)
  raise "#{relative}: no change" if changed == original
  File.binwrite(path, changed)
end

def replace_once(data, pattern, replacement, label)
  matches = data.scan(pattern).length
  raise "#{label}: expected one match, found #{matches}" unless matches == 1
  data.sub(pattern, replacement)
end

transform(root, "XProcess/XProc_FirstLoad.cpp") do |data|
  data = replace_once(
    data,
    /#include "XWindow_GMCommand\.h"\r?\n#include "XWindow_GM_SendNoticeMessage\.h"\r?\n#include "XWindow_GM_UserCoordination\.h"\r?\n#include "XWindow_GM_MonsterManage\.h"\r?\n#include "XWindow_GM_StatusControl\.h"\r?\n#include "XWindow_GM_EventManager\.h"/,
    "#ifndef _XPLAYERCLIENT\r\n#include \"XWindow_GMCommand.h\"\r\n#include \"XWindow_GM_SendNoticeMessage.h\"\r\n#include \"XWindow_GM_UserCoordination.h\"\r\n#include \"XWindow_GM_MonsterManage.h\"\r\n#include \"XWindow_GM_StatusControl.h\"\r\n#include \"XWindow_GM_EventManager.h\"\r\n#endif",
    "first-load includes"
  )
  data = replace_once(
    data,
    /\t_XWindow_GM_MonsterManage\* pWindow = .*?\r?\n\t}\r?\n(?=\r?\n\tSetMessage\( _XGETINTERFACETEXT\(ID_STRING_FIRSTLOAD_NPCMODEL\))/m,
    "#ifndef _XPLAYERCLIENT\r\n\\0#endif\r\n",
    "monster-list update"
  )
  replace_once(
    data,
    /\t\/\/ GM Command window =+.*?(?=\t\/\/ Create Gamble Window)/m,
    "#ifndef _XPLAYERCLIENT\r\n\\0#endif\r\n\r\n",
    "GM window construction"
  )
end

transform(root, "XProcess/XProc_MainGame.cpp") do |data|
  data = replace_once(
    data,
    /\/\/ GM Control windows\r?\n#include "XWindow_GMCommand\.h"\r?\n#include "XWindow_GM_SendNoticeMessage\.h"/,
    "// GM Control windows are excluded from the normal-player target.\n#ifndef _XPLAYERCLIENT\n#include \"XWindow_GMCommand.h\"\n#include \"XWindow_GM_SendNoticeMessage.h\"\n#endif",
    "main GM includes"
  )
  data = replace_once(
    data,
    /(\t_XWindow_GMCommand\* pGMCommandWindow = .*?\n\t}\n)\n(\treturn true;)/m,
    "#ifndef _XPLAYERCLIENT\n\\1\n#endif\n\n\\2",
    "main device GM maintenance"
  )
  replace_once(
    data,
    /(\t_XWindow_GMCommand\* pGMCommandWindow = .*?\n\t}\n)\t\n(\t_XWindow_Option\*)/m,
    "#ifndef _XPLAYERCLIENT\n\\1#endif\n\n\\2",
    "main draw GM maintenance"
  )
end

transform(root, "XProcess/XProc_MainGameCallBackFunctions.cpp") do |data|
  replace_once(
    data,
    /^#include "XWindow_GM_StatusControl\.h"\n/,
    "#ifndef _XPLAYERCLIENT\n#include \"XWindow_GM_StatusControl.h\"\n#endif\n",
    "callback GM include"
  )
end

transform(root, "XProcess/XProc_MainGameMessageHandler.cpp") do |data|
  replace_once(
    data,
    /#include "XWindow_GMCommand\.h"\r?\n#include "XWindow_GM_SendNoticeMessage\.h"\r?\n#include "XWindow_GM_UserCoordination\.h"\r?\n#include "XWindow_GM_MonsterManage\.h"\r?\n#include "XWindow_GM_StatusControl\.h"\r?\n#include "XWindow_GM_EventManager\.h"/,
    "#ifndef _XPLAYERCLIENT\r\n#include \"XWindow_GMCommand.h\"\r\n#include \"XWindow_GM_SendNoticeMessage.h\"\r\n#include \"XWindow_GM_UserCoordination.h\"\r\n#include \"XWindow_GM_MonsterManage.h\"\r\n#include \"XWindow_GM_StatusControl.h\"\r\n#include \"XWindow_GM_EventManager.h\"\r\n#endif",
    "message-handler GM includes"
  )
end

transform(root, "XProcess/XWindow_WorldMinimap.cpp") do |data|
  data = replace_once(
    data,
    /^#include "XWindow_GM_UserCoordination\.h"\r?\n/,
    "#ifndef _XPLAYERCLIENT\r\n#include \"XWindow_GM_UserCoordination.h\"\r\n#endif\r\n",
    "minimap GM include"
  )
  replace_once(
    data,
    /\t{6}_XWindow_GM_UserCoordination\* pWindow = .*?\r?\n\t{6}}\r?\n/m,
    "#ifndef _XPLAYERCLIENT\r\n\\0#endif\r\n",
    "minimap coordinate transfer"
  )
end

transform(root, "Network/XNetwork.cpp") do |data|
  replace_once(
    data,
    /^#include "XWindow_GM_UserCoordination\.h"\r?\n/,
    "#ifndef _XPLAYERCLIENT\r\n#include \"XWindow_GM_UserCoordination.h\"\r\n#endif\r\n",
    "network GM include"
  )
end

transform(root, "EmperorOfDragons.vcxproj") do |data|
  us_defs = "NDEBUG;WIN32;_WINDOWS;NOUSE_VORBIS;NOUSE_WMA;_XUSEFMOD;_XNOCHECKMEMORYUSAGE;_XESTABLISHEDSERVER;_NEW_TYPE;_ACCLAIM_VERSION;_XENGLISH;_ACCLAIM_RUBICONADSYSTEM;_X_US_EXPANDSERVERLIST;%(PreprocessorDefinitions)"
  data = replace_once(data, us_defs, "_XPLAYERCLIENT;#{us_defs}", "US player definition")
  %w[
    XWindow_GMCommand.cpp
    XWindow_GM_EventManager.cpp
    XWindow_GM_MonsterManage.cpp
    XWindow_GM_SendNoticeMessage.cpp
    XWindow_GM_StatusControl.cpp
    XWindow_GM_UserCoordination.cpp
  ].each do |name|
    old = "    <ClCompile Include=\"XProcess\\#{name}\" />"
    new = "    <ClCompile Include=\"XProcess\\#{name}\">\r\n      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='US_Release|Win32'\">true</ExcludedFromBuild>\r\n    </ClCompile>"
    data = replace_once(data, old, new, "project exclusion #{name}")
  end
  data
end

transform(root, "stdafx.h") do |data|
  marker = "#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_\r\n"
  policy = <<~POLICY.gsub("\n", "\r\n")

    #if defined(_XPLAYERCLIENT) && (defined(_XGMCLIENT) || defined(_XADMINISTRATORMODE))
    #error GM and administrator modes are forbidden in the normal-player target
    #endif
  POLICY
  replace_once(data, marker, marker + policy, "player policy assertion")
end
