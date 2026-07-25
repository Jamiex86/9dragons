# Proposed VC6 Player-Restoration Patch Set

Date: 2026-07-25  
Status: **prepared change specification; not applied**

No `.dsp`, `.dsw`, source, library, or runtime file was changed by this step.
This specification maps already approved recovery decisions onto the original
VC6 project lineage. It is intentionally split into review gates so a missing
symbol cannot cause unrelated behavior changes to be smuggled into one patch.

## Baseline fingerprints

The future patch must refuse to apply unless these inputs match:

| File | SHA-256 |
|---|---|
| `EmperorOfDragons.dsp` | `af0d53d843c73de86efb92cc657720b46e596e763a839e153a0b612bbdc65d04` |
| `XKernel/XKernel.dsp` | `f8e652f4f5beced7f1eda1e7dbd7f543711d14da91fbe9ab4ea0689384d82f8c` |
| `XFControl/XFControl.dsp` | `f6e844c35741039f238094593cfdcc5c43daee5dfc1a55dd11860cbed3142c61` |

## Patch order

### VC6-01 — declare the normal player target

Target: only the active `# ADD CPP` line inside:

```text
EmperorOfDragons - Win32 US_Release
```

Add:

```text
/D "_XPLAYERCLIENT"
```

Do not add it to:

- `US_Debug`;
- `US_Release_Debug`;
- `US_Release_GM`;
- any regional configuration.

Purpose: activate the already reviewed source guards that prevent normal-player
construction and maintenance of GM UI windows.

### VC6-02 — remove missing GM translation units from the player build

In `EmperorOfDragons.dsp`, preserve the file entries for project archaeology
but add a VC6 per-file exclusion for exactly
`EmperorOfDragons - Win32 US_Release` to these six `.cpp` files:

```text
XProcess\XWindow_GM_EventManager.cpp
XProcess\XWindow_GM_MonsterManage.cpp
XProcess\XWindow_GM_SendNoticeMessage.cpp
XProcess\XWindow_GM_StatusControl.cpp
XProcess\XWindow_GM_UserCoordination.cpp
XProcess\XWindow_GMCommand.cpp
```

The expected VC6 source-file form is:

```text
SOURCE=.\XProcess\<file>.cpp

!IF  "$(CFG)" == "EmperorOfDragons - Win32 US_Release"
# PROP Exclude_From_Build 1
!ENDIF
```

Headers remain listed because they are project metadata and some guarded source
still references their declarations outside the normal target.

Reason: all 12 raw project gaps are these six GM `.cpp/.h` pairs. They are not
normal-player dependencies.

### VC6-03 — disable legacy anti-cheat for the normal player

In the active US Release `# ADD CPP` line of `EmperorOfDragons.dsp`:

1. add:

```text
/D "_XPLAYER_NO_LEGACY_ANTICHEAT"
```

2. remove only these include paths:

```text
/I "./HackDetector/HackShield"
/I "./HackDetector/GameGuard"
/I "./HackDetector/XTrap"
```

3. exclude only this translation unit from US Release:

```text
Network\XNetwork_XTrap.cpp
```

using the same VC6 per-file configuration syntax as VC6-02.

In the active US Release `# ADD CPP` line of `XKernel/XKernel.dsp`:

1. add:

```text
/D "_XPLAYER_NO_LEGACY_ANTICHEAT"
```

2. remove only:

```text
/I "../HackDetector/HackShield"
/I "../HackDetector/GameGuard"
/I "../HackDetector/XTrap"
```

Do not delete anti-cheat source from the archive and do not alter any other
configuration.

### VC6-04 — D3DX route selection (**superseded pending acquisition**)

Do not remove `d3dx9dt.lib` by default. The supplied reference executable
contains static debug D3DX code and Microsoft `nt32_chk` D3DX source paths,
showing that the historical dependency was real.

For the fidelity route, retain the active link order:

```text
EmperorOfDragons - Win32 US_Release
```

```text
d3dx9dt.lib d3dx9.lib
```

For the coherent June 2010 route, a separately labelled patch may remove only
`d3dx9dt.lib` while keeping:

```text
dxguid.lib
d3d9.lib
d3dx9.lib
dsound.lib
dinput8.lib
dxerr9.lib
d3dxof.lib
```

Do not modify `# ADD BASE LINK32` or other configurations in either route.

### VC6-05 — controlled Flash MP3 promotion

This patch is gated and must not be created until provenance is approved.

Proposed active location:

```text
Library\FlashMP3\mp3decoder.lib
```

Expected SHA-256:

```text
3d27b37d6bf9ba95d8d3f5e1c75c1256b0759f76f6980627dbd936f25bd9558c
```

Once approved, change only the active US Release `XFControl.dsp`
`# ADD LINK32` entry from the unqualified:

```text
mp3decoder.lib
```

to:

```text
./../Library/FlashMP3/mp3decoder.lib
```

This eliminates accidental resolution from a machine-global `LIB` directory.
The original quoted CRT suppression must remain byte-for-byte unchanged until
its VC6 behavior is proven.

### VC6-06 — controlled SpeedTree promotion

This patch is also provenance-gated.

Proposed active locations:

```text
Library\SpeedTreeRT\SpeedTreeRT.h
Library\SpeedTreeRT\SpeedTreeRT.lib
```

Expected hashes:

```text
SpeedTreeRT.h
b5f8ad8e94e22c2df8ab590bde504245fe2eb21941b01cf0282778fdb2ea5543

SpeedTreeRT.lib
bfaf506d886195d1cb1330c04ac1fc6ba60da553217da0069812571f52ce620f
```

After approval:

- add `./Library/SpeedTreeRT` to only the US Release include path;
- replace unqualified `SpeedTreeRT.lib` with
  `./Library/SpeedTreeRT/SpeedTreeRT.lib` in only the active US Release link
  line;
- retain the matched DLL for the later runtime manifest, not the link tree.

## Existing source changes reused by VC6

The VC6 project patch does not need to duplicate the already committed source
guards. Those guards are compiler-compatible with VC6 and cover:

- removed master/disciple references;
- `_XPLAYERCLIENT` GM isolation;
- `_XPLAYER_NO_LEGACY_ANTICHEAT` selectors and assertions.

The source guard commits remain independently reviewable:

```text
bbdacca  guarded removed master/disciple references
e550d16  isolated GM capability from US player
9e9a67f  disabled legacy anti-cheat in US player
```

## Review invariants

The generated patch must satisfy all of these static checks:

1. exactly one normal-player configuration gains `_XPLAYERCLIENT`;
2. exactly the main and XKernel US Release compiler lines gain
   `_XPLAYER_NO_LEGACY_ANTICHEAT`;
3. exactly seven `.cpp` files are excluded from main US Release: six GM windows
   and `XNetwork_XTrap.cpp`;
4. no GM configuration is edited;
5. no regional configuration is edited;
6. `d3dx9dt.lib` remains for the fidelity route, or exactly one effective
   occurrence is removed in the explicitly labelled June 2010 route;
7. CRT ordering and `/NODEFAULTLIB` spelling are untouched;
8. MP3 and SpeedTree paths remain unchanged until their separate promotion
   gates are approved;
9. the `.dsp` remains parseable by Visual Studio 6;
10. no generated `.mak` file is trusted without regeneration and diff review.

## Why this is not yet an applied `.patch`

VC6 `.dsp` per-file configuration blocks are IDE-generated syntax. The safest
artifact is produced by:

1. opening a copy of the exact hashed `.dsp` in the frozen VC6 VM;
2. making only the exclusions above through VC6 project settings;
3. closing the IDE;
4. diffing the generated `.dsp` against the immutable baseline;
5. reducing the diff to the approved US Release changes;
6. saving that exact unified diff as the executable proposal.

Hand-inserting plausible VC6 condition blocks before the VC6 parser is
available would create false confidence. This specification is frozen so the
IDE-generated diff can be mechanically checked against it later.

## Current gate

The source-side restoration decisions are complete. Producing the final
applicable VC6 unified diff now depends on acquiring and hashing the frozen VC6
toolchain/VM; it does not require compiling the client.
