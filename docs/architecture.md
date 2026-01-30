# Architekturübersicht für pingstats

## Kurzkontext

pingstats ist ein in C++20 entwickeltes, mit CMake gebautes Konsolenwerkzeug zur Analyse von Netzwerk-Latenzen. Die Architektur ist plattformübergreifend ausgelegt und unterstützt Linux, macOS, WSL, Cygwin und MinGW. Systemnahe Details wie ICMP-Handhabung und Socket-APIs werden über klar abgegrenzte Backend-Schnittstellen gekapselt.

## Hauptmodule und Verantwortlichkeiten

### PlatformPingBackend

- Implementiert die OS-spezifische Erzeugung und Auswertung von ICMP-Echo-Requests und -Replies.
- Kapselt Unterschiede der zugrunde liegenden Netzwerk-APIs (raw sockets, Berechtigungen, Timeouts) pro Plattform.
- Stellt eine einheitliche Schnittstelle bereit, über die höhere Schichten Ping-Anfragen für ein bestimmtes Ziel und einen Timestamp auslösen und die gemessene Round-Trip-Time (RTT) zurückerhalten.
- Ermöglicht den Austausch oder die Ergänzung weiterer Backends, ohne dass die übrige Anwendung angepasst werden muss.

### PingSession und PingWorker

- Repräsentieren die laufende Messung für ein Ziel (Host oder IP) mit festgelegtem Intervall.
- PingSession verwaltet den Lebenszyklus für ein Ziel (Start, Stop, Fehlerbehandlung) und hält Referenzen auf PlatformPingBackend und StatisticsAggregator.
- PingWorker führt die eigentlichen Ping-Operationen in einem wiederholten Takt aus (z. B. eigener Thread pro Ziel oder aus einem Threadpool heraus).
- Jede Messung erzeugt ein Rohdatenergebnis mit Zeitstempel, RTT, Erfolgsstatus und Sequenznummer, das an den StatisticsAggregator übergeben wird.

### StatisticsAggregator

- Nimmt Rohmessungen aus allen aktiven PingSessions entgegen.
- Berechnet fortlaufend Kenngrößen wie Minimum, Maximum, arithmetisches Mittel und Median der RTT.
- Führt ein Histogramm über Latenzbereiche und verwaltet einen Zeitreihenpuffer für den Verlauf der Messwerte.
- Ermittelt Paketverlust über gezählte gesendete vs. erfolgreich beantwortete Pakete pro Ziel.
- Stellt aggregierte Sichten pro Ziel sowie ggf. globale Sichten für die ConsoleView und Exportfunktionen bereit.

### TargetConfig und Config

- Beschreiben statisch oder aus Konfigurationsdateien geladene Ziele inklusive Hostnamen/IPs, Ping-Intervalle und Laufzeiteinstellungen.
- Definieren Formatoptionen für Ausgabe und Export (z. B. aktivierte Metriken, Histogrammauflösung, Zeitreihenlänge).
- Dienen main als zentrale Quelle für alle Konfigurationen, sodass Module wie PingSession, StatisticsAggregator und ConsoleView konsistent initialisiert werden.

### ConsoleView und Renderer

- Visualisieren die aktuellen Statistiken auf der Konsole.
- Bauen tabellarische Übersichten pro Ziel mit Kennzahlen wie Min/Max/Mean/Median, Paketverlust sowie aktuellem RTT-Wert auf.
- Stellen Zeitverlaufs-Ansichten (z. B. kompakte Sparklines oder vereinfachte Zeitachsen) dar, basierend auf dem Zeitreihenpuffer des StatisticsAggregator.
- Rendern Histogramme der Latenzverteilung in textueller Form.
- Aktualisieren die Konsole periodisch oder eventgesteuert, indem sie die jeweils aktuellen Daten aus dem StatisticsAggregator abfragen.

### main

- Startpunkt der Anwendung und zentrale „Verkabelung“ der Module.
- Führt CLI-Parsing durch (Ziele, Intervalle, Ausgabeoptionen, Exportformate) und erzeugt darauf basierend TargetConfig und globale Config.
- Initialisiert PlatformPingBackend und StatisticsAggregator.
- Erzeugt für jedes konfigurierte Ziel eine PingSession und den zugehörigen PingWorker und startet die parallele Ausführung.
- Initialisiert ConsoleView und setzt die periodische Aktualisierung auf, typischerweise über einen eigenen Rendering-Loop oder Timer.
- Koordiniert sauberes Herunterfahren, beendet Sessions und sorgt für einen konsistenten Abschluss aller Hintergrundaktivitäten.

## Datenflüsse

- PingWorker in jeder PingSession erzeugen Messereignisse für ihr Ziel, indem sie über PlatformPingBackend ICMP-Anfragen senden und Antworten auswerten.
- Jede erfolgreiche oder fehlgeschlagene Messung wird als Event an den StatisticsAggregator übergeben.
- Der StatisticsAggregator aktualisiert seine internen Strukturen (Statistiken, Histogramme, Zeitreihenpuffer, Paketverlustzähler).
- ConsoleView ruft in festen Abständen die aktuellen Aggregationsdaten vom StatisticsAggregator ab und rendert die Ausgabe in der Konsole.
- Exportfunktionen (z. B. CSV/JSON) greifen ebenfalls lesend auf die vom StatisticsAggregator bereitgestellten Daten zu, ohne in den Messfluss einzugreifen.

Die Datenflüsse sind überwiegend eventbasiert (Messereignisse von Sessions zum Aggregator) und werden von der View und Exportpfaden über periodisches Pulling der aggregierten Daten ergänzt.

## Parallelität und Synchronisation

- Für jedes Ziel kann eine eigene PingSession mit separatem PingWorker in einem Thread laufen; alternativ kann ein Threadpool genutzt werden, der mehrere Ziele bedient.
- Alle PingWorker schreiben konkurrierend in den gemeinsamen StatisticsAggregator.
- Der StatisticsAggregator verwendet geeignete Synchronisationsmechanismen (z. B. Mutexe oder Lock-free Strukturen, wo passend), um konsistente Updates der Statistikdatenstrukturen sicherzustellen.
- Lesezugriffe der ConsoleView und Exportfunktionen auf den StatisticsAggregator laufen typischerweise parallel zu schreibenden Messereignissen und werden durch dieselben Synchronisationsprimitive abgesichert.
- Die Konfiguration (TargetConfig/Config) wird nach Initialisierung als weitgehend unveränderlich behandelt und kann daher in der Regel ohne zusätzliche Synchronisation gelesen werden.

## Erweiterbarkeit

- PlatformPingBackend ist als Austauschpunkt für unterschiedliche ICMP-Implementierungen ausgelegt. Weitere Backends können z. B. spezielle System-APIs oder benutzerlandbasierte Pings (UDP/TCP) implementieren, ohne dass PingSession oder StatisticsAggregator angepasst werden müssen.
- ConsoleView und Renderer sind so strukturiert, dass zusätzliche Ausgabeziele ergänzt werden können, etwa farbige Konsole, alternative Layouts oder andere rendererseitige Darstellungsformen.
- Die Export-Schicht unterstützt CSV- und JSON-Ausgaben auf Basis der vom StatisticsAggregator bereitgestellten Daten. Weitere Exportpfade wie Prometheus-Endpoints oder eine WebUI können ergänzt werden, indem sie auf dieselben Aggregationsdaten zugreifen.
- Neue Metriken oder zusätzliche Auswertungen lassen sich im StatisticsAggregator implementieren, ohne den Messpfad grundlegend zu ändern, solange die Schnittstellen für Rohmessungen stabil bleiben.

