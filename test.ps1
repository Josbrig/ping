<#!
PowerShell script to configure (if needed), build, run ctest, and optionally run pingstats against one or more hosts.
Usage:
  ./test.ps1 [-Config Debug|Release] [-Help] [--] host1 [host2 ...]
Examples:
  ./test.ps1                    # build+ctest only (Debug)
  ./test.ps1 -Config Release    # build+ctest in Release
  ./test.ps1 8.8.8.8 1.1.1.1    # build+ctest, then run pingstats on two hosts
!>

[CmdletBinding()]
param(
    [ValidateSet('Debug','Release')]
    [string]$Config = 'Debug',
    [switch]$Help,
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$Hosts
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ($Help) {
    Write-Output @'
Usage: ./test.ps1 [-Config Debug|Release] [--] host1 [host2 ...]

Configures (if necessary) and builds the project in ./build, then runs ctest.
If one or more hosts are provided, pingstats is executed afterwards with those hosts.
'@
    exit 0
}

function Ensure-BuildDir {
    param([string]$Dir)
    if (-not (Test-Path -LiteralPath $Dir)) {
        New-Item -ItemType Directory -Path $Dir | Out-Null
    }
}

function Run-CMakeConfigure {
    param([string]$Dir)
    Write-Host "[cmake] Configuring ($Config)" -ForegroundColor Cyan
    cmake -S . -B $Dir | Write-Output
}

function Run-CMakeBuild {
    param([string]$Dir)
    Write-Host "[cmake] Building ($Config)" -ForegroundColor Cyan
    cmake --build $Dir --config $Config | Write-Output
}

function Run-CTests {
    param([string]$Dir)
    Write-Host "[ctest] Running tests ($Config)" -ForegroundColor Cyan
    ctest --test-dir $Dir --build-config $Config --output-on-failure | Write-Output
}

function Run-PingStats {
    param([string]$Dir, [string[]]$Args)
    if (-not $Args -or $Args.Count -eq 0) { return }

    $exeCandidates = @(
        Join-Path -Path $Dir -ChildPath (Join-Path -Path $Config -ChildPath 'pingstats.exe'),
        Join-Path -Path $Dir -ChildPath 'pingstats.exe',
        Join-Path -Path $Dir -ChildPath (Join-Path -Path $Config -ChildPath 'pingstats'),
        Join-Path -Path $Dir -ChildPath 'pingstats'
    )

    $exe = $exeCandidates | Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1
    if (-not $exe) {
        Write-Error "pingstats executable not found in $Dir (checked config and root)"
        exit 1
    }

    Write-Host "[run] $exe $Args" -ForegroundColor Cyan
    & $exe @Args
    if ($LASTEXITCODE -ne 0) {
        throw "pingstats exited with code $LASTEXITCODE"
    }
}

$buildDir = Join-Path -Path (Get-Location) -ChildPath 'build'

Ensure-BuildDir -Dir $buildDir

Run-CMakeConfigure -Dir $buildDir
Run-CMakeBuild -Dir $buildDir
Run-CTests -Dir $buildDir
Run-PingStats -Dir $buildDir -Args $Hosts
