# External Client Artifacts

These artifacts are part of the preservation project but are not stored in
Git. They must be copied from the canonical Mac or another verified carrier to
the Windows quarantine area.

| Artifact | Mac size | Git policy | Verification |
|---|---:|---|---|
| `EmperorOfDragons.zip` | 408,285,930 bytes | External storage | SHA-256 recorded in `MIGRATION_WINDOWS.md` |
| Runtime `Data/` | Approximately 6.1 GB, 44 files | External storage | `external-artifacts/runtime-data-SHA256SUMS.txt` |
| Seven reference runtime files | Approximately 9.7 MB | External quarantine pending redistribution decision | `external-artifacts/reference-runtime-SHA256SUMS.txt` |
| Preservation toolchain media | Approximately 7.5 GB | External controlled storage | `external-artifacts/toolchain-SHA256SUMS.txt` |
| DirectX October 2004 installer carriers | Approximately 282 MB | External controlled storage | Acquisition reports and dependency audits |

The coherent VC6 DirectX headers and libraries required for continued static
work are stored under `external-dependencies/DirectX9-October2004-VC6/`.
Their complete hashes are recorded in `EXTERNAL-DEPENDENCIES-SHA256.txt`.

The repository also contains the small quarantined SpeedTree and Flash MP3
candidates. Their presence preserves the current evidence state; it does not
resolve licensing, redistribution, or promotion approval.

Do not split an ISO merely to force it into Git. Preserve the original file,
filename, byte count, and digest through external storage.

