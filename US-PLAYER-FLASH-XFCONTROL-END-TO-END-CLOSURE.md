# US Normal Player Flash/XFControl End-to-End Closure

Date: 2026-07-25  
Scope: static inspection only. No compilation, linking, DLL loading, or client
execution.

## Conclusion

The Flash/XFControl subsystem is technically almost complete:

- all normal-player `XFControl` source and public interfaces are present;
- `Library/US/XFControl.lib` is generated project output, not missing source;
- the supplied reference `XFControl.dll` has the exact export consumed by the
  reference player;
- all English SWF assets named by source exist in `XSCENESCRIPT.XP`;
- the recovered `mp3decoder.lib` is a 99–100% technical lineage/API match.

The remaining blocker is the Fraunhofer/Macromedia decoder's legal provenance,
plus later runtime validation—not an unknown binary or ABI.

## Criticality

The player imports
`?GetFlashControl@@YAPAVCFlashControlInterface@@XZ` from `XFControl.dll`.
The returned interface drives title, login, character selection,
creation/deletion, server unification, off-screen rendering, timers, input,
animation state, and embedded MP3 audio.

An absent DLL prevents process startup. Failure to obtain the control also
returns false from several title/character initialization paths. Flash is
login/UI-flow critical.

## Source and generated outputs

The project includes its Flash 4-derived playback/rendering source, Win32
control, JPEG/ZLib integration, public interface, and MP3 wrapper.

Expected outputs are:

```text
Game/US/XFControl.dll
Library/US/XFControl.lib
```

The import library is generated while linking the DLL; its pre-build absence
is expected.

Reference DLL:

```text
/Users/jamiemarshall/Downloads/XFControl.dll
SHA-256: ab2716c0abf5bc5c73e4ae32528cc5bf874b446ab46de93d9cdfaa95ae6c29ec
```

It exports the exact factory imported by the reference executable and imports
`MSVCRT.dll`, confirming the `/MD` boundary.

## Flash asset closure

The source reads SWFs from `Data/Script/XSCENESCRIPT.XP`.

```text
SHA-256: ccd1480303c4c99af94a71e64a3ead6f095b21628205744d2a99ced6142834c5
Size: 174,351,375 bytes
Entries: 70
SWFs: 9
```

Contained:

```text
DELETE00.swf
MAKING00.swf
MAKING02.swf
SELECT00.swf
TITLE00.swf
TITLE00_1.swf
TITLE01.swf
TITLE02.swf
us_002_NineDragonsMap.swf
```

Every English/core SWF literal referenced by normal player source is present.
Vietnamese and Taiwanese prefixed SWFs are regional alternatives and are not
required by `_XENGLISH`.

## MP3 decoder

US Release defines `FLASHMP3`. The quarantined candidate is:

```text
SHA-256: 3d27b37d6bf9ba95d8d3f5e1c75c1256b0759f76f6980627dbd936f25bd9558c
```

It has all 11 exact functions, three byte-identical headers, and all 19
meaningful diagnostics found in the reference DLL. Technical identity
confidence is 99–100%.

The decoder requests `LIBCMT`; XFControl uses `/MD`. The reconstructed policy
is to retain `MSVCRT` and suppress `LIBCMT` only. Decoder handle ownership stays
inside its API boundary.

Its headers state Fraunhofer IIS copyright and “All Rights Reserved,” while the
public repository supplies no license chain. Technical closure is achieved;
promotion/redistribution permission is not.

## Routes

| Route | Fidelity | Remaining issue |
|---|---:|---|
| Rebuild with authorized original decoder | Highest | Documentary permission |
| Preserve supplied reference XFControl DLL | Very high | DLL redistribution authority |
| Clean-room 11-function adapter | Potentially high | Timing/buffering validation |
| Disable `FLASHMP3` | Lower | Changes UI audio behavior |
| Replace Flash | Modernization | Major UI compatibility project |

## Verdict

| Item | Status |
|---|---|
| XFControl source/interface | Present |
| `Library/US/XFControl.lib` | Generated output |
| Reference-compatible DLL | Supplied |
| Normal US SWFs | Present |
| Decoder binary | Exact-lineage candidate |
| Decoder legal provenance | Unresolved |
| Runtime behavior | Deferred |

No additional critical Flash source or data file is presently known missing.
Before a controlled build, select an authorized decoder or clean-room adapter.
Do not silently disable `FLASHMP3`.
