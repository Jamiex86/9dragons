# US Normal Player RAD/Bink Dead-Link Audit

Date: 2026-07-25  
Scope: `US_Release|Win32`, static inspection only. No compilation, linking, or
binary execution.

## Conclusion

`radsdk6.lib` and `binkw32.lib` are present on the historical US player link
line, but the normal US player does not enable the source feature that calls
them. They are stale unconditional link entries, not critical normal-player
dependencies.

Confidence: **97–99%**.

The source/API feature is controlled by `_ACCLAIM_IGAADSYSTEM`. The normal
`US_Release` configuration instead defines `_ACCLAIM_RUBICONADSYSTEM`. These
are separate advertising implementations; the latter does not call the RAD
IGA SDK.

Because MSVC static archives are demand-loaded, merely naming
`radsdk6.lib` on a link line does not pull its objects into the executable.
With the IGA call sites preprocessed out, no RAD archive member is required.
The same source guard encloses the Bink wrapper and Bink declarations.

## Evidence

`EmperorOfDragons.dsp` defines `_ACCLAIM_RUBICONADSYSTEM`, not
`_ACCLAIM_IGAADSYSTEM`, for normal US Release. Nevertheless its link line
unconditionally names:

```text
./Library/BinkSDK/binkw32.lib
./Library/radsdk/radsdk6.lib
```

The relevant implementation and interface files are wholly or materially
guarded by `_ACCLAIM_IGAADSYSTEM`:

- `XIGAADWrapper/XIGAADWrapperCore.cpp`
- `XIGAADWrapper/XWindow_BinkStatic.cpp`
- `Library/radsdk/radsdk.h`
- `Library/radsdk/radsdk_ll.h`
- `Library/BinkSDK/bink.h`
- `Library/BinkSDK/binktextures.h`
- `Library/BinkSDK/dx9rad3d.cpp`

The RAD calls found in the tree, including `radStart`, `radStop`,
`radSetting`, and `radGetLocalContent`, occur inside the disabled IGA feature.
Rubicon-guarded code remains active but is a separate in-client path.

Static PE inspection of the supplied reference
`/Users/jamiemarshall/Downloads/NineDragons.exe` shows no `binkw32.dll`
import. String comparison found `deleterubiconad`, but no `radStart`,
`radStop`, `radSetting`, `radGetLocalContent`, `radsdk`, `IGAAD`,
`libtommath-0.39`, or distinctive RAD cache strings.

The staged `radsdk6.lib`, by contrast, contains those RAD API names, cache
strings, SQLite symbols, and embedded `libtommath-0.39` paths. Their absence
from the reference executable corroborates that no RAD member was selected.

## Dependency disposition

| Item | Normal US necessity | Disposition |
|---|---:|---|
| `Library/radsdk/radsdk6.lib` | No | Remove from future normal-player link line |
| `Library/BinkSDK/binkw32.lib` | No | Remove from future normal-player link line |
| `Library/BinkSDK/binkw32.dll` | No | Exclude from normal-player runtime package |
| RAD/Bink headers and wrapper source | No active implementation | Preserve for archaeology; do not enable |
| Rubicon-guarded source | Yes where referenced | Keep separate from RAD/Bink analysis |

This conclusion applies only to normal `US_Release`. Any configuration defining
`_ACCLAIM_IGAADSYSTEM` must be assessed separately.

## CRT consequence and proof gate

`radsdk6.lib` contains `/DEFAULTLIB:LIBCMT` directives, but directives in
unselected archive members do not affect the final link. RAD therefore does
not justify a mixed `MSVCRT`/`LIBCMT` policy for the normal US player.

A future controlled patch should remove RAD/Bink from only the normal-player
link line and retain `/MD`/`MSVCRT`. A later verbose VC6 link and map should
confirm that neither archive contributes a member. That is a fidelity
validation gate, not a prerequisite for classifying these files as noncritical.
