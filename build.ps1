<#
Minimal PowerShell helper to configure and build the project with CMake
using a multi-config build tree in ./build. Keeps behavior identical while
documenting parameters and steps for quick reference.
#>

param(
    [Parameter(HelpMessage = 'Configuration name (Debug or Release).')]
    [ValidateSet('Debug','Release')]
    [string]$Config = 'Debug',

    [Alias('h','?')]
    [switch]$Help
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ($PSVersionTable.PSVersion.Major -ge 7) {
    # Forward PowerShell error preference to native commands in PS7+
    $PSNativeCommandUseErrorActionPreference = $true
}

function Show-Usage {
    # Print short usage/help text and exit.
    Write-Output "Usage: .\\build.ps1 [-Config Debug|Release] [-Help]"
    Write-Output ""
    Write-Output "Builds the project with CMake using the multi-config 'build' directory."
    Write-Output "Options:"
    Write-Output "  -Config <Debug|Release>  Optional. Defaults to Debug."
    Write-Output "  -Help (-h, -?)           Show this help message."
}

if ($Help) {
    Show-Usage
    exit 0
}

try {
    # Resolve repo root from script location and ensure build directory exists.
    $repoRoot = $PSScriptRoot
    $buildDir = Join-Path -Path $repoRoot -ChildPath 'build'

    if (-not (Test-Path -Path $buildDir)) {
        New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
    }

    # Configure (generator inferred) into multi-config build dir.
    $configureArgs = @('-S', $repoRoot, '-B', $buildDir)
    Write-Host "Configuring: cmake $($configureArgs -join ' ')" -ForegroundColor Cyan
    & cmake @configureArgs
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configure failed with exit code $LASTEXITCODE."
    }

    # Build chosen configuration in the same tree.
    $buildArgs = @('--build', $buildDir, '--config', $Config)
    Write-Host "Building: cmake $($buildArgs -join ' ')" -ForegroundColor Cyan
    & cmake @buildArgs
    $buildExitCode = $LASTEXITCODE
    if ($buildExitCode -ne 0) {
        throw "CMake build failed with exit code $buildExitCode."
    }

    exit $buildExitCode
}
catch {
    # Surface any failure with non-zero exit.
    Write-Error $_
    exit 1
}
