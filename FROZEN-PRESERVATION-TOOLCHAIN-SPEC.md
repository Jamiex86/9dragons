# Frozen Normal-US Preservation Toolchain Specification

Date: 2026-07-25  
Scope: planning and static evidence only; no compilation, linking, installation,
or binary execution.

## Outcome

The compiler and DirectX generation are now frozen with high confidence:

```text
Microsoft Visual C++ 6.0 SP6, x86
original VC6 .dsp/.dsw projects
US_Release normal-player configuration
October 2004 DirectX 9.0c SDK headers and x86 libraries
October 2004 Extras/D3dx9 Visual Studio 6 replacement libraries
D3DX header ABI version 22
```

Confidence in this family: **98–99%**.

This does not mean a build is authorized. Static API closure selects the
Microsoft Platform SDK February 2003 Edition, x86. Its authentic media and the
actual VC6 tool executable hashes are not yet available and remain
base-toolchain acquisition items.

## Frozen compiler and linker behavior

| Property | Required preservation value | Evidence |
|---|---|---|
| Compiler | VC6 SP6 x86 `cl.exe` | Original `.dsp` lineage and VC6-era binary ABI |
| Linker/librarian | `link.exe` and `lib.exe` from the same VC6 SP6 image | Avoid cross-generation COFF/default-library behavior |
| Target | Win32/PE32/I386 GUI | `/subsystem:windows /machine:I386` |
| C++ mode | Native VC6 language mode | No later `/std:` model applies |
| Character set | MBCS/ANSI | `_MBCS` in every normal-US project |
| Player CRT | DLL CRT, `/MD` | Recorded in normal `US_Release` projects |
| Exceptions | `/GX` | Recorded in normal `US_Release` |
| Optimization | `/O2` | Recorded in normal `US_Release` |
| Warning level | `/W3` | Recorded in normal `US_Release` |
| Compile symbols | `NDEBUG`, `WIN32`, `_WINDOWS`, `_MBCS`, regional and feature symbols exactly per project | Do not infer symbols from GM/debug variants |
| Main regional symbols | `_ACCLAIM_VERSION`, `_XENGLISH`, `_ACCLAIM_RUBICONADSYSTEM`, `_X_US_EXPANDSERVERLIST` | Main `US_Release` project |
| Kernel regional symbols | `_ACCLAIM_VERSION`, `_XENGLISH` | `XKernel` normal-US project |
| XFControl symbols | `_ACCLAIM_VERSION`, `_XENGLISH`, `FLASHMP3`, `SOUND`, `NOEDITTEXT`, `FIXEDSIZE`, and recorded library symbols | `XFControl` normal-US project |
| GM symbol | Absent | `_XGMCLIENT` belongs to the separate GM configuration |
| Anti-cheat | Disabled by the separately reviewed removal/guard policy | User-selected future replacement; do not restore legacy products |

The original main project contains a contradictory hand-edited link line that
both names `msvcrt.lib`/`libcmt.lib` and suppresses a quoted string containing
both. That string must be resolved by a later isolated VC6 argument-parsing
experiment; it must not be normalized speculatively. The module-level CRT
decisions already established remain: normal player and XFControl use `/MD`,
with `LIBCMT` suppressed where the reviewed XFControl policy requires it.

## Frozen DirectX family

The preservation route must use exactly one D3DX generation:

| Component | Frozen choice |
|---|---|
| Main SDK | DirectX 9.0c SDK, October 2004 |
| D3DX ABI | `D3DX_SDK_VERSION 22` |
| D3DX library variant | Microsoft's October 2004 VC6 Extras replacements |
| Link order | `d3dx9dt.lib` before `d3dx9.lib`, matching the original project |
| Architecture | x86 libraries only |
| June 2010/v43 artifacts | Prohibited from the preservation search paths |

The player actively dereferences `ID3DXEffect`. Its vtable differs between
v22 and v43, so mixed headers and libraries may link but corrupt dispatch at
runtime. This makes the coherent family a correctness requirement, not merely
a historical preference.

The quarantined coherent candidate is:

```text
recovery/emperor-of-dragons-normal-client-overlay/
  candidate-dependencies/DirectX9-October2004-untrusted/
  Coherent-VC6-October2004/
```

The eight DirectX libraries explicitly named by the player are already present
and hash-pinned in `DIRECTX-OCTOBER-2004-COHERENT-SET-STATUS.md`. The complete
ten-header D3DX family is also present. No file has been promoted into the
active source or runtime tree.

## Include and library resolution

Future preservation resolution must be recorded, deterministic, and checked
from generated evidence:

1. October 2004 DirectX headers;
2. project headers, with the local v43 D3DX family unable to resolve;
3. one pinned period-compatible Windows Platform SDK;
4. VC6 SP6 headers.

The corresponding library resolution is:

1. freshly produced internal normal-US libraries;
2. reviewed, hash-pinned middleware;
3. October 2004 x86 DirectX libraries with VC6 Extras overrides;
4. pinned Platform SDK/VC6 system libraries.

Required future static/build-record proofs are an include trace or preprocessed
output showing `D3DX_SDK_VERSION 22`, a verbose linker library-selection log,
and a map file. Merely setting `INCLUDE` and `LIB` is insufficient.

## Windows Platform SDK status

The Platform SDK generation is frozen as **Microsoft Platform SDK February
2003 Edition, x86**. The player explicitly links conventional Win32 libraries
including:

```text
kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32
ole32 oleaut32 uuid odbc32 odbccp32 winmm ws2_32
imagehlp imm32 msacm32
```

Source inspection found a Windows 2000 API target (`_WIN32_WINNT 0x0500`) and
no active normal-US requirement above it. Microsoft identifies February 2003
as the last Platform SDK edition compatible with VC6. Therefore:

- February 2003 x86 headers/libs are the preservation Platform SDK;
- VC6's bundled headers/libs remain behind it in the controlled search order;
- current Windows SDK headers or libraries are prohibited on the preservation
  route;
- the staged `Library/dbghelp.lib` must be evaluated with the chosen
  `dbghelp.dll` runtime rather than treated as proof of an SDK version.

This is an acquisition/provenance gate, not evidence that proprietary source
is missing.

## Separate modernization route

A modernization experiment may use `v142` or later and the complete June 2010
D3DX v43 family (`d3dx9.lib` plus `d3dx9_43.dll`, no `d3dx9dt.lib`). It must
have separate project files, output paths, manifests, and test results.

It cannot validate preservation correctness because its compiler CRT, C++ ABI,
undefined-behavior handling, and D3DX effect ABI differ.

## Remaining pre-build gates

1. Acquire and hash a legitimate VC6 SP6 tool image without altering the
   archive or host.
2. Acquire and hash the Microsoft Platform SDK February 2003 Edition, x86.
3. Resolve the original quoted `/NODEFAULTLIB` behavior using the exact VC6
   linker in an isolated, later-authorized experiment.
4. Promote the ten October D3DX headers and matched libraries only through the
   hash-pinned placement manifest.
5. Generate the VC6 project exclusions through the VC6 IDE and review the
   resulting textual `.dsp` diff.
6. Re-run full source/header/link-input closure after those planned changes.

Until these gates pass, the correct status is:

```text
compiler generation: resolved
linker generation: resolved
DirectX/D3DX generation: resolved
exact tool hashes: unresolved acquisition
Platform SDK generation: resolved; authentic media/hash acquisition pending
permission to build: not granted
```
