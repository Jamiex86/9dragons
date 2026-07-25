# VC6 CRT and Default-Library Policy Audit

Date: 2026-07-25  
Scope: `US_Release|Win32`, static inspection only. No compiler or linker was
run.

## Conclusion

The `MSVCRT`/`LIBCMT` mixture is **not Visual Studio conversion damage**. It is
present verbatim in the original Visual C++ 6 `.dsp` projects. The Flash MP3
decoder actively requests `LIBCMT`; `radsdk6.lib` also contains that directive
but is now established as inactive in normal US Release.

The most likely historical policy was:

- compile the client executable, `XGamebase`, `XKernel`, and `XFControl` with
  `/MD`, using the DLL CRT (`MSVCRT`);
- retain old static third-party objects originally built with `/MT`;
- allow or explicitly order `LIBCMT` after `MSVCRT` to satisfy middleware-only
  runtime dependencies.

Confidence in that intent: **high (85–95%)**.

Confidence in the exact VC6 linker's interpretation of the unusual quoted
two-library `/NODEFAULTLIB` argument: **medium (65–80%)**. That behavior must be
established from an original link map/log or a later authorized linker
experiment before changing the project.

## Primary VC6 evidence

### Main player

`EmperorOfDragons.dsp`, configuration
`EmperorOfDragons - Win32 US_Release`, records:

```text
CPP ... /MD ...

LINK32=link.exe
# ADD LINK32 msvcrt.lib libcmt.lib ... ./Library/BinkSDK/binkw32.lib
  ./Library/radsdk/radsdk6.lib ...
  /nodefaultlib:"msvcrt.lib libcmt.lib"
  /out:"./Game/US/NineDragons.exe"
```

Therefore all of these facts predate `.vcxproj` conversion:

1. `/MD` was intentional for player-owned objects.
2. Both CRT libraries were explicitly named.
3. `MSVCRT` was ordered before `LIBCMT`.
4. Both names were also placed in one quoted `/NODEFAULTLIB` argument.
5. The policy appears in the US configuration alongside Acclaim-specific Bink
   and RAD dependencies.

The current `.vcxproj` preserves the same relationship with
`MultiThreadedDLL`, explicit `msvcrt.lib;libcmt.lib`, and
`IgnoreSpecificDefaultLibraries`.

### XFControl

`XFControl/XFControl.dsp`, configuration
`XFControl - Win32 US_Release`, records:

```text
CPP ... /MD ... /D "FLASHMP3" ...
LINK32 ... mp3decoder.lib ... /nodefaultlib:"libcmt.lib msvcrt.lib"
```

The current `.vcxproj` preserves `/MD`, `FLASHMP3`, `mp3decoder.lib`, and the
same two suppressed names.

### Internal libraries

The original US Release configurations of `XGamebase/XGamebase.dsp` and
`XKernel/XKernel.dsp` compile with `/MD`. The converted projects likewise
select `MultiThreadedDLL`. There is no evidence that normal player-owned code
was intended to use `/MT`.

## Middleware directive evidence

### Located `mp3decoder.lib`

The exact-interface candidate contains:

```text
-defaultlib:LIBCMT
-defaultlib:OLDNAMES
```

Its API is favorable for containing CRT ownership:

- `mp3decOpen` creates an opaque handle internally;
- `mp3decClose` destroys it inside the same library;
- decoding writes into caller-provided buffers;
- no public function asks the client to `free` a library-owned allocation.

That does not remove all dual-CRT risk, but materially lowers allocator
crossing risk at this boundary.

### `radsdk6.lib` — inactive in normal US Release

The staged RAD static library contains numerous
`/DEFAULTLIB:"LIBCMT" /DEFAULTLIB:"OLDNAMES"` directives and is named on the
US link line. Static archives are demand-loaded, however. Normal US Release
does not define `_ACCLAIM_IGAADSYSTEM`, so the RAD/Bink implementation and API
calls are preprocessed out. No RAD member, and thus no member-local CRT
directive, is expected to enter the link. The reference executable corroborates
this; see `US-PLAYER-RAD-BINK-DEAD-LINK-AUDIT.md`.

### Import libraries

Bink, FMOD, QHTM, XWebPage, SpeedTree, and DbgHelp are import libraries. Their
implementations live in DLLs, so they do not create the same static-CRT object
problem in the client link.

## Meaning of the unusual `/NODEFAULTLIB` syntax

### Superseding US Release conclusion

The user-supplied reference `XFControl.dll` imports `MSVCRT.dll`, and all
ordinary non-US release configurations suppress only `LIBCMT`. Microsoft's
syntax requires one `/NODEFAULTLIB` option per ignored library, while the
converted project retains `libcmt.lib msvcrt.lib` as one malformed list item.

For normal `US_Release|Win32`, the reconstructed policy is therefore `/MD`
plus suppression of `LIBCMT.LIB` only. `MSVCRT` must remain selected. See
`US-XFCONTROL-CRT-POLICY-RESOLUTION.md`.

The project does not emit two conventional switches:

```text
/NODEFAULTLIB:MSVCRT.LIB /NODEFAULTLIB:LIBCMT.LIB
```

It emits one quoted argument:

```text
/NODEFAULTLIB:"msvcrt.lib libcmt.lib"
```

The documented switch accepts one library name. The VC6 linker may therefore
have treated this as a single nonexistent library name containing a space. If
so, it suppressed neither CRT:

- `/MD` objects requested `MSVCRT`;
- MP3/RAD objects requested `LIBCMT`;
- the main executable additionally ordered `MSVCRT` before `LIBCMT`.

That would produce the observed mixed-runtime arrangement. It also explains
why converting the field into two effective modern `/NODEFAULTLIB` switches
could break a historically working link.

This remains an inference until confirmed with an original command echo,
`.map`/verbose library trace, independently preserved successful binary, or a
later authorized isolated VC6 linker experiment.

## Risk assessment

| Risk | Severity | Evidence |
|---|---:|---|
| Heap allocation crosses CRT boundary | High generally; reduced for MP3 API | MP3 handle creation/destruction are paired internally |
| Separate `errno`, locale, and `FILE` state | Medium | Static `LIBCMT` and DLL `MSVCRT` may coexist |
| C++ exception/RTTI crosses boundary | High if present | Old static C++ middleware; not demonstrated at the public MP3 C API |
| Modern linker changes suppression semantics | High | Modern MSBuild list handling need not preserve VC6 quoted behavior |
| Replacing everything with `/MT` | High | Changes player/DLL CRT ABI and ownership assumptions |
| Replacing everything with `/MD` | High | Proprietary MP3/RAD static objects cannot presently be recompiled |

## Preservation policy

Do not normalize the runtime settings before the first preservation-link
investigation.

The preservation matrix should retain:

- `/MD` for player-owned US Release source;
- the original library order, especially `MSVCRT` before `LIBCMT`;
- the original VC6 quoted `/NODEFAULTLIB` spelling as a recorded baseline;
- `mp3decoder.lib` as the demonstrated static-CRT exception;
- `radsdk6.lib` and `binkw32.lib` as stale inactive normal-player inputs,
  pending later verbose-link confirmation.

A modernized project should not inherit this blindly. A later modernization
route should replace or isolate static middleware behind a narrow DLL boundary
so one CRT ownership model governs each module.

## Remaining evidence gate

Before authoring any CRT linker patch, obtain at least one of:

1. an original `NineDragons.exe` and `XFControl.dll` from the same US vintage;
2. an original US `.map`, `.pdb`, `.ilk`, or full build log;
3. the exact VC6 linker version plus a later-authorized argument-parsing/link
   trace using inert test objects.

Current status:

**Historically intentional mixed-CRT policy identified; exact resolution
mechanism not yet proven.**
