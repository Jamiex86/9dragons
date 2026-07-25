# US `XFControl` CRT Policy Resolution

## Outcome

For the normal `US_Release|Win32` player, the intended CRT policy is:

- compile `XFControl` with VC6 `/MD`;
- retain `MSVCRT.lib` as the selected release CRT import library;
- suppress the static decoder's `/DEFAULTLIB:LIBCMT`;
- do **not** suppress `MSVCRT`;
- keep decoder allocation/deallocation inside the decoder API boundary.

This is a static reconstruction. No link or execution was performed.

## Project-family evidence

The original `XFControl.dsp` consistently compiles release configurations with
`/MD`. Every ordinary non-US regional release line suppresses only:

```text
/NODEFAULTLIB:"LIBCMT.LIB"
```

The US Release line uniquely contains:

```text
/nodefaultlib:"libcmt.lib msvcrt.lib"
```

Microsoft's linker syntax accepts one library name per `/NODEFAULTLIB:library`
option and directs users to specify a separate option for each library. Quoting
the entire space-separated phrase does not constitute two canonical options.
It denotes one malformed library-name argument unless the IDE rewrites it
before invocation.

The converted `.vcxproj` preserved the phrase as a single
`IgnoreSpecificDefaultLibraries` item:

```text
libcmt.lib msvcrt.lib;%(IgnoreSpecificDefaultLibraries)
```

Modern Visual Studio expects semicolon-separated library names in that
property. The conversion therefore did not safely normalize the original
ambiguity.

## Shipped-binary evidence

The user-supplied reference:

`XFControl.dll`

has SHA-256:

`ab2716c0abf5bc5c73e4ae32528cc5bf874b446ab46de93d9cdfaa95ae6c29ec`

Its PE import table explicitly names `MSVCRT.dll` and imports release CRT
functions including:

```text
__CxxFrameHandler
_adjust_fdiv
_ftol
_initterm
_purecall
calloc
fopen
fread
free
frexp
getenv
malloc
printf
sprintf
sscanf
strchr
```

This proves the shipped US DLL uses the dynamic release CRT. It rules out an
interpretation where the intended final policy removed `MSVCRT` and used only
the static CRT.

## Decoder directive evidence

The exact-lineage `mp3decoder.lib` requests:

```text
/DEFAULTLIB:LIBCMT
/DEFAULTLIB:OLDNAMES
```

The decoder's interface keeps heap ownership contained:

- an opaque handle is created by `mp3decOpen`;
- the same handle is destroyed by `mp3decClose`;
- caller-owned buffers are passed into decode/fill operations;
- no API requires the caller to free decoder-owned memory.

Suppressing the decoder's `LIBCMT` default and satisfying its ordinary runtime
references through the `/MD` link is therefore consistent with the original
regional projects and the shipped DLL.

## Interpretation of the US anomaly

The most likely explanation is a hand-edited US project typo: the author tried
to express multiple exclusions inside one quoted argument, or accidentally
retained `MSVCRT` from CRT-conflict troubleshooting. It is not a distinct,
coherent US runtime design because:

1. US compilation is `/MD`;
2. all peer release configurations suppress only `LIBCMT`;
3. the final US DLL imports `MSVCRT.dll`;
4. suppressing both CRTs without explicit replacement libraries would leave
   ordinary runtime references unresolved;
5. the converted property remains syntactically malformed as a single item.

Confidence in the reconstructed normal-player policy: **95–99%**.

## Future controlled project correction

For `US_Release|Win32` only, the eventual reviewed project change should
normalize:

```xml
<IgnoreSpecificDefaultLibraries>
  LIBCMT.LIB;%(IgnoreSpecificDefaultLibraries)
</IgnoreSpecificDefaultLibraries>
```

It must not retain the space-separated `libcmt.lib msvcrt.lib` item, and it
must not add `MSVCRT.lib` to the ignore list.

This recommendation does not apply automatically to debug, release-debug, GM,
launcher, or main executable configurations. Those need their own selected CRT
and static-library closure. In particular, the launcher's historical
`/NODEFAULTLIB:LIBC` addresses the obsolete single-threaded CRT and is a
different rule.

## Validation gate

When building is eventually authorized, the first link must use verbose library
tracing and a map file. It must demonstrate:

1. `LIBCMT.lib` is not searched or selected;
2. `MSVCRT.lib` is selected;
3. no `LIBCMTD`, `MSVCRTD`, or single-threaded `LIBC` enters the release link;
4. there is no `LNK4098` warning;
5. decoder `new/delete`, allocation, and exception-support references resolve
   consistently;
6. the output import table retains `MSVCRT.dll`;
7. the map is compared with the reference DLL at subsystem and symbol-family
   level.

Until that trace exists, this is a high-confidence policy reconstruction—not a
claim that the link is proven.
