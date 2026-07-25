# Emperor of Dragons normal-client recovery overlay

## Purpose

This directory is a quarantined static-analysis overlay. It is not a source-tree
merge and is not a build directory.

- The supplied ZIP/RAR and extracted audit tree were not changed.
- No candidate overwrites a path already present in the archive.
- No executable, DLL, installer, script, compiler or linker was run.
- DLLs and libraries are retained as evidence and were inspected only as data.

## Provenance

- Source repository: `https://github.com/osfarrapos/spree`
- Source subtree: `project2`
- Repository commit: `aefd705a94983adf24aea18e76438871d8dcf406`
- Project2 content import commit:
  `a136b8150d81b8d10c693cbfd877ea27b947d0a2`
- GitHub project2 content date: 3 May 2012
- Supplied archive SHA-256:
  `4b2daf495fdc7aa5a13f7ee0f784157c2a3877deb12a9923d5c5ac9f10a6bda7`

The exact size and SHA-256 of every overlay file are recorded in
`analysis/overlay-files.csv`.

## Included scope

### Normal-player core

```text
XProcess/XProc_MainGame.cpp
XProcess/XProc_MainGameCallBackFunctions.cpp
XProcess/XProc_MainGame.h
XProcess/XProc_MainGameDef.h
```

### Project-matched middleware and packaging evidence

```text
Library/BinkSDK/bink.h
Library/BinkSDK/binktextures.h
Library/BinkSDK/binkw32.dll
Library/BinkSDK/binkw32.lib
Library/BinkSDK/dx9rad3d.cpp
Library/BinkSDK/rad3d.h
Library/BinkSDK/radbase.h
Library/CWebPage/XWebPage.h
Library/CWebPage/XWebPage.lib
Library/FMod/fmod.h
Library/FMod/fmod_errors.h
Library/FMod/fmoddyn.h
Library/FMod/fmodvc.lib
Library/FMod/wincompat.h
Library/QHTM/QHTM.dll
Library/QHTM/QHTM.h
Library/QHTM/QHTM.lib
Library/dbghelp.lib
Library/radsdk/radsdk.h
Library/radsdk/radsdk6.lib
Library/radsdk/radsdk_ll.h
res/NineDragons.exe.manifest
```

## Explicit exclusions

- all GM `.cpp` and `.h` files;
- master-and-disciple files removed from the later archive project;
- alternate target-selection files removed from the later archive project;
- launchers, patchers and transfer utilities;
- region-specific engine libraries;
- generated objects, PDBs and other build intermediates;
- replacements for any file already present in the archive.

Reference-only copies of the 12 GM files and 13 deliberately removed
master/disciple/target-selection files are stored under `reference-only/`.
They are excluded from `overlay/` and listed with hashes in
`analysis/reference-only-files.csv`.

## Legal status

Public repository availability is provenance evidence, not a determination that
the code or proprietary middleware may be redistributed. Licensing and
redistribution rights remain an independent gate.
