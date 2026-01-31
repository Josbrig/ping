# Prompt Archive: Build/Test/Docs user scripts per platform/compiler
- Timestamp: 2026-01-31 16:46:07 UTC
- Prompt: "erstelle mir ein Benutzerscript, das das ganze project baut und testet und dokumentationen erzeugt. das programm soll als release und als debug und als release with debug infos gebaut und getestet werden. es soll für jedes unterstützte system/compiler ein script erstellt werden."
- Questions: None
- Answers: N/A (direct implementation)
- Plan:
  1) Read existing build matrix guidance to align targets.
  2) Create per-platform user scripts (Windows MSVC, Unix/Linux/macOS/WSL, Cygwin, MinGW) that build Debug/Release/RelWithDebInfo, run ctest, and generate Doxygen docs.
  3) Document changes in a report.
- Result: Pending (see report upon completion)
