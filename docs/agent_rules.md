Du hast völlig recht, Jörg.  
Wenn das Ziel ein **Regelwerk für das Verhalten eines KI‑Agenten** ist, dann gehören dort **keine Projekt‑Details** hinein – keine Build‑Befehle, keine CLI‑Spezifikationen, keine Komponentenbeschreibungen.  

Ein KI‑Agenten‑Regelwerk sollte ausschließlich beantworten:

- *Wie soll der Agent arbeiten?*  
- *Wie soll er dokumentieren?*  
- *Wie soll er mit TODOs umgehen?*  
- *Wie soll er Prompts archivieren?*  
- *Wie soll er sich verhalten, wenn er Code generiert?*  
- *Wie soll er mit Fehlern umgehen?*  

Alles andere ist **Projekt‑Doku**, nicht Agenten‑Verhalten.

Ich erstelle dir jetzt eine **neue, saubere Version**, die **nur** Agenten‑Verhaltensregeln enthält – ohne jegliche Projekt‑Details.

---

# **KI‑Agenten‑Verhaltensregeln (reine Version, ohne Projektdetails)**

## **1. Prompt‑Archivierung**
- Jeder Prompt wird archiviert.  
- Archiviert werden:
  - Prompt  
  - Rückfragen  
  - Antworten  
  - Ausgaben  
- Speicherort: `docs/prompts/`
- Wenn Ausgaben in Dateien geschrieben wurden: **nur Dateinamen referenzieren**.
- Dateiname:  
  `NNN_YYYYMMDD_HHMMSS_KurzerName.md`

---

## **2. Report‑Regeln**
- Jeder Report wird im Ordner `docs/Reports/` abgelegt.
- Format: Markdown.
- Dateiname:  
  `Report_YYYYMMDD_HHMMSS_Name.md`
- Reports müssen vollständig, nachvollziehbar und reproduzierbar sein.

---

## **3. TODO‑Regeln**
- Hauptpunkte: nummeriert (1, 2, 3, …)
- Unterpunkte: große Buchstaben (A, B, C, …)
- Unter‑Unterpunkte: römische Zahlen (I, II, III, …)
- Jeder Punkt hat ein Status‑Kästchen:
  - `[ ]` offen  
  - `[x]` erledigt
- Der KI‑Agent markiert erledigte Punkte **selbstständig**, sobald die Aufgabe erfüllt ist.
- Einträge dürfen **erweitert**, aber niemals gelöscht werden.

---

## **4. Arbeitsweise des KI‑Agenten**
- Der Agent arbeitet **schrittweise, nachvollziehbar und dokumentiert**.
- Der Agent erzeugt **keine stillen Änderungen**.  
  Jede Änderung wird im Report dokumentiert.
- Der Agent erzeugt **keine Dateien**, ohne sie im Report zu erwähnen.
- Der Agent hält sich strikt an die Struktur des Projekts.
- Der Agent fragt nach, wenn Informationen fehlen.
- Der Agent erzeugt Code nur, wenn:
  - der Zweck klar ist  
  - die Anforderungen eindeutig sind  
  - die Ziel‑Datei oder der Ziel‑Ort definiert ist  
- Der Agent überschreibt niemals Dateien ohne explizite Anweisung.

---

## **5. Fehler‑ und Konfliktverhalten**
- Wenn Anforderungen unklar sind → Rückfrage.
- Wenn Anforderungen widersprüchlich sind → Konflikt benennen und Lösung vorschlagen.
- Wenn der Agent etwas nicht ausführen kann → Grund nennen und Alternativen anbieten.
- Wenn der Agent eine Aufgabe abschließt → im TODO‑System abhaken.

---

## **6. Konsistenz‑Regeln**
- Der Agent hält sich an:
  - die Projektstruktur  
  - die Dokumentationsstruktur  
  - die Namenskonventionen  
  - die Archivierungsregeln  
- Der Agent erzeugt konsistenten Stil:
  - klare Sprache  
  - reproduzierbare Schritte  
  - nachvollziehbare Entscheidungen  

---

## **7. Selbstkontrolle**
Vor jeder Aufgabe prüft der Agent:

1. **Gehört diese Aufgabe in einen Report?**  
2. **Muss ein TODO‑Punkt aktualisiert werden?**  
3. **Sind alle Informationen vorhanden?**  
4. **Ist die Ausgabe reproduzierbar?**  
5. **Ist die Dokumentation vollständig?**

---

