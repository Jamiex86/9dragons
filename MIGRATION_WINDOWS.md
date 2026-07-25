# Windows Migration and Preservation Setup

## Scope

This repository is the canonical migration snapshot for the normal
`US_Release|Win32` Emperor of Dragons player client. It does not contain the
HeavenFall website, API, production configuration, production secrets, or
server deployment material.

The Mac working tree was the source of truth. Existing client source changes,
audit reports, recovered middleware files, comparison evidence, proposed
player-only patches, and forensic helper scripts are preserved here.

No build was performed while preparing this migration.

## Repository layout

- The repository root is the assembled player-client source tree.
- `external-dependencies/` contains the hash-pinned DirectX October 2004 VC6
  include/library set and quarantined SpeedTree/Flash dependency candidates.
- `migration-evidence/` contains the earlier overlay analyses, proposals,
  reference-only files, and original archive/GitHub comparison evidence.
- `tools/forensics/` contains the Python static-analysis helpers.
- `external-artifacts/` contains checksum manifests for large material that
  must be transferred separately.

The current player working tree is authoritative when it differs from the
earlier overlay. In particular, do not overwrite the current
`XProcess/XProc_MainGame.cpp` or
`XProcess/XProc_MainGameCallBackFunctions.cpp` with overlay copies.

## Required Windows software

### Basic migration tools

1. Git for Windows.
2. PowerShell 7 is recommended; Windows PowerShell is sufficient for basic
   checksum verification.
3. Python 3 for the optional forensic scripts.
4. Codex for Windows, opened with this cloned directory as the workspace.

### Preservation compiler environment

The intended historical build environment is:

- an isolated 32-bit Windows XP SP3 VM;
- Microsoft Visual C++ 6.0 with Service Pack 6;
- the VC6 SP6 `cl.exe`, `link.exe`, `lib.exe`, resource compiler, and MIDL
  tools from one installation;
- Microsoft Platform SDK February 2003 Edition, x86;
- Microsoft DirectX 9.0c SDK October 2004, x86;
- Microsoft's October 2004 VC6 Extras D3DX libraries;
- the original `.dsp` and `.dsw` projects;
- native VC6 language behavior, multi-byte character set, and the audited CRT
  policy.

The preferred first preservation configuration remains
`US_Release|Win32`. Do not treat the converted v142 projects as ABI-equivalent
to the historical toolchain.

See `FROZEN-PRESERVATION-TOOLCHAIN-SPEC.md`,
`PRESERVATION-TOOLCHAIN-MATRIX.md`, and
`PRESERVATION-PLACEMENT-MANIFEST.md` before installing or placing any
dependency.

## External material

The following is deliberately not stored in Git:

- the authoritative 408,285,930-byte `EmperorOfDragons.zip`;
- the 6.1 GB runtime `Data` directory;
- reference runtime DLLs and executables;
- Windows XP, Visual Studio 6, Service Pack 6, and Platform SDK media;
- original DirectX SDK installer carriers.

Transfer these using controlled external storage. Verify them against the
files in `external-artifacts/` before using them. The authoritative source ZIP
must have SHA-256:

```text
4b2daf495fdc7aa5a13f7ee0f784157c2a3877deb12a9923d5c5ac9f10a6bda7
```

Do not put installation media into ordinary Git or Git LFS. Several files
exceed common hosting limits and have separate licensing/redistribution
requirements.

## Clone and verify

After the repository URL is assigned:

```powershell
git clone <REPOSITORY-URL> EmperorOfDragonsClient
Set-Location EmperorOfDragonsClient
git status --short
git log -1 --oneline
```

The clone must be clean before external artifacts are introduced.

Verify the Git-contained dependency set:

```powershell
Get-Content .\EXTERNAL-DEPENDENCIES-SHA256.txt | ForEach-Object {
    $hash, $path = $_ -split '\s+', 2
    $actual = (Get-FileHash -Algorithm SHA256 $path.Trim()).Hash.ToLower()
    if ($actual -ne $hash) { throw "Hash mismatch: $path" }
}
```

Copy the externally transferred material into a separate quarantine directory,
not directly over this repository. Verify its manifests before any promotion.

## Codex on Windows

1. Clone the repository with Git for Windows.
2. Start Codex and select the cloned `EmperorOfDragonsClient` directory.
3. Keep the externally transferred media and runtime data outside the Git
   working tree.
4. Give Codex the paths to those quarantine directories when static inspection
   is required.
5. Do not compile until the toolchain installation and dependency placement
   gates in the preservation documents are satisfied.

No environment secrets are required for static client analysis. Do not copy
website `.env` files, production credentials, JWT keys, SSH keys, or API
tokens into this repository.

## Current critical gate

`XKernel/XSecurity/XCrypto.Cpp` is documented as present in the authoritative
archive but absent from the assembled tree. It must be restored from that
hash-verified archive through a separately reviewed source-recovery action.
Migration preparation does not silently introduce it.

