# Preservation Toolchain Acquisition Locator

Date: 2026-07-25
Scope: acquisition and static verification only. No installer, ISO, executable,
or VM was launched; no ISO was mounted.

## Outcome

Every required toolchain layer now has a quarantined acquisition candidate.
The October 2004 DirectX layer was already present locally. VC6, SP6, the
February 2003 Platform SDK carrier, and Windows XP media were acquired and
verified byte-for-byte against the catalog metadata.

Availability is not the same as authorization: Visual Studio and Windows
installation media require the user's own valid licence/key or other lawful
entitlement.

| Layer | Located | Local now | Confidence |
|---|---:|---:|---:|
| Windows XP Professional SP3 x86 | Yes | Yes | 99% catalog identity; all published hashes match |
| Visual Studio 6.0 Enterprise/Visual C++ 6.0 RTM | Yes | Yes | 98% original-disc candidate; all catalog hashes match |
| Full Visual Studio 6.0 SP6 containing VC++ fixes | Yes | Yes | 99% package identity; documented SHA-1 matches |
| Platform SDK February 2003 | Yes | Yes | 99% carrier identity; catalog hashes and internal payload match |
| DirectX SDK October 2004 main SDK | Yes | Yes | 99% |
| DirectX October 2004 VC6 Extras | Yes | Yes | 99% |
| x86 VM application for Apple Silicon Mac | Not selected | No detected installation | Acquisition choice remains |

## 1. Windows XP Professional SP3 x86

Preferred media identity:

```text
en_windows_xp_professional_with_service_pack_3_x86_cd_x14-80428.iso
Edition: English retail/FPP
Bytes: 617,756,672
MD5: f424a52153e6e5ed4c0d44235cf545d5
SHA-1: 1c735b38931bf57fb14ebd9a9ba253ceb443d459
SHA-256: 62b6c91563bad6cd12a352aa018627c314cfc5162d8e9f8af0756a642e602a46
```

Located catalog:

```text
https://archive.org/details/XPPRO_SP3_ENU
```

The catalog exposes the original ISO and supplies matching MD5/SHA-1 metadata.
Use the retail/FPP image only if the user owns a valid compatible licence/key.
Do not use modified, unattended, preactivated, “lite,” or preconfigured VM
images.

The volume-licence alternative is also identifiable, but should only be used
with an actual volume entitlement:

```text
en_windows_xp_professional_with_service_pack_3_x86_cd_vl_x14-73974.iso
Bytes: 617,754,624
SHA-1: 66ac289ae27724c5ae17139227cbe78c01eefe40
SHA-256: fd8c8d42c1581e8767217fe800bfc0d5649c0ad20d754c927d6c763e446d1927
```

## 2. Visual Studio 6.0 / Visual C++ 6.0 RTM

Best located English original-disc candidate:

```text
https://archive.org/details/ms-vs60ent-devdays98
```

Catalog identity:

```text
Microsoft Visual Studio 6.0 Enterprise Edition — DevDays '98 — RTM
VSE600ENU1.ISO  643,289,088 bytes
MD5              0b72992a8fec95cb395857d1f66c18f5
SHA-1            fc2f568db490d4115e301a400e15082888bf7296

VSE600ENU2.ISO  616,828,928 bytes
MD5              e9de0e46288cea4ba72b1b2ea23eb24e
SHA-1            5cde8ac7660bba9ee7a8cab705ac25e87c78ea64

VSE600ENU3.ISO  104,286,208 bytes
MD5              160bb7a803439a5ed73a1faf4d940eff
SHA-1            0529fd68bfd7f01c9af5922480453de781ba7656
```

This is third-party archival hosting of Microsoft media, not a current
Microsoft download. Treat it as a candidate until hashes, volume labels,
filesystem dates, signatures where available, and installed compiler versions
are independently recorded. A valid Visual Studio/Visual C++ licence is still
required.

An alternate item `X04-58174` was rejected as the preferred candidate because
its manifest has a user report disputing Disc 1.

## 3. Full Visual Studio 6.0 Service Pack 6

The required package is:

```text
Vs6sp6.exe
Expected size: approximately 61 MiB
Expected SHA-1: 2292437a8967349261c810ae8b456592eeb76620
Historical Microsoft URL:
https://download.microsoft.com/download/1/9/f/
19fe4660-5792-4683-99e0-8d48c22eed74/Vs6sp6.exe
```

A live archival mirror was located at:

```text
https://ftp.zx.net.nz/pub/dev/microsoft-updates/vc6/Vs6sp6.exe
```

The SHA-1 is independently recorded in a US Election Assistance Commission
test-plan evidence table for a build environment using Visual Studio 6 SP6.
The mirror must not be trusted by name alone: acquisition is acceptable only
if its computed SHA-1 is exactly the value above, after which SHA-256 must also
be recorded.

Do not substitute `Vs6sp6B.exe`. That 26.49 MiB package is the Visual Basic
service-pack distribution; it is not evidence of the full Visual C++ update.

## 4. Microsoft Platform SDK February 2003

Located on an MSDN subscription disc:

```text
https://archive.org/details/msdn-july-2004-disc-2426.8
File: MSDN July 2004 Disc 2426.8.iso
Bytes: 6,023,501,824
MD5: df752baf72e4f7c50268cea5c89fa2fa
SHA-1: 10858ca08f43181a62d83b2785215a9ee405eaec
```

The scanned/recorded disc label explicitly lists “Platform SDK February 2003
Edition.” The disc is large because it contains several additional SDKs.

Microsoft's later Server 2003 SP1 Platform SDK page confirms that February
2003 was the last Platform SDK compatible with VC6:

```text
https://www.microsoft.com/en-ie/download/details.aspx?id=15656
```

This candidate still needs internal disc inspection before acquisition is
closed.

Static 7-Zip inspection is now complete. The carrier contains
`ENGLISH/PLATSDK`, including `SETUP.EXE`, `SETUP/CORESDK-X86.MSI`,
`CORESDK-X86.CAB`, the common Core SDK CAB sequence, x86 debugger/WMI/Windows
Media SDK packages, and the SDK header/library payload. The relevant files
carry February-April 2003 timestamps. This confirms the February 2003 Platform
SDK payload independently of the outer disc label. The ISO was listed directly
and was not mounted.

Verified local carrier:

```text
recovery/toolchain-quarantine/platform-sdk/MSDN July 2004 Disc 2426.8.iso
Bytes: 6,023,501,824
MD5: df752baf72e4f7c50268cea5c89fa2fa
SHA-1: 10858ca08f43181a62d83b2785215a9ee405eaec
SHA-256: 191d8dce9cc30b5e7b9b1f2b60f6d0060c0dca519462255322d7217ce5f0b741
```

## 5. DirectX October 2004

Already held in quarantine:

```text
recovery/emperor-of-dragons-normal-client-overlay/
candidate-dependencies/DirectX9-October2004-untrusted/
```

Main SDK:

```text
dxsdk_oct2004.internet-archive.untrusted.exe
Bytes: 229,290,240
SHA-256: 9995e138a009572abe2292f051fbb0a21f3b3478e07527fa718c1c4a2e09fb3b
```

VC6 Extras:

```text
dxsdk_oct2004_extras.internet-archive.untrusted.exe
Bytes: 66,817,792
SHA-256: 3820eb13684d4dd7aa02d2880767886149ddd7d4ace5ef15145a97bd79c5d22f
```

The statically extracted coherent include/library set is already present under
`Coherent-VC6-October2004/`. Neither installer was executed.

## 6. VM host requirement

This Mac is Apple Silicon (`arm64`) running macOS 26.5.2. No UTM, QEMU,
VirtualBox, VMware, or Parallels installation was detected in the normal
application/command locations.

The VM must emulate an x86 PC; ARM virtualization alone cannot run 32-bit
Windows XP. The safest preservation choice is an x86-emulation VM with:

- networking disabled;
- immutable base disk plus snapshots;
- 1 virtual CPU initially;
- 512 MiB–1 GiB RAM;
- a separate exchange disk rather than shared folders;
- no clipboard integration or host filesystem write-through;
- clock and VM configuration recorded.

VM software selection and installation require a separate user decision.

## What is and is not closed

Closed:

- exact product generations;
- viable acquisition locations;
- DirectX local availability;
- local acquisition of VC6, SP6, XP, and the Platform SDK carrier;
- full carrier size and hash verification;
- static confirmation of the February 2003 Platform SDK x86 payload;
- reference filenames and available hashes.

Still required:

1. Preserve the user's lawful VS6 and Windows XP licence/key evidence.
2. Choose and install an Apple-Silicon-capable x86 emulator.
3. Create the offline XP VM and snapshot it before any tool installation.
4. Install and version-capture each layer only inside that isolated VM.
5. Compare the resulting compiler, linker, headers, and libraries against the
   client project requirements before permitting a controlled build.

The complete SHA-256 carrier manifest is
`recovery/toolchain-quarantine/SHA256SUMS.txt`.

No build should occur during acquisition or VM preparation.
