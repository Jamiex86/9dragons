# DirectX SDK Lineage and Link-Input Audit

Date: 2026-07-25  
Scope: `US_Release|Win32`, static inspection only. No SDK was installed and no
compiler, linker, library, or executable was run.

## Conclusion

The preserved source contains DirectX evidence from two different eras:

1. `d3dx9dt.lib` is a legacy, statically linked **debug** D3DX library from the
   early DirectX 9 SDK generation. Microsoft removed it in the February 2005
   SDK transition.
2. The included D3DX headers declare `D3DX_SDK_VERSION 43`, matching the final
   June 2010 DirectX SDK generation. A converted project also records an
   explicit `DXSDK (June 2010)\Include` path.

Therefore `d3dx9dt.lib` is not a valid June 2010 SDK requirement. It is an
inherited pre-February-2005 build dependency. Static inspection of the supplied
reference `NineDragons.exe` subsequently found debug D3DX assertions and
`nt32_chk\...\d3dx9` source paths in its protected payload, demonstrating that
this old static debug implementation was genuinely embedded in at least that
shipped derivative.

Recommended coherent-maintenance baseline: **DirectX SDK (June 2010), x86
libraries, version 9.29.1962.1**, using `d3dx9.lib` and the matching D3DX 9.43
runtime. The fidelity baseline separately requires a validated early
`d3dx9dt.lib`.

Do not obtain an arbitrary `d3dx9dt.lib` merely to satisfy the project list.
For historical fidelity, locate and validate the correct pre-February-2005
variant. For a coherent version-43 maintenance build, omit it deliberately and
record that this differs from the observed reference binary.

## Confirmed source evidence

### Included D3DX generation

`XKernel/d3dx9core.h` contains:

```cpp
#define D3DX_VERSION 0x0902
#define D3DX_SDK_VERSION 43
```

`D3DXCheckVersion` uses this header constant to verify that the application and
D3DX runtime generations agree.

The converted projects also retain this developer-machine path in the
`JP_Debug` configuration:

```text
D:\ND2_Program\trunk\Sdk\zerodin\ExtrasSDK\DXSDK(June 2010)\Include
```

Although that absolute path applies only to one converted configuration, it
corroborates the origin of the copied version-43 headers used from `XKernel`.

### Legacy library entry

The original VC6 `EmperorOfDragons.dsp` names:

```text
dxguid.lib d3d9.lib d3dx9dt.lib d3dx9.lib dsound.lib
dinput8.lib dxerr9.lib d3dxof.lib
```

`d3dx9dt.lib` appears 67 times in that project, spanning normal, debug,
release-debug, GM, test-server, and regional configurations. It is always
placed immediately before `d3dx9.lib`.

This means:

- it is not an Acclaim/US feature;
- it is not evidence of a special player subsystem;
- it is a globally inherited project template/link-list entry;
- its presence predates `.vcxproj` conversion.

## What `d3dx9dt.lib` was

Historical DirectX 9 documentation describes:

- `d3dx9.lib`: release D3DX implementation/import library, depending on SDK
  generation;
- `d3dx9d.lib`: debug DLL import library in later SDKs;
- `d3dx9dt.lib`: statically linked debug/test D3DX implementation in early
  SDKs.

The February 2005 DirectX SDK moved D3DX to side-by-side DLL deployment and
removed `d3dx9dt.lib`, directing developers toward `d3dx9d.lib` for debug use.

Sources:

- Microsoft legacy SDK:
  `https://www.microsoft.com/en-us/download/details.aspx?id=6812`
- Historical February 2005 release-note reproduction:
  `https://archives.developerzen.com/2005/02/10/directx-february-2005-release/`

## Why linking both names matters

Microsoft COFF libraries are searched to resolve symbols as they are needed.
With `d3dx9dt.lib` ordered first, an old SDK installation could satisfy common
D3DX calls from the static debug library before `d3dx9.lib` was considered.
That could:

- embed debug D3DX code in `NineDragons.exe`;
- increase executable size;
- introduce a further static CRT/compiler-era dependency;
- cause only newer APIs to fall through to the later library;
- produce behavior unlike a normal release D3DX runtime.

The fact that the link list says “Release” does not make `d3dx9dt.lib` a
release library.

## D3DX usage in the player

The source uses conventional DirectX 9-era D3DX facilities:

- matrices, vectors, planes, and quaternions;
- texture creation/loading and image inspection;
- sprite and font creation;
- effect and shader assembly;
- primitive mesh creation;
- surface copying/saving;
- projection and render-to-surface helpers.

No source-level evidence was found that deliberately requires the old static
debug/test implementation. These are normal D3DX APIs intended to be supplied
by the header-matched `d3dx9.lib`.

## SDK component classification

| Library | Source | Preservation source | Runtime consequence |
|---|---|---|---|
| `d3d9.lib` | Windows/DirectX SDK import library | June 2010/compatible Windows SDK x86 | Imports system `d3d9.dll` |
| `d3dx9.lib` | Legacy DirectX SDK import library | June 2010 x86 | Requires matching side-by-side `d3dx9_43.dll` |
| `d3dx9dt.lib` | Pre-Feb-2005 static debug D3DX | **Do not restore for v43 baseline** | Would embed obsolete debug D3DX |
| `dxguid.lib` | GUID definitions | June 2010 x86 | Static GUID data |
| `dxerr9.lib` | Legacy diagnostic helper | June 2010 x86 | Static/helper dependency; not in modern Windows SDK |
| `d3dxof.lib` | Legacy `.x` file helper | June 2010 x86 | Legacy DirectX SDK dependency |
| `dinput8.lib` | DirectInput import library | June 2010/Windows SDK x86 | Imports system `dinput8.dll` |
| `dsound.lib` | DirectSound import library | June 2010/Windows SDK x86 | Imports system `dsound.dll` |

## Runtime requirement

Using June 2010 `d3dx9.lib` normally produces an import on
`d3dx9_43.dll`. That DLL is not present in the supplied source, working tree,
or inspected `Data` directory. This is expected: D3DX is a redistributable
runtime component, not source.

The future runtime package must use Microsoft's approved legacy DirectX
redistribution route or a documented application-local package. The SDK itself
should not be treated as the end-user runtime.

Microsoft currently describes the June 2010 SDK as end-of-life and intended
for maintaining older software:

`https://www.microsoft.com/en-us/download/details.aspx?id=6812`

## Preservation versus historical archaeology

Two different questions must remain separate:

### Current recovered-source preservation

Use June 2010 headers and x86 libraries because:

- the copied source headers identify D3DX SDK version 43;
- the converted lineage explicitly names June 2010;
- it is the final coherent legacy SDK set;
- Microsoft still identifies it as the maintenance SDK for old DirectX code.

For this route, `d3dx9dt.lib` is deliberately excluded as a historical
compatibility dependency, not dismissed as unused.

### Reproducing an original pre-2005 build environment

An early SDK containing `d3dx9dt.lib` may help explain the oldest VC6 project
history, but it does not match the current version-43 headers. It belongs in a
separate archival experiment, not the primary recovered-player toolchain.

### Located historical package identities

The safest fidelity candidate is now the **October 2004 DirectX 9.0c SDK plus
its separately distributed Extras package**. Contemporary reports identify the
Extras package as carrying the Visual C++ 6-compatible libraries. The main SDK
identity is independently recorded as:

| Field | October 2004 main SDK |
|---|---|
| Filename | `dxsdk_oct2004.exe` |
| Size | `229,290,240` bytes |
| SHA-1 | `8097bb69676a20e55a98a67d960a8b6edc89dff9` |

The next-best comparison candidate is:

| Field | December 2004 main SDK |
|---|---|
| Filename | `dxsdk_dec2004.exe` |
| Size | `233,784,600` bytes |
| SHA-1 | `69f7a521a787c3a914c2dee9489a29235caa7d15` |
| SHA-256 | `77e4229a8b77768c3e835e5077fa8bfc732d6a1902d040247ba22cdce46cb428` |

The historical filenames `dxsdk_oct2004_extras.exe` and
`dxsdk_dec2004_extras.exe` are confirmed, but no independently corroborated
hash for the October Extras package has yet been located. A community-hosted
copy without a known-good hash is therefore **not approved for promotion**.
The original Microsoft Download Center identity for the October package is
family ID `d6f237de-a6ee-4ded-8bb6-139536162eb8`; a contemporary link also
records Microsoft link token `QYV4F3C`. Collector documentation reports the
file as approximately `65.252 MB`, but this is not yet an exact byte count.
Direct Internet Archive CDX queries for the known filename and Microsoft family
page returned no archived payload record, so the Wayback Machine currently
does not provide a provenance chain for the executable.
Any acquired installer must be treated only as an archive: quarantine it,
verify its whole-file hashes, extract it statically without executing it, and
inventory the resulting x86 libraries.

Promotion of an extracted `d3dx9dt.lib` requires all of the following:

1. the containing main SDK or Extras package matches an independently recorded
   Microsoft filename, size, and hash;
2. the library is x86 COFF and is compatible with the VC6 linker;
3. its symbol set covers the D3DX calls visible in the source and reference
   executable;
4. its embedded strings, object-member timestamps, and D3DX build paths are
   consistent with the reference executable's `nt32_chk` evidence;
5. the exact package and extracted-library hashes are recorded in quarantine.

A community-preserved candidate is documented in
`DIRECTX-D3DX9DT-CANDIDATE-AUDIT.md`. Its exact `nt32_chk` `createmesh.cpp`
build path matches the supplied reference executable, and uniform 29 September
2004 member timestamps identify the October release generation at 97–99%
confidence. However, its required members contain VC7-era security-cookie and
exception-support references. It is therefore the standard October lineage
comparison oracle, not an approved substitute for the still-missing
`Extras\D3dx9 Visual Studio 6` replacement.

## Gate before a project patch

No dependency was removed in this audit. Before authoring the future patch:

1. acquire the official June 2010 SDK through Microsoft;
2. hash and inventory its x86 `d3dx9.lib`, `dxerr9.lib`, `d3dxof.lib`,
   `dxguid.lib`, `dinput8.lib`, and `dsound.lib`;
3. statically verify that all non-inline D3DX calls used by the player are
   represented by the version-43 import library;
4. record `d3dx9_43.dll` in the runtime manifest;
5. create a reviewable, explicitly labelled coherent-maintenance patch that
   removes only `d3dx9dt.lib` from `US_Release`, while the fidelity route
   retains the original order;
6. leave other regional/historical configurations unchanged unless separately
   audited.

Current status:

**Two routes are required: historical fidelity retains a validated early
`d3dx9dt.lib`; the coherent June 2010 maintenance route excludes it and uses
D3DX 9.43.**
