# DirectX Companion Library Audit

## Scope and handling

This is a static, no-build, no-execution audit of the seven DirectX libraries
named after `d3dx9dt.lib` in `US_Release|Win32`.

The files were retrieved from the same preserved GitHub SDK directory as the
leading `d3dx9dt.lib` candidate and placed in:

`recovery/emperor-of-dragons-normal-client-overlay/candidate-dependencies/DirectX9-October2004-untrusted/companions`

They remain quarantined and have not been promoted into the normal-player
source.

## Whole-file verification

Each retrieved payload exactly matches the size and SHA-256 declared by its Git
LFS pointer:

| File | Bytes | SHA-256 |
|---|---:|---|
| `DxErr9.lib` | 3,901,404 | `0dcf72695c4dc029768d2d8fadb5d61d56dcf896bb9683dc936f8a046c608495` |
| `d3d9.lib` | 4,988 | `461a9baf9d7a3e6ce4b04ef7814296c0fcad36ec77deb7566b361cbdabf6e3d7` |
| `d3dx9.lib` | 87,600 | `0b1cd009d5e5ed15080c4d7ad0245a273242da07e0747921e80ac3ccf9bc0fc2` |
| `d3dxof.lib` | 1,722 | `6e02101d54f570d2bbb985292f4c6f0db105b067d32e865991a698faa78a34e6` |
| `dinput8.lib` | 284,368 | `277158fb325b22fd10680779e73b7dec1b2f577c7129ac25f632168325bd4f5e` |
| `dsound.lib` | 4,038 | `14cfc17975a4fa6e4d556f54e428feddfaf18bed92de0d88ff5cec056b12a232` |
| `dxguid.lib` | 896,294 | `2980caa0e1508434f235e508774cd04ba10c7307ac1ac6cd9882c27af3d5a905` |

All seven are readable Microsoft/COFF-style library archives.

## Static archive and import analysis

| File | Members | Role/evidence | Runtime target |
|---|---:|---|---|
| `DxErr9.lib` | 4 | Static error-name/description implementation; exports `DXGetErrorDescription9A/W`, `DXGetErrorString9A/W`, `DXTraceA/W` | Static code; also references normal Windows components such as `oleaut32.dll` |
| `d3d9.lib` | 18 | Import library; supplies `Direct3DCreate9` and D3DPERF functions | `d3d9.dll` |
| `d3dx9.lib` | 334 | D3DX import library, 661 visible global-symbol lines | **`d3dx9_43.dll`** |
| `d3dxof.lib` | 6 | Import library; supplies `DirectXFileCreate` | `d3dxof.dll` |
| `dinput8.lib` | 12 | Import/data library; supplies `DirectInput8Create` and standard `c_dfDI*` data formats | `dinput8.dll` |
| `dsound.lib` | 15 | Import library; supplies `DirectSoundCreate8` and related capture/enumeration functions | `dsound.dll` |
| `dxguid.lib` | 7 | Static GUID-definition archive; approximately 1,023 visible global-symbol lines | No DLL target |

The direct external calls visible in the normal-player project closure are
covered:

| Source call | Provider |
|---|---|
| `Direct3DCreate9` | `d3d9.lib` |
| `DirectInput8Create` | `dinput8.lib` |
| `DirectSoundCreate8` | `dsound.lib` |

No direct `DirectXFileCreate` or DXErr function call was found in the 482
project-listed player compilation units. `d3dxof.lib` and `DxErr9.lib` may be
inherited/stale linker inputs or may serve conditional/header-generated/internal
paths not visible through simple source-call scanning. Their presence is not
currently evidence of missing proprietary code.

## Critical lineage finding: this is a hybrid set

The recovered source contains:

```text
#define D3DX_SDK_VERSION 43
```

in `XKernel/d3dx9core.h`. The quarantined `d3dx9.lib` imports
`d3dx9_43.dll`, so it is consistent with those recovered headers and the June
2010 D3DX generation.

The quarantined `d3dx9dt.lib`, however, is the older statically linked debug
D3DX implementation with `nt32_chk` build paths and was removed from the SDK
model before version 43.

Therefore the GitHub directory is **not a pure October 2004 SDK directory**.
It is a later hybrid/collected SDK set containing:

1. an old static `d3dx9dt.lib`;
2. a version-43 `d3dx9.lib`;
3. system DirectX import/GUID libraries that are not, by filename alone,
   release-specific.

This is nevertheless consistent with the recovered project ordering:

```text
d3dx9dt.lib d3dx9.lib
```

Because the static debug archive is first, its matching members can satisfy
D3DX references before the linker reaches the version-43 import library.
Source-level analysis has already shown that the old static candidate supplies
all 51 external D3DX calls visible in the normal-player closure. Thus
`d3dx9.lib` may contribute no D3DX functions in that path, although only a
future controlled linker map could prove actual member selection.

## Suitability

| Library | Technical suitability | Provenance/selection decision |
|---|---|---|
| `d3dx9.lib` | Very high for the recovered version-43 headers | Valid maintenance-route candidate; not an October 2004 companion |
| `d3d9.lib` | Very high | Prefer an authenticated Microsoft SDK/Windows SDK copy for final preservation |
| `dinput8.lib` | Very high | Prefer authenticated Microsoft source |
| `dsound.lib` | Very high | Prefer authenticated Microsoft source |
| `d3dxof.lib` | High, but direct player use unproven | Retain as conditional/inherited pending link-map evidence |
| `DxErr9.lib` | High ABI confidence, direct player use unproven | Retain pending link-map evidence; case difference from `dxerr9.lib` is harmless on Windows |
| `dxguid.lib` | High symbol-family confidence | Prefer authenticated Microsoft source because GUID archive contents vary over SDK generations |

None of these files should be described as proprietary Emperor of Dragons
libraries. They are replaceable SDK dependencies.

## Verdict

The seven DirectX linker-input filenames are no longer technical unknowns:
credible x86 candidates exist for all of them, their basic roles are correct,
and the three directly invoked non-D3DX APIs are covered.

They do **not** establish a single coherent October 2004 SDK. The correct
preservation model remains two explicitly separated routes:

- **fidelity route:** the validated old static `d3dx9dt.lib`, with companion
  system libraries frozen and authenticated separately;
- **maintenance route:** recovered version-43 headers, `d3dx9.lib`, and
  `d3dx9_43.dll`, deliberately excluding `d3dx9dt.lib`.

The next static gate is to determine the old `d3dx9dt.lib` revision more
precisely by comparing its exported API and internal build metadata with
historical D3DX header generations. This will show whether October 2004 is the
best release attribution or whether the static archive comes from an earlier
DirectX 9 SDK.

