# KI-Agenten Regeln für dieses Projekt

> Hinweis: Diese Regeln sind vor jeder Arbeit am Projekt zu lesen und strikt einzuhalten.

## Anweisungen an den ki-Agenten für JEDEN Prompt (und ergänzende Prompts)
- Jede Anweisung/Aufgabe, soll in einer Reportdatei für Prompts im Verzeichnis "docs/prompts" Archiviert werden. Soll der Prompt mit nachfragen, deren Antworten und den Ausgaben des KI-Agenten Archiviert werden. Ausgaben die in eine Datei gingen nur referenziert werden. Der Report-Dateiname mit laufender Nummer, Datum+Uhrzeit und kurzem, aussagekräftigem Namen.

## Build & Tests
- Nutze CMake mit getrennter Konfiguration und Build: `cmake -S . -B build`, dann `cmake --build build --config <Debug|Release>`.
- Tests laufen über `ctest --test-dir build --build-config <Config> --output-on-failure`.
- Ziel `run_tests` vorhanden: ruft `ctest` mit `--output-on-failure` auf.
- Catch2 wird via FetchContent aus `http://raspirepo/mirrors/Catch2.git` (GIT_TAG v3.5.3) bezogen.
- Test-Targets dürfen nicht gegen das Executable `pingstats` linken; benötigte Quellen werden direkt in die Test-Executables eingebunden und gegen `Catch2::Catch2WithMain` gelinkt.

## CLI/Runtime
- CLI-Flags: `-i/--interval <sec>`, `--output-format {none,csv,json}`, `--output-file <path>`, `-h/--help`, `--version`. Alle anderen args sind Hosts.
- Default-Intervall: 1.0s, Default-Render-Intervall: 500ms.
- Output-Format wird auf CSV gesetzt, wenn `--output-file` ohne Format angegeben ist; Default-Dateinamen: `pingstats.csv` (CSV), `pingstats.json` (JSON).
- CSV-Export: zyklisch alle 5s und final am Ende, nur bei OutputFormat::Csv.
- JSON-Export: nur final am Ende bei OutputFormat::Json.
- ConsoleView rendert Header, Tabelle (HOST/N/LOSS%/MIN/MAX/MEAN/MED/LAST), Sparkline (Breite 40, Charset " .:-=+*#%@"), Histogramm (Breite 30 mit '#'), ANSI Clear `\x1b[2J\x1b[H`.

## Komponenten
- PingSession: eigener Thread, periodisches Pingen, Timeout ca. 0.8*Intervall (clamp 100..5000ms), Ausnahmen als Misserfolg loggen, add_sample pro Versuch.
- StatisticsAggregator: Min/Max/Mean/Median, Packet-Loss, Histogramm-Buckets, recent RTT Ringbuffer; Thread-safe mit Mutex; Defaults: Histogrammboundaries {10,20,50,100,200,500}, Median-Puffer 1024, Time-Series 256.
- ConsoleView: nutzt snapshot_all(), sortiert Hosts, periodischer Thread stoppbar via stop().

## Skripte
- `build.ps1`: CMake-Configure/Build (Debug Default), ctest, optional pingstats-Run mit Hosts.
- `test.sh`: analog für Bash, optional pingstats mit Hosts.
- `test.py`: Python-Variante, optional pingstats mit Hosts.

## Sonstiges
- Reports in Dateien unter `docs/` ablegen (z.B. `docs/report.md`).
- AgentTodo-Einträge erweitern, nicht löschen; Build-Test-Punkt nach OS/Compiler gruppiert.
- AgentTodo-Einträge nach erfolgreicher Fertigstellung als erledigt markieren.

## Reports
- Reports sind immer im MarkDown-Format und werden immer in Ordner docs/Reports abgelegt. Jeder Reportdateinamen beginnt mit "Report_", gefolgt vom Datum und dann die Uhrzeit gefolgt von einem Sinnstiftenden Namen.

## TODO-Dateien
- Punkte in der Todo-Liste werden mit laufenden nummern aufgezählt
- Unterpunkte werden mit grossen Buchstaben nummeriert.
- UnterUnterpunkte werden mit römischen Zahlen nummeriert.
- Jeder punkt und unterpunkt wird mit "[ ]" gekennzeichnet ob ein Punkt erledigt ist oder nicht.
- Der KI-Agent hat selbständig nach erfolgreicher umsetzung eines Punkte ihn als erledigt zu markieren.