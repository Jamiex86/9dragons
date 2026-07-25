# `d3dx9dt.lib` Candidate Audit

## Scope and handling

This is a static, no-build audit. No downloaded file was executed and no
installer was run. The candidate remains quarantined and has not been promoted
into the recovered player dependency tree.

Quarantine path:

`recovery/emperor-of-dragons-normal-client-overlay/candidate-dependencies/DirectX9-October2004-untrusted/d3dx9dt.fd431d9.untrusted.lib`

## Discovery

GitHub code search located `d3dx9dt.lib` in four nominally separate
repositories:

| Repository lineage | Repository path |
|---|---|
| `perilouswithadollarsign/cstrike15_src` | `dx9sdk/lib/d3dx9dt.lib` |
| `usernameunavalible0/OrangeBoxSDK` | `hl2_src/dx9sdk/lib/d3dx9dt.lib` |
| `nn-firc/srcsdk` | `dx9sdk/lib/d3dx9dt.lib` |
| `q4a/source-engine-2017` | `dx9sdk/lib/d3dx9dt.lib` |

All four repository entries are identical Git LFS pointer files:

```text
version https://git-lfs.github.com/spec/v1
oid sha256:fd431d9ec65c3d3fd401b89aba77d83551e06ef80dcb725fdc89b8d78e90cf79
size 6980296
```

This establishes strong cross-repository consistency, but not four independent
provenance chains: the repositories likely inherited the same Source-engine
SDK payload.

## Payload verification

The LFS object was retrieved from one repository into quarantine.

| Property | Observed |
|---|---|
| Size | `6,980,296` bytes |
| SHA-256 | `fd431d9ec65c3d3fd401b89aba77d83551e06ef80dcb725fdc89b8d78e90cf79` |
| Container | Microsoft/COFF-style `ar` library archive |
| Archive members | `256` |
| Apparent target | x86 (`obj\i386\...` member paths) |
| Global-symbol output | approximately `17,911` lines through the host `nm` reader |

The retrieved payload exactly matches the size and SHA-256 declared by every
located LFS pointer.

## Match to the supplied reference executable

The candidate contains the same highly distinctive internal evidence already
recovered from `NineDragons.exe`:

```text
D3DX Assertion Failure; application terminated
d:\builds\nt32_chk\multimedia\directx\dxg\d3dx9\mesh\createmesh.cpp
```

It also contains:

- `D3DX Assertion Failure`;
- Microsoft-decorated x86 D3DX exports such as `_D3DXCreateMesh@24`;
- `obj\i386\createmesh.obj`;
- extensive `D:\BUILDS\NT32_CHK\multimedia\directx\dxg\d3dx9\...`
  object and source paths.

The exact `createmesh.cpp` build path in both the static library and the shipped
reference executable is substantially stronger evidence than a filename-only
match. It indicates that the executable incorporated code from this Microsoft
debug D3DX build lineage.

## Assessment

| Question | Assessment | Confidence |
|---|---|---|
| Is this a real `d3dx9dt.lib` rather than a renamed import library? | Yes; it is a large static archive with 256 D3DX object members | Very high |
| Is it x86 and technically relevant to the VC6-era player? | Yes | Very high |
| Does it match the D3DX build lineage embedded in `NineDragons.exe`? | Yes, including the exact `nt32_chk` `createmesh.cpp` path | Very high |
| Is it certainly the October 2004 VC6 Extras revision? | Not yet proven | Medium |
| Is its redistribution provenance independently established? | No | Low |

Source-lineage match estimate: **97–99%** for the October 2004 static D3DX
generation visible in the reference executable.

Exact-release/provenance estimate: **65–80%** pending comparison with an
original October 2004 Extras package, Microsoft catalog hash, or another
independently preserved SDK image.

Subsequent object-level ABI inspection found unresolved
`__security_cookie`/`__security_check_cookie` and newer C++ exception-support
references in members used by the player. This substantially lowers its clean
VC6 suitability and indicates that it is probably the standard October library,
not the `Extras\D3dx9 Visual Studio 6` replacement. See
`DIRECTX-D3DX9DT-REVISION-ABI-AUDIT.md`.

## Decision

This is a high-confidence October 2004 source-lineage reference for the missing
`d3dx9dt.lib`. It is suitable for continued forensic comparison but remains
**untrusted/quarantined**. Its current VC7-era compiler-support dependencies
make it unsuitable for promotion as the clean VC6 fidelity library.

Promotion gate:

1. identify its D3DX revision from headers, symbols, or object metadata;
2. compare it with a second genuinely independent SDK/CD/archive source;
3. verify the player’s D3DX symbol requirements against the archive
   (**completed at source level: 51/51 external calls covered; see
   `DIRECTX-D3DX9DT-SYMBOL-COVERAGE.md`**);
4. document redistribution/licensing status;
5. preserve its source URL, repository commit, size, and hashes.
