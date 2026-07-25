# DirectX October 2004 Visual Studio 6 Extras Acquisition

## Outcome

The exact missing Microsoft package has been located, downloaded into
quarantine, and statically unpacked without executing either of its two
self-extracting executables.

Internet Archive item:

- identifier: `dxsdk_oct2004_extras`
- title: `Microsoft DirectX SDK October 2004 Extras`
- archive URL:
  `https://archive.org/details/dxsdk_oct2004_extras`
- attributed creator: Microsoft Corporation
- dated/signing metadata: 7 October 2004

## Installer identity

Quarantined file:

`candidate-dependencies/DirectX9-October2004-untrusted/dxsdk_oct2004_extras.internet-archive.untrusted.exe`

| Property | Value |
|---|---|
| Size | 66,817,792 bytes |
| MD5 | `c700fa404c0e28f879c640327fdc0036` |
| SHA-1 | `bcb41b7ab17ebe674e68e0c4998d66f52d11300e` |
| SHA-256 | `3820eb13684d4dd7aa02d2880767886149ddd7d4ace5ef15145a97bd79c5d22f` |
| Container | 32-bit Microsoft cabinet self-extractor |

The downloaded MD5 exactly equals the archive's independently published file
metadata. The archive metadata also records the executable as Microsoft-signed.
The package remains untrusted/quarantined until provenance and redistribution
policy are formally accepted.

## Exact recovered directory

The outer cabinet contains `extras.exe`. Static cabinet listing of that nested
file exposes the directory:

`Extras/D3dx9 Visual Studio 6/`

It contains:

| File | Size | SHA-256 |
|---|---:|---|
| `d3dx9.lib` | 5.7 MiB | `1ee03173ca703e514e129aab42c32c451044a2f252080a3cab5cd5fb416f73ce` |
| `d3dx9dt.lib` | 6.6 MiB | `d9743322b0b814351ec76b331e67e3f0c007d44c8c94bab2b9192ee8f496f68d` |
| `d3dx9d.dll` | 2.2 MiB | `a988276b5a45f9fcc802930750d736c2ebca43b4fd78eb2d8f45e1ed283821d3` |
| `d3dx9d.lib` | 81 KiB | `4f96d4775b0d6b50a78c6680d54ef705ceba7740ef693d9e1e1fc83c9355d7d7` |
| `d3dx9d.pdb` | 2.2 MiB | `622ac9df5e1e83cda6d250726666182ea8d82505062cf37c1054b6a6d7451eaa` |

The files are quarantined at:

`candidate-dependencies/DirectX9-October2004-untrusted/Visual-Studio-6/`

Their cabinet timestamps are 30 September 2004.

## Static ABI proof

The recovered `d3dx9dt.lib`:

- is a 32-bit COFF archive with 256 members;
- contains all 46 directly named D3DX externals and all five ANSI forms needed
  by the `US_Release|Win32` MultiByte build;
- therefore covers all 51 source-level D3DX calls previously catalogued;
- contains `D3DXMatrixTransformation2D`, which the older GunZ candidate lacked;
- has **zero** undefined `__security_cookie` or
  `__security_check_cookie` references;
- contains predominantly `@comp.id` value `0x00600fc3`, distinct from the
  incompatible security-cookie-bearing comparison family;
- may legitimately reference the VC6-era `__CxxFrameHandler`; that symbol alone
  is not evidence of a VC7 dependency.

Most importantly, this library is not inferred to be VC6-compatible from symbol
shape: Microsoft placed it in the package's explicitly named
`D3dx9 Visual Studio 6` directory.

## Relationship to earlier candidates

This SHA-256 is distinct from both earlier public candidates:

| Candidate | SHA-256 | Classification |
|---|---|---|
| Exact Microsoft VC6 Extras | `d9743322…496f68d` | Correct preservation candidate |
| October comparison library | `fd431d9e…e90cf79` | Different build; security-cookie-bearing members |
| GunZ/MaIET library | `115cc600…8aad4` | Older 2002 family; one required API absent |

The earlier conclusion that no third family could be found is superseded.

## Gate decision

The narrow acquisition blocker for the October 2004 Visual Studio 6
`d3dx9dt.lib` is **closed**.

The file has deliberately not been copied into an active compiler/library
search path. Before promotion:

1. preserve the complete installer and extracted-file hash manifest;
2. decide the legal/provenance policy for using the archived Microsoft SDK;
3. pair these VC6 D3DX files with a coherent October 2004 header and companion
   DirectX library set;
4. remove the hybrid `d3dx9_43.dll` import library from the proposed historical
   set;
5. perform the planned CRT/default-library directive audit.

No build, link, installer execution, or binary execution was performed.
