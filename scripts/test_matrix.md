# Anleitung: CMake-Tests plattform- und compilerübergreifend ausführen

Ziel: Einheitliche Schritte für Debug/Release, verschiedene Plattformen/Generatoren.

## Windows (MSVC, Mehrkonfig)
```
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build --build-config Debug --output-on-failure

cmake --build build --config Release
ctest --test-dir build --build-config Release --output-on-failure
```

## Linux / WSL / macOS / Cygwin / MinGW (Einkonfig)
```
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
ctest --test-dir build-debug --output-on-failure

cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
ctest --test-dir build-release --output-on-failure
```

## Hinweise
- Standard-Generator wird verwendet; bei Bedarf `-G` angeben (z.B. Ninja, Unix Makefiles, Visual Studio).
- Tests aus `ctest` nutzen Catch2-Discovery sowie den Netzwerktest `ping_simbrig` (erfordert Internet/ICMP).
- Label-basiert filtern: z.B. `ctest -L network` oder `ctest -LE network`.
- Timeout für `ping_simbrig` ist 10s gesetzt.
