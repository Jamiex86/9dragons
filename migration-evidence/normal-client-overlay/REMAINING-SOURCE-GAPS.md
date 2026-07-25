# Remaining source and development-input gaps

## Revised scope

Per project direction, the following are not treated as current blockers:

- runtime DLLs that will be supplied from an existing client;
- the complete game-data directory, which is already available;
- Heavenfall/server interoperability, which belongs to a separate effort;
- original anti-cheat SDKs, because all legacy anti-cheat integrations will be
  stripped and replaced in a future phase.

This document therefore answers only: **what is still absent or inconsistent
for the normal US player source and its development inputs?**

## Current answer

After the 26-file recovery overlay, the remaining items divide into four groups:

| Item | Classification | US player status |
|---|---|---|
| `NMClass/ADBalloon.h` | Stale duplicate project entry | Remove from project; not missing functionality |
| `res/NineDragons.exe.manifest` | Missing packaging file | Recovered from GitHub into overlay |
| 12 `XWindow_GM*` files | Available older structural source | Policy/integration decision, not unavailable source |
| Three master/disciple headers and five command IDs | Older feature removed in later tree | Guarded by review-only patch; do not restore feature |
| `Library/US/XKernel.lib` | Generated internal output | All project source present |
| `Library/US/XFControl.lib` | Generated import library | All project source present; accompanies `XFControl.dll` |
| `SpeedTreeRT.h` | Compatible proprietary SDK candidate recovered | Technically closed; provenance/licensing gate remains |
| `SpeedTreeRT.lib` | Exact 146/146 import/export candidate recovered | Technically closed; provenance/licensing gate remains |

Consequently, there is currently **no confirmed unavailable third-party
development file required by the normal renderer**. SpeedTreeRT has a
high-confidence technical candidate set, while lawful-use/provenance remains a
non-technical gate. GM handling remains a source architecture decision rather
than a file-discovery problem.

## `NMClass/ADBalloon.h`

### What it is

ADBalloon is a Korean Nexon advertising component. It opens an embedded
Internet Explorer/COM browser window displaying a Nexon exit advertisement.
Its public interface is:

```cpp
struct ADBallonParam;
int ShowADBalloon(const ADBallonParam& adp);
```

The call is made from `XKernel/XApplication/X3DApp.cpp` only under:

```cpp
_XDEF_NEXONENDINGADVERTISE_071213_KERYGMA
```

That macro is not defined by `US_Release|Win32`.

### Why the project says it is missing

The main client project retains a stale entry for:

```text
NMClass/ADBalloon.h
```

GitHub's version of that header merely wraps the same declaration in the Nexon
feature macro. The archive already contains the actual header and complete
implementation here:

```text
NexonADBalloonLib/ADBalloon.h
NexonADBalloonLib/ADBalloon.cpp
NexonADBalloonLib/NexonADBalloonLib.vcxproj
```

`X3DApp.cpp` includes the real header directly using:

```text
../../NexonADBalloonLib/ADBalloon.h
```

The advertising library project has only Korean configurations and emits only
`Library/KR/NexonADBalloonLib*.lib`. The US executable neither defines the
feature macro nor links the library.

### Disposition

**Remove the stale `NMClass/ADBalloon.h` entry from the normal client project.**
Do not recover it into the US player overlay. No US functionality is lost.

Confidence: **99%**.

## Executable manifest

The archive resource script and converted project both reference:

```text
res/NineDragons.exe.manifest
```

GitHub contains the matching file. It declares:

- x86 processor architecture;
- executable identity `NineDragons`;
- `requireAdministrator`;
- `uiAccess="false"`.

It has now been added to the recovery overlay with provenance and hash tracking.
Whether the eventual modernized client should continue requiring administrator
rights is a separate design review; the recovered manifest represents the
historical project intent.

Disposition: **resolved**.

## Internal engine outputs

### `Library/US/XKernel.lib`

`XKernel/XKernel.vcxproj` contains 144 listed source/header/resource items.
Every listed item exists. Its US Release output is explicitly:

```text
../Library/US/XKernel.lib
```

This is not missing proprietary source. It is an absent generated static
library. Compiler compatibility and ABI remain future validation questions.

Disposition: **source-complete internal output**.

### `Library/US/XFControl.lib`

`XFControl/XFControl.vcxproj` contains 68 listed source/header/resource items.
Every listed item exists.

XFControl is a DLL project. Its US Release outputs are:

```text
Game/US/XFControl.dll
Library/US/XFControl.lib
```

The `.lib` is the import library consumed by the main executable. Its local
non-system dependencies are present:

```text
XFControl/mp3decoder.lib
Library/US/XGameBase.lib
```

Disposition: **source-complete internal DLL/import-library output**.

## SpeedTreeRT

The archive contains the game's extensive `XSTreeWrapper` integration but lacks
both core SDK inputs:

```text
SpeedTreeRT.h
SpeedTreeRT.lib
```

The header is included by:

```text
XSTreeWrapper/XSTWrapperConfig.h
XSTreeWrapper/XSTShader.h
XSTreeWrapper/XSTWrapperCore.h
```

The wrapper directly constructs `CSpeedTreeRT`, uses its nested geometry and
texture structures, loads packed tree data, computes LODs, manages wind, and
renders branches, fronds, leaves and billboards. It cannot be reconstructed from
the wrapper alone.

The wrapper carries IDV proprietary notices dated 2001–2004 and uses the
pre-5.x-style `CSpeedTreeRT` API. A random newer SpeedTree SDK is unlikely to be
ABI or source compatible.

Required acquisition:

1. the exact compatible `SpeedTreeRT.h`;
2. the matching 32-bit Visual C++ `SpeedTreeRT.lib`;
3. its licence/redistribution evidence;
4. optionally, any accompanying version/readme metadata that identifies the SDK
   build.

An existing runtime client may help identify tree asset compatibility, but it
will not normally contain the development header or static library.

Original disposition: **genuine external SDK gap**. This was subsequently
reduced to a provenance/licensing gate by the candidate recovery described
below.

A public candidate was subsequently located and statically assessed. It is
SpeedTreeRT 1.6.0 from December 2003, with a January 2004 VC6/x86 DLL and
matching import library. It matches all 42 methods and 30 nested data fields
used by the EOD wrapper. See
`SPEEDTREE-CANDIDATE-RESEARCH.md`.

The technical gap is now provisionally closed. The recovered import library
matches all 146 exports of the user's runtime DLL, and the header covers all
wrapper calls and public fields. Static inspection of the supplied data further
confirmed 524 SPT models, complete SpeedTree configuration, all 45 configured
composition maps, and a 58/58 `SpeedWind.ini` parser-token match. Three inferred
branch textures affecting six models remain absent, but they are a localized,
deferrable visual-content issue rather than a build/startup blocker. Lawful
usage and provenance remain unresolved.

## GM structural source

All 12 absent GM files are available in the GitHub lineage and preserved under
`reference-only/gm/`. They are not missing from the evidence set.

The issue is architectural:

- the normal client constructs six hidden GM windows;
- privileged UI behavior is largely controlled by `_XGMCLIENT` and
  `_XADMINISTRATORMODE`;
- some GM object construction, network dispatch and two status-control packet
  sends are not compile-time guarded.

Possible future dispositions:

1. retain these files as a clearly labelled shared structural component while
   leaving all GM flags disabled; or
2. remove the hidden windows and their packet paths through a reviewed
   player-only patch.

This decision must not be confused with missing source. The files are available.

## Removed master/disciple feature

The GitHub main-game source predates deliberate removal of the
master-and-disciple feature from the later archive project.

The feature source is preserved under `reference-only/removed-features/` but is
not proposed for restoration. The review-only patch:

```text
proposals/0001-disable-removed-mnd-references.patch
```

guards the three obsolete includes and five removed command IDs. It has been
dry-run validated but not applied.

Disposition: **known revision adaptation, not missing desired functionality**.

## Anti-cheat

The missing headers detected by a raw include scan are:

```text
HShield.h
NPGameLib.h
ApexClient.h
XTrap.h
```

For `US_Release|Win32`, these are already behind feature macros such as
`_XUSE_HACKSHEILD`, `_XUSE_GAMEGUARD`, `_XUSEAPEXMODULE` and `_XUSE_XTRAP`.
The baseline US configuration does not define their enabling macros.

Because the project direction is complete removal and later replacement, these
SDKs should not be acquired. The future stripping patch must remove:

- conditional includes and pragma-linked libraries;
- initialization and shutdown paths;
- callbacks and periodic checks;
- challenge/response packet handlers;
- XTrap source integration;
- stale project entries and include/library directories.

Disposition: **explicitly out of acquisition scope; future controlled removal**.

## Effective remaining list

With runtime files and data supplied separately, the normal player currently
needs decisions or acquisition only for:

1. **SpeedTreeRT provenance/licensing gate:** a revised SpeedTreeRT 1.6
   `SpeedTreeRT.h` and Win32 import library have now been located in the public
   MultiversePlatform repository. All 146 import symbols match all 146 exports
   of the supplied 2006 DLL exactly. The technical file gap is therefore
   provisionally closed. A subsequent wrapper audit confirmed all 42 called
   methods and all 31 directly accessed public data fields. The original source
   contains an explicit product authorization call, but lawful use, provenance
   before integration. The packed model/configuration structure is now
   statically coherent; runtime behaviour remains unproven.
2. **GM policy:** retain available structural classes or remove their hidden
   player integration.
3. **Master/disciple revision patch:** review and eventually apply the drafted
   guards.
4. **Anti-cheat stripping patch:** design and review in its dedicated future
   phase.

`XKernel.lib`, `XFControl.lib`, the manifest and `ADBalloon.h` are no longer
classified as genuine missing-source problems.
