#!/usr/bin/env ruby
# Disables legacy third-party and built-in anti-cheat selection for US player.

root = File.expand_path(ARGV.fetch(0))

selection_files = %w[
  EmperorOfDragons.cpp
  Network/packet_for_ctrl_gms.h
  Network/XNetwork.cpp
  XProcess/XProc_MainGame.cpp
  XKernel/XApplication/X3DApp.cpp
  XKernel/XApplication/XKernel.cpp
  XProcess/XProc_SelectCharacter.cpp
  DataStructure/Notice/XNoticeProcess.cpp
  XProcess/XWindow_Login.cpp
]

selection = "#if !defined(_XDWDEBUG) && !defined(_XADMINISTRATORMODE) && !defined(_DEBUG) && !defined(_XGMCLIENT)"
disabled = selection + " && !defined(_XPLAYER_NO_LEGACY_ANTICHEAT)"

changed_count = 0
selection_files.each do |relative|
  path = File.join(root, relative)
  data = File.binread(path)
  if data.include?(disabled)
    count = data.scan(disabled).length
  else
    count = data.scan(selection).length
    raise "#{relative}: no selector found" if count == 0
    data = data.gsub(selection, disabled)
  end
  data = data.gsub(/(^[^\r\n]*_XPLAYER_NO_LEGACY_ANTICHEAT[^\r\n]*?)[ \t]+(\r?\n)/, "\\1\\2")
  File.binwrite(path, data)
  changed_count += count
end
raise "expected 13 selector edits, found #{changed_count}" unless changed_count == 13

project_path = File.join(root, "EmperorOfDragons.vcxproj")
project = File.binread(project_path)
marker = "_XPLAYERCLIENT;NDEBUG;"
raise "player definition marker" unless project.scan(marker).length == 1
project.sub!(marker, "_XPLAYERCLIENT;_XPLAYER_NO_LEGACY_ANTICHEAT;NDEBUG;")

us_pattern = /(<ItemDefinitionGroup Condition="'\$\(Configuration\)\|\$\(Platform\)'=='US_Release\|Win32'">.*?<\/ItemDefinitionGroup>)/m
match = project.match(us_pattern)
raise "US block" unless match
us_block = match[1]
clean_block = us_block.gsub("./HackDetector/HackShield;./HackDetector/GameGuard;", "")
                      .gsub(";./HackDetector/XTrap", "")
raise "US include paths unchanged" if clean_block == us_block
project.sub!(us_block, clean_block)

xtrap_entry = "    <ClCompile Include=\"Network\\XNetwork_XTrap.cpp\" />"
xtrap_replacement = <<~XML.gsub("\n", "\r\n").chomp
    <ClCompile Include="Network\\XNetwork_XTrap.cpp">
      <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='US_Release|Win32'">true</ExcludedFromBuild>
    </ClCompile>
XML
raise "XTrap project entry" unless project.scan(xtrap_entry).length == 1
project.sub!(xtrap_entry, xtrap_replacement)
File.binwrite(project_path, project)

xkernel_project_path = File.join(root, "XKernel/XKernel.vcxproj")
xkernel_project = File.binread(xkernel_project_path)
xkernel_us_pattern = /(<ItemDefinitionGroup Condition="'\$\(Configuration\)\|\$\(Platform\)'=='US_Release\|Win32'">.*?<\/ItemDefinitionGroup>)/m
xkernel_match = xkernel_project.match(xkernel_us_pattern)
raise "XKernel US block" unless xkernel_match
xkernel_us = xkernel_match[1]
xkernel_clean = xkernel_us.gsub("../HackDetector/HackShield;../HackDetector/GameGuard;../HackDetector/XTrap;", "")
                         .sub("NDEBUG;WIN32;_LIB;", "_XPLAYER_NO_LEGACY_ANTICHEAT;NDEBUG;WIN32;_LIB;")
raise "XKernel US block unchanged" if xkernel_clean == xkernel_us
xkernel_project.sub!(xkernel_us, xkernel_clean)
File.binwrite(xkernel_project_path, xkernel_project)

stdafx_path = File.join(root, "StdAfx.h")
stdafx = File.binread(stdafx_path)
policy_end = "#error GM and administrator modes are forbidden in the normal-player target\r\n#endif\r\n"
extra_policy = <<~POLICY.gsub("\n", "\r\n")

  #if defined(_XPLAYER_NO_LEGACY_ANTICHEAT) && (defined(_XUSE_GAMEGUARD) || defined(_XUSE_HACKSHEILD) || defined(_XUSE_XTRAP) || defined(_XUSEAPEXMODULE) || defined(_XDEF_MEMORYHACKDEFENCE) || defined(_XDEF_DETECTSPEEDHACK))
  #error Legacy anti-cheat is forbidden in the normal-player target
  #endif
POLICY
raise "policy marker" unless stdafx.scan(policy_end).length == 1
File.binwrite(stdafx_path, stdafx.sub(policy_end, policy_end + extra_policy))
