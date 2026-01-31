# Projektübersicht

Dieses Projekt stellt ein plattformübergreifendes Ping-Programm bereit, das Langzeitstatistiken über die Erreichbarkeit und Antwortzeiten einer oder mehrerer Netzwerkziele sammelt. Ziel ist eine zuverlässige, kontinuierliche Überwachung mit aussagekräftigen Kennzahlen und Visualisierungen der Latenz über längere Zeiträume.

Das Programm wird in C++20 entwickelt und mit CMake gebaut. Es ist darauf ausgelegt, unter Linux, macOS, Windows WSL, Windows Cygwin und Windows MinGW kompiliert und ausgeführt werden zu können. Dabei werden die jeweiligen betriebssystemspezifischen Anforderungen an ICMP-Pakete berücksichtigt.

# Hauptfunktionen

- Kontinuierliches Senden von ICMP-Pings an ein oder mehrere Ziele in konfigurierbaren Intervallen.
- Erfassung und Auswertung der Antwortzeiten jeder einzelnen Ping-Antwort.
- Individuelle Statistiken pro IP-Adresse oder Hostname (pro Ziel getrennte Auswertung).
- Darstellung eines zeitlichen Verlaufs der Antwortzeiten (z. B. als einfache Textgrafik / Latenzkurve in der Konsole).
- Erstellung eines Histogramms der Ping-Antwortzeiten, um die Verteilung der Latenzen zu visualisieren.
- Berechnung von Minimum, Maximum, Mittelwert und Median pro Ziel auf Basis der bisher beobachteten Antworten.
- Laufende Aktualisierung der Messung und Statistik in quasi Echtzeit: Die Konsole wird in regelmäßigen Abständen aktualisiert, solange das Programm läuft.
- Gleichzeitige Überwachung mehrerer Ziele: Jedes Ziel wird in einer eigenen Sitzung gemessen und in der Ausgabe klar getrennt dargestellt.

# Systemvoraussetzungen

## Allgemeines

- C++20-fähiger Compiler (z. B. GCC, Clang, MSVC bzw. MinGW mit C++20-Unterstützung).
- CMake in einer aktuellen Version, empfohlen ab 3.20.
- Netzwerkzugriff auf die zu überwachenden Ziele.
- Berechtigungen zum Senden von ICMP-Echo-Requests (Ping). Je nach Plattform und Konfiguration können erhöhte Rechte erforderlich sein (z. B. root unter Linux, Administratorrechte unter Windows oder entsprechende Capabilities wie `CAP_NET_RAW`).

## Linux

- Linux-Distribution mit installiertem C++20-Compiler (z. B. `g++` oder `clang++`).
- CMake ≥ 3.20.
- Ausreichende Rechte für ICMP (z. B. Ausführung mit `sudo` oder Setzen von `CAP_NET_RAW` für das Binärprogramm).

## macOS

- Aktuelle Xcode-Toolchain bzw. Command Line Tools mit C++20-Unterstützung.
- CMake ≥ 3.20 (installiert z. B. über Homebrew oder MacPorts).
- Ausführung in einer Terminalumgebung mit ausreichenden Rechten für ICMP-Pakete.

## Windows WSL

- Windows mit installiertem Windows Subsystem for Linux (WSL oder WSL2).
- Linux-Umgebung in WSL mit C++20-Compiler und CMake ≥ 3.20.
- ICMP-Berechtigungen innerhalb der WSL-Distribution (ggf. `sudo`).

## Windows Cygwin

- Cygwin-Installation mit C++20-fähigem Compiler (z. B. `g++` im Cygwin-Paket).
- CMake ≥ 3.20 als Cygwin-Paket.
- Ausführung im Cygwin-Terminal mit den für ICMP erforderlichen Rechten.

## Windows MinGW

- MinGW- oder MSYS2-Umgebung mit C++20-fähigem Compiler (z. B. `g++` aus MinGW oder `clang++`).
- CMake ≥ 3.20 (z. B. als MSYS2-Paket).
- Ausführung in der entsprechenden Shell (MSYS2/MinGW) mit Zugriff auf das Netzwerk und gegebenenfalls Administratorrechten, falls ICMP dies erfordert.

# Installation

Die folgenden Schritte beschreiben einen typischen Build mit CMake. Der genaue Ablauf kann je nach Umgebung (Generator, Pfade, Compiler) variieren, das Grundprinzip bleibt jedoch gleich.

## Unix-artige Systeme (Linux, macOS, WSL)

Im Projektverzeichnis:

```bash
mkdir -p build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Für einen Debug-Build kann der Build-Typ entsprechend angepasst werden:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Je nach verwendetem Generator (z. B. Ninja, Makefiles) werden die passenden Build-Werkzeuge automatisch aufgerufen.

## Windows (Cygwin / MinGW)

Im Projektverzeichnis, aus einer Cygwin- bzw. MSYS2/MinGW-Shell heraus:

```bash
mkdir -p build
cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Für Debug-Builds entsprechend:

```bash
cmake -S . -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Sofern ein anderer Generator (z. B. Ninja) bevorzugt wird, kann dieser über `-G` angegeben werden.

# Verwendung

Im Folgenden wird beispielhaft davon ausgegangen, dass das erzeugte Programm [`pingstats`](build/pingstats) heißt und sich nach dem Build im `build`-Verzeichnis befindet.

Die allgemeine Aufrufstruktur lautet:

```bash
./build/pingstats [Optionen] <ziel1> [ziel2 ...]
```

Typische Ziele sind IP-Adressen oder Hostnamen, etwa `8.8.8.8` oder `example.org`.

## Einfaches Beispiel: Einzelnes Ziel pingen

Kontinuierliches Pingen eines einzelnen Ziels mit Standardintervall (z. B. 1 Sekunde):

```bash
./build/pingstats 8.8.8.8
```

Die Messung läuft, bis der Benutzer sie explizit abbricht (z. B. mit `Strg+C`). Statistik und Anzeige werden fortlaufend aktualisiert.

## Mehrere Ziele parallel überwachen

Gleichzeitige Überwachung mehrerer IP-Adressen oder Hosts:

```bash
./build/pingstats 8.8.8.8 1.1.1.1 example.org
```

Für jedes Ziel wird intern eine eigene Sitzung geführt, und die Ausgabe zeigt getrennte Statistiken pro Adresse/Host.

## Intervall in Sekunden angeben

Mit einer Option zur Intervallsteuerung (beispielhaft `-i` in Sekunden):

```bash
./build/pingstats -i 2 8.8.8.8 1.1.1.1
```

In diesem Beispiel werden alle angegebenen Ziele alle 2 Sekunden angepingt. Das Intervall wirkt sich direkt auf die zeitliche Auflösung der Statistiken aus.

## Ausgabe in Datei (z. B. CSV oder JSON)

Zur späteren Auswertung können Statistiken regelmäßig oder am Programmende in eine Datei geschrieben werden, z. B. in CSV- oder JSON-Format. Eine mögliche Syntax (beispielhaft):

```bash
./build/pingstats -i 1 --output-format=csv --output-file=pingstats.csv 8.8.8.8 1.1.1.1
```

oder für JSON:

```bash
./build/pingstats -i 1 --output-format=json --output-file=pingstats.json 8.8.8.8 1.1.1.1
```

Je nach Implementierung können die Dateien entweder fortlaufend aktualisiert oder am Ende der Messung geschrieben werden.

## Beispielhafte Konsolenausgabe

Während der Laufzeit zeigt das Programm fortlaufend aktualisierte Statistiken, typischerweise in tabellarischer Form. Ein vereinfachtes Beispiel könnte wie folgt aussehen:

```text
Zeitstempel: 2026-01-30 12:34:56

Ziel           Pakete   Min [ms]   Max [ms]   Mittel [ms]   Median [ms]   Verlust
-------------  -------  ---------  ---------  ------------  ------------  -------
8.8.8.8           120       12.3       25.8          14.7          13.9     0.0 %
1.1.1.1           120       10.8       21.2          13.5          12.7     0.0 %
example.org       120       18.5       60.2          22.1          20.4     0.8 %
```

Zusätzlich kann in regelmäßigen Abständen ein einfacher zeitlicher Verlauf der letzten Messwerte je Ziel dargestellt werden, etwa als Textgrafik:

```text
Zeitverlauf letzte 60 Sekunden (Latenz in ms, niedriger ist besser)

8.8.8.8:     ▁▁▂▂▃▃▄▅▆▆▆▇█▇▆▅▄▃▂▂▁▁
1.1.1.1:     ▁▁▁▂▂▂▃▄▅▅▆▆▇▇▆▅▄▃▂▂▁▁
example.org: ▂▃▃▄▅▆▇██▇▆▅▄▃▃▂▂▁▁▁▁▁
```

Das Histogramm der Antwortzeiten kann z. B. als Bucket-Liste ausgegeben werden:

```text
Histogramm 8.8.8.8 (Antwortzeit in ms)

  0–10   ms:  5 ▓▓
 10–20   ms: 80 ██████████████████
 20–30   ms: 25 █████
 30–40   ms: 10 ██
 > 40   ms:  0 
```

Die Messung läuft im Normalfall unbegrenzt und wird vom Benutzer beendet. Mit jedem Aktualisierungszyklus werden neue Ping-Ergebnisse in die Statistiken einbezogen, und die dargestellten Kennzahlen (Min/Max/Mittelwert/Median, Zeitverlauf, Histogramm) passen sich dynamisch an.

# Projektarchitektur

Die interne Struktur ist so angelegt, dass Messlogik, Statistikberechnung, Konfiguration, Ausgabe und Betriebssystemabstraktion klar getrennt sind. Dadurch bleibt der Code wartbar, erweiterbar und gut testbar.

Zentrale Bausteine können unter anderem folgende Komponenten sein:

- [`PingSession`](src/ping_session.hpp:1) bzw. [`PingWorker`](src/ping_worker.hpp:1): Verantwortlich für das Ausführen von ICMP-Pings an ein einzelnes Ziel. Steuert das Sendeintervall, sammelt Rohmessdaten (Antwortzeiten, Paketverluste) und meldet diese an die Statistikkomponente.
- [`StatisticsAggregator`](src/statistics_aggregator.hpp:1): Nimmt die Rohdaten der einzelnen Sitzungen entgegen, berechnet daraus Minimum, Maximum, Mittelwert und Median, führt Zähler für Paketverlust und erzeugt Datenstrukturen für Histogramme und zeitliche Verläufe (Ringpuffer für die letzten N Messwerte).
- [`TargetConfig`](src/config.hpp:1): Repräsentiert ein einzelnes Ziel, inklusive IP/Hostname, Ping-Intervall und optionaler Ziel-spezifischer Einstellungen. Eine übergeordnete Konfigurationsinstanz verwaltet die Gesamtheit der Ziele.
- [`ConsoleView`](src/console_view.hpp:1): Erzeugt in regelmäßigen Abständen die Konsolenausgabe. Liest den aktuellen Stand aus [`StatisticsAggregator`](src/statistics_aggregator.hpp:1) aus und rendert Tabellen, Zeitverläufe und Histogramme, idealerweise ohne das Terminal mit übermäßigem Scrollen zu überfluten (z. B. durch Aktualisierung im bestehenden Bildschirmbereich).
- [`PlatformPingBackend`](src/platform_ping_backend.hpp:1): Abstraktionsschicht für die betriebssystemspezifische ICMP-Implementierung. Kapselt Unterschiede in den verfügbaren Systemaufrufen, Sockets und Rechten unter Linux, macOS, WSL, Cygwin und MinGW.

Der Programmstart (z. B. in einer Funktion [`main`](src/main.cpp:1)) liest Konfiguration und Kommandozeilenargumente ein, erzeugt für jedes Ziel eine [`TargetConfig`](src/config.hpp:1) und startet die zugehörigen [`PingSession`](src/ping_session.hpp:1)-Instanzen, typischerweise in eigenen Threads oder asynchronen Tasks. Jede Sitzung nutzt [`PlatformPingBackend`](src/platform_ping_backend.hpp:1), um ICMP-Pakete zu senden und Antwortzeiten zu messen, und übergibt die Ergebnisse an [`StatisticsAggregator`](src/statistics_aggregator.hpp:1).

Parallel dazu ruft [`ConsoleView`](src/console_view.hpp:1) in festen Intervallen den aktuellen Stand aus [`StatisticsAggregator`](src/statistics_aggregator.hpp:1) ab und aktualisiert die Darstellung auf der Konsole. Auf diese Weise entsteht eine kontinuierlich aktualisierte Sicht auf die Laufzeitstatistik sämtlicher überwachten Ziele.

# Beitragen / Erweiterung

Beiträge von außen sind willkommen, sofern sie die Zielsetzung des Projekts unterstützen: eine robuste, plattformübergreifende und gut nachvollziehbare Langzeit-Ping-Statistik.

Ein üblicher Workflow für Beiträge kann wie folgt aussehen:

1. Repository forken.
2. Eigenen Branch für die Änderung anlegen (z. B. `feature/…` oder `bugfix/…`).
3. Änderungen vornehmen, inklusive sinnvoller Tests bzw. manueller Prüfungen (z. B. auf mehreren Plattformen, sofern möglich).
4. Code-Stil und Struktur an den bestehenden Code anpassen (klare Trennung von Logik, Statistik, Ausgabe und Plattformabstraktion).
5. Merge Request bzw. Pull Request mit aussagekräftiger Beschreibung erstellen.

Mögliche sinnvolle Erweiterungen umfassen unter anderem:

- Export von Metriken in ein Prometheus-kompatibles Format (z. B. über eine lokale HTTP-Schnittstelle oder eine Textdatei), um die Daten in externen Monitoring-Systemen weiterzuverarbeiten.
- Web-basierte Benutzeroberfläche, die die Statistiken in Echtzeit in einem Browser visualisiert (z. B. mit Diagrammen für Latenzverlauf, Histogramme, Verfügbarkeit pro Ziel).
- Zusätzliche Visualisierungen in der Konsole, etwa Prozentil-Darstellungen (P95, P99), Jitter-Berechnungen oder Heatmaps für Latenzen über die Zeit.
- Unterstützung weiterer Protokolle und Checks neben ICMP, z. B. TCP-Port-Reachability, HTTP-GET-Latenzmessungen oder TLS-Handshake-Zeiten.
- Konfigurationsdateien (z. B. YAML oder JSON), um eine größere Anzahl von Zielen und Einstellungen komfortabel zu verwalten.

# Lizenz

MIT Licens