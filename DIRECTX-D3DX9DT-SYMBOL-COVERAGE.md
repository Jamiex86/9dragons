# `d3dx9dt.lib` Static Symbol-Coverage Audit

## Scope

This audit is static and uses `US_Release|Win32` as the player baseline. It did
not compile, link, or execute any source or binary.

The inspected project closure is:

- `EmperorOfDragons.vcxproj`;
- `XKernel/XKernel.vcxproj`;
- `XGamebase/XGamebase.vcxproj`;
- `XFControl/XFControl.vcxproj`.

These projects contain 482 existing, project-listed compilation units. The main
US configuration is Win32, MultiByte, `_XPLAYERCLIENT`, and explicitly links
`d3dx9dt.lib` before `d3dx9.lib`.

Candidate:

`recovery/emperor-of-dragons-normal-client-overlay/candidate-dependencies/DirectX9-October2004-untrusted/d3dx9dt.fd431d9.untrusted.lib`

SHA-256:

`fd431d9ec65c3d3fd401b89aba77d83551e06ef80dcb725fdc89b8d78e90cf79`

## Result

Source scanning found 64 distinct `D3DX…(` tokens in the project closure:

| Classification | Count | Result |
|---|---:|---|
| Exact external symbols in candidate | 46 | Covered |
| Generic TCHAR names resolving to candidate `A`/`W` symbols | 5 | Covered; US configuration is MultiByte, therefore `A` |
| Types, macros, inline functions, or locally implemented helpers | 13 | No library export required |
| Unexplained missing external D3DX symbols | **0** | None |

Therefore every D3DX call statically identifiable in the normal US player
project closure is satisfied by this candidate or by source/header code.

This is source-level coverage, not proof that a future compile will introduce
no additional template, macro, conditional, or compiler-generated reference.

## Exact external coverage

The candidate contains all 46 directly named external calls:

```text
D3DXAssembleShader
D3DXColorLerp
D3DXCreateCylinder
D3DXCreateSprite
D3DXCreateTexture
D3DXCreateTextureFromFileInMemoryEx
D3DXDeclaratorFromFVF
D3DXGetImageInfoFromFileInMemory
D3DXLoadSurfaceFromSurface
D3DXMatrixDeterminant
D3DXMatrixIdentity
D3DXMatrixInverse
D3DXMatrixLookAtLH
D3DXMatrixMultiply
D3DXMatrixPerspectiveFovLH
D3DXMatrixReflect
D3DXMatrixRotationAxis
D3DXMatrixRotationQuaternion
D3DXMatrixRotationX
D3DXMatrixRotationY
D3DXMatrixRotationYawPitchRoll
D3DXMatrixRotationZ
D3DXMatrixScaling
D3DXMatrixTransformation2D
D3DXMatrixTranslation
D3DXMatrixTranspose
D3DXPlaneNormalize
D3DXQuaternionDot
D3DXQuaternionMultiply
D3DXQuaternionRotationAxis
D3DXQuaternionRotationMatrix
D3DXQuaternionRotationYawPitchRoll
D3DXQuaternionSlerp
D3DXVec2Normalize
D3DXVec3Cross
D3DXVec3Dot
D3DXVec3Length
D3DXVec3LengthSq
D3DXVec3Normalize
D3DXVec3Project
D3DXVec3Subtract
D3DXVec3Transform
D3DXVec3TransformCoord
D3DXVec3TransformNormal
D3DXVec4Normalize
D3DXVec4Transform
```

## MultiByte alias coverage

The following generic API names are header aliases. `US_Release|Win32` sets
`CharacterSet` to `MultiByte`, so they resolve to the `A` forms present in the
candidate:

| Source name | Required candidate form |
|---|---|
| `D3DXAssembleShaderFromFile` | `D3DXAssembleShaderFromFileA` |
| `D3DXCreateFont` | `D3DXCreateFontA` |
| `D3DXCreateFontIndirect` | `D3DXCreateFontIndirectA` |
| `D3DXCreateTextureFromFileEx` | `D3DXCreateTextureFromFileExA` |
| `D3DXSaveSurfaceToFile` | `D3DXSaveSurfaceToFileA` |

Both ANSI/Unicode variants exist in the candidate where expected, but the ANSI
form is the relevant normal-player ABI.

## Non-exported tokens explained

| Token(s) | Classification |
|---|---|
| `D3DXCOLOR`, `D3DXMATRIX`, `D3DXPLANE`, `D3DXQUATERNION`, `D3DXVECTOR2`, `D3DXVECTOR3`, `D3DXVECTOR4` | D3DX C++ value types/constructors supplied through headers |
| `D3DXToRadian` | Header macro in the supplied `XKernel/d3dx9math.h` |
| `D3DXQuaternionIdentity` | Header-inline implementation in the matching SDK math inline file |
| `D3DXQuaternionAxisToAxis` | Locally implemented inline helper in `XKernel/XD3DUtilities/d3dutil.cpp` |
| `D3DXQuaternionUnitAxisToUnitAxis2` | Locally implemented inline helper in `XKernel/XD3DUtilities/d3dutil.cpp` |
| `D3DXLineEquation` | Locally implemented in `XKernel/XMath/XMeshMath.cpp` |
| `D3DXVec3Identity` | Locally implemented in `XKernel/XMath/XMeshMath.cpp` |

None is evidence of a missing SDK export.

## Preserved binary-artifact check

The six `.lib` files already present in the player tree are middleware import
libraries (`XWebPage`, FMOD, Bink, QHTM, RAD, and dbghelp). Static undefined
symbol inspection found no additional D3DX dependency in those libraries. No
preserved player `.obj` files exist in this working tree from which further
undefined D3DX symbols can be recovered.

## Companion DirectX libraries

The working tree still contains none of the DirectX SDK libraries named by
`US_Release`:

```text
dxguid.lib
d3d9.lib
d3dx9.lib
dsound.lib
dinput8.lib
dxerr9.lib
d3dxof.lib
```

The same GitHub SDK directory that supplied the quarantined `d3dx9dt.lib`
contains all seven filenames. They have now been downloaded into quarantine
and audited in `DIRECTX-COMPANION-LIBRARY-AUDIT.md`. All seven are technically
credible, but the directory is a hybrid set: its `d3dx9.lib` targets
`d3dx9_43.dll`, so it is not a pure October 2004 SDK preservation.

## Verdict

The candidate passes the normal-player source-level D3DX *API* coverage gate:

**51 of 51 external D3DX calls covered; zero unexplained D3DX symbol gaps.**

This proves API availability but not compiler-runtime compatibility.
Object-level inspection subsequently found VC7-era security-cookie and exception
support references in members required by the player. The archive is therefore
a high-confidence October 2004 comparison reference, not yet a clean VC6
link-input candidate. See `DIRECTX-D3DX9DT-REVISION-ABI-AUDIT.md`.

The next static gate is a companion-library audit of the seven remaining
DirectX inputs from the same preserved SDK set, followed by cross-checking their
import targets against the Windows/DirectX runtime assumptions.
