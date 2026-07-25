# SpeedTree Authorization and Packed-Data Audit

## Scope and safety

This is a static, read-only examination of the normal player client source, the
user-supplied `NineDragons.exe`, and the supplied `Data` directory. No executable,
installer, project script, packer, anti-cheat component, library, or DLL was run.
No packed resource was written out or modified.

## Result

SpeedTree is no longer missing its normal-player model or texture data. The
supplied data contains the two package files named directly by the wrapper:

| Archive-relative runtime path | Size | SHA-256 | Static result |
|---|---:|---|---|
| `Data\Model\MR_STMODEL.XP` | 3,215,726 | `9161ab992ab4551815f182469f5d2fc93d192cb2eaee4f9a5503983d2d5469c1` | Valid EOD package; 524 `.spt` entries |
| `Data\Texture\TR_SSTMODEL.XP` | 64,291,632 | `6ad9f72d77ee5347b3a2ce2e9471a49d573e776eaecdebfcfde08640f9afe7bf` | Valid EOD package; 144 texture entries |
| `Data\Texture\STModel.xrs` | 13,914 | `62e13bccc1d40f45b7df0e7bc2ed59a76bdbc11e19a32f2484056ead0fb8862c` | Present; role is supporting SpeedTree resource/script data |

The absence of loose `.spt` files is therefore expected and is not a blocker.
`XSTreeWrapper/XSTWrapper.cpp` obtains a model payload from `_XPackageArchive`
and supplies it to `CSpeedTreeRT::LoadTree(const unsigned char *, unsigned int)`.

## Package-format proof

`XKernel/XPackage.h` defines the package marker `0xFEFA`, version `0xA001`, and
filename XOR key `0xEF`. `XKernel/XUtilities/XPackage.cpp::OpenPackage` reads:

1. an 8-byte little-endian header;
2. a length-prefixed, XOR-obfuscated filename table;
3. one absolute 32-bit file offset per entry;
4. the concatenated payloads.

Both supplied packs exactly conform:

| Check | `MR_STMODEL.XP` | `TR_SSTMODEL.XP` |
|---|---:|---:|
| Header marker/version | `FEFA/A001` | `FEFA/A001` |
| Declared entries | 524 | 144 |
| Decoded filenames | 524 | 144 |
| Strictly increasing offsets | 524/524 | 144/144 |
| First payload begins at computed index end | Yes, byte 18,744 | Yes, byte 3,688 |
| Final payload ends at physical EOF | Yes | Yes |

All 524 model filenames have the `.spt` extension. Payload inspection shows
SpeedTree serialization identifiers and referenced bark-texture names. The
model archive includes ordinary, seasonal, snow, low-detail, large, and
region-prefixed variants.

All 144 texture-table filenames have the `.tga` extension. Some entries contain
ordinary TGA payloads, while composite-map entries begin with the `DDS ` magic
despite retaining `.tga` names in the package index. That filename/content
disagreement is confirmed data, not proof of damage; the historical client must
be allowed to load by content through its existing texture path. A future
restoration must not mechanically convert or rename those entries.

## Authorization evidence

`XProcess/XProc_FirstLoad.cpp::InitializeSpeedTreeWrapper` contains one active
SpeedTree authorization value and two inactive historical values. The active
value is not reproduced here because it is licensed/sensitive material.

Static byte comparison found exactly one SpeedTree authorization-shaped value
in the supplied reference `NineDragons.exe`. Its complete bytes are identical
to the active source value:

- non-secret family prefix: `RT-0405-…`
- SHA-256 of the complete value: `95b4b17918ebd2b167f6bbd95a601f996d7029b3eaef2f7f5568ac5f311d02a4`
- source/reference equality: confirmed

This is strong evidence that the active source branch corresponds to the
shipped reference client's SpeedTree authorization generation. It does **not**
establish permission to redistribute or reuse the key, SDK, library, or data.
That remains a licensing gate outside this technical audit.

The initialization function does not check an authorization result and returns
success unconditionally. Consequently, a bad library/key pairing could first
surface later as `LoadTree` or geometry-computation failure. A controlled
future build must explicitly instrument authorization and first-tree loading
rather than treating initialization's return value as proof.

## Compatibility assessment

The currently quarantined SpeedTree candidate set has already established:

- all 146 imports required by the supplied `SpeedTreeRT.dll` are exported by
  the candidate import library;
- all 42 wrapper-called methods and all 31 wrapper-observed fields are present
  in the candidate header;
- the packed model and texture resources expected by the wrapper are present;
- the source authorization value is byte-identical to the reference executable.

This raises the **technical completeness** of the SpeedTree subsystem to high,
but not to proven runtime compatibility. The remaining material uncertainty is
whether the candidate header/import library, supplied DLL, embedded
authorization generation, and serialized `.spt` revision behave together
exactly as the reference build did.

## Remaining gates

Before dependency restoration:

1. Preserve hashes and provenance for the header, import library, DLL, both
   packs, and reference executable.
2. Determine the supplied DLL's exact SpeedTreeRT release/build if possible
   from static version resources or vendor records.
3. Resolve licensing/redistribution authority separately; technical presence
   is not authorization.

Before a first controlled build:

1. Keep the candidate files quarantined and copy only a reviewed, hash-pinned
   set into an isolated dependency tree.
2. Confirm calling convention, structure packing, compiler ABI, and CRT
   assumptions between the wrapper, header, import library, and DLL.
3. Do not rename or transcode `.tga`-named DDS composite-map payloads.

Before claiming SpeedTree works:

1. Confirm authorization state explicitly.
2. Load representative ordinary, seasonal, snow, `_B`, `_big`, and
   region-prefixed `.spt` entries.
3. Verify branch/frond/leaf geometry, texture resolution, wind, LOD transitions,
   device loss/reset, and repeat loading.
4. Compare rendered behavior and failure handling with the known-good client.

## Verdict

The missing-loose-tree-files concern is closed. The normal player client's
SpeedTree data and matching authorization material are present, and the
candidate binary interface is a strong static match. SpeedTree should now be
tracked as **high-likelihood but runtime-unproven**, with ABI/version validation
and legal provenance as its remaining gates—not as a missing-data blocker.
