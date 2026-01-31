Natürlich, Jörg. Ich formuliere dir das so um, dass es **wirklich jeder KI-Agent versteht** – selbst der schlichteste, vergesslichste und kontextblödeste Roo‑Abkömmling.  
Die Regeln bleiben vollständig erhalten, aber in **extrem klarer, stumpfsicherer, unmissverständlicher Formulierung**.

---

# **Einfache Regeln für den KI‑Agenten (C++20 Projekt)**  
*(Version für sehr einfache KI‑Agenten)*

## **1. Regeln für jeden Prompt**
- Jeder Prompt muss gespeichert werden.  
- Speicherort: `docs/prompts/`  
- Speichere:
  - den Prompt  
  - alle Rückfragen  
  - alle Antworten  
  - alle Ausgaben  
- Wenn Ausgaben in Dateien geschrieben wurden: **nur den Dateinamen notieren**, nicht den Inhalt kopieren.  
- Der Dateiname des Reports muss so aussehen:  
  `NNN_YYYYMMDD_HHMMSS_KurzerName.md`  
  (NNN = laufende Nummer)

---

## **2. Build & Tests**
- Projekt konfigurieren:  
  `cmake -S . -B build`
- Projekt bauen:  
  `cmake --build build --config Debug`  
  oder  
  `cmake --build build --config Release`
- Tests ausführen:  
  `ctest --test-dir build --build-config <Config> --output-on-failure`
- Es gibt ein Build‑Target `run_tests`, das automatisch `ctest` mit `--output-on-failure` startet.
- Catch2 kommt über FetchContent aus:  
  `http://raspirepo/mirrors/Catch2.git` (Tag: `v3.5.3`)
- Testprogramme **dürfen NICHT** gegen das Hauptprogramm `pingstats` linken.  
  Stattdessen:
  - benötigte Quellen direkt in die Test‑Executables einbinden  
  - gegen `Catch2::Catch2WithMain` linken

---

## **3. CLI / Laufzeit**
- Erlaubte Flags:
  - `-i` oder `--interval <sekunden>`
  - `--output-format none|csv|json`
  - `--output-file <pfad>`
  - `-h` oder `--help`
  - `--version`
- Alle anderen Argumente sind Hostnamen.
- Standardwerte:
  - Intervall: **1.0 s**
  - Render‑Intervall: **500 ms**
- Wenn `--output-file` gesetzt ist, aber kein Format angegeben wurde:
  - Format = CSV
  - Standarddateien:
    - CSV → `pingstats.csv`
    - JSON → `pingstats.json`
- CSV‑Export:
  - alle 5 Sekunden  
  - zusätzlich am Ende  
  - nur wenn Format = CSV
- JSON‑Export:
  - nur am Ende  
  - nur wenn Format = JSON
- ConsoleView zeigt:
  - Header  
  - Tabelle: HOST / N / LOSS% / MIN / MAX / MEAN / MED / LAST  
  - Sparkline (Breite 40, Charset `" .:-=+*#%@"`)  
  - Histogramm (Breite 30, Zeichen `#`)  
  - ANSI‑Clear: `\x1b[2J\x1b[H`

---

## **4. Komponenten**
### **PingSession**
- Läuft in eigenem Thread  
- Führt regelmäßig Pings aus  
- Timeout = 0.8 × Intervall (mind. 100 ms, max. 5000 ms)  
- Exceptions zählen als Fehlschlag  
- Jede Messung wird mit `add_sample` gespeichert

### **StatisticsAggregator**
- Berechnet:
  - Min, Max, Mean, Median  
  - Packet‑Loss  
  - Histogramm  
  - RTT‑Ringbuffer
- Thread‑safe (Mutex)
- Standardwerte:
  - Histogramm‑Grenzen: `{10,20,50,100,200,500}`  
  - Median‑Puffer: 1024  
  - Time‑Series: 256

### **ConsoleView**
- Nutzt `snapshot_all()`  
- Sortiert Hosts  
- Läuft in eigenem Thread  
- Thread kann mit `stop()` beendet werden

---

## **5. Skripte**
- `build.ps1`:  
  - CMake‑Configure  
  - Build (Debug als Standard)  
  - Tests  
  - optional: pingstats mit Hosts starten
- `test.sh`:  
  - wie oben, aber für Bash
- `test.py`:  
  - Python‑Version  
  - optional: pingstats mit Hosts starten

---

## **6. Sonstiges**
- Reports kommen in `docs/` (z. B. `docs/report.md`)
- AgentTodo:
  - Einträge **nur erweitern**, niemals löschen  
  - Build‑Test‑Punkte nach OS/Compiler gruppieren  
  - Nach erfolgreicher Umsetzung: Punkt als erledigt markieren

---

## **7. Reports**
- Format: **Markdown**  
- Speicherort: `docs/Reports/`  
- Dateiname beginnt immer mit:  
  `Report_YYYYMMDD_HHMMSS_Name.md`

---

## **8. TODO‑Dateien**
- Hauptpunkte: nummeriert (1, 2, 3, …)
- Unterpunkte: große Buchstaben (A, B, C, …)
- Unter‑Unterpunkte: römische Zahlen (I, II, III, …)
- Jeder Punkt hat ein Kästchen:
  - `[ ]` = offen  
  - `[x]` = erledigt
- Der KI‑Agent muss erledigte Punkte **selbstständig** abhaken.

---

Wenn du möchtest, kann ich dir diese Regeln auch als **DokuWiki‑Modul**, **Markdown‑Datei**, **README**, **Projekt‑Policy**, **Checkliste** oder **Maschinen‑Regelwerk** aufbereiten.
