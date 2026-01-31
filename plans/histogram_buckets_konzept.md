# Konzept: Konfiguration der Histogramm-Buckets (Build- und Startzeit)

## Zielsetzung
- Ermögliche zwei Wege zur Definition der Histogramm-Buckets:
  1. **Build-/Compile-Zeit**: Vorgabedatei im Repository (wird mitgebaut/mitgeliefert), liefert Default-Buckets für alle Targets.
  2. **Startzeit/CLI**: Parameter zur Laufzeit, um Buckets pro Aufruf zu setzen.
- Klare Priorität: CLI > Compile-Time-Konfig > harter Fallback-Default.

## Ist-Stand
- Fester Default `{10,20,50,100,200,500}` in [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:17); Hosts erhalten diesen Satz bei Anlage [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:44).
- Buckets werden bei Snapshot übernommen/gerendert [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:109) und in der Konsole dargestellt [`console_view_impl.cpp`](src/console_view_impl.cpp:212); Export nutzt dieselben Werte [`json_exporter.cpp`](src/json_exporter.cpp:78).
- CLI kennt keine Bucket-Option; `TargetConfig.bucket_boundaries` existiert, wird aber nie gesetzt [`config.hpp`](include/config.hpp:15) [`main.cpp`](src/main.cpp:198).
- PingSession übernimmt TargetConfig unverändert; Buckets müssen vor Session-Erzeugung feststehen [`ping_session.cpp`](src/ping_session.cpp:14).

## Anforderungen & Annahmen
- Eingaben: aufsteigende, nicht-leere Liste nicht-negativer Double-Werte.
- Anzahl Buckets begrenzen (z. B. max 64), Duplikate entfernen/ablehnen.
- Fehlerhaftes Input führt zu klarer CLI-Fehlermeldung und Exit-Code ≠ 0; kein stilles Fallback, außer wenn Option fehlt.
- Thread-Safety: Buckets werden nur bei Host-Anlage gelesen; bestehende Mutex-Logik bleibt ausreichend, solange Buckets vor Session-Start final sind.

## Weg 1: Compile-Time-Konfiguration über Datei
- **Datei**: `config/buckets_default.json` (oder `.yaml`), wird im Repo gepflegt und vom Build (CMake) ins Ausgabeverzeichnis kopiert/installiert.
- **Format (JSON-Beispiel)**:
  ```json
  {
    "bucket_boundaries": [0.5, 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000]
  }
  ```
- **Ladepunkt**: Startup in `main` vor Target-Erzeugung:
  - Pfad ableiten relativ zum Executable (z. B. `./config/buckets_default.json`) oder über Option `--config-file` überschreibbar.
  - Bei erfolgreichem Laden und Validierung werden die Grenzen als globaler Default gesetzt.
  - Bei fehlender Datei/Parsing-Fehler → warnen und auf hart codierten Fallback zurückfallen (kein Crash).
- **Integration**:
  - Utility-Funktion `load_bucket_defaults_from_file(path) -> std::vector<double>` mit Validierung.
  - In `parse_arguments` optionalen Parameter `--config-file <path>` hinzufügen (nur für Bucket-Defaults nötig, erweiterbar).
  - Beim Bau der `TargetConfig` (vor `make_ping_session`) den geladenen Satz übernehmen, sofern kein CLI-Override kommt.

## Weg 2: Startzeit/CLI-Override
- **Option**: `--bucket-boundaries <list|preset>`
  - Liste: Komma-separiert, z. B. `--bucket-boundaries 0.5,1,2,5,10,20,50,100,200,500,1000`
  - Presets (optional, falls gewünscht): `fine`, `log`, `coarse`; werden auf fest definierte Tabellen gemappt.
- **Parsing**:
  - Split nach Komma, `std::stod`, negative Werte ablehnen, aufsteigende Sortierung prüfen (oder sortieren und Duplikate entfernen mit Warnung).
  - Max-Länge prüfen (z. B. 64 Elemente); leere Liste ablehnen.
- **Anwendung**:
  - CLI-Wert pro Prozesslauf global anwenden (gleicher Satz für alle Targets); wenn künftig pro Host erforderlich, CLI könnte mehrfach angegeben werden, aktuell nicht nötig.
  - Precedence: Wenn Option gesetzt, überschreibt sie Compile-Time-Datei und den hart codierten Fallback.
- **Fehlerfälle**:
  - Ungültige Zahl/Sortierung → Fehlertext und Exit.
  - Zu viele Buckets → Fehlertext und Exit.

## Validierung & Fehlerbehandlung
- Gemeinsame Validator-Funktion für alle Quellen (CLI, Datei):
  - Nicht-leer, strikt aufsteigend, keine `NaN/Inf`, keine negativen Werte, Max-Länge.
- Logging/Warnungen: Datei fehlt → Hinweis + Fallback; CLI-Fehler → abort mit Message.

## Thread-Safety
- Buckets werden nur beim Host-Setup verwendet; solange Konfiguration vor Start der Sessions feststeht, genügt die bestehende Mutex-Synchronisation in [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:122).
- Keine Laufzeitänderung vorgesehen; daher kein Re-Binning nötig.

## Offene Punkte
- Pfadstrategie für `config/buckets_default.json` unter Windows/macOS/Linux (Install-/Run-Layout klären).
- Ob Presets benötigt werden oder nur freie Listen.
- Ob pro Host unterschiedliche Buckets später erforderlich sind.

## Umsetzungsschritte (architekturorientiert)
- CLI erweitern (`parse_arguments`): neue Option `--bucket-boundaries`; optional `--config-file`.
- Loader/Validator implementieren (Datei + CLI) und Fallback-Kaskade festlegen (CLI > Datei > Default).
- `TargetConfig` beim Bau mit finalen Boundaries füllen; `StatisticsAggregator` damit initialisieren.
- Tests: Parsing/Validierung, Aggregator mit Custom-Buckets, Fehlerszenarien.
