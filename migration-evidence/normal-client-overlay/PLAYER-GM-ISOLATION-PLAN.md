# Normal-player GM isolation plan

## Status

The design has now been instantiated in the separate preserved player working
tree and committed as `e550d16`. The original archive and recovery overlay
remain unchanged. No compilation has been performed.

## Decision

The normal player must not compile, construct, register, display, or call any
of the six legacy GM window classes:

| Class | Player disposition |
|---|---|
| `_XWindow_GMCommand` | Remove |
| `_XWindow_GM_SendNoticeMessage` | Remove |
| `_XWindow_GM_UserCoordination` | Remove |
| `_XWindow_GM_MonsterManage` | Remove |
| `_XWindow_GM_StatusControl` | Remove |
| `_XWindow_GM_EventManager` | Remove |

The 12 `.cpp`/`.h` files remain preserved under `reference-only/gm/` for
lineage evidence and a possible separate GM client. They must not be copied
into the normal-player source overlay.

## Why hidden windows are insufficient

`XProc_FirstLoad.cpp` unconditionally allocates, initializes and inserts all six
windows into `g_MainWindowManager`, then calls `ShowWindow(FALSE)`.

Hidden state is not a security boundary:

- `_XWindow_GM_StatusControl` has two unguarded callbacks that send
  `MSG_NO_CTRL_CHARAC_SETTING` with operation `9`;
- several normal-client paths find and manipulate GM window objects without a
  GM compile-time guard;
- a future input, UI or window-manager regression could expose a hidden object.

Removing the objects gives a simpler and auditable player boundary.

## Required source adaptations

### `XProcess/XProc_FirstLoad.cpp`

Remove the six GM header includes and the six construction/initialization/
registration blocks. Remove the post-monster-load lookup and
`UpdateMonsterList()` call.

This is the central isolation change. No normal-player object retains pointers
to these windows.

### Recovered `XProcess/XProc_MainGame.cpp`

Remove the two unconditional GM header includes. Remove normal-flow lookups of
`_XWindow_GMCommand`, including:

- initialization/reset positioning and animation;
- end/reset window maintenance;
- the normal `#else` branch whose only action is keeping the window hidden.

Retain GM-only historical code only in the separate reference set. Do not make
the normal executable depend on a class merely so it can repeatedly hide it.

### Recovered `XProcess/XProc_MainGameCallBackFunctions.cpp`

Remove the unconditional `_XWindow_GM_StatusControl` include. The actual status
window callback path is already under `_XGMCLIENT`; exclude that entire path
from the normal-player source rather than retaining a dead reference.

### `XProcess/XProc_MainGameMessageHandler.cpp`

Remove all six GM header includes. The mapped GM UI message-handler block is
under `_XGMCLIENT`; exclude the block from the player source.

### `XProcess/XWindow_WorldMinimap.cpp`

Remove `_XWindow_GM_UserCoordination` and its unguarded coordinate-field update.
That block only copies a clicked minimap coordinate into a hidden GM movement
window. Coordinate calculation used by normal minimap behavior outside this
block is retained.

### `Network/XNetwork.cpp`

Remove the `_XWindow_GM_UserCoordination` include and all `_XGMCLIENT` send
implementations for:

- `MSG_NO_GM_REQUEST`;
- `MSG_NO_CTRL_MONSTER_SPAWN`;
- `MSG_NO_CTRL_CHARAC_SETTING`.

Do not renumber or delete packet constants in shared protocol headers during
this isolation pass. Packet numeric stability is more important than removing
unused declarations.

The unguarded inbound `MSG_NO_GM_RESULT` dispatch and `MSGGMSResult()` only
display result messages and do not grant capability or emit packets. Two safe
options exist:

1. preferred strict player policy: consume/ignore `MSG_NO_GM_RESULT` without
   calling the GM result presenter;
2. compatibility policy: retain the harmless inbound presenter temporarily.

The first option produces the clearest isolation claim. Unknown or unsolicited
server traffic must still be consumed according to the existing framing logic.

### `EmperorOfDragons.vcxproj` and filters

The applied planning patch excludes the six GM `.cpp` translation units from
`US_Release|Win32` while preserving them for the explicitly separate historical
GM configurations. Do not delete the preserved reference files.

## Macro policy

For every normal-player configuration:

- `_XGMCLIENT` must be absent;
- `_XADMINISTRATORMODE` must be absent;
- no project, property sheet, generated header or command line may reintroduce
  either definition.

The planning patch adds this compile-time assertion to the shared precompiled
header, active only when `_XPLAYERCLIENT` is defined:

```cpp
#if defined(_XGMCLIENT) || defined(_XADMINISTRATORMODE)
#error GM or administrator mode is forbidden in the normal-player target
#endif
```

`_XPLAYERCLIENT` is added only to `US_Release|Win32`.

## What remains shared

The shared packet header `Network/packet_for_ctrl_gms.h` is also referenced by
network and render-state code. It should not be deleted wholesale in this pass.
Unused GM packet structures can be removed later only after a full protocol and
layout audit.

Normal login must continue to select `MSG_NO_LOGIN`, never `MSG_NO_GMLOGIN`.
Existing selection is macro-controlled; the player macro assertion makes that
choice enforceable.

## Risk assessment

| Risk | Assessment |
|---|---|
| Normal UI regression | Low; removed windows are hidden and GM-specific |
| Normal rendering regression | Low; monster-list update serves the GM manager |
| Packet-layout regression | Low if constants/structs are retained |
| Privileged send remaining | Low after a fresh whole-tree send-call scan |
| Separate GM-client impact | None if reference source remains separate |
| Historical fidelity | Intentional divergence: capability removal for player safety |

## Static acceptance gates

Before applying a player patch:

1. assemble the selected normal-player source in a separate working tree;
2. generate a patch limited to the files listed above and the project metadata;
3. confirm no `_XGMCLIENT` or `_XADMINISTRATORMODE` definition exists in any
   inherited project setting;
4. confirm no normal-player translation unit includes any `XWindow_GM*` header;
5. confirm no normal-player source constructs or references any GM window ID;
6. confirm no reachable normal-player call emits the three privileged packet
   families listed above;
7. confirm no packet enum values or shared structure layouts changed;
8. preserve the GM source and hashes as reference-only evidence.

## Conclusion

The GM files are not missing player dependencies. They are separate-client
capability code accidentally retained as hidden structural UI in the historical
normal client. Removing them from the player target is technically narrow and
preferable to restoring them as dependencies.
