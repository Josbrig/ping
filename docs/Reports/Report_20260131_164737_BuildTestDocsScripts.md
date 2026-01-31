# Report: User scripts for build/test/docs across platforms
- Timestamp: 2026-01-31 16:47:37 UTC
- Purpose: Provide user-facing scripts to build, test, and generate documentation for Debug, Release, and RelWithDebInfo across supported platforms/compilers.

## Scripts added
- [`scripts/user/build_all_windows.ps1`](scripts/user/build_all_windows.ps1:1): Windows PowerShell (MSVC multi-config); builds Debug/Release/RelWithDebInfo, runs ctest, tries Doxygen.
- [`scripts/user/build_all_msvc.bat`](scripts/user/build_all_msvc.bat:1): Windows batch variant (MSVC multi-config); same flow as above.
- [`scripts/user/build_all_unix.sh`](scripts/user/build_all_unix.sh:1): Unix shell (Linux/WSL/macOS single-config); builds Debug/Release/RelWithDebInfo, runs ctest, tries Doxygen.
- [`scripts/user/build_all_cygwin_mingw.sh`](scripts/user/build_all_cygwin_mingw.sh:1): Unix-style for Cygwin/MinGW (single-config); same flow.

## Documentation update
- [`scripts/build_matrix.md`](scripts/build_matrix.md:1): Expanded matrix to include RelWithDebInfo and optional Doxygen target.

## Behavior
- All scripts: configure, build, test; Doxygen is attempted once (Release) and is non-fatal if unavailable.
- Generators: optional flag where applicable (PowerShell, batch, bash variants) to choose Ninja/Unix Makefiles/VS.

## TODO impact
- No TODO items changed.

## References
- Prompt archive: [`docs/prompts/Prompt_20260131_164607_BuildTestDocsScripts.md`](docs/prompts/Prompt_20260131_164607_BuildTestDocsScripts.md:1)
