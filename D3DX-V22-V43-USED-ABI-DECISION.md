# D3DX v22 versus v43 Used-ABI Decision

Date: 2026-07-25  
Scope: normal `US_Release|Win32`; static declaration and source-use comparison.
No compilation, linking, or binary execution.

## Decision

The historical preservation route must compile against the complete
**October 2004 D3DX SDK version 22 header family** and link against the matched
October 2004 VC6 libraries.

The local June 2010/version-43 D3DX headers must not remain active on that
route. Mixing them with October's `d3dx9dt.lib` is ABI-unsafe even if the
compiler and linker report success.

Confidence: **99%**.

The v43 route remains a separate modernization experiment using the matching
`d3dx9_43` import library/runtime and excluding `d3dx9dt.lib`.

## Why this is an ABI issue

The local header family declares:

```text
D3DX_SDK_VERSION 43
```

The recovered October family declares:

```text
D3DX_SDK_VERSION 22
```

This is not merely a version-check constant. Public COM-style interfaces
changed between these releases.

### Active `ID3DXEffect` incompatibility

Between v22 and v43:

- `IID_ID3DXBaseEffect` changed;
- `IID_ID3DXEffect` changed;
- `IID_ID3DXEffectCompiler` changed;
- `SetPixelShader` and `SetVertexShader` were removed from the base-effect
  method sequence;
- `DeleteParameterBlock` and `SetRawValue` were added later.

Removing methods from the inherited interface changes the vtable indices of
later methods. The player actively calls:

```text
FindNextValidTechnique
SetTechnique
Begin
Pass
SetMatrix
SetFloat
SetFloatArray
SetBool
SetVector
SetTexture
End
```

Several of these occur after the changed base-interface slots. Code compiled
with v43 declarations but receiving a v22 object can call the wrong function
through the vtable. This is a runtime-corruption risk that a normal link cannot
detect.

The water renderer actively uses `LPD3DXEFFECT`; this is not an unused header
difference.

## Used API coverage

A focused scan of player `.cpp` files found 90 D3DX-prefixed identifiers.
The October v22 headers contain every actual SDK declaration used.

Five names not found as October SDK declarations are not missing SDK APIs:

| Name | Explanation |
|---|---|
| `D3DXLineEquation` | Local `XMeshMath` function |
| `D3DXVec3Identity` | Local `XMeshMath` function |
| `D3DXQuaternionAxisToAxis` | Local `d3dutil.cpp` helper |
| `D3DXQuaternionUnitAxisToUnitAxis2` | Local `d3dutil.cpp` helper |
| `D3DXVECTOR` | Occurs in a comment |

The October VC6 `d3dx9dt.lib` separately covers all 51 identified external
D3DX calls.

## Relevant declaration findings

| Area | v22/v43 finding | Player impact |
|---|---|---|
| Core vectors/matrices/planes/quaternions | Required types and functions exist in v22 | Compatible source surface |
| `D3DXFONT_DESC` and font functions | Required declarations exist | Compatible source surface |
| Sprite/render-to-surface/buffer interfaces | Required interfaces exist | Must use matching header/library family |
| Effect interface | GUID and vtable changes | Hard prohibition on mixed versions |
| Half-float minimum exponent | Constant changed from `-12` to `-14` | No active player use found |
| Later UV atlas/IMT/tangent APIs | v43 additions | Not used by player source |
| Later shader/effect “Ex” functions and flags | v43 additions | Not used |
| DDS mip-skip macros | v43 additions | Not used |

The player gains no statically identified required API from v43.

## Lineage interpretation

The original VC6 project names `d3dx9dt.lib` before `d3dx9.lib`. The supplied
reference executable contains October-era static debug D3DX implementation
paths and assertions. This is direct evidence for the v22/October lineage.

The local v43 headers and a converted developer path naming the June 2010 SDK
are later maintenance/transplant evidence. They do not describe the header
generation that produced the reference executable.

## Include-order consequence

The ten local headers reside under `XKernel`, which is an active include
directory. Quoted/local inclusion can cause them to win even if the October SDK
include path is added first.

For the future preservation patch:

1. preserve the current v43 files as reference evidence;
2. replace the active local D3DX header family with the hash-pinned October
   v22 family, or otherwise guarantee that no local v43 header can resolve;
3. use all headers from one SDK generation;
4. retain October VC6 `d3dx9dt.lib` before October `d3dx9.lib`;
5. verify `D3DX_SDK_VERSION == 22` in preprocessed output;
6. inspect the link map to prove which D3DX members are selected.

Three local files were already byte-identical to October:

```text
d3dx9anim.h
d3dx9shape.h
d3dx9xof.h
```

Seven differ and must not stay active:

```text
d3dx9.h
d3dx9core.h
d3dx9effect.h
d3dx9math.h
d3dx9mesh.h
d3dx9shader.h
d3dx9tex.h
```

Using the complete ten-file October family avoids hidden cross-header
dependencies.

## Separate modernization route

If a v43 maintenance experiment is later authorized:

- retain the complete v43 headers;
- use the June 2010 x86 `d3dx9.lib`;
- package the legitimate `d3dx9_43.dll` runtime;
- remove `d3dx9dt.lib`;
- label the result as behaviorally different from the historical reference.

That route must not be used to validate the preservation route.

## Final gate

The D3DX generation choice is now resolved. Before a controlled build, the
remaining work is mechanical verification:

- file-by-file October header hashes;
- explicit include resolution;
- preprocessed version proof;
- verbose link/map proof.

No further D3DX version guess is required.
