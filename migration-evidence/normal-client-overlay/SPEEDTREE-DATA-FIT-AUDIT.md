# SpeedTree Data Fit Audit

## Scope and method

This is a static inspection of the supplied `Data` directory. No executable,
client, packer, installer, or library was run. The XP archives were parsed from
the format implemented in the supplied `XKernel/XPackage.h` and
`XKernel/XUtilities/XPackage.cpp` source:

- header `0xFEFA`, version `0xA001`;
- filenames XOR-encoded with `0xEF`;
- absolute payload offsets;
- encrypted script payloads XOR-encoded with `0xA5`.

The repeatable audit script is `tools/audit_speedtree_data.rb`.

## Confirmed required files

| Runtime path | Size | SHA-256 | Finding |
|---|---:|---|---|
| `Data/Model/MR_STMODEL.XP` | 3,215,726 | `9161ab992ab4551815f182469f5d2fc93d192cb2eaee4f9a5503983d2d5469c1` | Valid XP; 524 SPT payloads |
| `Data/Texture/TR_SSTMODEL.XP` | 64,291,632 | `6ad9f72d77ee5347b3a2ce2e9471a49d573e776eaecdebfcfde08640f9afe7bf` | Valid XP; 144 texture payloads |
| `Data/Texture/STModel.xrs` | 14,336 | `62e13bccc1d40f45b7df0e7bc2ed59a76bdbc11e19a32f2484056ead0fb8862c` | Present |
| `Data/Script/SCR_BASE.XP` | 48,530,302 | not recorded in this focused check | Valid XP; 95 encrypted script payloads |

`SCR_BASE.XP` contains and successfully decodes:

- `SpeedTreeList.ini` — 32,630 bytes, declares 524 trees;
- `SpeedWind.ini` — 1,318 bytes, 71 lines of readable wind parameters;
- `SpeedTreeLUT.ini` — 1,143 bytes.

## Model-to-configuration reconciliation

`SpeedTreeList.ini` contains 524 unique `.spt` references.

- 522 names match `MR_STMODEL.XP` directly, case-insensitively.
- Two configured names do not match literally:
  - `Myrtle_RT_987.spt`
  - `Myrtle_RT_987_B.spt`
- The model pack has exactly two otherwise-unreferenced counterparts:
  - `004_Crape Myrtle_RT_987.spt`
  - `004_Crape Myrtle_RT_987_B.spt`

The shared `Myrtle`, `RT_987`, and base/`_B` pairing, together with the exact
two-for-two count, makes this a high-confidence stale-name mismatch rather than
missing tree data. It must be resolved before a faithful runtime test, preferably
by comparing the known-good client's package/configuration pair before changing
either file.

All 524 model payloads contain the legacy `__IdvSpt_02_` marker. The package
contains no duplicate SPT payload hashes.

## Texture reconciliation

`SpeedTreeList.ini` references 45 unique `.tga` composition maps. All 45 exist
in `TR_SSTMODEL.XP`, case-insensitively. This is the texture mapping the wrapper
actively applies to tree objects.

Earlier raw-string scans of embedded SPT leaf/self-shadow names must not be
treated as proof of missing runtime textures:

- the wrapper replaces leaf texture selection with composition-map entries from
  `SpeedTreeList.ini`;
- self-shadow texture loading is commented out in the supplied wrapper;
- branch texture references require a separate semantic check, because they are
  read from each SPT rather than from the tree list.

Thus the configured composition-map layer is complete.

### Branch textures

The wrapper actively calls `GetTextures()` and passes
`STextures::m_pBranchTextureFilename` to
`g_SpeedTreeTextureArchive.GetResourceIndex()`. Static inspection used the first
embedded TGA filename in each legacy SPT as the branch-texture field. This is a
high-confidence format inference: known examples identify bark/stem/skin in that
position, and all 524 SPT files have exactly one candidate in that position.
Definitive field-level proof would require documentation or the matching
SpeedTree parser.

- 524/524 SPT payloads yield an inferred branch reference.
- There are 103 unique inferred branch texture names.
- 100/103 exist in `TR_SSTMODEL.XP`.
- The missing names occur in only six models (three base/variant pairs):

| Missing inferred branch texture | Affected SPT models |
|---|---|
| `RoseBushStem.tga` | `018_PricklyPearCactusCluster_LD_372.spt`, `018_PricklyPearCactusCluster_LD_372_B.spt` |
| `SaguaroCactus_Skin.tga` | `018_SaguaroCactus_RT_558.spt`, `018_SaguaroCactus_RT_558_B.spt` |
| `AzaleaFrond.tga` | `020_Azalea_RT_54.spt`, `020_Azalea_RT_54_B.spt` |

None of those three names occurs in the filename table of any of the 21 supplied
XP archives. `TR_SSTMODEL.XP` does contain `AzaleaBark.tga`, but that is used by
different Azalea models and is not an exact substitute for `AzaleaFrond.tga`.
No substitution should be made without a known-good client or visual evidence.

This is a localized content defect, not a general SpeedTree data failure. Its
likely runtime effect is missing/invalid branch or frond rendering for those six
models, subject to the texture manager's failure behavior.

## Wind parser reconciliation

The decoded `SpeedWind.ini` was compared with the literal token table in
`XSTreeWrapper/SpeedWindParser.cpp`.

- parser expects 58 tokens;
- configuration contains 58 parameter lines and 58 unique keys;
- 58/58 expected keys are present;
- zero unknown keys;
- zero duplicate keys;
- zero non-numeric values.

The parser treats unknown tokens as warnings and then fails the overall load.
This configuration would not trigger that path based on its static structure.
Numeric range behavior is not validated by the parser itself, but the file is
structurally complete and source-compatible.

## Effect on the SpeedTree dependency verdict

The supplied data removes “missing SpeedTree model/configuration packs” as a
critical blocker. In conjunction with the separately audited header/import
library/DLL candidate—whose decorated exports match 146/146 and whose header
matches all wrapper calls—the SpeedTree candidate set is now substantially
stronger.

This still does not prove runtime compatibility. The remaining gates are:

1. reconcile the two Crape Myrtle names against a known-good client;
2. recover or verify the three missing inferred branch textures;
3. establish that the supplied DLL accepts the authorization/version context;
4. eventually test in a controlled 32-bit client process, only after the
   broader client dependency audit permits a build.

## Current classification

| Item | Status | Severity |
|---|---|---|
| SpeedTree model archive | Present and structurally valid | Closed |
| Configured SPT coverage | 522 exact + 2 high-confidence renamed counterparts | Low |
| Configured composition textures | 45/45 present | Closed |
| Wind configuration | Exact 58/58 parser-token match | Closed statically |
| Branch texture coverage | 100/103 unique inferred names present; 3 absent across 6 models | Medium-localized |
| SpeedTree SDK candidate ABI surface | 146/146 export/import match | Low pending runtime proof |
| Faithful runtime behavior | Not proven; no build or execution performed | Gate |

## Conclusion

The data folder is internally coherent with the supplied SpeedTree wrapper and
greatly reduces the likelihood that missing SpeedTree content is a critical
normal-player-client blocker. The only direct tree-list discrepancy is a
two-entry naming mismatch with obvious counterparts already present. It should
be treated as recoverable configuration drift, not missing proprietary content.
