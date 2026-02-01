<#
User script: configure, build, test, and generate docs on Windows (MSVC, multi-config).
- Builds Debug, Release, and RelWithDebInfo in a shared multi-config build tree.
- Runs ctest per config.
- Attempts to build Doxygen docs if the `doxygen` target exists (skips otherwise).

Usage examples:
  ./scripts/user/build_all_windows.ps1
  ./scripts/user/build_all_windows.ps1 -BuildDir build -Generator "Visual Studio 17 2022"
#>

[CmdletBinding()]
param(
    [string]$BuildDir = "build",
    [string]$Generator
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ($PSVersionTable.PSVersion.Major -ge 7) {
    $PSNativeCommandUseErrorActionPreference = $true
}

$Configs = @('Debug', 'Release', 'RelWithDebInfo')

$CoreCount = [Environment]::ProcessorCount
if (-not $CoreCount -or $CoreCount -lt 1) { $CoreCount = 1 }
$ParallelBuild = @('--parallel', "$CoreCount")
$ParallelCTest = @('--parallel', "$CoreCount")

function Invoke-CMake {
    param([string[]]$Args)
    Write-Host "[cmake] $($Args -join ' ')" -ForegroundColor Cyan
    & cmake @Args
    if ($LASTEXITCODE -ne 0) { throw "cmake failed with exit code $LASTEXITCODE" }
}

function Invoke-CTest {
    param([string]$BuildDir, [string]$Config)
    $args = @('--test-dir', $BuildDir, '--build-config', $Config, '--output-on-failure')
    if ($ParallelCTest) { $args += $ParallelCTest }
    Write-Host "[ctest] $($args -join ' ')" -ForegroundColor Cyan
    & ctest @args
    if ($LASTEXITCODE -ne 0) { throw "ctest failed with exit code $LASTEXITCODE" }
}

function Try-BuildDocs {
    param([string]$BuildDir, [string]$Config)
    $args = @('--build', $BuildDir, '--config', $Config, '--target', 'doxygen')
    if ($ParallelBuild) { $args += $ParallelBuild }
    Write-Host "[docs] cmake $($args -join ' ')" -ForegroundColor Cyan
    & cmake @args
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "Doxygen target not available or failed; skipping docs for $Config (exit $LASTEXITCODE)."
    }
}

# Resolve repo root relative to this script.
$RepoRoot = Split-Path -Parent $PSScriptRoot
$BuildPath = Join-Path -Path $RepoRoot -ChildPath $BuildDir
if (-not (Test-Path -LiteralPath $BuildPath)) {
    New-Item -ItemType Directory -Path $BuildPath -Force | Out-Null
}

$configureArgs = @('-S', $RepoRoot, '-B', $BuildPath)
if ($Generator) { $configureArgs += @('-G', $Generator) }

Invoke-CMake -Args $configureArgs

foreach ($cfg in $Configs) {
    Invoke-CMake -Args (@('--build', $BuildPath, '--config', $cfg) + $ParallelBuild)
    Invoke-CTest -BuildDir $BuildPath -Config $cfg
}

# Attempt docs once (Release config preferred for consistency)
Try-BuildDocs -BuildDir $BuildPath -Config 'Release'
