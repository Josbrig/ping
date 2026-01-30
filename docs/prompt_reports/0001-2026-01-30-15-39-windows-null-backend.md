# Prompt Report 0001 – 2026-01-30 15:39 – Windows Null Backend causing 100% loss

## Beobachtung
- Manuelle Pings zu `dan-o-mat` und `megagamer` funktionieren mit 0% Verlust.
- Lauf mit `scripts/test_hil.bat` zeigt 100% Paketverlust (keine RTTs) und endlose Ausgabe.

## Ursache
- Auf Windows liefert `make_platform_ping_backend()` in [`src/platform_ping_backend_factory.cpp`](src/platform_ping_backend_factory.cpp:1) ein `NullPingBackend`, weil nur macOS (`__APPLE__`) und Linux (`__linux__`) unterstützt werden. Auf anderen Plattformen (Windows) wird ein Stub-Backend genutzt, dessen `send_ping` immer `success=false` zurückgibt.

## Implikation
- pingstats kann unter Windows mit dem aktuellen Backend keine echten ICMP-Pings senden; alle Ziele zeigen 100% Loss, unabhängig von tatsächlicher Erreichbarkeit.

## Vorschlag zur Behebung
1) Windows-Backend implementieren (z. B. mit IcmpSendEcho oder Raw Sockets) und im Factory-Guard ergänzen (WIN32).
2) Alternativ temporär: für Windows über WSL-Ausführung dokumentieren oder einen externen Ping-Aufruf nutzen, bis ein natives Backend verfügbar ist.

## Nächste Schritte (empfohlen)
- Priorität: Backend-Unterstützung für Windows ergänzen; Factory um WIN32-Zweig erweitern.
- Bis dahin: README/Skripte kennzeichnen, dass unter Windows aktuell nur Stub aktiv ist und reale Pings nicht funktionieren.
