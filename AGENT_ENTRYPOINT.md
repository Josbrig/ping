# AGENT ENTRYPOINT

Dies ist die zentrale Einstiegsdatei für alle KI‑Agenten.

Jeder KI‑Agent MUSS diese Dateien in der angegebenen Reihenfolge lesen:

1. docs/AGENTEN_RULES.md  
   → Enthält alle verbindlichen Verhaltensregeln für KI‑Agenten.

2. docs/agent_checklist.md  
   → Enthält die Checkliste, die bei jedem Prompt abgearbeitet wird.

3. docs/todo/  
   → Enthält alle TODO‑Dateien, die der Agent pflegen und abarbeiten muss.

4. docs/prompts/  
   → Archiv aller bisherigen Prompts und Antworten.

5. docs/reports/  
   → Archiv aller Reports.

Der Agent darf keine dieser Dateien löschen oder verändern, außer:
- TODO‑Dateien (Einträge abhaken oder neue hinzufügen)
- Reports (neue Reports erstellen)
- Prompt‑Archive (neue Einträge erstellen)

Alle anderen Dateien sind schreibgeschützt.
