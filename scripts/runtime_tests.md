# Runtime tests (real ICMP)

Short manual/CI smoke instructions for a ~30s run with CSV export.

Ziele/Targets: 8.8.8.8, 1.1.1.1, example.org
Intervall: 1s, Laufzeit: ca. 30s
Ausgabe: CSV-Export

## Windows (MSVC multi-config)
```
cmake -S . -B build
cmake --build build --config Release
build\Release\pingstats.exe -i 1 --output-format=csv --output-file=runtime_windows.csv 8.8.8.8 1.1.1.1 example.org
# Laufzeit: manuell ca. 30s laufen lassen, dann Enter zum Beenden
```

## Linux / WSL / macOS / Cygwin / MinGW (single-config)
```
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
./build-release/pingstats -i 1 --output-format=csv --output-file=runtime_unix.csv 8.8.8.8 1.1.1.1 example.org
# Laufzeit: manuell ca. 30s laufen lassen, dann Enter zum Beenden
```

Hinweise:
- ICMP erfordert ggf. erhöhte Rechte (root/CAP_NET_RAW unter Linux/WSL, Admin unter Windows).
- CSV wird zyklisch aktualisiert und final beim Exit geschrieben.
- Bei Firewalls/Netzwerkausfall schlagen Pings fehl; Erfolg/Misserfolg in CSV ersichtlich.
