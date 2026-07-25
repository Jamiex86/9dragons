# SpeedTreeRT wrapper fit audit

Date: 25 July 2026

Scope: static comparison of the original Emperor of Dragons source against the
quarantined Multiverse header/import library and the user-supplied 2006 runtime.
No compilation or execution was performed.

## Verdict

The candidate set is a **very high-confidence technical fit** for the normal
player client's SpeedTree integration:

- all **42 distinct SpeedTree methods** called by `XSTreeWrapper` are declared
  by the candidate header;
- all **31 SpeedTree geometry/texture fields** directly read by the wrapper are
  present;
- the import library's **146 decorated imports** equal the DLL's **146
  decorated exports**, with no differences after import-alias normalization;
- the three later ABI changes that distinguish this family from the Metin2
  2004 set agree with the supplied DLL;
- the source's authorization call resolves to the candidate library and DLL.

No source-level SpeedTree mismatch was found. The remaining blockers are
licensing/provenance and packed-tree asset validation.

## Candidate identities

| File | SHA-256 |
|---|---|
| `SpeedTreeRT.h` | `b5f8ad8e94e22c2df8ab590bde504245fe2eb21941b01cf0282778fdb2ea5543` |
| `SpeedTreeRT.lib` | `bfaf506d886195d1cb1330c04ac1fc6ba60da553217da0069812571f52ce620f` |
| `SpeedTreeRT.dll` | `ff3c3f003d943f53f1718c2fe36446faab2617a500e8a60e8f6ad83deb03a4c3` |

## Wrapper API coverage

The audit found 42 distinct called methods:

```text
Compute
ComputeLodLevel
ComputeWindEffects
DeleteTransientData
GetBoundingBox
GetBranchMaterial
GetCurrentError
GetFrondMaterial
GetGeometry
GetLeafBillboardTable
GetLeafLightingAdjustment
GetLeafLodSizeAdjustments
GetLeafMaterial
GetNumBranchLodLevels
GetNumFrondLodLevels
GetNumLeafLodLevels
GetTextures
GetTreePosition
LoadTree
MakeInstance
SetBranchLightingMethod
SetBranchWindMethod
SetCamera
SetDropToBillboard
SetFrondLightingMethod
SetFrondWindMethod
SetLeafLightingMethod
SetLeafRockingState
SetLeafWindMethod
SetLightAttributes
SetLightState
SetLocalMatrices
SetLodLevel
SetLodLimits
SetNumLeafRockingGroups
SetNumWindMatrices
SetTextureFlip
SetTime
SetTreePosition
SetTreeSize
SetWindMatrix
SetWindStrength
```

Candidate declaration coverage: **42/42**.

The wrapper uses the public `GetGeometry(...)` function, whose full decorated
signature is present in both the import library and DLL. It does not directly
call the private `GetLeafGeometry` or `Get360BillboardGeometry` functions that
changed between the earlier and later 1.6 headers. Those changes nevertheless
remain important because they identify the correct complete class ABI.

## Structure and field coverage

The wrapper reads 31 candidate-owned fields across `SGeometry`, `SIndexed`,
`SLeaf`, `SBillboard` and `STextures.`

```text
m_bIsActive
m_fAlphaTestValue
m_fBranchAlphaTestValue
m_fFrondAlphaTestValue
m_nDiscreteLodLevel
m_pBranchTextureFilename
m_pCenterCoords
m_pColors
m_pCoords
m_pLeafClusterIndices
m_pLeafMapCoords
m_pLeafMapTexCoords
m_pNormals
m_pSelfShadowFilename
m_pStripLengths
m_pStrips
m_pTexCoords
m_pTexCoords0
m_pTexCoords1
m_pWindMatrixIndices
m_pWindWeights
m_sBillboard0
m_sBillboard1
m_sBranches
m_sFronds
m_sHorizontalBillboard
m_sLeaves0
m_sLeaves1
m_usLeafCount
m_usNumStrips
m_usVertexCount
```

Candidate field coverage: **31/31**. The compared 2004 and later headers do not
change the layout of these public structures. Their meaningful differences are
class implementation members, added horizontal-billboard support state and
several method signatures.

## Build and macro behaviour

`US_Release|Win32`:

- links the unqualified input `SpeedTreeRT.lib`;
- includes the project root and `XSTreeWrapper`, so the intended historical
  placement was a header reachable as `SpeedTreeRT.h` and a library reachable
  through a global/project library search path;
- builds as Win32 with `/MD` (`MultiThreadedDLL`);
- does not define `SPEEDTREERT_DYNAMIC_LIB` or
  `IDV_SPEEDTREERT_EXPORTS`.

With no dynamic-library macro, the header leaves its class decoration empty.
The wrapper therefore emits ordinary decorated external references rather than
requiring `__declspec(dllimport)` at each call. The candidate import library
contains the corresponding direct import thunks for the wrapper-used methods,
as well as `__imp_` aliases. This is consistent with the project's historical
configuration; adding `SPEEDTREERT_DYNAMIC_LIB` is not currently justified.

The runtime imports `MSVCP60.dll` and `MSVCRT.dll`, confirming the expected VC6
C++ ABI and CRT generation. Modernizing this boundary without isolating it
would introduce allocator, exception and object-layout risk.

## Authorization path

The normal client calls `CSpeedTreeRT::Authorize(...)` from
`XProc_FirstLoad::InitializeSpeedTreeWrapper()`. One product key is active and
two historical alternatives are present as comments. The function then returns
success unconditionally:

- there is no source call to `IsAuthorized()`;
- failure is therefore deferred to later SpeedTree operations and
  `GetCurrentError()` handling;
- the matching `Authorize(const char*)` symbol exists in both candidate import
  library and supplied DLL;
- the DLL imports no registry API, so this build appears to validate the
  supplied authorization string internally rather than reading a registry
  licence.

The keys are intentionally omitted from this report. They are proprietary
licensing material already present in the source and must not be borrowed,
published or silently repurposed. Technical presence does not establish
current deployment rights.

## Asset path

The wrapper does not load loose `.spt` files from the filesystem. It:

1. opens `MR_STMODEL.XP` (`tsMR_STMODEL.XP` in the test-server branch);
2. reads each tree model as a memory block through `_XPackageArchive`;
3. calls `CSpeedTreeRT::LoadTree(const unsigned char*, unsigned int)`;
4. loads composite/self-shadow texture resources through related package
   archives.

No loose `.spt` files or `MR_STMODEL.XP` data pack are present in the source
archive. Asset compatibility cannot be proven until the user's known-good
runtime data is inventoried. The exact required first target is
`MR_STMODEL.XP`.

## Controlled placement plan

Do not alter the active project yet. For a later controlled-build preparation:

```text
source root/SpeedTreeRT.h
project library search path/SpeedTreeRT.lib
Game/US/SpeedTreeRT.dll
Game/US/Data/.../MR_STMODEL.XP (according to the reference installation layout)
```

The final physical library location should be made explicit in a preservation
property sheet or project dependency path rather than relying on an
unrecorded machine-global library directory.

## Remaining gates

1. Establish lawful entitlement to use and redistribute this SpeedTree
   generation and the original EOD authorization.
2. Inventory the reference `MR_STMODEL.XP` and identify its package entry
   names, sizes and hashes without executing the client.
3. Confirm that the supplied DLL accepts representative packed tree blobs and
   produces the expected error-free metadata only in a later controlled
   runtime phase.
4. Keep the VC6 runtime boundary stable for preservation; do not substitute a
   modern SpeedTree SDK.

Subject to those gates, SpeedTree is no longer a missing-code or missing-linker
dependency.
