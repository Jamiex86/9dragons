# US Normal Player SpeedTree End-to-End Closure

Date: 2026-07-25  
Scope: static inspection only. No compilation, linking, DLL loading, or client
execution.

## Conclusion

The normal player's SpeedTree subsystem is **technically high-likelihood and
substantially complete**, not a missing-SDK or missing-data blocker.

Confidence in binary/API/data lineage: **98–99%**.  
Confidence in correct runtime rendering without a controlled test: **85–95%**.

The remaining issues are:

- provenance and redistribution authority for the legacy SDK;
- two stale model names with obvious packaged counterparts;
- three inferred missing branch textures affecting six model variants;
- runtime proof of authorization, serialized SPT compatibility, rendering,
  wind, LOD, and device reset.

## Binary and ABI closure

| Item | SHA-256 | Static result |
|---|---|---|
| Candidate `SpeedTreeRT.h` | `b5f8ad8e94e22c2df8ab590bde504245fe2eb21941b01cf0282778fdb2ea5543` | Covers 42/42 called methods and 31/31 directly used fields |
| Candidate `SpeedTreeRT.lib` | `bfaf506d886195d1cb1330c04ac1fc6ba60da553217da0069812571f52ce620f` | Exact import mate |
| Supplied/candidate `SpeedTreeRT.dll` | `ff3c3f003d943f53f1718c2fe36446faab2617a500e8a60e8f6ad83deb03a4c3` | Exact 146/146 function/data match |

The supplied DLL is byte-identical to the quarantined candidate DLL. The
reference `NineDragons.exe` imports `SpeedTreeRT.dll` and visibly references
`CSpeedTreeRT::SetWindStrengthAndLeafAngles`.

SpeedTree is a C++ DLL boundary. Exact header structure and compiler ABI remain
important even though the import library itself is valid COFF x86. The matched
candidate set should be kept together rather than mixing another SpeedTree
header or DLL revision.

## Authorization

The active source authorization value is byte-identical to the sole
authorization-shaped value in the reference executable:

```text
SHA-256: 95b4b17918ebd2b167f6bbd95a601f996d7029b3eaef2f7f5568ac5f311d02a4
```

The secret value is intentionally not repeated here. This proves reference
lineage, not legal redistribution authority.

`InitializeSpeedTreeWrapper()` calls `CSpeedTreeRT::Authorize()` but always
returns true. It does not verify authorization success. An authorization/DLL
mismatch can therefore appear later as model-load or geometry-compute failure.

## Required data closure

| Runtime path | SHA-256 | Result |
|---|---|---|
| `Data/Model/MR_STMODEL.XP` | `9161ab992ab4551815f182469f5d2fc93d192cb2eaee4f9a5503983d2d5469c1` | Valid XP; 524 SPT models |
| `Data/Texture/TR_SSTMODEL.XP` | `6ad9f72d77ee5347b3a2ce2e9471a49d573e776eaecdebfcfde08640f9afe7bf` | Valid XP; 144 textures |
| `Data/Texture/STModel.xrs` | `62e13bccc1d40f45b7df0e7bc2ed59a76bdbc11e19a32f2484056ead0fb8862c` | Present |
| `Data/Script/SCR_BASE.XP` | `6e609b764f8a3224faa6f2f213fcf5b48973ddb61f32ebfad2e1428ae8d84ab2` | Contains SpeedTree list/wind configuration |

`SCR_BASE.XP` contains:

- `SpeedTreeList.ini`: 524 configured trees;
- `SpeedWind.ini`: all 58 parser tokens, no unknown or duplicate keys;
- `SpeedTreeLUT.ini`.

All 45 configured composition maps exist. Loose `.spt` files are not expected;
the wrapper reads each model from `MR_STMODEL.XP` and passes its memory payload
to `CSpeedTreeRT::LoadTree`.

## Localized discrepancies

Two configured names lack literal matches:

```text
Myrtle_RT_987.spt
Myrtle_RT_987_B.spt
```

The model pack contains exactly two otherwise-unreferenced counterparts:

```text
004_Crape Myrtle_RT_987.spt
004_Crape Myrtle_RT_987_B.spt
```

This is 99% likely to be stale configuration naming, not absent model data.
Do not rename anything until compared with a known-good data generation.

Three inferred branch textures are absent across six base/variant models:

```text
RoseBushStem.tga
SaguaroCactus_Skin.tga
AzaleaFrond.tga
```

This is a localized rendering defect. It may produce missing or incorrect
branches/fronds for those six variants; it does not prevent DLL loading or
general client startup.

## Failure behavior

| Stage | Failure consequence |
|---|---|
| Missing `SpeedTreeRT.dll` | Windows loader prevents process startup |
| Missing/wrong import library | Main executable cannot link |
| Authorization mismatch | Not detected by initialization; likely deferred model/compute failures |
| Missing `SpeedTreeList.ini` or invalid `SpeedWind.ini` | Template load returns false, but one caller ignores the return; later tree loading may fail |
| Missing model/texture pack | Zone/terrain loading returns false |
| Missing individual SPT or template entry | Terrain load returns false with a fatal diagnostic |
| Missing localized branch texture | Likely visual defect for affected trees |

The DLL is startup-critical. The model/texture packs are world-entry-critical
for terrain containing SpeedTree data rather than necessarily login-screen
critical.

## Preservation decision

For a first preservation baseline:

1. keep the exact header/import library/DLL together;
2. keep the reference-matching authorization value unchanged and restricted;
3. preserve all four data/configuration inputs by hash;
4. do not rename the Crape Myrtle entries or substitute the three textures
   without known-good evidence;
5. record the three localized textures as a visual fidelity issue, not a
   compilation blocker;
6. require later instrumentation of authorization, first `LoadTree`, first
   `Compute`, textures, wind, LOD, and device loss/reset.

Technically, SpeedTree no longer belongs on the critical missing-files list.
Its active remaining gate is provenance/licensing, followed by controlled
runtime verification.
