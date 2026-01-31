<#!
PowerShell helper to display agent rules before working on the project.
Keeps output read-only; no side effects besides printing rules.
Usage: ./scripts/read_rules.ps1 (can be run from any directory).
!>

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir
$RulesPath = Join-Path $ProjectRoot "docs/AGENTEN_RULES.md"

if (-not (Test-Path -LiteralPath $RulesPath)) {
    Write-Error "AGENTEN_RULES.md not found at $RulesPath"
    exit 1
}

Write-Host "=== Agent Rules (from $RulesPath) ===" -ForegroundColor Cyan
Get-Content -LiteralPath $RulesPath | Write-Host

Write-Host "" 
Write-Host "[info] Please read and acknowledge these rules before proceeding." -ForegroundColor Yellow
