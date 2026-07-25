# Normal-player legacy anti-cheat isolation audit

## Status

Static planning change only. No compilation, executable, SDK, service, driver or
anti-cheat component was run.

The change exists in the preserved working tree as commit `9e9a67f` and as:

```text
proposals/0003-disable-legacy-anticheat-us-player.patch
```

Patch SHA-256:

```text
a8884449443f9830ddb44277ce73e8be2a7b7d2bc531c0b65f5af10fc612f9d7
```

## Critical baseline finding

GameGuard was active in the historical `US_Release|Win32` preprocessing path.
The project defines `_ACCLAIM_VERSION`; repeated source-local selection blocks
therefore define `_XENABLE_GAMEGUARD`, then `_XUSE_GAMEGUARD`, and include
`NPGameLib.h`.

This means the missing GameGuard SDK was a real US player compilation and
runtime dependency before isolation. It was not merely a stale include path.

## Other legacy systems

| System | US baseline | Evidence/disposition |
|---|---|---|
| GameGuard/nProtect | Active | Disabled by player policy macro |
| HackShield | Dormant | Enabling macro absent; forbidden by assertion |
| XTrap | Dormant | SDK define commented out; translation unit excluded from US player |
| Apex | Dormant | `_XUSEAPEXMODULE` absent in US; forbidden by assertion |
| Memory-hack defence | Dormant | US macro absent; forbidden by assertion |
| Internal speed-hack detector | Dormant | enabling define commented out; forbidden by assertion |

Shared packet constants and structures are retained to avoid protocol
renumbering or layout changes.

## Patch behavior

Both `EmperorOfDragons.vcxproj` and `XKernel/XKernel.vcxproj` define:

```text
_XPLAYER_NO_LEGACY_ANTICHEAT
```

for `US_Release|Win32` only.

Thirteen repeated anti-cheat selection sites across nine source/header files
now refuse to select a legacy provider when that policy macro is present.

The US player include paths no longer contain:

```text
HackDetector/HackShield
HackDetector/GameGuard
HackDetector/XTrap
```

`Network/XNetwork_XTrap.cpp` is excluded from the US player configuration.

The player precompiled header rejects accidental definitions of:

```text
_XUSE_GAMEGUARD
_XUSE_HACKSHEILD
_XUSE_XTRAP
_XUSEAPEXMODULE
_XDEF_MEMORYHACKDEFENCE
_XDEF_DETECTSPEEDHACK
```

## Preserved behavior

- No packet ID was deleted or renumbered.
- No packet structure was changed.
- Other regional and historical configurations were not converted.
- GM configurations remain separate and unchanged.
- No replacement anti-cheat API or placeholder security claim was introduced.

A future security implementation should use a new interface and protocol
extension rather than reusing vendor-specific GameGuard/XTrap/Apex structures.

## Validation

- transformation reproduced from commit `e550d16`;
- all 12 changed files matched byte-for-byte;
- generated patch passed `git apply --check` against a fresh `e550d16` tree;
- `git diff --check` passed;
- no build or execution was performed.

## Consequence

`NPGameLib.h`, GameGuard libraries, services and runtime files are no longer
required development inputs for the planned normal US player. This removes a
genuine external dependency blocker while explicitly leaving replacement
security as a later, separate engineering phase.
