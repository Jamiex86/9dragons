# Static virtual-overlay reassessment

> Scope refinement: runtime DLLs and game data will be supplied from an existing
> client; Heavenfall interoperability is separate; legacy anti-cheat will be
> removed and replaced later. The focused current gap assessment is
> [REMAINING-SOURCE-GAPS.md](REMAINING-SOURCE-GAPS.md).

## Outcome

The Tier-1 overlay materially closes the principal normal-player source gap, but
it also confirms that the recovered 2012 main-game implementation is not a
drop-in, dependency-complete representation of the later archive.

No compilation is required to establish the following:

| Static check | Result |
|---|---:|
| Overlay files | 26 |
| Paths colliding with surviving archive files | 0 |
| Quoted includes in overlay source | 176 |
| Quoted includes resolved by virtual merged tree | 170 |
| Unresolved quoted includes | 6 |
| Converted-project items still absent | 14 |
| US Release local linker inputs examined | 10 |
| US Release local linker inputs resolved | 7 |
| Surviving message-handler member definitions | 72 |
| Handler method names found in recovered header | 72/72 |

The last result is a strong structural match, but it compares member names and
declarations, not machine-level ABI.

## Six unresolved source includes

### Removed feature: master and disciple

```text
XWindow_NPCMnDInfo.h
XWindow_MnDTotalPoint.h
XWindow_MnDCurrentPoint.h
```

GitHub's main-game source includes these headers unconditionally, but its uses
are guarded by `_XDEF_MASTER_AND_DISCIPLE_080721_MAGVIPER`. That macro is absent
from `US_Release|Win32`, and the later archive project deliberately removed the
feature's 11 source/header/network entries.

Planning conclusion: do not restore the feature automatically. A future
integration proposal should guard/remove these three obsolete includes for the
US player configuration, or retain their headers as compatibility declarations
without enabling the feature. That would be an adaptation to review—not
something performed in this static phase.

### GM structural references

```text
XWindow_GMCommand.h
XWindow_GM_SendNoticeMessage.h
XWindow_GM_StatusControl.h
```

The normal US configuration does not define `_XGMCLIENT` or
`_XADMINISTRATORMODE`, but the old main-game source includes these headers
unconditionally. `XWindow_GMCommand` is also referenced by unguarded window
initialization/reset code. This confirms the boundary leak identified in the
lineage report.

Planning conclusion: keep GM behavior and implementations in a separate
package, but do not claim the player source is independent yet. Choose between:

1. retaining the legacy GM window classes with all privileged features disabled
   by the normal configuration; or
2. introducing carefully reviewed guards/neutral declarations while preserving
   window IDs, registration order and callbacks.

Option 1 is closer to the historical code. Option 2 gives a cleaner player-only
tree but carries greater behavioral-change risk.

## Project entries still absent after overlay

The four main-game and five Bink source/header entries are now satisfied in the
virtual tree. Remaining converted-project inconsistencies are:

- 12 GM files, intentionally held outside this player overlay;
- `NMClass/ADBalloon.h`, associated with regional advertising and not a normal
  US recovery priority;
- `res/NineDragons.exe.manifest`, replaceable packaging metadata.

The converted `.vcxproj` compiles the GM `.cpp` entries without
configuration-specific exclusion. This is stale/entangled project structure,
not evidence that the normal player should expose GM functionality.

## US Release local linker inputs

Resolved in the virtual tree:

```text
Library/US/XGamebase.lib
Library/dbghelp.lib
Library/QHTM/QHTM.lib
Library/FMod/fmodvc.lib
Library/CWebPage/XWebPage.lib
Library/BinkSDK/binkw32.lib
Library/radsdk/radsdk6.lib
```

Unresolved:

```text
Library/US/XKernel.lib
Library/US/XFControl.lib
SpeedTreeRT.lib
```

`XKernel` and `XFControl` have internal source projects and should be treated as
future rebuild outputs, not lost proprietary source. No build has been attempted
and their ABI compatibility with the recovered main-game revision remains
unproven.

`SpeedTreeRT.lib` is a genuinely absent proprietary SDK library. The included
`XSTreeWrapper` source does not replace the SpeedTree runtime implementation.

## Runtime evidence

- `binkw32.dll` is a 32-bit Intel PE DLL and matches the project's
  `binkw32.lib` naming.
- `QHTM.dll` is a 32-bit Intel PE DLL and accompanies `QHTM.lib`.
- `fmodvc.lib` imports `fmod.dll`.
- The recovered header declares `FMOD_VERSION 3.74f`.
- Neither compared tree contains `fmod.dll`.

Therefore the overlay improves static link-input coverage but does not provide
a complete runtime DLL set.

## Revised confidence

These estimates concern the evidence position after a hypothetical, reviewed
integration. They do not claim a build was attempted.

| Milestone | Previous broad outlook | Static-overlay outlook | Confidence |
|---|---:|---:|---|
| Source-complete normal-player translation units | Low | 65–80% | Medium |
| Resolve quoted includes without enabling removed features | Low | 75–90% after a reviewed GM/MnD policy | Medium |
| Obtain all non-system linker inputs | Low | 45–65% | Medium-low |
| Linked executable | 10–25% range in early audit | 40–60% conditional | Low |
| Stable local startup | 5–15% | 15–30% conditional | Low |
| Render login/UI | 5–15% | 15–30% conditional | Low |
| Load complete game data | 0–5% | 0–5% | Medium |
| Authenticate to Heavenfall | 0–5% | 0–5% | Low |
| Correct in-world play | 0–3% | 0–3% | Low |

The upper link/startup ranges require:

- a defensible normal-player GM isolation policy;
- removal or neutralization of obsolete master/disciple includes;
- compatible `XKernel` and `XFControl` outputs;
- a known-compatible SpeedTree runtime;
- the missing FMOD 3.74 runtime DLL;
- verification of library architecture, CRT and symbol ABI;
- known-good runtime data and configuration.

## Next non-build gate

Before considering any compilation, continue statically with:

1. compare every `XProc_MainGame` call against later surviving declarations;
2. map all GM class construction, window registration and packet-handler paths;
3. identify every use of the removed master/disciple feature macro;
4. inspect COFF symbol tables and linker directives in all candidate libraries;
5. compare packet structures and endpoint/authentication logic between the
   recovered main-game source and later surviving networking code;
6. produce a proposed integration patch for review without applying it.

Raw evidence is in the `analysis` directory.

## Deep static gate completed

The next non-build gate was subsequently performed:

- 169 `XProc_MainGame` definitions across the recovered main source and later
  surviving message handler were compared with the recovered header;
- all 169 match a header declaration by name, parameter count and `const`
  qualification;
- 83 GM-related lines were mapped across the main, callback, first-load,
  message-handler, network and minimap paths;
- 55 are lexically guarded by `_XGMCLIENT` or `_XADMINISTRATORMODE`; 28 are
  not, including includes, construction and hidden-window maintenance;
- 47 master/disciple-related lines occur in the recovered main source;
- 39 are guarded by the removed-feature macro; the eight unguarded lines are
  three includes and five command IDs;
- all six recovered import/static libraries exhibit 32-bit x86 `stdcall`
  decoration, consistent with `Win32`;
- import-library evidence names `binkw32.dll`, `fmod.dll`, `QHTM.dll`,
  `XWebPage.dll` and `dbghelp.dll`;
- `fmod.dll` and `XWebPage.dll` are absent from both compared trees.

The GM packet reachability pass found 35 GM-related packet lines and 14 packet
tokens. Twenty-nine lines are under a GM/admin feature guard. Six are not:

- four are normal-network receive/dispatch paths for `MSG_NO_GM_RESULT`,
  `MSG_GM_RESULT` and `MSG_NO_GM_REQUEST`;
- two are `MSG_NO_CTRL_CHARAC_SETTING` sends in the GM status-control window.

The latter window is created but hidden in the legacy normal client. Its sends
are driven by the window callback rather than compile-time GM guards. Therefore
privileged traffic is not cleanly isolated at preprocessing time; absence of a
visible activation path must be proven before treating the legacy GM classes as
safe structural dependencies.

The complete evidence is recorded in:

- `analysis/xproc-method-signatures.csv`;
- `analysis/gm-mnd-reference-map.csv`;
- `analysis/overlay-macro-usage.csv`;
- `analysis/candidate-library-symbol-summary.csv`;
- `analysis/gm-packet-reachability.csv`;
- `analysis/deep-static-gate-summary.json`.

The review-only integration proposal is in `PROPOSED-INTEGRATION.md`. Its narrow
master/disciple adaptation patch has not been applied.
