# Windows Platform SDK Selection Audit

Date: 2026-07-25  
Scope: normal `US_Release|Win32`; static inspection only. No build,
installation, or binary execution.

## Decision

Use the **Microsoft Platform SDK February 2003 Edition, x86**, with Visual C++
6.0 SP6 for the preservation environment.

Confidence: **95–98%** that this is the correct conservative Platform SDK
generation.

Microsoft's Windows Server 2003 SP1 Platform SDK download page states that the
SP1 SDK does not support VC6 and identifies February 2003 as the last SDK that
works with VC6. February 2003 is therefore the newest officially supported
Platform SDK boundary for the frozen compiler.

Primary Microsoft evidence:

- `https://www.microsoft.com/en-ie/download/details.aspx?id=15656`
- `https://learn.microsoft.com/en-us/windows/win32/winprog/using-the-windows-headers`

The SDK must be acquired and hash-pinned later. This audit does not authorize
downloading or installing it.

## Source requirement

The active normal player uses a conventional Windows 2000/XP-era surface:
core Win32, User32/GDI/common controls, IME, Winsock 2, WinINet, COM/OLE,
CryptoAPI, WinMM/MSACM, Tool Help, and DbgHelp crash reporting.

Two source files record a Windows 2000 target:

```text
XKernel/XApplication/XKernel.cpp: _WIN32_WINNT 0x0500
XKernel/XExceptionReporter/XExceptionReporter.cpp: _WIN32_WINDOWS 0x0500
```

The latter macro is historically odd, but the intended API is
`IsDebuggerPresent`, not a post-XP API. No active normal-US code was found
requiring `_WIN32_WINNT` above `0x0500`.

## Apparent exceptions that do not raise the SDK floor

### PSAPI

`EmperorOfDragons.cpp` contains `GetProcessMemoryInfo`, but its block requires
`_XCHECKMEMORYUSAGE`. Normal US defines `_XNOCHECKMEMORYUSAGE`, not that
enabling symbol. The other `psapi.h` use is Vietnamese-only. PSAPI is therefore
not a normal-US dependency.

### DbgHelp

The tree supplies `XKernel/dbghelp.h` and `Library/dbghelp.lib`. The normal
crash reporter calls `MiniDumpWriteDump`; the reference executable imports
that symbol from `dbghelp.dll`, and the local import library supplies the
required x86 `_MiniDumpWriteDump@28` import.

DbgHelp remains a separately matched header/import-library/runtime component.
It does not justify selecting a newer global Platform SDK.

### DirectX, GM, and anti-cheat

DirectX uses the separately frozen October 2004 SDK/VC6 Extras family. GM code
is a different configuration. Legacy anti-cheat is excluded by policy. None
of these raises the normal-player Platform SDK floor.

## Reference executable evidence

Static PE inspection of the supplied `NineDragons.exe` found:

```text
Machine: Intel 80386 / PE32
Major linker version: 6.0
Minimum OS version: 4.0
Minimum subsystem version: 4.0
DllCharacteristics: 0
```

The linker version agrees with VC6. The PE fields do not prove an installed
header snapshot, but they argue against a later target requirement. Observed
system imports are all Windows 2000/XP-era; no Vista-era DLL or post-XP
platform dependency was found.

## Frozen search order

Headers:

1. October 2004 DirectX headers;
2. deliberate project-local headers, including `XKernel/dbghelp.h`;
3. February 2003 Platform SDK x86 headers;
4. VC6 SP6 headers.

Libraries:

1. fresh internal normal-US outputs;
2. reviewed middleware/import libraries;
3. October 2004 DirectX x86/VC6 Extras libraries;
4. February 2003 Platform SDK x86 libraries;
5. VC6 SP6 CRT/MFC libraries according to each project.

A future include trace and verbose link map must prove the actual winners.

## Acquisition gate

Before a controlled build:

- acquire authentic February 2003 Platform SDK media;
- record URL, filename, size, SHA-256, signature/catalog evidence, and license;
- extract or install only inside the isolated Windows VM;
- hash the used Include and Lib trees;
- exclude Server 2003 SP1, Vista, and current Windows SDK paths;
- prove that VC6 accepts the selected headers without compatibility edits;
- independently match the local DbgHelp header/import/runtime trio.

## Effect on readiness

The Platform SDK **selection question is closed**. Acquisition and hash
verification remain. No missing proprietary normal-player source was found.

The frozen Windows environment is now:

```text
VC6 SP6 x86
Microsoft Platform SDK February 2003 x86
DirectX 9.0c SDK October 2004 + VC6 Extras
Windows XP SP3 isolated VM
```
