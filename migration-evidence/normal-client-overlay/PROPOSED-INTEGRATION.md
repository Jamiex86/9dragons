# Proposed normal-player integration — review only

Nothing in this proposal has been applied to the archive or overlay.

## Proposed file additions

Add the 26 files under `overlay/` only where the destination is absent. Existing
archive files always win.

The four `XProc_MainGame*` files are source recovery candidates. The other 22
files restore exact project-named middleware and packaging evidence. Binary
inclusion still requires licensing and ABI approval.

## Proposed source adaptation

`proposals/0001-disable-removed-mnd-references.patch` guards:

- three includes belonging to the master-and-disciple feature;
- five command IDs removed from the later archive's process definitions.

This aligns the older main-game source with the later project's deliberate
removal of that feature. The controlling macro is absent from
`US_Release|Win32`.

The patch is deliberately narrow. It does not alter packet layouts, class
layouts, window IDs, callbacks, authentication or game behavior outside that
disabled feature.

## GM player-isolation decision

Static evidence shows:

- six GM window classes are instantiated unconditionally during normal
  first-load initialization;
- the windows are hidden, while privileged behavior is mainly gated by
  `_XGMCLIENT` and `_XADMINISTRATORMODE`;
- main-game initialization and reset paths access the GM command window even
  when those macros are absent;
- the converted project compiles all six GM `.cpp` files in every configuration.

Consequently, the revised proposal is to remove all six GM window classes from
the normal-player target and retain their 12 files only under
`reference-only/gm/`. The exact review-only adaptation surface is documented in
`PLAYER-GM-ISOLATION-PLAN.md`.

Static packet mapping has already identified two unguarded
`MSG_NO_CTRL_CHARAC_SETTING` sends inside `XWindow_GM_StatusControl.cpp`.
Although the containing window is hidden in the normal flow, this prevents a
claim that privileged packet emission is compile-time impossible until the
player-isolation patch is applied and statically re-audited.

## Deliberately removed feature references

The 13 master/disciple and alternate target-selection project files are retained
under `reference-only/removed-features/`. They are not proposed for restoration.
Their purpose is to explain the older main source and allow static comparison.

## Gates before applying any proposal

1. Review the five master/disciple command cases against the later
   `XProc_ProcessDefine.h`.
2. Confirm the feature macro is absent from every intended normal-player
   configuration, not only `US_Release`.
3. Generate and review the player-isolation patch from
   `PLAYER-GM-ISOLATION-PLAN.md`.
4. Re-scan the resulting player tree for GM includes, window IDs and privileged
   packet sends.
5. Establish licensing rights for every proprietary source and binary.
