# Missing DLL static audit

Date: 25 July 2026

## Standalone-file revalidation

The user subsequently supplied all five DLLs as standalone files. Each is a
structurally readable PE32 x86 DLL and each reproduces the identity inferred
from the earlier material:

| Standalone file | Size | CRC32 | SHA-256 |
|---|---:|---|---|
| `XWebPage.dll` | 45,056 | `5544624b` | `23f5f9b55b64971481e3e8495e212c417e454f028a28900844b03d53be6a7562` |
| `SpeedTreeRT.dll` | 249,856 | `eec2eda4` | `ff3c3f003d943f53f1718c2fe36446faab2617a500e8a60e8f6ad83deb03a4c3` |
| `QHTM.dll` | 405,504 | `3a25dbc2` | `9e622b880d9afc7076a8c008272528bfea21ec3d2e95b354795246cd48b29e53` |
| `fmod.dll` | 162,304 | `ca8a4a0d` | `0287ce690f314826dd6ed82e77a64b518376308f4b4eeb50b73e4f2142576aec` |
| `binkw32.dll` | 166,912 | `3a25bb82` | `d776b5b874a80add5b3307de732ca41b2b6982142ab41b44f940f86da002ce62` |

`QHTM.dll` and `binkw32.dll` are byte-for-byte identical to the independently
recovered overlay copies. `fmod.dll` and `SpeedTreeRT.dll` are byte-for-byte
identical to the two CRC-valid entries recoverable from the damaged nested ZIP.
The standalone files therefore supersede the damaged container as preservation
evidence, without changing the compatibility findings below.

## Scope and safety

This is a static inspection of:

- `/Users/jamiemarshall/Downloads/dlls.zip`
- `/Users/jamiemarshall/Downloads/XWebPage.dll`

No DLL or executable was loaded or run. Nothing from these inputs has been
integrated into the overlay.

## Container integrity

| Item | SHA-256 | Result |
|---|---|---|
| outer `dlls.zip` | `b5671333957c2ef46aa03b296cf5dca5f445cc18d5f58fe15d84b2693320d769` | Readable, contains one nested ZIP |
| extracted nested `dlls.zip` | `cc9f25ae8c2e4d4037c7eea782440b7bd7d23ad433492ea399c190a351c35b91` | Damaged; outer extraction reports a CRC error and its stored records are displaced/overlapping |
| `XWebPage.dll` | `23f5f9b55b64971481e3e8495e212c417e454f028a28900844b03d53be6a7562` | Structurally valid PE32 x86 DLL |

The damaged nested ZIP must not be treated as a reliable distribution archive.
Individual entries were accepted only when their data matched the CRC recorded
in the nested central directory, or when the recorded CRC exactly identified an
already independently recovered file.

## Results

| DLL | Static result | Match assessment | Disposition |
|---|---|---|---|
| `XWebPage.dll` | Valid PE32 x86; VC6 linker 6.0; timestamp `2006-07-24 14:17:44`; exports exactly the six stdcall functions named by `XWebPage.lib` | **Exact build pair, 99%+**. The import library has the identical embedded linker timestamp and all six decorated names match | Runtime gap closed once provenance/use is accepted |
| `fmod.dll` | Intact stored member; CRC `ca8a4a0d`; SHA-256 `0287ce690f314826dd6ed82e77a64b518376308f4b4eeb50b73e4f2142576aec`; PE32 x86, UPX-packed; exports 232 functions | **Very high, 99%**. All 229 imports represented by recovered `fmodvc.lib` are exported; the DLL has only three additional exports | Runtime gap technically closed; retain this DLL with its paired import library |
| `binkw32.dll` | Payload in this nested ZIP is corrupt, but central-directory size `166912` and CRC `3a25bb82` exactly match the independently recovered overlay DLL | **Exact identity by size and CRC, 99%+** | No new binary is needed; current overlay copy is the file described by this ZIP |
| `QHTM.dll` | Payload in this nested ZIP is corrupt, but central-directory size `405504` and CRC `3a25dbc2` exactly match the independently recovered overlay DLL | **Exact identity by size and CRC, 99%+** | No new binary is needed; current overlay copy is the file described by this ZIP |
| `SpeedTreeRT.dll` | Intact stored member; CRC `eec2eda4`; SHA-256 `ff3c3f003d943f53f1718c2fe36446faab2617a500e8a60e8f6ad83deb03a4c3`; PE32 x86, VC6-era imports; PE timestamp `2006-01-18 00:47:03`; 146 exports | **Exact exported-symbol match to the subsequently located Multiverse release import library** | Technical pair identified; provenance/licensing and asset behaviour remain gated |

## XWebPage proof

The recovered `XWebPage.lib` and supplied DLL both carry the PE/COFF timestamp:

```text
Mon Jul 24 14:17:44 2006
```

Both sides contain exactly:

```text
_DisplayHTMLPage@8
_DisplayHTMLStr@8
_DoPageAction@8
_EmbedBrowserObject@4
_ResizeBrowser@12
_UnEmbedBrowserObject@4
```

This is stronger than a same-name match: it is consistent with the import
library having been generated with this DLL build.

## FMOD proof

The recovered `fmodvc.lib` represents 229 decorated FMOD imports. The supplied
DLL exports every one of those names. Its three additional exports are:

```text
_FSOUND_GetSpeakerMode@0
_FSOUND_Stream_Net_SetNetDataCallback@8
_FSOUND_Stream_Net_SetTimeout@4
```

An export superset does not break import-library compatibility. This is a
technically complete runtime match for the recovered library.

## SpeedTree qualification

The supplied 2006 DLL differs from the previously researched 2004
`SpeedTreeRT.h`/`SpeedTreeRT.lib` candidate at these ABI-decorated functions:

```text
2004 import library                         supplied 2006 DLL
?Get360BillboardGeometry...SGeometry...     ?Get360BillboardGeometry...SGeometry...K
?GetLeafGeometry...SGeometry...KF           ?GetLeafGeometry...SGeometry...F
?SetNumWindMatrices...SAXI@Z                ?SetNumWindMatrices...SAXH@Z
```

The differences encode changed parameter lists or parameter types, not merely
different ordinals. `SetNumWindMatrices` is used by the Emperor of Dragons
wrapper. Therefore the public 2004 import library cannot be treated as the
correct link pair for this 2006 DLL.

This DLL is valuable evidence that the reference client used a later
SpeedTreeRT build. A revised 1.6 header and release import library were
subsequently located in `multiversemmo/MultiversePlatform`. The import
library's 146 decorated imports equal the supplied DLL's 146 exports exactly.
It is therefore ABI-compatible even though the library records a 2005 source
DLL timestamp and the supplied runtime is from 2006.

## Revised runtime conclusion

For the normal player client, the supplied material closes or confirms the
runtime side of XWebPage, FMOD, Bink and QHTM. The later public search also
provisionally closes the technical SpeedTree development-file gap at the ABI
boundary. SpeedTree provenance, licence and tree-asset behaviour remain open.
The damaged nested ZIP should be replaced with a clean copy for preservation
even though its useful identities can be established statically.
