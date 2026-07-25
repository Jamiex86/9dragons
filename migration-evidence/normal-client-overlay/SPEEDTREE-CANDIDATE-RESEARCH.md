# SpeedTreeRT candidate research

## 25 July 2026 update: exact revised-ABI development pair located

A materially stronger candidate than the Metin2 2004 set has been located in
the public `multiversemmo/MultiversePlatform` repository:

- `lib/SpeedTree/include/SpeedTreeRT.h`
- `lib/SpeedTree/lib/Release/SpeedTreeRT.lib`

Source:

`https://github.com/multiversemmo/MultiversePlatform/tree/7e1aad33d48b9e47f3db2ca638cb57592336ddb7/lib/SpeedTree`

Static hashes of the inspected candidates:

```text
SpeedTreeRT.h
b5f8ad8e94e22c2df8ab590bde504245fe2eb21941b01cf0282778fdb2ea5543

SpeedTreeRT.lib
bfaf506d886195d1cb1330c04ac1fc6ba60da553217da0069812571f52ce620f
```

The header still identifies itself as SpeedTreeRT 1.6.0, but it contains the
later declarations encoded by the user's 2006 DLL:

```cpp
SetNumWindMatrices(int nNumMatrices);
GetLeafGeometry(SGeometry& sGeometry, short sOverrideLodValue = -1);
Get360BillboardGeometry(SGeometry& sGeometry, unsigned long ulBitVector);
```

Most importantly, the candidate release import library contains **146 unique
`CSpeedTreeRT`/nested-type imports**, and the user's DLL exports exactly the
same 146 decorated names:

```text
imports absent from supplied DLL: 0
supplied DLL exports absent from import library: 0
```

The library names `SpeedTreeRT.dll` and is a 32-bit COFF import library. Its
embedded source-DLL timestamp is 10 March 2005, whereas the supplied DLL is
timestamped 18 January 2006. The binaries are therefore not proven to originate
from one build, but their complete decorated export equality makes the import
library ABI-compatible with the supplied DLL.

The same revised header was independently found in
`marciohandrade/Sun2`; it differs from the Multiverse header only in its include
guard. Sun2 also contains matching static-library variants. This independent
duplication strengthens the version-family identification.

Revised technical assessment:

- header/API match: **99%+**, high confidence;
- import-library-to-supplied-DLL ABI match: **effectively exact at the exported
  symbol boundary**, high confidence;
- provenance/licensing and behavioural/asset compatibility: still unproven.

The earlier Metin2 2004 set remains useful historical evidence, but it is no
longer the preferred technical candidate.

## Result

A technically excellent candidate pair has been located:

- `SpeedTreeRT.h`, release **1.6.0**, dated **19 December 2003**;
- `SpeedTreeRT.lib`, a 32-bit COFF import library for `SpeedTreeRT.dll`;
- paired `SpeedTreeRT.dll`, PE32 x86, timestamped **9 January 2004**.

Technical match estimate: **97–99%**, high confidence.

Legal usability: **not established**. The header explicitly labels the material
IDV proprietary information supplied under a licence or nondisclosure
agreement. Public repository availability does not provide Heavenfall with
rights to use or redistribute it.

No candidate file has been added to the recovery overlay.

## Public locations

- Header:
  `https://git.old-metin2.com/eXploit/client/src/commit/0e97f74c94399d417fce380f91976e2c8e838ecb/extern/include/SpeedTreeRT.h`
- Import-library directory:
  `https://git.old-metin2.com/metin2/client/src/branch/master/extern/lib`
- Paired runtime directory:
  `https://git.old-metin2.com/metin2/client/src/commit/1e96dae60b9a26f1c584e08b42d9cd54ec233685/bin`

The files were downloaded only into `/tmp` for static inspection.

## Candidate hashes

```text
SpeedTreeRT.h
262dce2d2ef6e1172de90413f4b3eadfad1385b4a569646077ecfc50de3c2e7b

SpeedTreeRT.lib
beb0dcd40e7ee46158b6291b72d51a66c106d46dd87f6227802e17c9836be7e8

SpeedTreeRT.dll
ddba9b9b427d541ebc0bf1221fffc5d56a85d7b8ee0dfe6370a83a133da6967b
```

## Version evidence

The header identifies itself as:

```text
Release version 1.6.0 (December 19, 2003)
Copyright (c) 2001-2003 IDV, Inc.
```

This aligns with the EOD wrapper evidence:

- `XSTWrapper.h` identifies the wrapper as IDV example code from 2003;
- the included SpeedWind components carry 2002–2004 IDV dates;
- both use the same early `CSpeedTreeRT` object model;
- EOD targets Direct3D 9 and an original VC6-era toolchain.

The paired DLL:

- is PE32 Intel 80386;
- has a 9 January 2004 PE timestamp;
- imports `MSVCP60.dll` and `MSVCRT.dll`;
- therefore belongs to the correct VC6/Win32 period.

## Source API comparison

Every SpeedTree method called by the EOD wrapper is declared by the candidate
1.6.0 header: **42 of 42**.

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

The wrapper accesses 30 fields in `SGeometry`, `SIndexed`, `SLeaf`,
`SBillboard` and `STextures`. All 30 exist in the candidate header, including
the particularly version-sensitive fields:

```text
m_pCenterCoords
m_pLeafMapCoords
m_pLeafMapTexCoords
m_sHorizontalBillboard
m_pWindMatrixIndices
m_pWindWeights
m_pSelfShadowFilename
```

This is much stronger evidence than merely sharing the `CSpeedTreeRT` class
name.

## Binary symbol comparison

The `.lib` is not a self-contained static implementation. It is a Microsoft
COFF i386 import library naming:

```text
SpeedTreeRT.dll
```

All 42 methods used by EOD appear in the import-library symbol table. All 42
also appear in the paired DLL export table with VC6-style decorated C++ names.

Examples:

```text
?Compute@CSpeedTreeRT@@QAE_NPBMI_N@Z
?GetGeometry@CSpeedTreeRT@@QAEXAAUSGeometry@1@KFFF@Z
?SetNumWindMatrices@CSpeedTreeRT@@SAXI@Z
```

The paired DLL exports all 131 direct `CSpeedTreeRT`/nested-structure symbols
represented by the import library. The apparent extra import-library names are
only `__imp_` thunk aliases.

## Important runtime implication

Using this `SpeedTreeRT.lib` requires the matching `SpeedTreeRT.dll`. The user
has indicated that runtime DLLs are available from an existing client.

The next best verification is to hash and inspect that existing client's
`SpeedTreeRT.dll`:

1. if its SHA-256 is
   `ddba9b9b427d541ebc0bf1221fffc5d56a85d7b8ee0dfe6370a83a133da6967b`,
   the candidate set is byte-identical;
2. otherwise compare its PE timestamp, imports and complete decorated export
   set against the candidate import library;
3. reject it if the `GetGeometry`, constructors/destructors or nested structure
   symbols differ, because those differences imply ABI mismatch.

No `SpeedTreeRT.dll`, `.lib` or `.h` was found under the current Heavenfall
workspace or Downloads directory during this search.

## Recommendation

Technically, preserve the candidate hashes and repository commits as the leading
SpeedTreeRT 1.6.0 match. Do not integrate the files until:

- the existing-client DLL has been compared;
- provenance and usage rights have been established;
- the exact header/library/DLL trio is approved as one indivisible ABI set.

For a clean legal route, contact the current SpeedTree rights holder/support
with the version string, original game provenance, and hashes, requesting
confirmation or a legacy-runtime licence. Modern SpeedTree SDKs are not
drop-in-compatible with the 1.6.0 API.
