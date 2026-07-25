# Combined normal-player patch review package

## Status

Planning artifact only. No patch in this directory has been applied to the
archive, overlay, or extracted source. No compilation has been attempted.

## Recommended patch sequence

Do not combine all changes into one irreversible patch. Use three reviewable
commits against a future assembled source tree.

### Patch 1 — recover normal main-game source

Add only the four recovered normal-client files:

```text
XProcess/XProc_MainGame.cpp
XProcess/XProc_MainGame.h
XProcess/XProc_MainGameCallBackFunctions.cpp
XProcess/XProc_MainGameDef.h
```

Source: `overlay/XProcess/`.

Acceptance:

- existing destination files must be absent;
- hashes must match `MANIFEST.md`;
- no GM reference files are copied with them;
- no removed master/disciple implementation files are copied.

### Patch 2 — later-tree compatibility

Apply:

```text
proposals/0001-disable-removed-mnd-references.patch
```

It guards three obsolete includes and five obsolete command identifiers behind
the already absent `_XDEF_MASTER_AND_DISCIPLE_080721_MAGVIPER` macro.

Acceptance:

- the patch applies with no fuzz;
- only `XProcess/XProc_MainGame.cpp` changes;
- no packet enum, class layout or runtime branch changes;
- the removed feature macro remains undefined for `US_Release|Win32`.

### Patch 3 — strict GM isolation

The generated patch is:

```text
proposals/0002-isolate-gm-from-us-player.patch
```

Its file surface is:

```text
EmperorOfDragons.vcxproj
EmperorOfDragons.vcxproj.filters          (if present/used)
XProcess/XProc_FirstLoad.cpp
XProcess/XProc_MainGame.cpp
XProcess/XProc_MainGameCallBackFunctions.cpp
XProcess/XProc_MainGameMessageHandler.cpp
XProcess/XWindow_WorldMinimap.cpp
Network/XNetwork.cpp
```

Required effects:

1. Compile the six GM header include groups out of `US_Release|Win32`.
2. Compile all six GM window allocation/initialization/registration blocks out
   of the player target.
3. Remove the post-model-load GM monster-list update.
4. Remove the GM command-window maintenance from the recovered main process.
5. Remove the minimap coordinate transfer into the GM coordination window.
6. Exclude all six `XWindow_GM*.cpp` translation units from `US_Release|Win32`;
   their headers and source remain available to the distinct GM configurations.
7. Remove or permanently exclude privileged GM send implementations from the
   player target.
8. Consume and ignore unsolicited `MSG_NO_GM_RESULT` in the player network
   receive path; do not alter packet numbering or framing.
9. Enforce that `_XGMCLIENT` and `_XADMINISTRATORMODE` cannot be defined by the
   normal-player target.

The 12 GM files remain byte-for-byte preserved in `reference-only/gm/`. The
historical GM configurations are deliberately not modified.

## Deliberately excluded changes

This patch series must not:

- delete or renumber shared protocol constants;
- alter authentication, encryption or packet structure layouts;
- add GM files to the player source;
- restore master/disciple feature implementations;
- remove anti-cheat code in the same patch;
- integrate third-party libraries;
- change compiler/toolset settings;
- change runtime endpoints;
- alter SpeedTree configuration or substitute missing textures.

Those concerns require independent evidence and review.

## Static validation matrix

Run these checks only against the future assembled, patched source tree.

| Check | Required result |
|---|---|
| `XWindow_GM` includes in player translation units | 0 |
| GM window constructions/registrations | 0 |
| GM window title lookups in player source | 0 |
| `_XGMCLIENT` project definitions | 0 |
| `_XADMINISTRATORMODE` project definitions | 0 |
| Reachable GM request sends | 0 |
| GM `.cpp` compile entries in player project | 0 |
| GM `.h` project entries in player project | 0 |
| Shared packet constant numeric changes | 0 |
| Shared packet structure layout changes | 0 |
| Unguarded removed-MnD identifiers | 0 |

## Review order

1. Diff recovered main-game files against the GitHub lineage evidence.
2. Review Patch 2 independently because it is revision compatibility.
3. Review Patch 3 independently because it is capability removal.
4. Re-run include, project-entry and packet reachability inventories.
5. Record the resulting hashes and update the missing-file ledger.
6. Only then begin the separate anti-cheat stripping design.

## Current readiness

The authoritative source assembly now exists at
`recovery/emperor-of-dragons-player-working-tree`.

- forensic baseline commit: `0062959`;
- master/disciple compatibility commit: `bbdacca`;
- GM isolation commit: `e550d16`;
- GM patch SHA-256:
  `a9c04bf94460aee85366863f75ca9b9d04d9d170b146cb323b77aa92d8dcec07`.

The GM transformation was reproduced from commit `bbdacca` with
`tools/apply_player_gm_isolation.rb`; all eight changed files matched
byte-for-byte. The generated patch also passed `git apply --check` against a
fresh archive of that commit. No compilation was performed.
