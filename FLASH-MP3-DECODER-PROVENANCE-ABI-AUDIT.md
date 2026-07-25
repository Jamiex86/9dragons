# Flash MP3 Decoder Provenance and ABI Audit

## Outcome

The quarantined `mp3decoder.lib` is a technically exact-lineage candidate for
the normal player and launcher. It is not deployment-approved because its
headers explicitly identify Fraunhofer IIS copyright and state “All Rights
Reserved,” while the public GitHub repository supplies no license or original
distribution provenance for the binary.

No library, DLL, client, launcher, or test program was executed.

## Candidate identity

| Property | Evidence |
|---|---|
| File | `candidate-dependencies/FlashMP3/mp3decoder.lib` |
| SHA-256 | `3d27b37d6bf9ba95d8d3f5e1c75c1256b0759f76f6980627dbd936f25bd9558c` |
| Format | Microsoft COFF i386 static archive |
| Object members | 21 implementation objects |
| Public interface | 11 exact Win32 stdcall entry points |
| Default libraries | `LIBCMT`, `OLDNAMES`, and one `uuid.lib` directive |
| Implementation paths | `D:\code\mp3decoder\mp3decoder\...` |
| Interface copyright | Fraunhofer IIS, 1998, All Rights Reserved |

## Repository provenance

The candidate was recovered from:

`playbar/nstest/dependency/mp3decoder/lib/mp3decoder.lib`

The path entered GitHub in the repository's unsigned root commit:

- commit: `cb8538c0f1177c1265c850d02bfde693d26211da`
- author/committer date: 1 August 2016
- message: `add files`
- parent commits: none

The later commit previously used as the pin
(`d56141912bc2b0e22d1652aa7aff182e05142005`) did not modify the decoder; it
merely still contained it. Path history contains only the initial import.
Consequently GitHub proves preservation, not authorship, authorization,
redistribution rights, or a chain back to Fraunhofer/Macromedia.

## Source-header match

The repository candidate's `mp3decifc.h`, `mp3sscdef.h`, and
`mp3streaminfo.h` were already confirmed byte-identical to the corresponding
`XFControl` files.

The player archive independently contains a second identical header set under
`XLauncher/FlashCtrl/`. Both projects enable `FLASHMP3` and name
`mp3decoder.lib`, showing that this was a shared decoder dependency rather than
an accidental orphan under `XFControl`.

The headers include CVS lineage from `/home/cvs/mms/corelibs/mp3dec` dated
1998–1999 and identify the implementation as the Fraunhofer IIS MPEG Decoder.

## Reference-binary match

Static string comparison between the candidate archive and the user-supplied
reference `XFControl.dll` found all 19 meaningful decoder diagnostic strings
byte-for-byte, including the complete success, handle, memory, parameter, I/O,
CRC, MPEG-layer, buffer, main-data, and synchronization result set.

This is much stronger than a coincidental API-name match. Combined evidence is:

1. three byte-identical public headers;
2. all 11 exact decorated entry points;
3. all 19 meaningful implementation diagnostics in the shipped reference DLL;
4. matching historical Fraunhofer/Macromedia decoder lineage;
5. the dependency named by both original project families.

Technical identity confidence is **99–100% at the callable and implementation
lineage boundary**. It is not proof that the archive bytes are identical to the
original library used for the shipped link; the final DLL does not retain an
extractable copy of each input object.

## CRT boundary

The candidate requests the static multithreaded `LIBCMT` runtime. Player-owned
`XFControl` code was built with `/MD`, while the original `.dsp` deliberately
suppresses `LIBCMT`—and in some US configurations also suppresses `MSVCRT`.

The decoder interface contains allocator ownership well:

- `mp3decOpen` allocates and returns an opaque handle;
- `mp3decClose` destroys it inside the same library;
- decode/fill operations use caller-provided buffers;
- no public API requires the caller to free decoder-owned memory.

This reduces cross-CRT heap risk but does not explain the historical
`/NODEFAULTLIB` policy. The candidate must not be forced through a modern link
by ignoring conflicts.

## Criticality and options

| Option | Technical fidelity | Legal/provenance position | Decision |
|---|---:|---:|---|
| Preserve candidate in quarantine | Excellent | Safe as evidence | Done |
| Promote candidate into active link path | Excellent | Unresolved proprietary redistribution/use | Not approved |
| Remove `FLASHMP3` | Changes Flash/launcher audio behavior | Low dependency risk | Not faithful |
| Implement a clean-room API-compatible adapter using a permitted decoder | Potentially high after validation | Potentially supportable | Preferred future replacement if no license is obtained |
| Route Flash audio through FMOD | Larger behavioral/timing change | Depends on FMOD rights | Modernization only |

For a faithful historical reconstruction, this remains a legal/provenance gate,
not a missing-binary or unknown-ABI gate. For a distributable future client, a
clean-room adapter is likely safer unless documentary permission for the
Fraunhofer library is obtained.

## Verdict

`mp3decoder.lib` is no longer technically missing: an essentially exact
candidate is preserved. It remains prohibited from active promotion because
the available public source establishes no license chain and the headers
expressly reserve rights.

The remaining engineering blocker is the historical CRT/default-library
policy. The remaining non-engineering blocker is authorization to use or
redistribute the proprietary decoder.
