# US Normal Player FMOD Audio Dependency Audit

Date: 2026-07-25  
Scope: `US_Release|Win32`, static inspection only. No compilation, DLL
loading, unpacking, or binary execution.

## Conclusion

FMOD is an **active and startup-critical** normal US player dependency. The
supplied `fmod.dll` is a 99% import/export match for the included FMOD 3.74
interface, and the required packed sound archive is present.

There is no currently identified missing FMOD codec DLL or missing normal
player audio source.

## Startup path

Normal `US_Release` defines `_XUSEFMOD`. During first load:

```cpp
if( !g_FMODWrapper.Initialize() ) return false;
if( !PreLoadSoundEffect() ) return false;
```

Initialization opens `Data/Sound/SR_SOUND.XP`, reads registry audio settings,
requires `FSOUND_GetVersion() >= 3.74`, installs packed-file callbacks,
initializes DirectSound or WinMM, and configures 2D/3D playback. Failure stops
first-load. Because the DLL is a normal PE import, its absence prevents process
startup before this code runs.

FMOD supplies BGM streaming, interface effects, character and monster audio,
positioned environmental sound, listener updates, looping, fades, and volume
control.

## Binary compatibility

| File | SHA-256 |
|---|---|
| `Library/FMod/fmod.h` | `0188c2c5718f9f227bccc765d9a51f397716d34c003142a81be5ea8a96c7d526` |
| `Library/FMod/fmodvc.lib` | `560aaa3e34d3fa2ced9fbdac35486595bc53f65922e6a11667633b819253b6ad` |
| Supplied `fmod.dll` | `0287ce690f314826dd6ed82e77a64b518376308f4b4eeb50b73e4f2142576aec` |

The header declares `FMOD_VERSION 3.74f`. All 229 required functions exist in
the DLL with exact decorated names. Its three additional exports are additive
and harmless. The DLL is x86, VC6-era, and imports `MSACM32`, `WINMM`, and
`MSVCRT`.

The DLL is UPX-packed and was not unpacked or executed, so its reported runtime
version remains unobserved. The client accepts versions at least 3.74 and the
complete required API is present. Pairing confidence: **99%**.

The reference `NineDragons.exe` imports `fmod.dll` and contains the same FMOD
diagnostics and audio asset names.

## Sound-data closure

The expected archive exists:

```text
/Users/jamiemarshall/Downloads/Data/Sound/SR_SOUND.XP
```

| Property | Value |
|---|---|
| Size | 424,652,347 bytes |
| SHA-256 | `e43b0b3e17a98401f2cd8ffcd74e6686b49b06e9c74236953586afb9f6ebd4f0` |
| Runtime destination | `Data/Sound/SR_SOUND.XP` |

Static evidence exposes WAV references and Ogg stream signatures. The
`NOUSE_VORBIS` definition disables a separate client-side Vorbis route, not
FMOD's internal Ogg decoding. No separate Vorbis or WMA DLL is indicated.
`tsSR_SOUND.XP` is only selected under `_XTESTSERVER`, which normal US Release
does not define.

## Disposition

| Item | Status |
|---|---|
| FMOD header | Present |
| FMOD import library | Present |
| Compatible runtime DLL | Supplied; all required APIs satisfied |
| Normal sound archive | Supplied at the expected relative structure |
| Additional codec DLL | No evidence one is required |

FMOD is not a missing-dependency blocker. Remaining work is controlled runtime
placement and later behavior validation.

For preservation, retain `_XUSEFMOD` and `fmodvc.lib`, manifest the supplied
DLL beside the executable, and manifest the sound pack under `Data/Sound`.
Do not substitute FMOD Ex or FMOD Studio; their APIs are incompatible.
