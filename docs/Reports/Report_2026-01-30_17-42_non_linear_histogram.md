# Report – Effort to switch histogram to non-linear buckets (finer at low RTT, coarser at high RTT)

## Kontext
- Histogramm-Buckets sind aktuell fest auf lineare Grenzen {10,20,50,100,200,500} ms gesetzt in [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:17) und werden pro Host beim Anlegen übernommen [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:44).
- Bucket-Zuordnung läuft über einfaches Schwellen-Vergleichen [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:148) und Rendering nutzt die Grenzen direkt [`console_view_impl.cpp`](src/console_view_impl.cpp:213).
- CLI/Config enthält zwar `bucket_boundaries`, wird aber derzeit nicht in die Aggregator-Instanzen injiziert [`config.hpp`](include/config.hpp:15).

## Optionen und Aufwand
1) **Statische logarithmische Defaults (Quick Win)**
   - Ändere Default-Grenzen auf log-ähnliche Staffel (z. B. 0.5,1,2,5,10,20,50,100,200,500,1000 ms) an [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:17).
   - Bestehende Zuweisung/Anzeige bleibt unverändert; Labels passen sich automatisch an.
   - Aufwand: ~1–2 h (Code + kurze Doku/Changelog + Smoke-Test).

2) **Konfigurierbare Bucket-Schemata (Presets/Custom)**
   - Übergib benutzerdefinierte Grenzen aus CLI/TargetConfig und verwende sie beim Host-Anlegen anstelle des Defaults [`statistics_aggregator.cpp`](src/statistics_aggregator.cpp:44).
   - Füge Presets ("fine", "log", "coarse") plus freies List-Flag hinzu; Doku/Help anpassen; Unit-Tests für Zuordnung/Labeling.
   - Aufwand: ~2–4 h (CLI-Plumbing, Tests, Doku).

3) **Adaptives Histogramm (dynamisch)**
   - Kanten aus Beobachtungen ableiten (z. B. log um Median/IQR oder Quantile), Re-Binning bei Kantenänderung, Konsistenz der Snapshots sichern.
   - Erfordert zusätzliche State-Verwaltung und Thread-Safety-Review.
   - Aufwand: ~1–2 Tage (Design, Implementierung, Stabilitätstests).

## Empfehlung
- Für schnellen Mehrwert: Option 1 (log-spaced Defaults) umsetzen und als neuen Default dokumentieren.
- Für Nutzersteuerung: Option 2 implementieren (Presets + freie Liste), Standard weiterhin log-spaced.
- Option 3 nur bei expliziter Anforderung nach adaptiven Buckets.
