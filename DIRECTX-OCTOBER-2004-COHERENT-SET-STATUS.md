# DirectX October 2004 Coherent-Set Status

## Newly preserved main SDK

The matching main Microsoft SDK has now been downloaded into quarantine from
the Internet Archive item `dxsdk_oct2004`.

| Property | Value |
|---|---|
| File | `dxsdk_oct2004.internet-archive.untrusted.exe` |
| Size | 229,290,240 bytes |
| Published MD5 | `7400addc1ef83cc8a813040e192168ca` |
| Observed MD5 | `7400addc1ef83cc8a813040e192168ca` |
| Published SHA-1 | `8097bb69676a20e55a98a67d960a8b6edc89dff9` |
| Observed SHA-256 | `9995e138a009572abe2292f051fbb0a21f3b3478e07527fa718c1c4a2e09fb3b` |
| Archive attribution | Microsoft Corporation, 7 October 2004 |

No installer was run. The outer cabinet and nested `dxsdk.exe` were inspected
statically. The latter was decoded with the official standalone macOS 7-Zip
console extractor held under `/private/tmp`; it was not installed into the
system. The nested Microsoft executable was never launched.

The main SDK's complete `Include` (184 files) and x86 `Lib` (50 files)
directories were extracted into quarantine. A coherent preservation candidate
was then assembled as:

1. the main October 2004 SDK `Include` and x86 `Lib` directories;
2. Microsoft's `Extras/D3dx9 Visual Studio 6` versions of `d3dx9.lib`,
   `d3dx9d.lib`, and `d3dx9dt.lib` replacing the ordinary main-SDK copies.

Candidate location:

`candidate-dependencies/DirectX9-October2004-untrusted/Coherent-VC6-October2004/`

## Critical header-lineage correction

The D3DX headers currently stored under `XKernel/` are not wholly October 2004
headers. `XKernel/d3dx9core.h` declares:

```text
D3DX_SDK_VERSION 43
```

The matching October header declares version 22. Version 43 is the June 2010
D3DX generation and agrees with the previously found hybrid `d3dx9.lib`
importing `d3dx9_43.dll`. It must not be assumed ABI-coherent with the recovered
October 2004 static VC6 `d3dx9dt.lib`.

Of the ten local `d3dx9*.h` files, three are byte-identical to October
(`d3dx9anim.h`, `d3dx9shape.h`, and `d3dx9xof.h`) and seven differ. The changed
files are `d3dx9.h`, `d3dx9core.h`, `d3dx9effect.h`, `d3dx9math.h`,
`d3dx9mesh.h`, `d3dx9shader.h`, and `d3dx9tex.h`.

Static source-token comparison found 88 distinct D3DX identifiers outside the
bundled D3DX headers. All 80 identifiers that are SDK declarations exist in the
October headers. The other eight are comments/guard fragments or local helper
functions (`D3DXLineEquation`, `D3DXQuaternionAxisToAxis`,
`D3DXQuaternionUnitAxisToUnitAxis2`, and `D3DXVec3Identity`). This means no
currently referenced SDK identifier requires the v43 header generation.

Because `XKernel` is itself an include directory, these local headers can take
precedence over an SDK include path. Merely installing or pointing at the
October SDK would therefore not fix the mismatch.

## Current decision

The exact October 2004 VC6 D3DX binary acquisition is closed, but coherent
header selection is now an explicit pre-build gate:

1. retain the extracted main SDK and Extras override hashes as immutable
   provenance evidence;
2. complete a focused declaration/interface-layout diff for the D3DX types
   actually instantiated or dereferenced by the player;
3. identify whether the version-43 headers were a later deliberate source
   update or an accidental dependency transplant;
4. choose one coherent route:
   - historical October headers plus static VC6 D3DX libraries; or
   - a separately engineered v43 modernization using the DLL import library;
5. never combine the v43 headers/import library with October's static
   `d3dx9dt.lib` merely to obtain a link.

The preservation route remains the primary fidelity candidate. The v43 route
is a modernization experiment and would require source/API review and
behavioral comparison.

No source, project, or active dependency path was changed.

## Coherent player link inputs

The preservation candidate now contains exact October versions of every
DirectX library named by `US_Release|Win32`:

| Library | SHA-256 |
|---|---|
| `dxguid.lib` | `42a1585a635968c65ab9d03b8bcd3e61e72bcf821bed1c1a0a34456480522d47` |
| `d3d9.lib` | `461a9baf9d7a3e6ce4b04ef7814296c0fcad36ec77deb7566b361cbdabf6e3d7` |
| `d3dx9dt.lib` | `d9743322b0b814351ec76b331e67e3f0c007d44c8c94bab2b9192ee8f496f68d` |
| `d3dx9.lib` | `1ee03173ca703e514e129aab42c32c451044a2f252080a3cab5cd5fb416f73ce` |
| `dsound.lib` | `3149b985832bd620623630ac45a7dee69aa9fb61cba3a2991f438c360d2da449` |
| `dinput8.lib` | `1aefd62d9df8a26aa312817bd98b2fc3340477b238c918fc9edc04f7498a5b34` |
| `DxErr9.lib` | `0dcf72695c4dc029768d2d8fadb5d61d56dcf896bb9683dc936f8a046c608495` |
| `d3dxof.lib` | `6e02101d54f570d2bbb985292f4c6f0db105b067d32e865991a698faa78a34e6` |

The earlier public companion set was partly mixed: `d3d9.lib`, `DxErr9.lib`,
and `d3dxof.lib` are byte-identical to the official October set, while
`dxguid.lib`, `dsound.lib`, and `dinput8.lib` differ. The coherent candidate
uses only the official October copies.
