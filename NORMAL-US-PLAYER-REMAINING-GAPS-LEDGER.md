# Normal US Player Remaining Gaps Ledger

Date: 2026-07-25  
Baseline: `US_Release|Win32`, normal player only  
Method: static reconciliation; no compilation, linking, dependency loading, or
client execution.

## Executive result

There is currently **no confirmed unavailable proprietary normal-player
source/header/resource file**.

One active source file is absent from the assembled working tree but present in
the authoritative archive:

```text
XKernel/XSecurity/XCrypto.Cpp
```

This is an assembly omission, not an acquisition problem. All 12 absent paths
in the main executable project are GM window files and are not normal-player
dependencies under the reviewed isolation.

The remaining critical gates are:

1. restore the exact archived `XCrypto.Cpp` into any future controlled source
   baseline;
2. approve or replace the proprietary Flash MP3 decoder;
3. resolve SpeedTree SDK redistribution/provenance;
4. promote the hash-pinned legacy DirectX and middleware inputs through a
   reviewed manifest;
5. freeze the preservation compiler/linker/Platform SDK environment;
6. validate later with verbose link maps and controlled runtime tests.

These are not equivalent to missing player source.

## Fresh project-path reconciliation

Case-insensitive Windows-path checking of the four player build projects found:

| Project | Listed items | Absent from assembled tree | Classification |
|---|---:|---:|---|
| `EmperorOfDragons.vcxproj` | 626 | 12 | GM-only `.cpp/.h` paths |
| `XGamebase/XGamebase.vcxproj` | 154 | 0 | Source-complete |
| `XKernel/XKernel.vcxproj` | 144 | 1 | Exact source exists in authoritative archive |
| `XFControl/XFControl.vcxproj` | 68 | 0 | Source-complete |

Launcher, patcher, packer, file-transfer, registry utility, and GM outputs are
not part of this normal-player source closure.

## Exact archived XCrypto source

Expected path:

```text
XKernel/XSecurity/XCrypto.Cpp
```

Authoritative extracted evidence path:

```text
/private/tmp/emperor-dragons-audit.W6lgyC/extracted/EmperorOfDragons/XKernel/XSecurity/XCrypto.Cpp
```

Evidence:

| Property | Value |
|---|---|
| SHA-256 | `c78a0ce9eb0285c584a34c70ee637397417568cf4c975973cce443933da4160f` |
| Lines | 499 |
| Project-listed | Yes, original `.dsp` and converted `.vcxproj` |
| Header | `XKernel/XCrypto.h`, present |
| Status | Exact authoritative recovery; not copied during this audit |

`XCrypto.h` defines `_XNEWCRYPTO`. The source supplies
`_j_Crypto::Init(FILE*)`, while most new crypto operations are inline in the
header. Normal player startup constructs `CCapsulateCrypto g_PacketCrypto` and
calls `g_PacketCrypto.Init()` using key material from the script archive.

It is therefore active and important to packet-crypto initialization. Its
absence from the assembled tree would block XKernel compilation or leave a
missing implementation. Because the exact file is in the supplied archive,
recoverability is **100%**.

## Main-project GM paths

The 12 absent main-project paths are six GM `.cpp/.h` pairs:

```text
XWindow_GMCommand
XWindow_GM_EventManager
XWindow_GM_MonsterManage
XWindow_GM_SendNoticeMessage
XWindow_GM_StatusControl
XWindow_GM_UserCoordination
```

They are preserved as regional/GitHub evidence but excluded from the normal
player plan. Their absence is not a normal-player gap.

## Reconciled dependency ledger

| Component | Technical state | Normal-player decision |
|---|---|---|
| XGamebase | Complete source; `.lib` is generated | Build internally later |
| XKernel | Complete after exact archived XCrypto restoration | Build internally later |
| XFControl | Complete source; DLL/import library are generated | Decoder decision remains |
| Flash MP3 decoder | 99–100% exact technical candidate | Licensing or clean-room adapter gate |
| SpeedTree | 98–99% matched header/lib/DLL/auth/data | Provenance/licensing and runtime gate |
| DirectX 9 October 2004 | Exact coherent VC6 SDK set recovered | Manifest/promotion gate |
| QHTM | Active; exact import/DLL pair | Retain |
| XWebPage | Active; exact import/DLL pair | Retain initially; obsolete endpoints later |
| FMOD 3.74 | Active; all 229 APIs satisfied | Retain; sound pack present |
| RAD/Bink | Inactive stale US link entries | Remove from normal-player link/runtime |
| DbgHelp | Import library present | Pair with preservation Platform SDK/runtime |
| Windows/Platform SDK libs | External toolchain inputs | Freeze compatible SDK |
| Legacy anti-cheat | Intentionally removed from player scope | Do not acquire |

## Runtime/data state

The user-supplied Data tree supplies the packs required by the audited
subsystems, including:

- `Data/Script/XSCENESCRIPT.XP` with every normal-English SWF;
- `Data/Sound/SR_SOUND.XP`;
- SpeedTree model, texture, render-style, and script inputs;
- the broader model, texture, animation, environment, and script packages.

Runtime DLLs supplied and statically paired:

- `QHTM.dll`;
- `XWebPage.dll`;
- `fmod.dll`;
- `SpeedTreeRT.dll`;
- `XFControl.dll`.

They still need controlled destination manifests. Runtime availability does not
authorize redistribution and does not constitute runtime-behavior proof.

`binkw32.dll` was also supplied and pairs exactly, but it is not required by
normal US Release.

## Remaining issues by severity

### Gate A — before dependency restoration

| Issue | Type | Severity |
|---|---|---:|
| Restore exact archived `XCrypto.Cpp` | Source-tree assembly | Critical but fully recoverable |
| Decide Flash decoder authority/replacement | Legal and engineering | Critical for faithful XFControl rebuild |
| Decide SpeedTree authority | Legal/provenance | Critical for distributable preservation build |
| Hash-pin destinations for DLLs/SDKs | Configuration control | High |

### Gate B — before a controlled build

| Issue | Type | Severity |
|---|---|---:|
| Acquire/hash VC6 SP6 x86 and the selected February 2003 Platform SDK | Toolchain acquisition | Critical |
| Promote coherent October 2004 DirectX set | SDK placement | Critical |
| Apply reviewed player-only GM/anti-cheat isolation | Configuration | Critical |
| Remove inactive RAD/Bink link inputs | Link hygiene | Medium |
| Normalize XFControl CRT policy to `/MD`, retain MSVCRT, ignore LIBCMT only | ABI/linker | Critical |
| Produce verbose linker trace and map | Validation | Critical |

### Gate C — before claiming a working client

| Issue | Type | Severity |
|---|---|---:|
| Runtime DLL loading and versions | Runtime | Critical |
| Full data-pack startup/load validation | Runtime/data | Critical |
| Flash title/login/character flows | UI behavior | Critical |
| SpeedTree authorization/rendering | Renderer behavior | High |
| Audio initialization/playback | Audio behavior | High |
| Protocol/authentication/server compatibility | Separate integration phase | Critical for online play |
| Three inferred tree textures | Localized content | Low/medium visual |
| Two Crape Myrtle names | Configuration drift | Low until affected terrain loads |

## What is genuinely missing now?

Under the stated normal-player scope:

- **Unavailable proprietary source:** none confirmed.
- **Unknown required third-party binary:** none confirmed.
- **Unknown required normal-US data pack:** none confirmed.
- **Exact file absent only from the assembled tree:** `XCrypto.Cpp`, already
  present in the authoritative archive.
- **Technical candidate withheld from active dependency path:**
  `mp3decoder.lib` and SpeedTree SDK set.
- **Legal/provenance evidence:** unresolved for the decoder and SpeedTree.
- **Runtime proof:** intentionally not attempted.

## Current readiness statement

The normal player has moved from file discovery to controlled restoration
planning. It is reasonable to say the source and known dependency set are
substantially complete. It is not yet reasonable to say the client can be
faithfully built or will work.

The next planning task should be a hash-pinned preservation manifest defining
the exact future destination of every approved source recovery, SDK header,
import/static library, DLL, and data pack—without copying or building them yet.

That planning task is now complete in
`PRESERVATION-PLACEMENT-MANIFEST.md`. No file was promoted or copied.
