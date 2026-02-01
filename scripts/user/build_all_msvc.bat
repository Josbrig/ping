@echo off
setlocal enabledelayedexpansion

rem User script: configure, build, test, and generate docs on Windows (MSVC multi-config).
rem Builds Debug, Release, and RelWithDebInfo; runs ctest per config; tries doxygen target.

set BUILD_DIR=build
set GENERATOR=

rem Detect logical core count for parallel builds (fallback to 1)
for /f "tokens=2 delims==" %%a in ('wmic cpu get NumberOfLogicalProcessors /value 2^>nul ^| find "NumberOfLogicalProcessors"') do set CORES=%%a
if not defined CORES set CORES=1
set PARALLEL_BUILD=--parallel %CORES%
set PARALLEL_CTEST=--parallel %CORES%

:parse
if "%~1"=="" goto done_parse
if /I "%~1"=="-g" (
  set GENERATOR=%~2
  shift & shift & goto parse
)
if /I "%~1"=="--generator" (
  set GENERATOR=%~2
  shift & shift & goto parse
)
if /I "%~1"=="-h" goto help
if /I "%~1"=="--help" goto help
echo Unknown option: %~1
goto help

:help
echo Usage: scripts\user\build_all_msvc.bat [-g "Visual Studio 17 2022"]
echo Builds Debug/Release/RelWithDebInfo, runs ctest, tries doxygen.
exit /b 1

:done_parse

set CONFIGS=Debug Release RelWithDebInfo

set REPO_ROOT=%~dp0..\..
pushd "%REPO_ROOT%" >nul

set CONFIGURE_ARGS=-S . -B %BUILD_DIR%
if defined GENERATOR set CONFIGURE_ARGS=%CONFIGURE_ARGS% -G "%GENERATOR%"

echo [cmake] %CONFIGURE_ARGS%
cmake %CONFIGURE_ARGS%
if errorlevel 1 goto :fail

for %%C in (%CONFIGS%) do (
  echo [cmake] --build %BUILD_DIR% --config %%C %PARALLEL_BUILD%
  cmake --build %BUILD_DIR% --config %%C %PARALLEL_BUILD%
  if errorlevel 1 goto :fail

  echo [ctest] --test-dir %BUILD_DIR% --build-config %%C --output-on-failure %PARALLEL_CTEST%
  ctest --test-dir %BUILD_DIR% --build-config %%C --output-on-failure %PARALLEL_CTEST%
  if errorlevel 1 goto :fail
)

rem Attempt docs once (Release); ignore failures
echo [docs] cmake --build %BUILD_DIR% --config Release --target doxygen %PARALLEL_BUILD%
cmake --build %BUILD_DIR% --config Release --target doxygen %PARALLEL_BUILD%
if errorlevel 1 echo [docs] doxygen target unavailable or failed; skipping

popd >nul
endlocal
exit /b 0

:fail
echo [error] Command failed with exit code %errorlevel%
popd >nul
endlocal
exit /b %errorlevel%
