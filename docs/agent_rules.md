# **AGENTEN_RULES.md – Verhaltensregeln für KI‑Agenten in diesem Projekt**  
*(Modern, minimalistisch, robust, KI‑freundlich)*

## **0. Zweck dieser Datei**
Diese Datei definiert **alle Regeln**, nach denen ein KI‑Agent in diesem Projekt arbeitet.  
Sie stellt sicher, dass **jeder KI‑Agent**:

- Aufgaben immer gleich verarbeitet  
- Ergebnisse vollständig dokumentiert  
- TODO‑Listen korrekt pflegt  
- Dateien an den richtigen Orten ablegt  
- konsistent, reproduzierbar und nachvollziehbar arbeitet  

Diese Regeln gelten **für jeden Prompt**, **jede Antwort** und **jede Aktion** des KI‑Agenten.

---

# **1. Grundprinzipien des KI‑Agenten**
1. **Alles, was der Agent erhält oder erzeugt, wird archiviert.**  
2. **Der Agent arbeitet transparent, reproduzierbar und nachvollziehbar.**  
3. **Der Agent überschreibt niemals Dateien ohne explizite Anweisung.**  
4. **Der Agent fragt nach, wenn Informationen fehlen.**  
5. **Der Agent hält sich strikt an die Projektstruktur.**  
6. **Der Agent arbeitet schrittweise und markiert erledigte Aufgaben selbstständig.**

---

# **2. Projektstruktur (Verzeichnisregeln)**

Der KI‑Agent hält sich an folgende feste Struktur:

| Bereich | Zweck | Pfad |
|--------|-------|------|
| **Prompts** | Archiv aller Prompts, Rückfragen, Antworten | `docs/prompts/` |
| **Reports** | Ergebnisberichte, Arbeitsprotokolle | `docs/reports/` |
| **TODO‑Dateien** | Aufgabenlisten für Agenten | `docs/todo/` |
| **Konzepte** | Architekturideen, Entwürfe, Überlegungen | `docs/concepts/` |
| **Dokumentation** | Technische Doku für Entwickler | `docs/documentation/` |
| **Benutzeranleitungen** | Anleitung für Menschen, die das Projekt nutzen | `docs/manuals/` |
| **Projekt‑Skripte** | Skripte, die Teil des Projekts sind | `scripts/project/` |
| **Benutzer‑Skripte** | Skripte zur Erleichterung der Bedienung | `scripts/user/` |

Der Agent legt Dateien **immer** in diesen Bereichen ab.  
Wenn ein Bereich fehlt, erstellt der Agent ihn.

---

# **3. Prompt‑Archivierung**

Für **jeden Prompt**, inklusive Rückfragen und Antworten, erstellt der Agent eine Archivdatei:

- Speicherort: `docs/prompts/`
- Format: Markdown
- Dateiname:  
  `Prompt_YYYYMMDD_HHMMSS_KurzerName.md`

Inhalt:

1. Original‑Prompt  
2. Rückfragen des Agenten  
3. Antworten des Nutzers  
4. Endergebnis  
5. Referenzen auf erzeugte Dateien

---

# **4. Reports**

Der Agent erstellt einen Report, wenn:

- eine Aufgabe abgeschlossen wurde  
- eine Datei erzeugt wurde  
- eine TODO‑Liste aktualisiert wurde  
- ein Konzept erstellt wurde  
- ein Problem auftrat  

Regeln:

- Speicherort: `docs/reports/`
- Format: Markdown
- Dateiname:  
  `Report_YYYYMMDD_HHMMSS_Name.md`
- Inhalt:
  - Zweck  
  - Schritte  
  - Ergebnisse  
  - erzeugte Dateien  
  - aktualisierte TODO‑Punkte  

---

# **5. TODO‑Regeln**

## **5.1 Struktur einer TODO‑Datei**
Eine TODO‑Datei besteht aus:

- **Hauptpunkten** (nummeriert: 1, 2, 3, …)
- **Unterpunkten** (A, B, C, …)
- **Unter‑Unterpunkten** (I, II, III, …)

Beispiel:

```
1. [ ] Hauptaufgabe
   A. [ ] Unteraufgabe
      I. [ ] Unterunteraufgabe
```

## **5.2 Regeln für TODO‑Einträge**
- Jeder Punkt hat ein Statusfeld:
  - `[ ]` offen  
  - `[x]` erledigt  
- Der Agent **darf neue Punkte hinzufügen**, aber **niemals löschen**.  
- Der Agent markiert Punkte **selbstständig als erledigt**, wenn:
  - die Aufgabe vollständig erfüllt wurde  
  - alle Unterpunkte erledigt sind  
  - die Ergebnisse dokumentiert wurden  

## **5.3 Selbstständige Abarbeitung**
Der Agent arbeitet TODO‑Listen so ab:

1. **Punkt lesen**  
2. **Verstehen, was verlangt wird**  
3. **Fehlende Informationen erfragen**  
4. **Aufgabe ausführen**  
5. **Ergebnisse dokumentieren (Report)**  
6. **Punkt abhaken**  
7. **Nächsten Punkt bearbeiten**

---

# **6. Regeln für Dateiformate**

Der Agent verwendet folgende Formate:

| Zweck | Format |
|-------|--------|
| Prompts | `.md` |
| Reports | `.md` |
| TODO‑Listen | `.md` |
| Konzepte | `.md` |
| Dokumentation | `.md` |
| Benutzeranleitungen | `.md` |
| Skripte | `.sh`, `.ps1`, `.py`, `.bat` |
| Konfiguration | `.json`, `.yaml`, `.toml` |

Regeln:

- Markdown ist Standard für alle textbasierten Inhalte.  
- Der Agent erzeugt **keine Binärdateien**, außer wenn explizit angewiesen.  
- Der Agent dokumentiert jede erzeugte Datei in einem Report.

---

# **7. Arbeitsweise des KI‑Agenten**

Der Agent arbeitet nach folgenden Prinzipien:

### **7.1 Schrittweises Vorgehen**
- Der Agent führt Aufgaben in klaren, nachvollziehbaren Schritten aus.
- Jeder Schritt wird dokumentiert.

### **7.2 Rückfragen**
Der Agent stellt Rückfragen, wenn:

- Informationen fehlen  
- Anforderungen unklar sind  
- mehrere Interpretationen möglich sind  

### **7.3 Konsistenz**
Der Agent:

- hält sich an die Projektstruktur  
- verwendet konsistente Sprache  
- erzeugt reproduzierbare Ergebnisse  
- vermeidet unnötige Änderungen  

### **7.4 Sicherheit**
Der Agent überschreibt **niemals**:

- bestehende Dateien  
- bestehende TODO‑Einträge  

ohne ausdrückliche Anweisung.

---

# **8. Abschlussregeln**

- Diese Datei gilt für **alle KI‑Agenten**, unabhängig von Modell, Version oder Fähigkeiten.  
- Der Agent liest diese Datei **vor jeder Aufgabe**.  
- Der Agent hält sich strikt an diese Regeln.  
- Änderungen an dieser Datei dürfen nur durch den Nutzer erfolgen, nicht durch den Agenten.

