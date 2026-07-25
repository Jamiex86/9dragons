# Emperor of Dragons Preservation VM Build Kit

This directory prepares the normal `US_Release|Win32` preservation build. It
does not authorize or perform installation, compilation, linking, or execution.

The modern Windows host is responsible for Git, checksum verification, source
snapshot creation, evidence retention, and output quarantine. The Windows XP
guest is responsible only for running the frozen historical tools after the
documented review gates are satisfied.

## Frozen build family

- Windows XP Professional SP3, 32-bit, offline VM
- Visual C++ 6.0 with Service Pack 6
- Microsoft Platform SDK February 2003 Edition, x86
- DirectX 9.0c SDK October 2004, x86
- October 2004 `Extras/D3dx9 Visual Studio 6` libraries
- complete D3DX v22 header family
- original `.dsp` and `.dsw` projects
- normal `US_Release|Win32`, MBCS, x86

Do not expose the preservation build to Visual Studio 2019/2022, a current
Windows SDK, D3DX v43 headers, or machine-global third-party library paths.

## Directory roles

```text
vm-build-kit/
  host/
    Verify-BuildInputs.ps1
    New-VmExchange.ps1
  guest/
    local-paths.example.cmd
    Set-Preservation-Environment.cmd
    Preflight.cmd
    Capture-Environment.cmd
    Build-US-Release.cmd
```

The host scripts are safe to run before the VM exists:

```powershell
Set-Location C:\path\to\9dragons

powershell -NoProfile -ExecutionPolicy Bypass `
  -File .\vm-build-kit\host\Verify-BuildInputs.ps1 `
  -TransferRoot C:\path\to\9dragons-quarantine\9dragons-windows-transfer
```

`New-VmExchange.ps1` creates a new exchange directory. It refuses to overwrite
an existing destination and does not place licensed media in Git.

## Guest workflow

1. Create the isolated XP VM with networking, shared folders, clipboard, and
   host write-through disabled.
2. Snapshot the clean XP installation.
3. Attach read-only copies of the verified installation media.
4. Install VC6, VC6 SP6, Platform SDK February 2003, and DirectX October 2004.
5. Copy `guest/` and the reviewed source snapshot to a guest-local work disk.
6. Copy `local-paths.example.cmd` to `local-paths.cmd` and edit only the four
   path values.
7. Run `Preflight.cmd`.
8. Run `Capture-Environment.cmd` and return its evidence directory to the host.
9. Use VC6 to generate the reviewed `.dsp` changes described by
   `VC6-RESTORATION-PATCHSET.md`.
10. Return the generated `.dsp` files to the host for diff review.
11. Build only after that diff and the dependency-promotion gates are approved.

## Hard gates

The build command refuses to run unless all of these are true:

- `local-paths.cmd` exists;
- the expected VC6, Platform SDK, DirectX, and source paths exist;
- `D3DX_SDK_VERSION` resolves to `22`;
- `XKernel\XSecurity\XCrypto.Cpp` exists;
- the caller explicitly sets `EOD_BUILD_AUTHORIZED=YES`.

The authorization variable prevents an accidental build; it is not a
substitute for reviewing the project patch, dependency provenance, or linker
policy.

## Required build order

1. `XGamebase - Win32 US_Release`
2. `XKernel - Win32 US_Release`
3. `XFControl - Win32 US_Release`
4. `EmperorOfDragons - Win32 US_Release`

The guest script uses `msdev.exe` with the original `.dsp` files. It captures
one log per project and stops on the first failure. It does not run the
resulting DLL or executable.

## Output handling

All produced objects, libraries, DLLs, executables, maps, logs, and environment
evidence are untrusted build outputs. Copy them to a new host-side quarantine
directory, hash them, and compare them with the reference evidence before any
runtime packaging or execution.
