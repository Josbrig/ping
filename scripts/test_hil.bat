@echo off
setlocal enabledelayedexpansion

REM Configure/build pingstats (Release) and run a short CSV-export run on Windows.
REM Keeps behavior unchanged; comments explain flow and assumptions.

REM Resolve project root (directory containing this script is scripts/)
set SCRIPT_DIR=%~dp0
for %%I in ("%SCRIPT_DIR%..") do set PROJECT_ROOT=%%~fI

echo [info] Project root: %PROJECT_ROOT%
pushd "%PROJECT_ROOT%"

cmake -S . -B build
if errorlevel 1 goto :fail

cmake --build build --config Release
if errorlevel 1 goto :fail

REM Define output CSV and sample hosts (ICMP may need admin/firewall allowances).
set OUTPUT_FILE=runtime_windows.csv
set HOSTS=dan-o-mat megagamer google.de google.com simbrig.eu simbrig.de
echo [info] Hinweis: ICMP kann Admin-Rechte/Firewall-Ausnahme erfordern; bei 100%% Loss prüfen Rechte/Netz.
echo [run] build\Release\pingstats.exe -i 1 --output-format csv --output-file %OUTPUT_FILE% %HOSTS%
start "pingstats_run" /B cmd /c "build\Release\pingstats.exe -i 1 --output-format csv --output-file %OUTPUT_FILE% %HOSTS%"

REM Fixed runtime (~30s), then stop the process to avoid long runs in CI/manual tests.
timeout /t 30 /nobreak >nul
taskkill /f /im pingstats.exe >nul 2>&1

echo [info] Done. Let it run ~30s, then stop as desired.
popd
endlocal
goto :eof

:fail
echo [error] Command failed with code %errorlevel%
popd
endlocal
exit /b 1
