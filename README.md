# Kabelsuchgerät (Sender + Handgerät)

Selbstbau-Leitungsortungsgerät für EMA/BMA-Leitungen (J-Y(St)Y, LIYY …):
Ein **Sender** speist ein Suchsignal in eine spannungsfreie Leitung ein, ein
**Handgerät** verfolgt die Leitung von außen, auch durch Wände.

## Stand

**Phase 2 von 8:** Lastenheft v1.0 ist freigegeben. Das Konzept v1.0 liegt zur Freigabe vor; offen ist die
Budgetentscheidung (Variante C, ca. 50 € pro Satz).

## Dokumente

| Datei | Inhalt |
|---|---|
| [`docs/lastenheft.md`](docs/lastenheft.md) | Was das Gerät können muss: Anforderungen, Abnahmetests, offene Fragen |
| [`docs/lastenheft-uebersicht.html`](docs/lastenheft-uebersicht.html) | Dasselbe als visuelle Übersicht, veröffentlicht unter <https://claude.ai/artifact/R5hurM3dSdJBk6UaUzjGMM> (privat) |
| [`docs/konzept.md`](docs/konzept.md) | Wie das Gerät funktioniert: Physik, Einspeisemodi, Signal, Auswertung, Varianten, Reichweiten, Budget |
| [`docs/konzept-uebersicht.html`](docs/konzept-uebersicht.html) | Konzept als visuelle Übersicht, veröffentlicht unter <https://claude.ai/artifact/VZtH7KZhMHQYaksFrRsrAv> (privat) |
| [`docs/berechnungen/feldmodell.py`](docs/berechnungen/feldmodell.py) | Feld- und Reichweitenrechnung zum Konzept (nur Python-Standardbibliothek) |
| [`docs/entscheidungen.md`](docs/entscheidungen.md) | Jede Entscheidung mit Begründung |
| [`stueckliste.csv`](stueckliste.csv) | Stückliste (ab Phase 4), Trennzeichen `;` |

## Projektstruktur

```
docs/                 Konzept, Entscheidungen, Anleitungen
hardware/sender/      Schaltplan, KiCad-Projekt bzw. Aufbauplan
hardware/handgeraet/
firmware/sender/      Arduino-Sketch
firmware/handgeraet/
gehaeuse/             OpenSCAD/CadQuery-Quellcode, STL, Druckeinstellungen
stueckliste.csv
```

## Phasen

1. Rückfragen und Lastenheft
2. Konzept
3. Machbarkeitstest (Steckbrett)
4. Schaltung und Stückliste
5. Platine bzw. Aufbauplan
6. Firmware
7. Gehäuse
8. Inbetriebnahme und Bedienungsanleitung

Nach jeder Phase wird freigegeben, erst dann geht es weiter.

## Sicherheit

Der Sender ist **nur für spannungsfreie Leitungen** gedacht und nicht für Netzspannung ausgelegt.
