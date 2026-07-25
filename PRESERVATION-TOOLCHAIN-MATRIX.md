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
- the Flash MP3 and RAD static libraries contain VC6-era C++ objects and
  `LIBCMT` directives;
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
| CRT exceptions | Preserve MP3/RAD `LIBCMT` directives and original link order; no normalization | Required by historical static objects; see CRT audit |
| Calling convention | Compiler default (`__cdecl`) except APIs explicitly declared `WINAPI`/`__stdcall` | Matches source and decorated imports |
| Structure packing | Compiler default x86 packing unless a source-local pragma changes it | No global alternate packing setting was found |
| Floating point | VC6 defaults; do not add `/fp:fast` or modern equivalents | Avoid changing simulation/render calculations |
| Windows headers/libs | VC6/period Platform SDK set frozen with the VM; record exact file hashes | Avoid silently mixing current Windows SDK libraries |
| DirectX SDK | Official DirectX SDK June 2010, x86 include/lib, version `9.29.1962.1` | Included headers are D3DX version 43; Microsoft maintenance baseline |
| D3DX fidelity route | Validated pre-Feb-2005 `d3dx9dt.lib` followed by `d3dx9.lib` | Reference EXE visibly embeds static debug D3DX |
| D3DX coherent-maintenance route | June 2010 `d3dx9.lib` and approved `d3dx9_43.dll` redistribution | Matches recovered version-43 headers but differs from reference binary |
| Output subsystem | Windows GUI, x86 | Original `/subsystem:windows /machine:I386` |
| Incremental/LTCG | Disabled for the reference Release link | Original final Release does not request LTCG; deterministic full link preferred |

### Important support caveat

The June 2010 SDK installer did not officially target VC6. This matrix is a
deliberate hybrid because:

- the recovered source directly includes version-43 D3DX headers;
- D3DX import libraries expose a C/COM-style binary surface rather than a
  C++ standard-library boundary;
- the source header still contains explicit `_MSC_VER >= 1200` handling;
- a pre-2005 `d3dx9dt.lib` is less coherent with the recovered header set but
  is now evidenced in the historical reference binary, requiring a separate
  fidelity route.

The SDK should be installed only in the isolated image, with include/library
order recorded. Compatibility must be proven statically before a build is
authorized.

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
| RAD/Bink | Replace, license appropriately, or isolate behind a stable C ABI |
| D3DX | Retain Microsoft legacy redist initially, then replace helpers incrementally |
| SpeedTree | Preserve behind matched DLL ABI or replace renderer/data pipeline as a separate project |
| Character handling | Keep MBCS initially; Unicode migration requires protocol/data/UI audit |
| Undefined behavior | Fix only with regression evidence; compiler changes can expose latent assumptions |
| Validation | Compare rendered frames, packet bytes, asset decoding, timing, and state transitions against the preservation reference |

## Frozen dependency placement plan

Nothing is copied into runtime output folders at this stage.

| Dependency | Controlled source location | Future link/runtime destination |
|---|---|---|
| Bink headers/lib | `Library/BinkSDK/` | Link from source tree; approved DLL beside `NineDragons.exe` |
| FMOD headers/lib | `Library/FMod/` | Link from source tree; approved `fmod.dll` beside EXE |
| QHTM header/lib | `Library/QHTM/` | Link from source tree; approved `QHTM.dll` beside EXE |
| XWebPage header/lib | `Library/CWebPage/` | Link from source tree; approved `XWebPage.dll` beside EXE |
| SpeedTree | quarantined `candidate-dependencies/SpeedTreeRT/` | Promote only by reviewed manifest; DLL beside EXE |
| Flash MP3 | quarantined `candidate-dependencies/FlashMP3/` | Future controlled placement where `XFControl` can link it |
| RAD | `Library/radsdk/` | Static link with historical CRT policy preserved |
| DirectX | official SDK installation in frozen VM | SDK x86 library path; redistributable runtime manifest |
| Game data | user-supplied `Data` evidence tree | Separate immutable runtime-data snapshot |

## Path precedence

The controlled environment must print and archive its resolved search order.
The intended precedence is:

1. project/source-local headers, including the recovered `XKernel/d3dx9*.h`;
2. VC6/period Platform SDK headers;
3. explicitly pinned June 2010 DirectX SDK headers where not supplied locally.

Library resolution must be explicit:

1. recovered internal project outputs;
2. approved staged/quarantined middleware inputs;
3. June 2010 DirectX SDK x86 libraries;
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
- acquire the official DirectX maintenance SDK and runtime redistribution.

### Before first controlled compilation

- materialize the frozen `VC6-RESTORATION-PATCHSET.md` specification as an
  IDE-generated, reviewable VC6 unified diff;
- statically validate June 2010 x86 D3DX imports against all client calls;
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
player-owned code + explicitly understood static-CRT middleware exceptions.
DirectX has two frozen variants: validated early static D3DX for historical
fidelity, and June 2010 D3DX 9.43 for coherent maintenance.

**Modernization reference:** `v142` or later only after the preservation
artifact and behavior oracle exist.
