# Runtime DLL and Import-Library Pairing Audit

Date: 2026-07-25  
Scope: static inspection only; no DLL was loaded or executed.

## Result

All five supplied Win32 DLLs are technically compatible with their currently
selected import libraries at the declared PE import/export boundary.

| Component | Pairing result | Confidence |
|---|---|---:|
| Bink | Exact 66/66 named export match | 99–100% |
| FMOD | All 229 library imports satisfied; DLL has 3 additional exports | 99% |
| QHTM | Exact 16/16 ordinal mapping | 99–100% |
| XWebPage | Exact 6/6 named export match | 99–100% |
| SpeedTreeRT | Exact 146/146 function/data import-export match | 99–100% |

These percentages apply only to binary pairing. They do not prove successful
startup, correct initialization, licensing, content behavior, or lawful
redistribution.

## Evidence table

| Component | Import library SHA-256 | Runtime DLL SHA-256 | Architecture | DLL identity |
|---|---|---|---|---|
| Bink | `174bd8888d66cfd969f147498e118d6c79f57bc4ccf17f2c3eda6c9042f22726` | `d776b5b874a80add5b3307de732ca41b2b6982142ab41b44f940f86da002ce62` | PE/COFF x86 | `binkw32.dll` |
| FMOD | `560aaa3e34d3fa2ced9fbdac35486595bc53f65922e6a11667633b819253b6ad` | `0287ce690f314826dd6ed82e77a64b518376308f4b4eeb50b73e4f2142576aec` | PE/COFF x86 | `fmod.dll` |
| QHTM | `b3d9d5badcceb19e90fa8ced0366558de87f998f561f865b863a6f85bdac2f67` | `9e622b880d9afc7076a8c008272528bfea21ec3d2e95b354795246cd48b29e53` | PE/COFF x86 | `QHTM.dll` |
| XWebPage | `b7d761dc7449438fbc445c302900148826ee02ebe58bf6e7b81df2f14921b4c2` | `23f5f9b55b64971481e3e8495e212c417e454f028a28900844b03d53be6a7562` | PE/COFF x86 | `XWebPage.dll` |
| SpeedTreeRT | `bfaf506d886195d1cb1330c04ac1fc6ba60da553217da0069812571f52ce620f` | `ff3c3f003d943f53f1718c2fe36446faab2617a500e8a60e8f6ad83deb03a4c3` | PE/COFF x86 | `SpeedTreeRT.dll` |

The user-supplied Bink, QHTM, and SpeedTree DLLs are byte-identical to the
copies already retained in the respective staged/quarantine locations.

## Component findings

### Bink

- `binkw32.lib` requests `binkw32.dll`.
- The import library's 66 unique API imports match all 66 DLL exports exactly.
- No required import is absent and the DLL has no unmatched public export.
- The staged header declares Bink `1.8r` (`1.8.18`).
- DLL PE timestamp: 2006-10-05 05:26:13 (timestamp is evidence, not proof of
  build date).

Verdict: **exact binary pair**.

### FMOD

- `fmodvc.lib` requests `fmod.dll`.
- All 229 unique functions represented by the import library exist with exact
  decorated names in the supplied DLL.
- The DLL exports 232 functions. Its three additions are:
  `_FSOUND_GetSpeakerMode@0`,
  `_FSOUND_Stream_Net_SetNetDataCallback@8`, and
  `_FSOUND_Stream_Net_SetTimeout@4`.
- Additive DLL exports do not alter or invalidate the 229 imports used by this
  library.
- The staged header declares FMOD `3.74`.
- DLL PE timestamp: 2005-11-02 08:37:58.
- The supplied DLL is UPX-packed. Static PE export inspection remains valid,
  but packing reduces the amount of version/compiler evidence available
  without unpacking. It was not unpacked or executed.

Verdict: **compatible pair; likely a slightly broader DLL from the same FMOD
3.x ABI family**.

### QHTM

The QHTM names initially appear different:

- the import library exposes decorated C/C++ linker symbols such as
  `_QHTM_Initialize@4`;
- the DLL export-name table displays undecorated aliases such as
  `QHTM_Initialize`.

This is not a mismatch. The import objects encode ordinal imports using the PE
high-bit ordinal flag. Their ordinals are:

`1, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18`

The supplied DLL exports exactly those same 16 ordinals, including
`QHTM_PrintSetImageCallback` at ordinal 18. Ordinals 2 and 8 are absent from
both sides.

DLL PE timestamp: 2006-07-18 19:45:40.

Verdict: **exact ordinal pair**. Comparing only displayed export names would
have produced a false negative.

### XWebPage

- `XWebPage.lib` requests `XWebPage.dll`.
- All six decorated stdcall imports match all six DLL exports exactly:
  `DisplayHTMLPage`, `DisplayHTMLStr`, `DoPageAction`,
  `EmbedBrowserObject`, `ResizeBrowser`, and `UnEmbedBrowserObject`.
- DLL PE timestamp: 2006-07-24 14:17:44.

Verdict: **exact binary pair**.

### SpeedTreeRT

- `SpeedTreeRT.lib` requests `SpeedTreeRT.dll`.
- The earlier dedicated audit established an exact 146/146 match including
  exported static data, not merely callable methods.
- A function-only count yields 140 imports; the remaining six DLL exports are
  public static data members. Treating only `T` symbols as imports would
  incorrectly report them as extras.
- The client wrapper has already been checked against the candidate header:
  42/42 called methods and 31/31 directly accessed fields are covered.
- DLL PE timestamp: 2006-01-18 00:47:03.

Verdict: **exact import/DLL pair and very strong wrapper fit**; provenance and
SpeedTree authorization remain separate gates.

## Runtime placement status

| Runtime | Present in active player working tree? | Status |
|---|---:|---|
| `Library/BinkSDK/binkw32.dll` | Yes | Staged dependency evidence |
| `Library/QHTM/QHTM.dll` | Yes | Staged dependency evidence |
| `fmod.dll` | No | Verified user-supplied input; not yet staged |
| `XWebPage.dll` | No | Verified user-supplied input; not yet staged |
| `SpeedTreeRT.dll` | No | Retained in quarantined candidate set |

The project does not necessarily load DLLs from their source-library folders at
runtime. Final packaging will require the approved DLLs beside the executable
or in another deliberately controlled Windows loader-search location.

No runtime file should be copied into `Game/US` until:

1. its provenance and redistribution status are recorded;
2. the preservation manifest identifies its exact destination;
3. runtime loading behavior is statically mapped, including any explicit
   `LoadLibrary` path assumptions;
4. the controlled-build and controlled-startup phases are separately
   authorized.

## Linker-readiness adjustment

The five DLL/import-library pairings are no longer technical blockers at their
declared import/export surfaces. Remaining pre-build blockers are now:

1. promotion/provenance decisions for SpeedTree and the Flash MP3 decoder;
2. reconstruction of the VC6 CRT/default-library policy;
3. selection of the historical DirectX 9 SDK and resolution of
   `d3dx9dt.lib`;
4. a frozen preservation compiler/linker matrix.

