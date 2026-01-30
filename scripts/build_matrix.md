# Build-Matrix Skripte/Konfigurationen (plattformübergreifend)

Ziel: Einfache, generator-unabhängige CMake-Aufrufe pro Plattform mit Debug/Release.

## Linux / WSL / macOS (Unix-Shell)
```
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
ctest --test-dir build-debug --output-on-failure

cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
ctest --test-dir build-release --output-on-failure
```

## Windows (MSVC, Mehrkonfigurations-Generator)
```
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build --build-config Debug --output-on-failure

cmake --build build --config Release
ctest --test-dir build --build-config Release --output-on-failure
```

## Cygwin / MinGW (Unix Makefiles o.ä.)
```
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
ctest --test-dir build-debug --output-on-failure

cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
ctest --test-dir build-release --output-on-failure
```

Hinweise:
- Keine spezielle Generator-Vorgabe (nutzt Standard des Systems); bei Bedarf `-G` angeben.
- Tests werden stets nach dem Build mit `ctest` ausgeführt.
- Separate Build-Verzeichnisse für Debug/Release bei Einkonfigurations-Generatoren; gemeinsames Build bei Mehrkonfigurations-Generatoren (MSVC).
