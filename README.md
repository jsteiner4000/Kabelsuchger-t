# Kabelsuchgerät (Sender + Handgerät)

Selbstbau-Leitungsortungsgerät für EMA/BMA-Leitungen (J-Y(St)Y, LIYY …):
Ein **Sender** speist ein Suchsignal in eine spannungsfreie Leitung ein, ein
**Handgerät** verfolgt die Leitung von außen, auch durch Wände.

Prinzip (Konzept): Ortung über das **Magnetfeld**. Alle Adern und der Schirm werden gemeinsam gegen Erde
eingespeist, damit kein Rückstrom im selben Kabel das Feld aufhebt. Zwei gekoppelte Töne ermöglichen eine
Stromrichtungsanzeige für volle Trassen. Gebaut wird die **Variante „Einfach“** (ca. 31 € pro Satz): Der Sender
hat zwei Klemmen, ein Display mit Strom in mA und eine Taste. Das Handgerät ist ein Stab mit dem Sensorkopf oben,
einer Ferritspule, zwei Verstärkerstufen und dem eingebauten Wandler des ESP32-S3 mit digitaler
Lock-in-Auswertung.

## Stand

**Phase 3 von 8:** Mini-Test mit den endgültigen Teilen. Julius hat am 13.10.2026 die Variante „Einfach“
freigegeben (E-034 bis E-042), mit Display in beiden Geräten und Aufbau auf Lochraster. Jetzt werden die Teile
für einen Satz bestellt, auf dem Steckbrett getestet und danach auf die Lochrasterplatine übernommen.

## Dokumente

| Datei | Inhalt | Visuell (privat) |
|---|---|---|
| [`docs/lastenheft.md`](docs/lastenheft.md) | Was das Gerät können muss, Abnahmetests mit Zielwerten | <https://claude.ai/artifact/R5hurM3dSdJBk6UaUzjGMM> |
| [`docs/konzept.md`](docs/konzept.md) | Wie es funktioniert: Physik, Einspeisemodi, Signal, Auswertung, Varianten, Reichweiten, Budget | <https://claude.ai/artifact/VZtH7KZhMHQYaksFrRsrAv> |
| [`docs/machbarkeitstest.md`](docs/machbarkeitstest.md) | Mini-Test: Einkaufsliste, Steckbrett-Aufbau, Spule wickeln, Software, Messprogramm M1–M8 | <https://claude.ai/artifact/EqZmsoxrUWFYdXUHB5X285> |
| [`docs/messprotokoll-phase3.md`](docs/messprotokoll-phase3.md) | Messprotokoll zum Ausfüllen | |
| [`docs/variante-einfach.md`](docs/variante-einfach.md) | Variante „Einfach“: was bleibt, was fliegt, Kosten (freigegeben) | |
| [`docs/platine-oder-lochraster.md`](docs/platine-oder-lochraster.md) | KiCad-Entwurf der Platinen, Vergleich mit Lochraster, Empfehlung | <https://claude.ai/artifact/UjBC8Tjy9ur99xCQnq3SMG> |
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
  testaufbau/                  Messkopf (OpenSCAD + STL), Schaltplan Handgerät-Steckbrett
firmware/machbarkeit/
  sender_test/                 Test-Sketch Sender (ESP32-S2 mini, OLED, Strommessung)
  empfaenger_test/             Test-Sketch Handgerät (ESP32-S3, eingebauter Wandler) + lockin.h (Signalverarbeitung)
  test_host/                   Prüfung auf dem PC: Syntaxprüfung + Simulation der ganzen Kette (./build.sh)
firmware/sender/               Endgültige Firmware (Phase 6)
firmware/handgeraet/
hardware/kicad/                KiCad-Entwurf beider Platinen (Schaltplan, Platine, Gerber), Generator-Skript
gehaeuse/                      Gehäuse (Phase 7)
  konzept/                     Ansichtsmodell beider Geräte (OpenSCAD + Bilder)
stueckliste.csv
```

## Phasen

1. Rückfragen und Lastenheft – **erledigt**
2. Konzept – **erledigt**
3. Machbarkeitstest (Mini-Test mit den endgültigen Teilen) – **läuft**
4. Schaltung und Stückliste
5. Lochraster-Aufbauplan (oder Platine nach KiCad-Entwurf, Entscheidung offen)
6. Firmware
7. Gehäuse
8. Inbetriebnahme und Bedienungsanleitung

Nach jeder Phase wird freigegeben, erst dann geht es weiter.

## Sicherheit

Der Sender ist **nur für spannungsfreie Leitungen** gedacht und nicht für Netzspannung ausgelegt.
Beim Steckbrett-Sender bitte Abschnitt 1 von `docs/machbarkeitstest.md` beachten (Masse, Oszilloskop, USB nur bei
ausgeschaltetem Batterieschalter, Pegel MAX nur gegen Erde).
