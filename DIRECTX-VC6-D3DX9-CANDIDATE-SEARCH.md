# October 2004 VC6 D3DX Candidate Search

## Objective

Locate the replacement D3DX static library distributed under:

```text
dxsdk_oct2004_extras.exe
\Extras\D3dx9 Visual Studio 6\
```

The required candidate must combine the October 2004 API/source generation
with an ABI usable by Visual C++ 6, specifically without the standard October
archive's unresolved security-cookie dependencies.

No candidate was executed, linked, or compiled.

## Search results

GitHub searches for the exact Extras directory name returned no indexed copy.
Filename searches found two distinct binary families:

| Family | Bytes | SHA-256 | Dates | Security-cookie references |
|---|---:|---|---|---|
| Source-engine SDK family | 6,980,296 | `fd431d9ec65c3d3fd401b89aba77d83551e06ef80dcb725fdc89b8d78e90cf79` | 29 Sep 2004 | Present |
| GunZ/MaIET SDK family | 5,146,120 | `115cc60093e70567fc4325f94a7c3bd479b09977ee14d1c305f8f406d568aad4` | 11 Dec 2002 | Absent |

The GunZ/MaIET family was located at the same Git blob identity in three
repositories:

- `open-gunz/ref-gunz1.0`;
- `khuyendev/Mu`;
- `zkxjzmswkwl/maiet_svn_publish_kor_vs2022`.

That three-way match establishes preservation consistency, but the repositories
share SVN ancestry and are not fully independent provenance sources.

The quarantined 2002 candidate is:

`recovery/emperor-of-dragons-normal-client-overlay/candidate-dependencies/DirectX9-October2004-untrusted/d3dx9dt.gunz-e2adecb.untrusted.lib`

## 2002 candidate analysis

| Property | Result |
|---|---|
| Archive format | Valid Microsoft/COFF-style library |
| Members | 208 |
| Object timestamps | 11 December 2002, 05:23 |
| Archive index timestamps | 11 December 2002, 05:25 |
| `__security_cookie` | Not referenced |
| `__security_check_cookie` | Not referenced |
| `__CxxFrameHandler` | Referenced; normal old MSVC C++ EH support, not by itself disqualifying |
| Matching `nt32_chk` `createmesh.cpp` path | Present |
| Player external D3DX calls covered | 50 of 51 |
| Missing player symbol | `D3DXMatrixTransformation2D` |

This is an early DirectX 9 D3DX archive and is materially closer to the desired
VC6 compiler ABI. It is not the October 2004 replacement:

- its dates precede the October release by almost two years;
- it has 208 members rather than the October archive's 256;
- it lacks one external function used by the player.

It must not be promoted or patched around merely to obtain a link. Replacing
`D3DXMatrixTransformation2D` with local matrix composition would change the
fidelity question and still leave the wrong D3DX generation.

## Corroborating historical evidence

Contemporary reports reproduce the exact failure caused by the standard
October libraries:

```text
d3dx9.lib(...): unresolved external symbol ___security_cookie
```

Those reports state that placing the October Extras
`D3dx9 Visual Studio 6` directory first in the VC6 library search path resolves
the problem. They also establish that the Extras package contained D3DX
replacement libraries specifically, rather than replacements for every DirectX
system import library.

This independently supports the ABI distinction reached from the two recovered
binary families.

## Search coverage at the time of the initial pass

The following initially yielded no third binary family:

- GitHub code search for the exact Extras directory;
- GitHub repository search for the package filename and VC6 directory;
- GitHub filename search for `d3dx9dt.lib`;
- Internet Archive item-catalog and Wayback CDX searches;
- general web searches across GitLab, Gitee, SourceForge and Archive.org.

That conclusion was superseded on 25 July 2026. A broader Internet Archive
metadata query located the dedicated item `dxsdk_oct2004_extras`; static
extraction recovered Microsoft's explicitly named
`Extras/D3dx9 Visual Studio 6/d3dx9dt.lib`.

See `DIRECTX-OCTOBER-2004-VC6-EXTRAS-ACQUISITION.md` for hashes and ABI proof.

## Superseding verdict

The October 2004 VC6 Extras replacement has now been recovered into quarantine.
Its SHA-256 is
`d9743322b0b814351ec76b331e67e3f0c007d44c8c94bab2b9192ee8f496f68d`.
Neither of the two earlier comparison binaries should be promoted.

The narrow acquisition blocker is closed. Promotion still requires a reviewed
manifest, coherent October 2004 headers/companion libraries, and the planned
CRT/default-library policy decision.
