# Emperor of Dragons: GitHub Lineage and Gap Analysis

## Scope and safety

This is a static comparison between:

- supplied archive tree: `EmperorOfDragons.zip`, authoritative SHA-256
  `4b2daf495fdc7aa5a13f7ee0f784157c2a3877deb12a9923d5c5ac9f10a6bda7`;
- comparison tree: `osfarrapos/spree/project2`, commit
  `aefd705a94983adf24aea18e76438871d8dcf406`.

No client executable, installer, script, anti-cheat component, compiler or linker
was run. Nothing has been copied into the supplied client tree. The comparison
script and machine-readable results are preserved beside this report.

## Verdict

**GitHub is an older, very close ancestor or sibling of the archive, not a newer
implementation.** Confidence: **high (90–95%)**.

The archive is best understood as a later, regionally adapted and partially
pruned tree derived from substantially the same code line. GitHub is therefore
strong recovery evidence for files that the later archive still names but no
longer contains. It must not be treated as a wholesale replacement for newer
archive files.

The strongest recovery candidates for the normal player executable remain:

| Path | Finding | Match confidence |
|---|---|---:|
| `XProcess/XProc_MainGame.cpp` | Missing from archive; still listed by archive projects; surrounding code is extremely close | 90–95% lineage, lower for exact revision |
| `XProcess/XProc_MainGameCallBackFunctions.cpp` | Missing from archive; still project-listed | 90–95% lineage |
| `XProcess/XProc_MainGame.h` | Missing from archive; still project-listed; declarations agree with the surviving message handler | 95%+ structural |
| `XProcess/XProc_MainGameDef.h` | Missing from archive; still project-listed | 95%+ structural |

These are credible restorations for analysis and future integration. They are
not proven byte-exact copies of the removed archive revision.

## Complete shared-file comparison

Paths were compared case-insensitively because the target is Win32:

| Measure | Count |
|---|---:|
| Archive files | 3,810 |
| GitHub files | 3,772 |
| Shared paths | 1,624 |
| Archive-only paths | 2,186 |
| GitHub-only paths | 2,148 |
| Shared paths with case-only spelling differences | 17 |

Shared content results:

| Result | All shared files | Shared C/C++/resource source |
|---|---:|---:|
| Byte-identical | 94 | 2 |
| Identical after decoding and EOL normalization | 1,161 | 1,052 |
| Content differs | 369 | 299 |
| **Total** | **1,624** | **1,353** |

Of the 299 changed shared source files:

- 152 are at least 99% line-similar;
- 210 are at least 98% line-similar;
- 276 are at least 95% line-similar;
- 293 are at least 90% line-similar.

This is far stronger than a merely similar game or reused engine. It establishes
a common source lineage.

The complete per-file result, including both hashes, sizes, line similarity and
archive-versus-GitHub line counts, is in
`emperor-of-dragons-comparison-data/shared-files.csv`.
The actual GitHub-to-archive unified diffs for every changed shared text file,
plus hash records for changed binaries, are in
`emperor-of-dragons-comparison-data/shared-content-differences.patch`.

## Older versus newer

### Evidence that GitHub is older

1. Git records `project2` entering the public repository in commit
   `a136b8150d81b8d10c693cbfd877ea27b947d0a2` on 3 May 2012. The repository
   commit currently checked is dated 5 May 2012; it did not update the project2
   client content.
2. The archive's preserved `EmperorOfDragons.dsp` timestamp is 4 July 2012.
   Its surviving `XProc_MainGameMessageHandler.cpp` timestamp is 12 November
   2013, and `EmperorOfDragons.cpp` is dated 13 September 2014.
3. Of 2,032 archive C/C++ and project files, 1,848 carry timestamps after the
   GitHub import. Timestamps alone are not proof, but they agree with the source
   deltas and project lineage.
4. GitHub contains only the VC6-era `.dsp`, `.dsw` and `.mak` at the root. The
   archive also contains `.vcproj`, `.vcxproj` and `.sln` conversions, culminating
   in the `v142` project already documented in the main audit.
5. The archive VC6 project contains **682 source entries, all of which occur in
   GitHub's 695-entry project**. GitHub has 13 additional entries. This is a
   clean parent/descendant-shaped relationship, not two unrelated project files.

### What cannot be concluded

- A later archive timestamp does not prove every archive file is newer. Regional
  trees were merged and pruned independently.
- A larger GitHub file does not make it newer. In 110 of the 303 changed text
  files GitHub has a net line surplus; this mainly reflects features removed or
  replaced in the later regional client.
- GitHub's four main-game files can be older than the exact deleted archive
  versions even though they are the correct lineage.

The correct merge policy is therefore: **archive wins for every file it already
contains; GitHub is considered only for demonstrably absent files.**

## What GitHub has that the archive does not

There are 2,148 GitHub-only case-insensitive paths. This raw number is not a
missing-player-source count:

| Category | Count | Interpretation |
|---|---:|---|
| Third-party/middleware | 853 | Mostly complete SDK/source distributions omitted from the archive |
| Generated/build artifact | 600 | Intermediate output; not authoritative source |
| Launcher/patcher | 369 | Separate Unicode launcher/patch programs |
| Resource/config/other | 145 | Mixed evidence; review individually |
| Normal/shared source | 105 | Includes obsolete, alternative and project-local files |
| Normal player/game process | 42 | Most relevant application-level candidates |
| Shared engine/internal library | 22 | Alternative/older internal components |
| GM/admin | 12 | Six GM window classes, source plus header |

Additional path reconciliation:

- 179 GitHub-only paths are byte-identical to a file elsewhere in the archive;
- 1,453 have the same basename somewhere else in the archive;
- those are moves, duplicates, versioned SDK layouts or alternate project
  copies—not automatically removed functionality.

The full inventory is in
`emperor-of-dragons-comparison-data/github-only-files.csv`. It includes hashes,
same-content and same-basename archive locations, include consumers and a
dependency partition.

## Project-listed files: deliberate feature removal versus accidental pruning

GitHub's VC6 project has exactly 13 entries not present in the later archive
VC6 project:

```text
Network/XNetwork_MasternDisciple.cpp
Network/packet_for_MnD.h
XProcess/XMnDStructInfo.h
XProcess/XWindow_MnDCurrentPoint.cpp
XProcess/XWindow_MnDCurrentPoint.h
XProcess/XWindow_MnDGroup.cpp
XProcess/XWindow_MnDGroup.h
XProcess/XWindow_MnDTotalPoint.cpp
XProcess/XWindow_MnDTotalPoint.h
XProcess/XWindow_NPCMnDInfo.cpp
XProcess/XWindow_NPCMnDInfo.h
XProcess/XWindow_TargetInfoSelect.cpp
XProcess/XWindow_TargetInfoSelect.h
```

These entries form two coherent features:

- master-and-disciple UI/networking;
- an alternate target-info selection window.

Because the later project removes both the source files and their project
entries, these should initially be classified as **deliberately removed,
superseded or region-disabled**, not missing build inputs. They should not be
restored automatically.

By contrast, the later archive project continues to list the following absent
files:

```text
Library/BinkSDK/bink.h
Library/BinkSDK/binktextures.h
Library/BinkSDK/dx9rad3d.cpp
Library/BinkSDK/rad3d.h
Library/BinkSDK/radbase.h
NMClass/ADBalloon.h
XProcess/XProc_MainGame.cpp
XProcess/XProc_MainGame.h
XProcess/XProc_MainGameCallBackFunctions.cpp
XProcess/XProc_MainGameDef.h
XProcess/XWindow_GMCommand.cpp
XProcess/XWindow_GMCommand.h
XProcess/XWindow_GM_EventManager.cpp
XProcess/XWindow_GM_EventManager.h
XProcess/XWindow_GM_MonsterManage.cpp
XProcess/XWindow_GM_MonsterManage.h
XProcess/XWindow_GM_SendNoticeMessage.cpp
XProcess/XWindow_GM_SendNoticeMessage.h
XProcess/XWindow_GM_StatusControl.cpp
XProcess/XWindow_GM_StatusControl.h
XProcess/XWindow_GM_UserCoordination.cpp
XProcess/XWindow_GM_UserCoordination.h
res/NineDragons.exe.manifest
```

This second list is genuine project/source inconsistency. It divides further by
configuration:

- the four `XProc_MainGame*` files are normal-player core;
- Bink headers/backend are normal-player middleware inputs when video support is
  enabled;
- `ADBalloon.h` is regional advertising middleware, not a US player priority;
- the 12 GM files are administrative UI;
- the manifest is packaging metadata and replaceable.

## Normal player and GM dependency separation

### Logical feature boundary

The normal `US_Release|Win32` configuration does not define `_XGMCLIENT`.
Therefore GM commands, administration screens and privileged behavior are not
part of the intended player feature set. The 12 named GM files are:

```text
XProcess/XWindow_GMCommand.cpp
XProcess/XWindow_GMCommand.h
XProcess/XWindow_GM_EventManager.cpp
XProcess/XWindow_GM_EventManager.h
XProcess/XWindow_GM_MonsterManage.cpp
XProcess/XWindow_GM_MonsterManage.h
XProcess/XWindow_GM_SendNoticeMessage.cpp
XProcess/XWindow_GM_SendNoticeMessage.h
XProcess/XWindow_GM_StatusControl.cpp
XProcess/XWindow_GM_StatusControl.h
XProcess/XWindow_GM_UserCoordination.cpp
XProcess/XWindow_GM_UserCoordination.h
```

They should be stored and assessed separately from the player recovery set.

### Structural boundary leak

The old client does **not** enforce that separation cleanly:

- `XProc_MainGame.cpp` includes `XWindow_GMCommand.h` and
  `XWindow_GM_SendNoticeMessage.h` unconditionally;
- `XProc_MainGameCallBackFunctions.cpp` includes
  `XWindow_GM_StatusControl.h` unconditionally;
- normal code in `XProc_MainGame.cpp` contains unconditional
  `_XWindow_GMCommand` references in device initialization and window-reset
  paths;
- `XProc_FirstLoad.cpp`, the surviving main-game message handler, networking and
  the world minimap also reference GM classes;
- the converted `.vcxproj` lists the six GM `.cpp` files without
  per-configuration `ExcludedFromBuild` conditions.

Thus there are two valid meanings of "GM dependency":

1. **Behavioral dependency:** not needed for a normal player's capabilities.
2. **Legacy compile/link dependency:** currently entangled with shared
   initialization and window registration.

For planning, the GM implementation remains a separate recovery package.
Before a player-only build, static integration work must either:

- retain the legacy GM classes while ensuring privileged behavior stays disabled;
  or
- introduce narrow compile-time guards/stubs and prove that window IDs,
  registration order, object layout and callbacks remain unchanged.

Blindly omitting all 12 GM files from an otherwise untouched GitHub
`XProc_MainGame.cpp` is unlikely to compile and is not yet a safe plan.

Machine-readable partitions:

- `emperor-of-dragons-comparison-data/github-only-normal-player.csv`
- `emperor-of-dragons-comparison-data/github-only-gm.csv`
- `emperor-of-dragons-comparison-data/github-only-unclassified.csv`

The partition is conservative and include-graph based. Preprocessor conditions
and runtime registration must still be reviewed before implementation.

## Player recovery shortlist

### Tier 1: recover into a quarantine overlay for further static integration

1. The four `XProc_MainGame*` files.
2. The exact Bink include/backend files named by the later archive project:
   `bink.h`, `binktextures.h`, `dx9rad3d.cpp`, `rad3d.h`, `radbase.h`.
3. Required binary/header dependencies already identified by the main audit,
   but only where the archive project names the same ABI/version:
   Bink, RAD SDK 6, FMOD 3.74, QHTM, CWebPage and `dbghelp.lib`.

Each recovered file should preserve its Git commit, original path and SHA-256.
It should not overwrite any surviving archive file.

### Tier 2: hold separately

- all 12 GM source/header files;
- master/disciple and alternate target-info files removed from the later project;
- regional advertising and anti-cheat variants;
- Unicode launcher/patcher projects;
- full third-party SDK source trees not required by `US_Release`.

### Tier 3: do not use as replacements

- any GitHub file where the archive already has a version;
- build intermediates (`.obj`, `.pdb`, `.idb`, `.ncb`, `.opt`, logs);
- region-specific engine binaries with a different name or ABI;
- stale copied files selected only because their basename matches.

## Reassessment

This comparison materially strengthens the source-completeness outlook:

- **Main-game source loss:** changes from an unknown proprietary gap to a
  high-confidence older-lineage recovery candidate.
- **Exact revision fidelity:** remains unproven. The surviving handler is later
  and 96.67% similar to GitHub, demonstrating both close lineage and real drift.
- **GM separation:** feasible as an architectural goal, but not a zero-change
  file exclusion because the legacy project entangles GM window types with the
  player process.
- **Master/disciple files:** available, but restoration would probably reintroduce
  a feature intentionally removed from the later regional tree.
- **Third-party headers/libs:** availability improves link planning, but licensing,
  runtime DLL versions and ABI provenance remain separate gates.
- **Functional Heavenfall client:** unchanged at the later stages. GitHub supplies
  neither the complete game-data distribution nor proof of Heavenfall protocol,
  authentication, encryption, endpoints or server-version compatibility.

No compilation probability should be promoted to a functionality claim. Before
any controlled build, the next planning gate is a virtual merged-tree analysis
that:

1. overlays only Tier 1 files;
2. resolves every include and linker input for `US_Release|Win32`;
3. records all compile-time drift between the recovered main-game code and the
   later surviving headers;
4. chooses and documents a GM isolation strategy;
5. proves the expected middleware ABI and runtime DLL set.
