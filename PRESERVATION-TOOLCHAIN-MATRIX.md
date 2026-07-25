# Frozen Preservation Toolchain Matrix

Date: 2026-07-25  
Scope: planning only. This document does not authorize installation,
compilation, linking, or execution.

## Decision

The first controlled preservation attempt should use the **Visual C++ 6
generation**, not `v142`.

The existing `v142` projects are useful conversion records, but they are not a
safe behavioral baseline:

- the original projects use VC6 `/MD`, meaning the historical `MSVCRT` model;
- `v142 /MD` uses the modern Universal CRT plus `VCRUNTIME140`, not the same
  runtime;
- the Flash MP3 static library contains VC6-era objects and `LIBCMT`
  directives;
- the surviving runtime DLLs include VC6-era dependencies such as
  `MSVCP60.dll`;
- C++ layout, exception, standard-library, floating-point optimization, and
  undefined-behavior handling can differ materially under `v142`.

Therefore a successful `v142` link would be a modernization experiment, not
the preservation reference.

## Matrix A — primary preservation baseline

| Layer | Frozen choice | Evidence / reason |
|---|---|---|
| Host | Isolated offline Windows XP SP3-era 32-bit VM snapshot | Matches the client generation and legacy SDK support; reproducible and disposable |
| IDE/project lineage | Visual Studio 6.0 / `.dsp` and `.dsw` | Original authoritative project format |
| C/C++ compiler | Microsoft Visual C++ 6.0 with Service Pack 6, x86 `cl.exe` | Closest available compiler generation to proprietary static middleware and runtime DLLs |
| Linker/librarian | VC6 SP6 x86 `link.exe` and `lib.exe` from the same installation | Preserves command parsing, COFF handling, and default-library behavior |
| Resource tools | VC6 `rc.exe`, MIDL and supporting bin tools from the same frozen image | Avoid cross-version resource differences |
| Architecture | Win32/x86 only | Every project and supplied binary is x86/PE32 |
| Configuration | `US_Release`; normal player only | Authoritative target; GM is excluded |
| Character model | MBCS/ANSI, not Unicode | Original `_MBCS` and converted `MultiByte` settings |
| C++ dialect | VC6 language mode; no modern `/std:` switch | Original compiler predates standard-selection switches |
| Exceptions | `/GX` as recorded | Original `.dsp` setting |
| Optimization | `/O2`, `/Ob1`-equivalent project defaults, function/data settings exactly as emitted by VC6 project | Preserve original Release intent; do not add LTCG |
| Debug info | `/Zi` on compilation where the US Release project records it; final EXE debug generation remains off | Matches project rather than assuming “Release means no symbols” |
| CRT for player-owned code | `/MD` | Explicit in all four primary VC6 US Release projects |
| CRT exceptions | Apply the reviewed Flash MP3/XFControl `LIBCMT` policy; do not normalize other CRT directives speculatively | Required by historical static objects; see CRT audit |
| Calling convention | Compiler default (`__cdecl`) except APIs explicitly declared `WINAPI`/`__stdcall` | Matches source and decorated imports |
| Structure packing | Compiler default x86 packing unless a source-local pragma changes it | No global alternate packing setting was found |
| Floating point | VC6 defaults; do not add `/fp:fast` or modern equivalents | Avoid changing simulation/render calculations |
| Windows headers/libs | Microsoft Platform SDK February 2003 Edition, x86 | Microsoft's last VC6-compatible Platform SDK; covers the active Windows 2000-era API target |
| DirectX SDK | Microsoft DirectX 9.0c SDK (October 2004), x86 headers/libs, with Microsoft's `Extras/D3dx9 Visual Studio 6` replacement libraries | Matches the reference EXE's static D3DX lineage and the VC6 compiler |
| D3DX preservation route | Complete October 2004 D3DX v22 header family; VC6 Extras `d3dx9dt.lib` followed by matched `d3dx9.lib` | The used API is covered and the matched header/library family avoids an active `ID3DXEffect` vtable mismatch |
| D3DX modernization route | Complete June 2010 v43 headers, June 2010 `d3dx9.lib`, and legitimate `d3dx9_43.dll`; no `d3dx9dt.lib` | Separate maintenance experiment, not a preservation variant |
| Output subsystem | Windows GUI, x86 | Original `/subsystem:windows /machine:I386` |
| Incremental/LTCG | Disabled for the reference Release link | Original final Release does not request LTCG; deterministic full link preferred |

### DirectX coherence requirement

The preservation route is no longer a June-2010/October-2004 hybrid.
`D3DX-V22-V43-USED-ABI-DECISION.md` established that the player uses methods
whose `ID3DXEffect` vtable positions differ between v22 and v43. A mixed build
could therefore link successfully and still call the wrong methods at runtime.

The frozen preservation family is:

- all ten October 2004 `d3dx9*.h` headers, declaring
  `D3DX_SDK_VERSION 22`;
- the October 2004 x86 DirectX libraries;
- Microsoft's October 2004 `Extras/D3dx9 Visual Studio 6` replacements for
  `d3dx9.lib`, `d3dx9d.lib`, and `d3dx9dt.lib`;
- `d3dx9dt.lib` before `d3dx9.lib`, as recorded by the original project.

The quarantined coherent set contains 184 main-SDK include files and 50 main
x86 library files with the three VC6 Extras replacements applied. It remains
quarantined until provenance and placement review; no installer needs to be
run to use the statically extracted files in a future isolated VM.

## Matrix B — converted-project diagnostic baseline

| Layer | Choice | Status |
|---|---|---|
| IDE | Visual Studio 2019 | Implied by `v142`; solution header itself still says VS15 from an earlier conversion |
| Toolset | MSVC `v142`, x86 | Explicit in converted projects |
| Project format | `.sln/.vcxproj` | Useful for dependency/file closure |
| Language standard | Toolset default; do not opt into C++17/20 | No `LanguageStandard` property is recorded |
| Character model | MultiByte | Explicit |
| Windows SDK | Must be pinned explicitly before use | `WindowsTargetPlatformVersion` is absent |
| CRT | Modern `/MD` UCRT/VCRUNTIME | **Not ABI-equivalent to VC6 `/MD`** |
| Purpose | Diagnostics, source-portability inventory, later modernization | Not the preservation reference |

No `v142` compilation should occur until a dedicated translation policy has
removed assumptions about legacy `MSVCRT`/`LIBCMT`, compiler-generated C++
symbols, and old middleware.

## Matrix C — later modernization option

This is a separate product-engineering route, not “the same client with a
newer compiler.”

| Area | Modernization requirement |
|---|---|
| Compiler | A currently supported MSVC x86 toolset, pinned by exact version |
| Runtime | One deliberate CRT ownership model per module |
| Flash MP3 | Replace decoder or isolate it in a VC6-built compatibility DLL |
| RAD/Bink | Keep excluded unless a future product requirement deliberately restores video/advertising behavior |
| D3DX | Retain Microsoft legacy redist initially, then replace helpers incrementally |
| SpeedTree | Preserve behind matched DLL ABI or replace renderer/data pipeline as a separate project |
| Character handling | Keep MBCS initially; Unicode migration requires protocol/data/UI audit |
| Undefined behavior | Fix only with regression evidence; compiler changes can expose latent assumptions |
| Validation | Compare rendered frames, packet bytes, asset decoding, timing, and state transitions against the preservation reference |

## Frozen dependency placement plan

Nothing is copied into runtime output folders at this stage.

| Dependency | Controlled source location | Future link/runtime destination |
|---|---|---|
| Bink/RAD | Historical source evidence only | Excluded from the normal US player; feature guards are inactive |
| FMOD headers/lib | `Library/FMod/` | Link from source tree; approved `fmod.dll` beside EXE |
| QHTM header/lib | `Library/QHTM/` | Link from source tree; approved `QHTM.dll` beside EXE |
| XWebPage header/lib | `Library/CWebPage/` | Link from source tree; approved `XWebPage.dll` beside EXE |
| SpeedTree | quarantined `candidate-dependencies/SpeedTreeRT/` | Promote only by reviewed manifest; DLL beside EXE |
| Flash MP3 | quarantined `candidate-dependencies/FlashMP3/` | Future controlled placement where `XFControl` can link it |
| DirectX preservation | quarantined October 2004 coherent VC6 set | Promote complete v22 headers and the matched x86 library family as one hash-pinned unit |
| DirectX modernization | official June 2010 SDK evidence | Separate v43 experiment only; never place on the preservation search path |
| Game data | user-supplied `Data` evidence tree | Separate immutable runtime-data snapshot |

## Path precedence

The controlled environment must print and archive its resolved search order.
The intended preservation precedence is:

1. the promoted October 2004 DirectX include directory;
2. project/source-local headers after the seven local v43 D3DX headers have
   been removed from resolution or replaced by their hash-pinned v22 forms;
3. the frozen Platform SDK headers;
4. VC6 headers.

The preprocessor must prove that every resolved `d3dx9*.h` belongs to the
October family and that `D3DX_SDK_VERSION` is 22. Path order alone is not
accepted as proof because quoted includes may prefer the including file's
directory.

Library resolution must be explicit:

1. recovered internal project outputs;
2. approved staged/quarantined middleware inputs;
3. October 2004 DirectX SDK x86 libraries with the VC6 Extras D3DX overrides;
4. frozen VC6/Platform SDK system libraries.

No user-global Visual Studio library directories, current Windows SDK paths, or
unversioned `PATH` entries should influence the reference attempt.

## Evidence capture required before compilation

The VM/toolchain evidence bundle must contain:

- installer filenames, source URLs, licenses, and SHA-256 hashes;
- `cl.exe`, `link.exe`, `lib.exe`, `rc.exe`, and `nmake.exe` version output and
  file hashes;
- every effective `INCLUDE`, `LIB`, and `PATH` entry;
- a recursive hash manifest of the relevant SDK headers and libraries;
- the exact project/patch commit;
- hashes and provenance manifests for all middleware;
- a clean-room command transcript template;
- an output quarantine path that cannot overwrite reference-client files.

## Gates

### Before dependency promotion

- legal/provenance decision for SpeedTree and Flash MP3;
- independently hash all user-supplied runtime DLLs;
- preserve the October 2004 main-SDK and VC6-Extras provenance records;
- keep June 2010 artifacts outside the preservation environment.

### Before first controlled compilation

- materialize the frozen `VC6-RESTORATION-PATCHSET.md` specification as an
  IDE-generated, reviewable VC6 unified diff;
- promote the complete hash-pinned October v22 header family and matched VC6
  library set;
- define a preprocessor/include-trace check proving version 22 resolution;
- resolve exact VC6 `/NODEFAULTLIB` argument behavior;
- freeze and hash the VM.

### Before first controlled link

- build-order and internal-library outputs must be isolated;
- linker response/command file must be archived before invocation;
- no missing symbol may be “fixed” by adding an unproven library;
- no `/FORCE`, ignored duplicate symbol, or blanket `/NODEFAULTLIB` workaround
  is permitted.

### Before claiming a working client

A linked executable is only an artifact. Separate proof is required for:

- stable local startup;
- renderer and login/UI behavior;
- complete asset loading;
- protocol-compatible connection and authentication;
- correct world entry and gameplay behavior.

## Frozen recommendation

**Preservation reference:** VC6 SP6 x86 + original VC6 projects + `/MD`
player-owned code + explicitly understood static-CRT middleware exceptions +
the complete October 2004 D3DX v22 headers and matched October VC6 libraries.
There is one preservation DirectX route, not two.

**Modernization reference:** `v142` or later with a coherent June 2010 v43
D3DX family only after the preservation artifact and behavior oracle exist.
