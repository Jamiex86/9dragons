# Quarantined DirectX October 2004 VC6 Candidate

This directory is forensic evidence and is not an active project dependency.
Nothing here has been compiled, linked, installed, or executed by the audit.

## Construction

- `Include/`: all 184 files statically extracted from Microsoft's
  `dxsdk_oct2004.exe`.
- `Lib/`: all 50 x86 libraries statically extracted from the same main SDK.
- `Lib/d3dx9.lib`, `Lib/d3dx9d.lib`, and `Lib/d3dx9dt.lib`: replaced with the
  Microsoft versions from
  `dxsdk_oct2004_extras.exe/Extras/D3dx9 Visual Studio 6/`.

The ordinary main-SDK D3DX files remain preserved separately under
`../Main-SDK/Lib/`. They must not be substituted back into a VC6 fidelity set.

## Installer provenance

| Installer | SHA-256 |
|---|---|
| `dxsdk_oct2004.exe` | `9995e138a009572abe2292f051fbb0a21f3b3478e07527fa718c1c4a2e09fb3b` |
| `dxsdk_oct2004_extras.exe` | `3820eb13684d4dd7aa02d2880767886149ddd7d4ace5ef15145a97bd79c5d22f` |

Both observed MD5 values match the Internet Archive metadata for the respective
Microsoft-attributed items.

## D3DX header hashes

| File | SHA-256 |
|---|---|
| `d3dx9.h` | `22090a940c1a24ff99205d793ae577e62e700408da3f5925ba17c3739bca2e3a` |
| `d3dx9anim.h` | `13e3d815d23e6daddde706dafef769bb40416e5c6c5ba817296377595a9716b2` |
| `d3dx9core.h` | `8fab79b43aefb87754e2861cb6e52c5d442ee291f35cae86419c1c18f489f601` |
| `d3dx9effect.h` | `58f9a379f256a150cfece7d006f71700e8b65dfd85bd9d260a6dea956e360d0b` |
| `d3dx9math.h` | `a5584c335e8ae063b804124550a05cc5abbf7317310561ba1b52a3ac52077d99` |
| `d3dx9math.inl` | `c79926f99577d6e62b2e264691689fbaa11b987c6303bc644509a0300a00d8c0` |
| `d3dx9mesh.h` | `6e7c633f3bae373861354b9cf81e0e569d72f6a8dfea18850294b452428ec81a` |
| `d3dx9shader.h` | `7b591a96655c8aabf5915a8646d60a46fd3618e31c925ffcc11802a41af38084` |
| `d3dx9shape.h` | `481ce5fc79bba00750dce857e6d0507cb0b7b82013bcea1728b1abfdc5fe9a7b` |
| `d3dx9tex.h` | `dbdc63e4dceb9eb145f66a563941c4d20cade0d2a019212a5a2bbe9ef347b794` |
| `d3dx9xof.h` | `145c211ccbcd880f411195e66881a9172df52ac495f309dc7c97875ba73d3c55` |

`d3dx9core.h` declares `D3DX_SDK_VERSION 22`.

## Promotion prohibition

Do not add this directory to a compiler or linker search path until the
preservation toolchain, CRT/default-library rules, local v43 header removal
strategy, and legal/provenance decision are approved.
