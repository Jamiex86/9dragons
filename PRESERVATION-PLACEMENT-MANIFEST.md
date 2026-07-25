# Normal US Player Preservation Placement Manifest

Date: 2026-07-25  
Status: planning manifest only; **no files placed and no build performed**

## Rules

Every future restoration action must:

1. verify the source SHA-256 before copying;
2. copy only to the destination recorded here;
3. keep evidence/quarantine inputs immutable;
4. record the approving decision for proprietary material;
5. verify the destination hash after copying;
6. refuse mixed versions or unlisted substitutes;
7. keep build-time inputs separate from runtime packaging.

The authoritative source archive remains:

```text
/Users/jamiemarshall/Downloads/EmperorOfDragons.zip
SHA-256: 4b2daf495fdc7aa5a13f7ee0f784157c2a3877deb12a9923d5c5ac9f10a6bda7
```

## Placement states

| State | Meaning |
|---|---|
| `APPROVED-TECHNICAL` | Exact or strongly matched technical input; placement still requires the stated gate |
| `GENERATED` | Must be produced by the future controlled build, never copied as source |
| `PROVENANCE-HOLD` | Technically suitable but no promotion until legal/provenance decision |
| `RUNTIME-HOLD` | Verified runtime input; no placement until packaging manifest approval |
| `DATA-HOLD` | Verified data input; preserve location/hash and stage only as a complete runtime set |
| `EXCLUDE` | Must not be placed in the normal US player |

## Exact source restoration

| Source identity | SHA-256 | Future destination | State | Gate |
|---|---|---|---|---|
| Authoritative archive member `XKernel/XSecurity/XCrypto.Cpp` | `c78a0ce9eb0285c584a34c70ee637397417568cf4c975973cce443933da4160f` | `XKernel/XSecurity/XCrypto.Cpp` | `APPROVED-TECHNICAL` | Extract afresh from the authoritative archive; do not depend on the temporary extraction path |

No other unavailable normal-player source/header/resource is currently known.

## Generated internal outputs

These are future outputs, not files to acquire:

| Project | Generated output |
|---|---|
| `XGamebase` | `Library/US/XGamebase.lib` |
| `XKernel` | `Library/US/XKernel.lib` |
| `XFControl` | `Game/US/XFControl.dll` |
| `XFControl` | `Library/US/XFControl.lib` |
| Main executable | `Game/US/NineDragons.exe` |

Do not satisfy these paths with unrelated regional binaries.

## Flash/XFControl inputs

| Evidence source | SHA-256 | Future destination | State | Gate |
|---|---|---|---|---|
| Quarantined exact-lineage decoder | `3d27b37d6bf9ba95d8d3f5e1c75c1256b0759f76f6980627dbd936f25bd9558c` | Proposed `Library/FlashMP3/mp3decoder.lib` | `PROVENANCE-HOLD` | Documentary permission, otherwise use a clean-room 11-function adapter |
| Supplied reference `XFControl.dll` | `ab2716c0abf5bc5c73e4ae32528cc5bf874b446ab46de93d9cdfaa95ae6c29ec` | Comparison oracle; optional preservation runtime `Game/US/XFControl.dll` | `RUNTIME-HOLD` | Decide rebuild versus preserved-binary route and redistribution authority |
| Supplied Flash pack | `ccd1480303c4c99af94a71e64a3ead6f095b21628205744d2a99ced6142834c5` | `Game/US/Data/Script/XSCENESCRIPT.XP` relative to executable root | `DATA-HOLD` | Stage through complete Data manifest |

`XFControl` must remain `/MD`; retain `MSVCRT` and suppress `LIBCMT` only.

## SpeedTree build/runtime set

These three files are one indivisible version set:

| Evidence source | SHA-256 | Future destination | State | Gate |
|---|---|---|---|---|
| Candidate `SpeedTreeRT.h` | `b5f8ad8e94e22c2df8ab590bde504245fe2eb21941b01cf0282778fdb2ea5543` | `Library/SpeedTreeRT/SpeedTreeRT.h` | `PROVENANCE-HOLD` | SDK authority |
| Candidate `SpeedTreeRT.lib` | `bfaf506d886195d1cb1330c04ac1fc6ba60da553217da0069812571f52ce620f` | `Library/SpeedTreeRT/SpeedTreeRT.lib` | `PROVENANCE-HOLD` | SDK authority |
| Supplied `SpeedTreeRT.dll` | `ff3c3f003d943f53f1718c2fe36446faab2617a500e8a60e8f6ad83deb03a4c3` | `Game/US/SpeedTreeRT.dll` | `RUNTIME-HOLD` | Package authority and approved manifest |

Do not reproduce the authorization value in manifests. Verify it by its
full-value SHA-256 only:

```text
95b4b17918ebd2b167f6bbd95a601f996d7029b3eaef2f7f5568ac5f311d02a4
```

Required data:

| SHA-256 | Future runtime path | State |
|---|---|---|
| `9161ab992ab4551815f182469f5d2fc93d192cb2eaee4f9a5503983d2d5469c1` | `Data/Model/MR_STMODEL.XP` | `DATA-HOLD` |
| `6ad9f72d77ee5347b3a2ce2e9471a49d573e776eaecdebfcfde08640f9afe7bf` | `Data/Texture/TR_SSTMODEL.XP` | `DATA-HOLD` |
| `62e13bccc1d40f45b7df0e7bc2ed59a76bdbc11e19a32f2484056ead0fb8862c` | `Data/Texture/STModel.xrs` | `DATA-HOLD` |
| `6e609b764f8a3224faa6f2f213fcf5b48973ddb61f32ebfad2e1428ae8d84ab2` | `Data/Script/SCR_BASE.XP` | `DATA-HOLD` |

## QHTM

| SHA-256 | Future destination | State |
|---|---|---|
| Header `3ca9196b83b38299dad870febd767b9e8f1de3fbc3441e651b8658b342e68cd0` | `Library/QHTM/QHTM.h` | `APPROVED-TECHNICAL` |
| Import library `b3d9d5badcceb19e90fa8ced0366558de87f998f561f865b863a6f85bdac2f67` | `Library/QHTM/QHTM.lib` | `APPROVED-TECHNICAL` |
| DLL `9e622b880d9afc7076a8c008272528bfea21ec3d2e95b354795246cd48b29e53` | `Game/US/QHTM.dll` | `RUNTIME-HOLD` |

The DLL/import library is an exact 16-ordinal pair.

## XWebPage

| SHA-256 | Future destination | State |
|---|---|---|
| Header `6f59e531b766b0619d22c1d294cdadb8e4ede55e334aa13eb1c463f491c5e4c3` | `Library/CWebPage/XWebPage.h` | `APPROVED-TECHNICAL` |
| Import library `b7d761dc7449438fbc445c302900148826ee02ebe58bf6e7b81df2f14921b4c2` | `Library/CWebPage/XWebPage.lib` | `APPROVED-TECHNICAL` |
| DLL `23f5f9b55b64971481e3e8495e212c417e454f028a28900844b03d53be6a7562` | `Game/US/XWebPage.dll` | `RUNTIME-HOLD` |

The pair matches all six exports. Historical web endpoints are not accepted as
future service configuration.

## FMOD

| SHA-256 | Future destination | State |
|---|---|---|
| Header `0188c2c5718f9f227bccc765d9a51f397716d34c003142a81be5ea8a96c7d526` | `Library/FMod/fmod.h` | `APPROVED-TECHNICAL` |
| Import library `560aaa3e34d3fa2ced9fbdac35486595bc53f65922e6a11667633b819253b6ad` | `Library/FMod/fmodvc.lib` | `APPROVED-TECHNICAL` |
| DLL `0287ce690f314826dd6ed82e77a64b518376308f4b4eeb50b73e4f2142576aec` | `Game/US/fmod.dll` | `RUNTIME-HOLD` |
| Sound pack `e43b0b3e17a98401f2cd8ffcd74e6686b49b06e9c74236953586afb9f6ebd4f0` | `Game/US/Data/Sound/SR_SOUND.XP` relative to executable root | `DATA-HOLD` |

All 229 import-library functions are satisfied. Do not substitute FMOD Ex or
FMOD Studio.

## DbgHelp

| SHA-256 | Future destination | State | Gate |
|---|---|---|---|
| `33dab36fbc5532c9486ec1de2cb7891516b22453b12ea1193fb597042efb3236` | `Library/dbghelp.lib` | `APPROVED-TECHNICAL` | Confirm preservation Platform SDK/runtime pairing |

## DirectX October 2004 VC6 set

Quarantine root:

```text
candidate-dependencies/DirectX9-October2004-untrusted/Coherent-VC6-October2004
```

The set contains 184 headers and 50 x86 libraries in the original acquisition;
the preserved coherent directory must be treated as a unit. Key pins:

| Input | SHA-256 |
|---|---|
| `Include/d3dx9.h` | `22090a940c1a24ff99205d793ae577e62e700408da3f5925ba17c3739bca2e3a` |
| `Lib/dxguid.lib` | `42a1585a635968c65ab9d03b8bcd3e61e72bcf821bed1c1a0a34456480522d47` |
| `Lib/d3d9.lib` | `461a9baf9d7a3e6ce4b04ef7814296c0fcad36ec77deb7566b361cbdabf6e3d7` |
| `Lib/d3dx9dt.lib` | `d9743322b0b814351ec76b331e67e3f0c007d44c8c94bab2b9192ee8f496f68d` |
| `Lib/d3dx9.lib` | `1ee03173ca703e514e129aab42c32c451044a2f252080a3cab5cd5fb416f73ce` |
| `Lib/dsound.lib` | `3149b985832bd620623630ac45a7dee69aa9fb61cba3a2991f438c360d2da449` |
| `Lib/dinput8.lib` | `1aefd62d9df8a26aa312817bd98b2fc3340477b238c918fc9edc04f7498a5b34` |
| `Lib/DxErr9.lib` | `0dcf72695c4dc029768d2d8fadb5d61d56dcf896bb9683dc936f8a046c608495` |
| `Lib/d3dxof.lib` | `6e02101d54f570d2bbb985292f4c6f0db105b067d32e865991a698faa78a34e6` |

Future destination is an isolated, immutable toolchain/dependency root—not the
Windows global SDK directories. The project must point explicitly to its
`Include` and `Lib`.

Do not combine the local v43 D3DX headers with the October v22 static
`d3dx9dt.lib`. The preservation decision is now the complete October v22
header family plus the matched October VC6 libraries. The seven differing
local headers must not remain active. See `D3DX-V22-V43-USED-ABI-DECISION.md`.

## Explicit exclusions

Do not place these in the normal-player build/runtime:

| Item | State | Reason |
|---|---|---|
| `Library/radsdk/radsdk6.lib` | `EXCLUDE` | IGA feature is inactive |
| `Library/BinkSDK/binkw32.lib` | `EXCLUDE` | IGA/Bink feature is inactive |
| `Game/US/binkw32.dll` | `EXCLUDE` | Reference player has no Bink import |
| GM window source | `EXCLUDE` from normal target | Separate GM build |
| Legacy anti-cheat SDKs/DLLs | `EXCLUDE` | Deliberately stripped |
| Regional `*_GM.lib` or regional engine binaries | `EXCLUDE` | Not normal US dependencies |

## Approval gates

### Gate 1 — source recovery

- Re-extract and hash-verify `XCrypto.Cpp`.
- Confirm the destination tree baseline hash.
- Apply only the reviewed player-isolation patch set.

### Gate 2 — dependency promotion

- Resolve Flash decoder and SpeedTree authority.
- Generate a complete file-by-file hash manifest for the DirectX directory.
- Approve runtime DLL redistribution/usage.
- Never promote from an unverified Downloads filename alone.

### Gate 3 — controlled build authorization

- Freeze VC6 SP6 x86, linker version, Platform SDK, include order, and library
  order.
- Resolve the D3DX v22/v43 header conflict.
- Require verbose linker output and map files.

### Gate 4 — runtime packaging authorization

- Place DLLs beside `Game/US/NineDragons.exe`.
- Place the complete Data directory relative to the executable as expected by
  `gModulePath`.
- Re-hash the assembled runtime tree before any controlled start.

This manifest does not authorize any of those actions; it defines what must be
verified if they are later authorized.
