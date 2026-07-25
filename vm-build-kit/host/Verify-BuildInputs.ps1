[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$TransferRoot,

    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'

function Get-NormalizedHash {
    param([Parameter(Mandatory = $true)][string]$Path)
    (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.ToLowerInvariant()
}

function Get-LfNormalizedHash {
    param([Parameter(Mandatory = $true)][string]$Path)

    $bytes = [System.IO.File]::ReadAllBytes($Path)
    $normalized = New-Object System.IO.MemoryStream
    try {
        for ($index = 0; $index -lt $bytes.Length; $index++) {
            if (
                $bytes[$index] -eq 13 -and
                ($index + 1) -lt $bytes.Length -and
                $bytes[$index + 1] -eq 10
            ) {
                continue
            }
            $normalized.WriteByte($bytes[$index])
        }
        $normalized.Position = 0
        $sha = [System.Security.Cryptography.SHA256]::Create()
        try {
            ([BitConverter]::ToString($sha.ComputeHash($normalized))).Replace('-', '').ToLowerInvariant()
        }
        finally {
            $sha.Dispose()
        }
    }
    finally {
        $normalized.Dispose()
    }
}

function Test-Manifest {
    param(
        [Parameter(Mandatory = $true)][string]$Manifest,
        [Parameter(Mandatory = $true)][string]$BaseDirectory
    )

    $checked = 0
    foreach ($line in Get-Content -LiteralPath $Manifest) {
        if ($line -notmatch '^([0-9a-fA-F]{64})\s+(.+)$') {
            continue
        }

        $expected = $matches[1].ToLowerInvariant()
        $relative = $matches[2].Trim() -replace '/', '\'
        $candidate = Join-Path $BaseDirectory $relative

        if (-not (Test-Path -LiteralPath $candidate -PathType Leaf)) {
            throw "Manifest file is missing: $candidate"
        }

        $actual = Get-NormalizedHash -Path $candidate
        if ($actual -ne $expected) {
            $textExtensions = @(
                '.c', '.cpp', '.h', '.hpp', '.idl', '.inl', '.md', '.rc',
                '.txt'
            )
            if ([System.IO.Path]::GetExtension($candidate).ToLowerInvariant() -in $textExtensions) {
                $actual = Get-LfNormalizedHash -Path $candidate
            }
        }

        if ($actual -ne $expected) {
            throw "Hash mismatch: $candidate`nExpected: $expected`nActual:   $actual"
        }
        $checked++
    }

    if ($checked -eq 0) {
        throw "Manifest contains no SHA-256 entries: $Manifest"
    }

    [PSCustomObject]@{
        Manifest = $Manifest
        Entries  = $checked
        Status   = 'OK'
    }
}

if (-not $RepositoryRoot) {
    $RepositoryRoot = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) '..\..'
}

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$transfer = (Resolve-Path -LiteralPath $TransferRoot).Path

$expectedCryptoHash = 'c78a0ce9eb0285c584a34c70ee637397417568cf4c975973cce443933da4160f'
$cryptoPath = Join-Path $repository 'XKernel\XSecurity\XCrypto.Cpp'

if (-not (Test-Path -LiteralPath $cryptoPath -PathType Leaf)) {
    throw "Recovered source is missing: $cryptoPath"
}

$cryptoHash = Get-NormalizedHash -Path $cryptoPath
if ($cryptoHash -ne $expectedCryptoHash) {
    throw "XCrypto.Cpp does not match the authoritative archive."
}

$dependencyManifest = Join-Path $repository 'EXTERNAL-DEPENDENCIES-SHA256.txt'
$checks = @(
    Test-Manifest -Manifest $dependencyManifest -BaseDirectory $repository
    Test-Manifest `
        -Manifest (Join-Path $transfer '02-runtime-data\runtime-data-SHA256SUMS.txt') `
        -BaseDirectory (Join-Path $transfer '02-runtime-data\extracted')
    Test-Manifest `
        -Manifest (Join-Path $transfer '03-reference-binaries\reference-runtime-SHA256SUMS.txt') `
        -BaseDirectory (Join-Path $transfer '03-reference-binaries\extracted')
    Test-Manifest `
        -Manifest (Join-Path $transfer '04-preservation-toolchain\toolchain-SHA256SUMS.txt') `
        -BaseDirectory (Join-Path $transfer '04-preservation-toolchain\extracted\toolchain-quarantine')
    Test-Manifest `
        -Manifest (Join-Path $transfer '05-directx-installers\SHA256SUMS.txt') `
        -BaseDirectory (Join-Path $transfer '05-directx-installers\extracted')
)

$requiredProjects = @(
    'EmperorOfDragons.dsp',
    'XGamebase\XGamebase.dsp',
    'XKernel\XKernel.dsp',
    'XFControl\XFControl.dsp'
)

foreach ($relative in $requiredProjects) {
    $project = Join-Path $repository $relative
    if (-not (Test-Path -LiteralPath $project -PathType Leaf)) {
        throw "Required VC6 project is missing: $project"
    }
}

$gitHead = (& git -C $repository rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0) {
    throw "Unable to read the repository commit."
}

$branch = (& git -C $repository branch --show-current).Trim()
$status = @(& git -C $repository status --short --untracked-files=no)
$substantiveStatus = @(
    $status | Where-Object {
        $_ -notmatch 'audio_sdk/.+\.dsp$' -and
        $_ -notmatch 'qhtm sdk/.+\.(dsp|dsw|rc)"?$'
    }
)

if ($substantiveStatus.Count -gt 0) {
    throw "Unexpected working-tree changes exist:`n$($substantiveStatus -join "`n")"
}

$checks | Format-Table -AutoSize

[PSCustomObject]@{
    Repository  = $repository
    Branch      = $branch
    Commit      = $gitHead
    XCryptoHash = $cryptoHash
    Result      = 'All build inputs verified; no build performed.'
} | Format-List
