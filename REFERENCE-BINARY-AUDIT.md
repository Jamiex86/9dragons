# Reference NineDragons Binary Audit

Date: 2026-07-25  
Inputs: user-supplied reference binaries  
Method: static PE inspection and printable-string analysis only. Neither binary
was loaded or executed.

## Input identity

| File | SHA-256 | Format |
|---|---|---|
| `NineDragons.exe` | `386dde8e1a19b22e90c8b7fd6274e994cdabeee3da24692aab8ea74740501c4d` | PE32 x86 Windows GUI executable |
| `XFControl.dll` | `ab2716c0abf5bc5c73e4ae32528cc5bf874b446ab46de93d9cdfaa95ae6c29ec` | PE32 x86 Windows GUI DLL |

## Principal findings

### VC6 is confirmed

Both PE headers report linker version `6.0`. `NineDragons.exe` imports
`MSVCP60.dll` and `MSVCRT.dll`; `XFControl.dll` imports `MSVCRT.dll`.

The executable also retains this source path:

```text
D:\nd1client\EmperorOfDragons_VC60\XProcess\XProc_NineDragonsTitle.cpp
```

Together these provide high-confidence confirmation that the shipped lineage
was built with Visual C++ 6, not merely converted from it.

### `XFControl.dll` matches the recovered interface

The DLL exports exactly one public entry:

```text
?GetFlashControl@@YAPAVCFlashControlInterface@@XZ
```

This is the symbol expected by the player and uses VC6 C++ name decoration.
The DLL contains the full distinctive `mp3dec`/`Mp3Ssc` diagnostic string set
found in the located `mp3decoder.lib`, confirming that the historical
`XFControl` statically incorporated that Flash MP3 decoder lineage.

Its PE timestamp is 2007-02-15 19:56:37. PE timestamps are evidence only and
can be rewritten.

### Historical static debug D3DX is present

The executable payload contains:

```text
D3DX Assertion Failure; application terminated
D3DXASSERT(%s);
d:\builds\nt32_chk\multimedia\directx\dxg\d3dx9\mesh\createmesh.cpp
d:\builds\nt32_chk\multimedia\directx\dxg\d3dx9\tex\cblt.cpp
d:\builds\nt32_chk\multimedia\directx\dxg\d3dx9\shader\api\d3dx9fragment.cpp
```

`nt32_chk` and the debug assertion implementation are strong evidence that
Microsoft's statically linked debug D3DX code was embedded in the shipped
client. This agrees with the original project ordering:

```text
d3dx9dt.lib d3dx9.lib
```

Therefore the old `d3dx9dt.lib` entry cannot be dismissed as inert project
debris when binary fidelity is the goal. It may still be undesirable for a
modernized build, but the historical client appears to have used it.

No side-by-side `d3dx9_*.dll` appears in the reconstructed import table. That
supports static D3DX, but the import table is protection-modified and cannot be
treated as complete evidence by itself.

### The executable is protected

`NineDragons.exe` is not a normal unprotected link image:

- section names include `.winlice` and `.boot`;
- normal code/data section names are removed;
- relocations and debug directory are stripped;
- entry point lies in the large boot/protection section;
- the import table is reduced to a very small set of gateway imports;
- image size is approximately 28 MiB.

This is consistent with a WinLicense-protected/post-processed executable. Its
2012-03-02 PE timestamp may represent protection or repackaging rather than the
underlying client link. Static strings and dependencies remain useful, but:

- absence from its visible IAT does not prove a DLL was unused;
- code-size comparison with a future unprotected build would be meaningless;
- entry-point behavior cannot serve as the clean source entry point;
- compiler/linker conclusions should be corroborated with the unprotected
  `XFControl.dll` and embedded source paths, as done above.

### This is a later derivative, not a pristine Acclaim baseline

The executable contains mixed service lineage:

- Acclaim URLs and `_ACCLAIM`-era strings;
- GamesCampus item-shop URL;
- Korean Nine Dragons URLs;
- `NineDragonsUS`;
- an additional import on `NDGame.dll`;
- extensive GameGuard code and error text.

This strongly indicates a later regional derivative carrying accumulated
source branches. It is an excellent compiler/middleware behavior reference,
but not proof that every endpoint, protection component, or anti-cheat feature
belongs in the recovered clean player.

`NDGame.dll` is not a dependency represented in the recovered US Release
project. Its visible `_DllMain@12` import and the protected executable suggest
post-link protection/launcher integration or a later publisher addition. It
must not be added to the recovered source merely because this derivative
imports it.

## Visible runtime dependencies

The protected executable exposes these relevant DLL names:

- `d3d9.dll`
- `DINPUT8.dll`
- `SpeedTreeRT.dll`
- `dbghelp.dll`
- `XFControl.dll`
- `QHTM.dll`
- `fmod.dll`
- `XWebPage.dll`
- `MSVCP60.dll`
- `MSVCRT.dll`
- `WININET.dll`
- `VERSION.dll`
- `NDGame.dll`

This corroborates the already paired SpeedTree, QHTM, FMOD, XWebPage, and
XFControl architecture. Bink is not visible in the reduced IAT; that does not
disprove Bink use because the executable is protected and RAD/Bink can perform
runtime loading.

## Effect on preservation decisions

| Prior question | Reference-binary answer |
|---|---|
| Was VC6 merely historical project debris? | No. VC6 linker/runtime/source-path evidence is direct. |
| Was Flash MP3 decoder actually embedded? | Yes. Distinctive decoder strings are in `XFControl.dll`. |
| Was `d3dx9dt.lib` certainly stale? | No. Static debug D3DX code is visibly embedded in the reference EXE. |
| Does the reference require retaining GameGuard? | No. It proves that this derivative used it, not that the clean future player must. |
| Should `NDGame.dll` be added? | No. It is a later/protection-lineage dependency with no recovered source-project basis. |
| Can this EXE be a byte-for-byte target? | No. Protection/post-link transformation prevents that. |

## Revised DirectX preservation routes

### Fidelity route

Retain the original link ordering:

```text
d3dx9dt.lib d3dx9.lib
```

and locate a provenance-valid pre-February-2005 DirectX 9 SDK/Extras
`d3dx9dt.lib`. Its exact variant remains unknown. Candidate comparison must use
embedded strings, symbol surface, object/compiler directives, and—only in a
later authorized environment—rendering behavior.

### Coherent recovered-header route

Use June 2010 D3DX version 43 headers/import library and
`d3dx9_43.dll`, excluding `d3dx9dt.lib`.

This is technically coherent and maintainable, but it is now explicitly a
controlled deviation from the observed historical binary.

## Remaining acquisition value

The most valuable additional artifact would be an **unprotected**
Acclaim-era `NineDragons.exe`, link map, or PDB from the same source vintage.
That could identify:

- the exact static D3DX generation;
- full imports before protection;
- actual link order and CRT resolution;
- whether `NDGame.dll` was post-link only;
- source/object contribution boundaries.

