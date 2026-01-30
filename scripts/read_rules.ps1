<#!
PowerShell helper to display agent rules before working on the project.
Usage: ./scripts/read_rules.ps1
Can be invoked from any directory; resolves project root relative to script location.
!>

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir
$RulesPath = Join-Path $ProjectRoot "docs/agent_rules.md"

if (-not (Test-Path -LiteralPath $RulesPath)) {
    Write-Error "agent_rules.md not found at $RulesPath"
    exit 1
}

Write-Host "=== Agent Rules (from $RulesPath) ===" -ForegroundColor Cyan
Get-Content -LiteralPath $RulesPath | Write-Host

Write-Host "" 
Write-Host "[info] Please read and acknowledge these rules before proceeding." -ForegroundColor Yellow
