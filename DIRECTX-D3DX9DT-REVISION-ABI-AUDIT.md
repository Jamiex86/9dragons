# `d3dx9dt.lib` Revision and VC6 ABI Audit

## Result

The quarantined archive is attributable to the **October 2004 DirectX 9.0c
SDK build window**, but it is not yet the desired Visual C++ 6-compatible
Extras variant.

This distinction materially changes its status:

- its D3DX source/revision lineage is an excellent match;
- its exported API covers the player;
- its current object ABI contains VC7-era compiler-support references that a
  plain VC6 SP6 environment does not supply.

The candidate must therefore remain quarantined and must not be promoted as the
final fidelity library.

## Revision evidence

Archive:

`d3dx9dt.fd431d9.untrusted.lib`

SHA-256:

`fd431d9ec65c3d3fd401b89aba77d83551e06ef80dcb725fdc89b8d78e90cf79`

All 256 archive members carry consistent timestamps:

- object members: **29 September 2004, 08:28**;
- archive index/long-name members: **29 September 2004, 08:33**.

The Microsoft October 2004 SDK was published on 7 October 2004. A coherent
internal build eight days before publication is strong release-attribution
evidence:

| Candidate release | Fit |
|---|---|
| Summer 2004 | Unlikely; candidate was rebuilt on 29 September after that release |
| **October 2004** | **Strong match; pre-release build date and matching D3DX focus** |
| December 2004 | Excluded by the September object timestamps |
| February 2005 or later | Excluded; that release removed `d3dx9dt.lib` |

The archive also contains the expected
`D:\BUILDS\NT32_CHK\multimedia\directx\dxg\d3dx9\...` build tree and the exact
`mesh\createmesh.cpp` path embedded in the supplied `NineDragons.exe`.

Revision attribution:

**October 2004 standard D3DX build: 97–99% confidence.**

This does not prove the wrapper package from which this copy was originally
extracted.

## VC6 compatibility finding

Static undefined-symbol inspection found:

```text
@__security_check_cookie@4
___security_cookie
___CxxFrameHandler
```

No object in the archive defines the security-cookie symbols. Seventy of the
256 members reference at least one of these VC7-era compiler-support symbols.
Relevant examples include:

- `obj\i386\d3dx9core.obj`;
- `obj\i386\d3dx9shader.obj`;
- `obj\i386\cfont.obj`;
- `obj\i386\createmesh.obj`;
- `obj\i386\cimage.obj`;
- `obj\i386\ceffect.obj`.

The player directly uses exports rooted in several affected members, including
font, sprite, shader and texture facilities. Consequently this is not a
theoretical unused-member concern.

Historical reports explain why Microsoft distributed the separate October
2004 Extras package: its `D3dx9 Visual Studio 6` directory contained replacement
libraries compatible with VC6. The present candidate is therefore most likely
the standard October archive copied from a later Source-engine SDK collection,
not the VC6 replacement archive required for a clean preservation toolchain.

## Relationship to `NineDragons.exe`

The reference executable:

- reports linker version 6.0;
- imports `MSVCP60.dll` and `MSVCRT.dll`;
- contains the same D3DX assertion strings and `nt32_chk` source paths.

This proves matching Microsoft D3DX source lineage, but not byte identity with
the candidate. The VC6-compatible Extras archive could have been compiled from
the same D3DX sources and therefore embed the same paths while omitting the
newer compiler-support dependencies.

It is also technically possible to link some newer-compiler objects with the
VC6 linker by adding later compiler-support libraries, but that would no longer
be a clean VC6-only environment and introduces C++ EH/CRT ABI risk. It is not
the preferred fidelity explanation while Microsoft explicitly supplied a VC6
replacement.

## Revised suitability grades

| Property | Grade |
|---|---|
| Exact October 2004 D3DX source generation | 97–99% |
| D3DX API coverage for player source | 100% of statically identified external calls |
| Match to reference EXE's embedded D3DX source paths | Very high |
| Clean VC6 SP6 link suitability without extra compiler-support libraries | Low |
| Probability this is the special October Extras VC6 replacement | 10–25% |
| Usefulness as comparison/reference evidence | Very high |
| Suitability for promotion as the final fidelity dependency | Not approved |

## Required next acquisition

The target is now more precise:

```text
dxsdk_oct2004_extras.exe
  \Extras\D3dx9 Visual Studio 6\...
```

The required library must:

1. retain the October 2004 D3DX API surface;
2. have September/October 2004-consistent source/build metadata;
3. omit unresolved `__security_cookie` and VC7-only exception/runtime
   dependencies, or include an explicitly documented Microsoft VC6 support
   mechanism;
4. cover the same 51 external player D3DX calls;
5. preserve the matching `nt32_chk` source lineage;
6. be independently hashed and quarantined before promotion.

A second binary family has since been located and audited in
`DIRECTX-VC6-D3DX9-CANDIDATE-SEARCH.md`. It is a security-cookie-free December
2002 archive and covers 50/51 player calls, but it lacks
`D3DXMatrixTransformation2D` and is not the October replacement. It remains
quarantined as comparative evidence only.

## Verdict

The revision question is resolved: **October 2004** is the correct attribution.

The dependency question is not yet resolved: this specific binary is the wrong
ABI candidate for a clean VC6 preservation build. It remains valuable as a
high-confidence comparison oracle while the October Extras VC6 replacement is
located.
