# Flash MP3 decoder provisional restoration candidate

Status: **quarantined candidate — not deployment-approved**

This directory deliberately sits outside the active player source overlay and
all project library-search paths. The binary is retained for static
compatibility analysis and a later controlled integration decision. It has not
been executed or linked.

## Candidate

| File | Origin | SHA-256 |
|---|---|---|
| `mp3decoder.lib` | `playbar/nstest`, root commit `cb8538c0f1177c1265c850d02bfde693d26211da`, `dependency/mp3decoder/lib/mp3decoder.lib` | `3d27b37d6bf9ba95d8d3f5e1c75c1256b0759f76f6980627dbd936f25bd9558c` |

Repository source:

`https://github.com/playbar/nstest/tree/d56141912bc2b0e22d1652aa7aff182e05142005/dependency/mp3decoder`

## Why this is a strong match

- Every member is 32-bit Microsoft COFF (`coff-i386`).
- It implements all 11 functions declared by the client's MP3 interface with
  exact Win32 stdcall names and argument-byte counts:

```text
_mp3decOpen@20
_mp3decClose@4
_mp3decReset@4
_mp3decDecode@16
_mp3decGetStreamInfo@4
_mp3decFill@16
_mp3decGetInputFree@8
_mp3decGetInputLeft@8
_mp3decSetInputEof@4
_mp3decIsEof@4
_mp3decGetErrorText@8
```

- The repository's `mp3decifc.h`, `mp3sscdef.h`, and `mp3streaminfo.h` are
  byte-identical to the corresponding headers in `XFControl`.
- Embedded object paths identify the expected historical implementation tree:
  `D:\code\mp3decoder\mp3decoder\corelibs\mp3dec`.
- The API belongs to the historical Macromedia/Fraunhofer Flash
  `fhdecoder` lineage, matching `XFControl`'s `FLASHMP3` integration.

The user-supplied reference `XFControl.dll` also contains all 19 meaningful
decoder diagnostics found in the archive, byte-for-byte.

Technical confidence at the declared API/ABI and implementation-lineage
boundary: **99–100%**.

## Important CRT finding

The archive's linker directives request `LIBCMT`/`LIBCMT.lib`, showing that the
candidate was built against the static multithreaded CRT. The player projects
select `/MD` and contain unusual explicit/default-library suppression involving
both `LIBCMT` and `MSVCRT`.

That relationship is evidence, not permission to force a link. The original
VC6 linker policy must be reconstructed before this candidate is placed at
`XFControl/mp3decoder.lib` or exposed through a project library path.

## Remaining gates

1. Establish lawful use and redistribution status for this historical decoder.
2. Preserve the repository commit and verify the binary against another
   independently sourced copy if one becomes available.
3. Reconstruct the VC6 CRT/default-library policy.
4. Define the exact controlled placement and record it as a separate change.
5. If a controlled build is eventually authorized, test Flash MP3 decoding
   behavior independently of FMOD game audio.
