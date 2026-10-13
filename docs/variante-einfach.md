# Vorschlag: Variante „Einfach“

**Stand:** 13.10.2026 · **Status:** freigegeben durch Julius am 13.10.2026 (E-034), mit OLED auch im Sender und
Aufbau auf Lochraster (E-042)

Anlass: Julius findet Variante C (ca. 50 € pro Satz, Phase-3-Einkauf ca. 57 €) zu teuer und zu aufwendig. Er möchte
ein ganz normales, einfaches Kabelsuchgerät mit möglichst wenig Aufwand. Außerdem soll die Abtastfläche des
Handgeräts oben am Kopf sitzen (siehe `gehaeuse/konzept/`).

## 1. Grundgedanke

Die Reichweite hängt vor allem an drei Dingen: wie eingespeist wird (alle Adern + Schirm per WAGO gegen Erde),
wie viel Strom fließt und wie empfindlich die Spule ist. Das kostet fast nichts und bleibt. Teuer und aufwendig
waren die Extras: 24-bit-Wandler, zweite Spule, E-Feld-Spitze, Display und Messschaltung im Sender,
Durchgangsprüfung, drei Frequenzsätze.

Die Variante „Einfach“ ist die Hardware von Variante B (eine Spule, Wandler im ESP32) mit der fertigen Software
von Variante C (Zweiton, Lock-in, Stromrichtung). Der Sender verliert Relais und Messschaltung, behält aber auf
Julius' Wunsch das Display (zeigt Strom, Pegel und Batterie).

## 2. Was bleibt, was fliegt

| Teil | Variante C (bisher) | Variante „Einfach“ |
|---|---|---|
| Einspeisung | Modus G, A, P, umschaltbar | nur zwei Leitungen A und E; Modus G/A ist nur Anklemmen, keine Umschaltung |
| Signal | Zweiton, 3 Frequenzsätze | Zweiton, nur Standard (8,3 + 4,2 kHz) |
| Sender: Endstufe | TL072 + BD139/BD140 an 24 V | **bleibt** |
| Sender: Anzeige | OLED 1,3″ | **bleibt** (OLED 1,3″: Strom in mA, Pegel, Frequenz, Batterie) |
| Sender: Messen | Strom, Lastbild, Länge, Durchgang, Fremdspannungssperre | nur Strom (10-Ω-Messwiderstand, ±10 %) |
| Sender: Schutz | PTC + Schutzdiode + Relais | PTC, Klemmdioden, TVS an 24 V, Strombegrenzung per Firmware |
| Handgerät: Rechner | ESP32-S3 SuperMini | **bleibt** (hat Gleitkomma-Einheit, `lockin.h` läuft ohne Umbau) |
| Handgerät: Wandler | PCM1808 (24 bit) + CD4053 | Wandler im ESP32-S3, zwei Verstärkerstufen auf zwei Eingängen (automatische Bereichswahl) |
| Handgerät: Sensoren | 2 Spulen + E-Feld-Spitze | 1 Spule im Kopf, direkt unter der Abtastfläche |
| Handgerät: Anzeige | OLED 1,3″, nT, Links/Rechts, Tiefe | OLED 1,3″: dB, Balken, Stromrichtungspfeil |
| Handgerät: Ton | Verstärker + Lautsprecher | Piezo (Tonhöhe steigt mit dem Signal) |
| Handgerät: Bedienung | Drehgeber + Tasten | 2 Tasten: REF und MODUS |
| Aufbau | 2 eigene Platinen | 2 Lochrasterplatinen, Module gesteckt, nur bedrahtete Teile |

## 3. Was du verlierst und wie du es ersetzt

| Fällt weg | So geht es ohne |
|---|---|
| Links/Rechts-Pfeil, Null, Tiefe | Gerät quer zur Leitung schwenken: lautester Punkt = Leitung (wie bei jedem normalen Suchgerät) |
| E-Feld-Spitze (Ader am fernen Ende finden) | Multimeter: Ader am Anfang auf Erde legen, am Ende durchmessen |
| Durchgangsprüfung im Sender | Multimeter |
| Anzeige in nT, Längenschätzung | dB-Wert und Balken im Handgerät; Strom in mA am Sender |
| Frequenzen „Tief“ und „Hoch“ | nur Standard; weitere Frequenzen kann man später per Software nachrüsten |
| Fremdspannungssperre | Schutz bleibt; du misst vor dem Anklemmen ohnehin auf Spannungsfreiheit |

Die Stromrichtung auf vollen Trassen (U-13) bleibt erhalten. Sie braucht nur eine Spule und ist reine Software.

## 4. Kosten (Schätzung, Preise wie im Konzept Kap. 11)

| Posten | Sender | Handgerät |
|---|---|---|
| Mikrocontroller | ESP32-S2 mini ≈ 3 € (hast du schon) | ESP32-S3 SuperMini ≈ 3,50 € |
| Anzeige | OLED 1,3″ ≈ 3 € | OLED 1,3″ ≈ 3 € |
| Endstufe / Verstärker | TL072, BD139/BD140, 24-V-Wandler ≈ 2,50 € | MCP6002 ≈ 0,50 € |
| Sensor | – | Ferritstab + Kupferlackdraht ≈ 2 € |
| Schutz und Messung | PTC, Klemmdioden, TVS, 47 Ω, Kondensatoren, Messwiderstand ≈ 1,80 € | – |
| Ton | – | Piezo ≈ 0,30 € |
| Messleitungen mit Krokoklemmen | ≈ 2,50 € | – |
| Batteriehalter, Schalter, Tasten | ≈ 1,80 € | ≈ 2,10 € |
| Kleinteile, Stiftleisten | ≈ 2 € | ≈ 1,50 € |
| **Summe Teile** | **≈ 16 €** | **≈ 13 €** |
| Lochrasterplatinen (2 Stück, 2,54 mm) | ≈ 2 € | |
| **Gesamt pro Satz** | **≈ 31 €** (du zahlst ≈ 28 €) | |

Filament ist nicht eingerechnet. Mit bestellten Platinen wären es ca. 38 € (Platinen ca. 9 € statt 2 €). Das
Sender-Display kostet ca. 3 € mehr als die zuerst vorgeschlagenen LEDs.

## 5. Eine Platine? (Entscheidung: Lochraster, E-042)

Julius hat sich für **Lochraster** entschieden. Die Überlegungen von vorher bleiben hier zum Nachlesen:

- **Je Gerät eine Platine:** Alles sitzt auf einer Platine: ESP32-Modul und OLED auf
  Stiftleisten, dazu die wenigen bedrahteten Bauteile. Batterie und Spule kommen über Stecker. Keine fliegende
  Verdrahtung, ca. 30 Lötstellen plus Stiftleisten pro Gerät.
- **Beide Platinen in einer Bestellung:** ca. 9 € inkl. Versand für je 5 Stück. Die übrigen Platinen sind Ersatz.
- **Beide Geräte auf einem Platinenstück** zum Auseinanderbrechen geht technisch, bringt aber nichts: Der Hersteller
  berechnet mehrere Designs auf einem Stück oft extra.
- **Fertig bestückt vom Hersteller (SMD):** lohnt sich erst ab ca. 10 Sätzen. Bei einem Satz kostet die Bestückung
  mehr als alle Teile zusammen.
- **Lochraster** spart ca. 7 €, kostet aber mehr Zeit und ist fehleranfälliger. Dafür ist eine Änderung nach dem
  Test schnell gelötet. Damit das Verdrahten sicher klappt, gibt es in Phase 5 einen Aufbauplan zum Abhaken.

## 6. Ablauf

Der große Steckbrett-Test mit eigener Einkaufsliste (ca. 57 €) entfällt. Stattdessen:

1. Die endgültigen Teile einmal bestellen (≈ 26 € Teile ohne den vorhandenen ESP32-S2, dazu Packungsreste und
   Steckbrett-Material; die Lochrasterplatinen kosten ca. 2 €).
2. Kurzer Test auf dem Steckbrett mit genau diesen Teilen, ein Abend:
   - Sender: sauberer Sinus an A/E?
   - Handgerät: reicht der Wandler im ESP32-S3 (Empfindlichkeit ca. 0,1 nT, sauberes Abtasten)?
   - Stromrichtung mit einer Spule an einer Testschleife.
3. Danach Phase 4 (Schaltung) und Phase 5 (Lochraster-Aufbauplan). Die Teile wandern vom Steckbrett auf die
   Lochrasterplatine.

## 7. Entscheidungen (beantwortet am 13.10.2026)

| # | Frage | Empfehlung | Julius |
|---|---|---|---|
| V-1 | Variante „Einfach“ statt C? | ja | **ja** |
| V-2 | Sender ohne Display, nur 3 LEDs? | LEDs | **nein: OLED in beiden Geräten** |
| V-3 | Platine bestellen oder Lochraster? | Platine | **Lochraster** (erst „Platine“, kurz danach geändert, E-042) |
| V-4 | Platz für eine zweite Spule vorsehen, erstmal nicht bestücken? | ja | **ja** |

Eingetragen als E-034 bis E-042 in [`entscheidungen.md`](entscheidungen.md).
