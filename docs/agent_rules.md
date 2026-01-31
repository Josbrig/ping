Gern, Jörg.  
Hier ist eine **bereinigte, klar strukturierte, KI‑Agenten‑Regeldatei**, in der **alles Überflüssige entfernt** wurde.  
Sie enthält **nur das**, was ein KI‑Agent wirklich als verbindliche Regeln braucht – ohne Implementierungsdetails, ohne Architektur‑Doku, ohne Skript‑Erklärungen.

Wenn du möchtest, kann ich sie auch direkt in **DokuWiki‑Format** umwandeln.

---

# **KI‑Agenten Regeln für dieses Projekt (bereinigte Version)**

## **1. Regeln für jeden Prompt**
- Jeder Prompt muss archiviert werden.  
- Speicherort: `docs/prompts/`
- Archiviert werden:
  - Prompt  
  - Rückfragen  
  - Antworten  
  - Ausgaben  
- Wenn Ausgaben in Dateien geschrieben wurden: **nur den Dateinamen referenzieren**.
- Dateiname:  
  `NNN_YYYYMMDD_HHMMSS_KurzerName.md`  
  (NNN = laufende Nummer)

---

## **2. Build & Test Regeln**
- Konfigurieren:  
  `cmake -S . -B build`
- Bauen:  
  `cmake --build build --config <Debug|Release>`
- Tests ausführen:  
  `ctest --test-dir build --build-config <Config> --output-on-failure`
- Target `run_tests` führt `ctest` automatisch mit `--output-on-failure` aus.
- Testprogramme:
  - dürfen **nicht** gegen das Executable `pingstats` linken  
  - binden benötigte Quellen direkt ein  
  - linken gegen `Catch2::Catch2WithMain`

---

## **3. CLI‑Regeln**
- Erlaubte Flags:
  - `-i`, `--interval <sekunden>`
  - `--output-format none|csv|json`
  - `--output-file <pfad>`
  - `-h`, `--help`
  - `--version`
- Alle anderen Argumente sind Hostnamen.
- Standardwerte:
  - Intervall: 1.0 s  
  - Render‑Intervall: 500 ms
- Wenn `--output-file` gesetzt ist, aber kein Format:
  - Format = CSV
- Exportregeln:
  - CSV: alle 5 s + final  
  - JSON: nur final

---

## **4. Komponenten‑Regeln (nur Verhalten, keine Details)**
### **PingSession**
- Läuft in eigenem Thread  
- Führt periodische Pings aus  
- Exceptions zählen als Fehlschlag  
- Jede Messung wird mit `add_sample` gespeichert

### **StatisticsAggregator**
- Berechnet grundlegende Statistiken  
- Thread‑safe (Mutex)

### **ConsoleView**
- Nutzt `snapshot_all()`  
- Sortiert Hosts  
- Läuft in eigenem Thread  
- Kann über `stop()` beendet werden

---

## **5. Reports**
- Format: Markdown  
- Speicherort: `docs/Reports/`
- Dateiname:  
  `Report_YYYYMMDD_HHMMSS_Name.md`

---

## **6. TODO‑Regeln**
- Hauptpunkte: nummeriert (1, 2, 3, …)
- Unterpunkte: große Buchstaben (A, B, C, …)
- Unter‑Unterpunkte: römische Zahlen (I, II, III, …)
- Jeder Punkt hat ein Kästchen:
  - `[ ]` offen  
  - `[x]` erledigt
- Der KI‑Agent markiert erledigte Punkte **selbstständig**.

---

## **7. AgentTodo‑Regeln**
- Einträge **erweitern**, niemals löschen  
- Build‑/Test‑Einträge nach OS/Compiler gruppieren  
- Nach erfolgreicher Umsetzung: als erledigt markieren

---


