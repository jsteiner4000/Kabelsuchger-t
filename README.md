# Kabelsuchgerät (Sender + Handgerät)

Selbstbau-Leitungsortungsgerät für EMA/BMA-Leitungen (J-Y(St)Y, LIYY …):
Ein **Sender** speist ein Suchsignal in eine spannungsfreie Leitung ein, ein
**Handgerät** verfolgt die Leitung von außen, auch durch Wände.

Prinzip (Konzept): Ortung über das **Magnetfeld**. Alle Adern und der Schirm werden gemeinsam gegen Erde
eingespeist, damit kein Rückstrom im selben Kabel das Feld aufhebt. Zwei gekoppelte Töne ermöglichen eine
Stromrichtungsanzeige für volle Trassen. Das Handgerät hat zwei Ferritspulen und eine E-Feld-Spitze und
wertet mit 24-bit-Wandler und digitaler Lock-in-Technik aus.

## Stand

**Phase 3 von 8:** Machbarkeitstest auf dem Steckbrett. Lastenheft und Konzept sind freigegeben. Jetzt
werden die Teile bestellt, die Prototypen aufgebaut und vermessen.

## Dokumente

| Datei | Inhalt | Visuell (privat) |
|---|---|---|
| [`docs/lastenheft.md`](docs/lastenheft.md) | Was das Gerät können muss, Abnahmetests mit Zielwerten | <https://claude.ai/artifact/R5hurM3dSdJBk6UaUzjGMM> |
| [`docs/konzept.md`](docs/konzept.md) | Wie es funktioniert: Physik, Einspeisemodi, Signal, Auswertung, Varianten, Reichweiten, Budget | <https://claude.ai/artifact/VZtH7KZhMHQYaksFrRsrAv> |
| [`docs/machbarkeitstest.md`](docs/machbarkeitstest.md) | Einkaufsliste, Steckbrett-Aufbau, Spulen wickeln, Software, Messprogramm M1–M9 | <https://claude.ai/artifact/EqZmsoxrUWFYdXUHB5X285> |
| [`docs/messprotokoll-phase3.md`](docs/messprotokoll-phase3.md) | Messprotokoll zum Ausfüllen | |
| [`docs/entscheidungen.md`](docs/entscheidungen.md) | Jede Entscheidung mit Begründung | |
| [`docs/berechnungen/feldmodell.py`](docs/berechnungen/feldmodell.py) | Feld- und Reichweitenrechnung (nur Python-Standardbibliothek) | |
| [`stueckliste.csv`](stueckliste.csv) | Stückliste (ab Phase 4), Trennzeichen `;` | |

## Projektstruktur

```
docs/                          Lastenheft, Konzept, Machbarkeitstest, Entscheidungen, Anleitungen
  berechnungen/                Feldmodell (Python)
hardware/sender/
  testaufbau/                  Schaltplan Sender-Steckbrett (schemdraw-Skript + SVG)
hardware/handgeraet/
  spule/                       Spulenkörper und Wickeldorn (OpenSCAD + STL)
  testaufbau/                  Messkopf (OpenSCAD + STL), Schaltplan Empfänger-Steckbrett
firmware/machbarkeit/
  sender_test/                 Test-Sketch Sender (ESP32-S2 mini)
  empfaenger_test/             Test-Sketch Empfänger (ESP32-S3) + lockin.h (Signalverarbeitung)
  test_host/                   Prüfung auf dem PC: Syntaxprüfung + Simulationstest (./build.sh)
firmware/sender/               Endgültige Firmware (Phase 6)
firmware/handgeraet/
gehaeuse/                      Gehäuse (Phase 7)
stueckliste.csv
```

## Phasen

1. Rückfragen und Lastenheft – **erledigt**
2. Konzept – **erledigt**
3. Machbarkeitstest (Steckbrett) – **läuft**
4. Schaltung und Stückliste
5. Platine (KiCad)
6. Firmware
7. Gehäuse
8. Inbetriebnahme und Bedienungsanleitung

Nach jeder Phase wird freigegeben, erst dann geht es weiter.

## Sicherheit

Der Sender ist **nur für spannungsfreie Leitungen** gedacht und nicht für Netzspannung ausgelegt.
Beim Steckbrett-Sender bitte Abschnitt 1 von `docs/machbarkeitstest.md` beachten (Masse, Oszilloskop).
