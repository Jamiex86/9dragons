# US player dependency closure after isolation

## Baseline

Source tree commit: `9e9a67f`.

Configuration: `US_Release|Win32`.

No compilation or executable execution was performed.

## Project inventory

The main project contains 627 unique file entries. A case-insensitive
Windows-path comparison reports 26 absent paths, classified as follows:

| Group | Count | Classification |
|---|---:|---|
| GM `.cpp`/`.h` entries | 12 | Not player dependencies; six `.cpp` files are excluded from US Release |
| Bink SDK source/header files | 5 | Present in approved recovery overlay; not yet copied into working tree |
| Stale `NMClass/ADBalloon.h` | 1 | Not a US player dependency |
| Required generated/resource header | 1 | Critical: `DataStructure/XSR_STRINGHEADER.H` |
| Required MD5 header | 1 | Critical: `Network/XMD5.H` |
| Icon/resource files | 6 | Four are directly required by the `.rc`; two appear project-only |

## Newly confirmed critical source inputs

### `DataStructure/XSR_STRINGHEADER.H` — recovered

This is not merely a stale project entry. More than 100 source files include
it, including the executable, network, login, loading, UI, scripts and recovered
main-game files. Its symbols are `ID_STRING_*` resource identifiers used by
`_XGETINTERFACETEXT`.

The file is likely generated from the version-specific string-resource
database. A guessed or unrelated regional copy could compile while mapping UI
messages to the wrong numeric identifiers. It therefore requires exact lineage
or regeneration evidence.

The exact GitHub tree contains the project-named header with 3,573 identifiers
through ID 3887. Comparison against the assembled player source found that it
covers 2,948 of 2,950 active referenced names. The only two active later names
are:

```text
ID_STRING_OPTION_USEMUSIC
ID_STRING_OPTION_USESOUND
```

The supplied `NDTEXTDB_US.xsd` identifies ID 906 as `Background Music` and ID
908 as `Basic Sound`; the recovered header already names those IDs
`ID_STRING_OPTION_MUSIC` and `ID_STRING_OPTION_SOUND`. Two explicit aliases
were therefore added without inventing numeric values.

`ID_STRING_NEW_3564` is the only remaining lexical name absent from the header,
but both occurrences are inside comments. ID 3564 is also absent from the US
runtime database.

Status: **resolved with runtime-database-validated aliases**.

### `Network/XMD5.H` — recovered

`Network/XMD5c.cpp` and `XProcess/XProc_LoginServer.cpp` require this header.
`XMD5Global.h` and the implementation source are present, but the public
context/prototype header is absent.

The implementation resembles the well-known RSA Data Security MD5 reference
API, so a technically compatible reconstruction may be possible. Its exact
types, calling declarations and licence header must be established before doing
so.

The exact project-named RSA reference header exists in the GitHub tree and
matches the included `_XMD5*` implementation API and `MD5_CTX` use.

Status: **resolved from exact lineage path**.

## Resource files

`EmperorOfDragons.rc` directly loads:

```text
res/GameMain.ico
res/play.ico
res/pause.ico
res/stop.ico
```

These four are resource-compilation blockers. The project additionally lists:

```text
EmperorOfDragons.ico
res/NineDragons.ico
```

All six exact project paths were recovered from the GitHub tree, including the
four `.rc` inputs.

## Bink source layer — staged

The following exact project-named files already exist in the approved overlay:

```text
Library/BinkSDK/bink.h
Library/BinkSDK/binktextures.h
Library/BinkSDK/dx9rad3d.cpp
Library/BinkSDK/rad3d.h
Library/BinkSDK/radbase.h
```

These files and the previously audited Bink runtime/import pair are now staged
in the preserved working tree together with the other audited middleware
inputs. Licensing approval remains a distribution gate.

## Link inputs

Current non-system inputs divide as follows:

| Input | Status |
|---|---|
| `Library/US/XGamebase.lib` | Internal generated output; project source available |
| `Library/US/XKernel.lib` | Internal generated output; project source available and anti-cheat isolated |
| `Library/US/XFControl.lib` | Internal generated import library; project source available |
| `SpeedTreeRT.lib` | Exact 146/146 import/export candidate recovered |
| `Library/dbghelp.lib` | Candidate previously recovered/audited |
| `Library/QHTM/QHTM.lib` | Candidate recovered with matching runtime |
| `Library/FMod/fmodvc.lib` | Candidate recovered; FMOD runtime supplied |
| `Library/CWebPage/XWebPage.lib` | Exact DLL/import pair recovered |
| `Library/BinkSDK/binkw32.lib` | Candidate recovered with matching runtime |
| `Library/radsdk/radsdk6.lib` | Candidate recovered; provenance/ABI remains a review item |

DirectX 9-era system/import libraries still require the preservation SDK/toolchain
environment; they are not proprietary EOD source gaps.

## Current blocker order

1. Perform COFF/ABI closure for every staged linker input.
2. Decide whether the quarantined SpeedTree candidate may be placed in the
   active dependency path.
3. Validate every project-to-project internal output path.
4. Produce the final pre-build readiness ledger.

## Conclusion

The string header, MD5 header, icon resources and audited middleware overlay are
now staged. The stale `ADBalloon.h` project entry was removed. The raw missing
project-path count fell from 26 to 12, all of which are the separately preserved
GM files; their six `.cpp` translation units are excluded from `US_Release`.
No confirmed normal-player source/header/resource gap remains in the main
project at this stage.
