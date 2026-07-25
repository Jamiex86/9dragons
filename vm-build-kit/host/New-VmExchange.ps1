[CmdletBinding(SupportsShouldProcess = $true)]
param(
    [Parameter(Mandatory = $true)]
    [string]$TransferRoot,

    [Parameter(Mandatory = $true)]
    [string]$Destination,

    [string]$RepositoryRoot
)

$ErrorActionPreference = 'Stop'

if (-not $RepositoryRoot) {
    $RepositoryRoot = Join-Path (Split-Path -Parent $MyInvocation.MyCommand.Path) '..\..'
}

$repository = (Resolve-Path -LiteralPath $RepositoryRoot).Path
$transfer = (Resolve-Path -LiteralPath $TransferRoot).Path
$destinationFull = [System.IO.Path]::GetFullPath($Destination)

if (Test-Path -LiteralPath $destinationFull) {
    throw "Destination already exists; refusing to overwrite: $destinationFull"
}

if ($destinationFull.StartsWith($repository, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw 'The VM exchange directory must be outside the Git repository.'
}

$savedWhatIfPreference = $WhatIfPreference
try {
    $WhatIfPreference = $false
    & (Join-Path $PSScriptRoot 'Verify-BuildInputs.ps1') `
        -TransferRoot $transfer `
        -RepositoryRoot $repository
}
finally {
    $WhatIfPreference = $savedWhatIfPreference
}

if (-not $PSCmdlet.ShouldProcess($destinationFull, 'Create immutable VM exchange directory')) {
    return
}

New-Item -ItemType Directory -Path $destinationFull | Out-Null

$sourceDestination = Join-Path $destinationFull 'source'
$mediaDestination = Join-Path $destinationFull 'media'
$directXDestination = Join-Path $destinationFull 'directx'
$kitDestination = Join-Path $destinationFull 'vm-build-kit'

New-Item -ItemType Directory -Path $sourceDestination | Out-Null
New-Item -ItemType Directory -Path $mediaDestination | Out-Null
New-Item -ItemType Directory -Path $directXDestination | Out-Null

$archivePath = Join-Path $destinationFull 'source.zip'
& git -C $repository archive --format=zip --output=$archivePath HEAD
if ($LASTEXITCODE -ne 0) {
    throw 'git archive failed.'
}

Copy-Item -LiteralPath (Join-Path $repository 'vm-build-kit') -Destination $kitDestination -Recurse

Copy-Item `
    -Path (Join-Path $transfer '04-preservation-toolchain\extracted\toolchain-quarantine\*') `
    -Destination $mediaDestination `
    -Recurse

Copy-Item `
    -Path (Join-Path $transfer '05-directx-installers\extracted\*') `
    -Destination $directXDestination `
    -Recurse

$manifest = @()
Get-ChildItem -LiteralPath $destinationFull -Recurse -File | ForEach-Object {
    $relative = $_.FullName.Substring($destinationFull.Length + 1).Replace('\', '/')
    $hash = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    $manifest += "$hash  $relative"
}

$manifest | Set-Content -LiteralPath (Join-Path $destinationFull 'SHA256SUMS.txt') -Encoding ASCII

Write-Host "VM exchange created: $destinationFull"
Write-Host 'No installer, compiler, linker, or executable was run.'
