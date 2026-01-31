## Zweck

Kurzanleitung für Entwickler:innen, wie das GitHub‑Wiki dieses Projekts als Git‑Submodul (`wiki/`) gepflegt wird. Submodul-Konfiguration: siehe [`.gitmodules`](.gitmodules:1).

## Schnellstart

1. **Repository mit Submodul klonen**
   ```bash
   git clone --recurse-submodules git@github.com:Josbrig/ping.git
   # Falls bereits geklont: git submodule update --init --recursive
   ```
2. **Status prüfen**
   ```bash
   git submodule status
   cd wiki && git status && cd ..
   ```
3. **Wiki bearbeiten**: Markdown-Dateien in `wiki/` anpassen oder neue Seiten erstellen (z. B. `wiki/Neue-Seite.md`).

## Arbeitsablauf für Änderungen am Wiki

1. **In das Submodul wechseln**
   ```bash
   cd wiki
   ```
2. **Änderungen vornehmen und prüfen**
   ```bash
   git status
   ```
3. **Commit im Submodul erstellen**
   ```bash
   git add .
   git commit -m "Update wiki: <kurze Beschreibung>"
   git push origin HEAD
   ```
4. **Zurück im Hauptrepo den Submodul-Zeiger aktualisieren**
   ```bash
   cd ..
   git add wiki
   git commit -m "Bump wiki submodule to latest"
   git push origin HEAD
   ```

Hinweis: Ohne den letzten Commit im Hauptrepo zeigt das Projekt nicht auf den neuen Wiki-Stand.

## Änderungen aus dem Wiki übernehmen (Pull)

Falls andere das Wiki aktualisiert haben:

```bash
git pull origin HEAD            # Hauptrepo
git submodule update --remote wiki  # holt den neuesten Wiki-Commit
git add wiki
git commit -m "Update wiki submodule pointer"
git push origin HEAD
```

## Typische Aufgaben

- **Neue Seite anlegen**: Datei unter `wiki/` erstellen, interne Links mit `[[Seitentitel]]` nutzen.
- **Diagramme/Doxygen-Bilder referenzieren**: Platzhalter wie `![Diagramm](images/diagram.png)` einfügen; Bilder später unter `wiki/images/` ablegen.
- **Prompts/Quellen nutzen**: Kontext für die Wiki-Struktur liegt in [`docs/prompts/PROMPT_to_generate_the_wiki.md`](docs/prompts/PROMPT_to_generate_the_wiki.md:1).

## Stolperfallen

- Submodul-Commit gepusht, aber **Zeiger im Hauptrepo nicht committet** → Team sieht alte Version.
- `git pull` im Hauptrepo ohne `git submodule update` → lokaler Wiki-Stand veraltet.
- Änderungen direkt im Hauptrepo committen, obwohl sie zum Wiki gehören → immer im Submodul arbeiten.

## Kurzübersicht der wichtigsten Befehle

```bash
# Initialisieren/aktualisieren
git submodule update --init --recursive

# Neueste Wiki-Version holen
git submodule update --remote wiki

# Im Submodul arbeiten
cd wiki && git status
git add . && git commit -m "Update wiki" && git push

# Zeiger im Hauptrepo aktualisieren
cd .. && git add wiki && git commit -m "Bump wiki submodule" && git push
```

