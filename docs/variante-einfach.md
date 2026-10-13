# Vorschlag: Variante „Einfach“

**Stand:** 13.10.2026 · **Status:** Vorschlag, wartet auf Julius' Freigabe

Anlass: Julius findet Variante C (ca. 50 € pro Satz, Phase-3-Einkauf ca. 57 €) zu teuer und zu aufwendig. Er möchte
ein ganz normales, einfaches Kabelsuchgerät mit möglichst wenig Aufwand. Außerdem soll die Abtastfläche des
Handgeräts oben am Kopf sitzen (siehe `gehaeuse/konzept/`).

## 1. Grundgedanke

Die Reichweite hängt vor allem an drei Dingen: wie eingespeist wird (alle Adern + Schirm per WAGO gegen Erde),
wie viel Strom fließt und wie empfindlich die Spule ist. Das kostet fast nichts und bleibt. Teuer und aufwendig
waren die Extras: 24-bit-Wandler, zweite Spule, E-Feld-Spitze, Display und Messschaltung im Sender,
Durchgangsprüfung, drei Frequenzsätze.

Die Variante „Einfach“ ist die Hardware von Variante B (eine Spule, Wandler im ESP32) mit der fertigen Software
von Variante C (Zweiton, Lock-in, Stromrichtung). Der Sender verliert Display, Relais und Messschaltung.

## 2. Was bleibt, was fliegt

| Teil | Variante C (bisher) | Variante „Einfach“ |
|---|---|---|
| Einspeisung | Modus G, A, P, umschaltbar | nur zwei Leitungen A und E; Modus G/A ist nur Anklemmen, keine Umschaltung |
| Signal | Zweiton, 3 Frequenzsätze | Zweiton, nur Standard (8,3 + 4,2 kHz) |
| Sender: Endstufe | TL072 + BD139/BD140 an 24 V | **bleibt** |
| Sender: Anzeige | OLED 1,3″ | 3 LEDs: EIN, STROM, BATT |
| Sender: Messen | Strom, Lastbild, Länge, Durchgang, Fremdspannungssperre | nur STROM-LED (fließt Strom: ja/nein) |
| Sender: Schutz | PTC + Schutzdiode + Relais | PTC + Schutzdiode |
| Handgerät: Rechner | ESP32-S3 SuperMini | **bleibt** (hat Gleitkomma-Einheit, `lockin.h` läuft ohne Umbau) |
| Handgerät: Wandler | PCM1808 (24 bit) + CD4053 | Wandler im ESP32-S3, zwei Verstärkerstufen auf zwei Eingängen (automatische Bereichswahl) |
| Handgerät: Sensoren | 2 Spulen + E-Feld-Spitze | 1 Spule im Kopf, direkt unter der Abtastfläche |
| Handgerät: Anzeige | OLED 1,3″, nT, Links/Rechts, Tiefe | OLED 1,3″: dB, Balken, Stromrichtungspfeil |
| Handgerät: Ton | Verstärker + Lautsprecher | Piezo (Tonhöhe steigt mit dem Signal) |
| Handgerät: Bedienung | Drehgeber + Tasten | 2 Tasten: REF und MODUS |
| Aufbau | 2 eigene Platinen | 2 eigene Platinen, Module gesteckt, nur bedrahtete Teile |

## 3. Was du verlierst und wie du es ersetzt

| Fällt weg | So geht es ohne |
|---|---|
| Links/Rechts-Pfeil, Null, Tiefe | Gerät quer zur Leitung schwenken: lautester Punkt = Leitung (wie bei jedem normalen Suchgerät) |
| E-Feld-Spitze (Ader am fernen Ende finden) | Multimeter: Ader am Anfang auf Erde legen, am Ende durchmessen |
| Durchgangsprüfung im Sender | Multimeter |
| Anzeige in nT und mA, Längenschätzung | dB-Wert und Balken; STROM-LED am Sender |
| Frequenzen „Tief“ und „Hoch“ | nur Standard; weitere Frequenzen kann man später per Software nachrüsten |
| Fremdspannungssperre | Schutz bleibt; du misst vor dem Anklemmen ohnehin auf Spannungsfreiheit |

Die Stromrichtung auf vollen Trassen (U-13) bleibt erhalten. Sie braucht nur eine Spule und ist reine Software.

## 4. Kosten (Schätzung, Preise wie im Konzept Kap. 11)

| Posten | Sender | Handgerät |
|---|---|---|
| Mikrocontroller | ESP32-S2 mini ≈ 3 € (hast du schon) | ESP32-S3 SuperMini ≈ 3,50 € |
| Anzeige | 3 LEDs ≈ 0,30 € | OLED 1,3″ ≈ 3 € |
| Endstufe / Verstärker | TL072, BD139/BD140, 24-V-Wandler ≈ 2,50 € | MCP6002 ≈ 0,50 € |
| Sensor | – | Ferritstab + Kupferlackdraht ≈ 2 € |
| Schutz | PTC, Schutzdiode, 47 Ω, Kondensator ≈ 1 € | – |
| Ton | – | Piezo ≈ 0,30 € |
| Messleitungen mit Krokoklemmen | ≈ 2,50 € | – |
| Batteriehalter, Schalter, Tasten | ≈ 1,80 € | ≈ 2,10 € |
| Kleinteile, Stiftleisten | ≈ 2 € | ≈ 1,50 € |
| **Summe Teile** | **≈ 13 €** | **≈ 13 €** |
| Platinen (beide Designs in einer Bestellung, 5 Stück je Design) | ≈ 9 € | |
| **Gesamt pro Satz** | **≈ 35 €** (du zahlst ≈ 32 €) | |

Mit Lochrasterplatine statt bestellter Platine: ≈ 28 €. Filament ist nicht eingerechnet.

## 5. Eine Platine?

- **Je Gerät eine Platine: ja, das ist der Plan.** Alles sitzt auf einer Platine: ESP32-Modul und OLED auf
  Stiftleisten, dazu die wenigen bedrahteten Bauteile. Batterie und Spule kommen über Stecker. Keine fliegende
  Verdrahtung, ca. 30 Lötstellen plus Stiftleisten pro Gerät.
- **Beide Platinen in einer Bestellung:** ca. 9 € inkl. Versand für je 5 Stück. Die übrigen Platinen sind Ersatz.
- **Beide Geräte auf einem Platinenstück** zum Auseinanderbrechen geht technisch, bringt aber nichts: Der Hersteller
  berechnet mehrere Designs auf einem Stück oft extra.
- **Fertig bestückt vom Hersteller (SMD):** lohnt sich erst ab ca. 10 Sätzen. Bei einem Satz kostet die Bestückung
  mehr als alle Teile zusammen.
- **Lochraster** spart ca. 7 €, kostet aber deutlich mehr Zeit und ist fehleranfälliger.

## 6. Ablauf

Der große Steckbrett-Test mit eigener Einkaufsliste (ca. 57 €) entfällt. Stattdessen:

1. Die endgültigen Teile einmal bestellen (≈ 26 € Teile, später kommen die Platinen dazu).
2. Kurzer Test auf dem Steckbrett mit genau diesen Teilen, ein Abend:
   - Sender: sauberer Sinus an A/E?
   - Handgerät: reicht der Wandler im ESP32-S3 (Empfindlichkeit ca. 0,1 nT, sauberes Abtasten)?
   - Stromrichtung mit einer Spule an einer Testschleife.
3. Danach Phase 4 (Schaltung) und Phase 5 (Platine). Die Teile wandern vom Steckbrett auf die Platine.

## 7. Offene Entscheidungen (mit Empfehlung)

| # | Frage | Empfehlung |
|---|---|---|
| V-1 | Variante „Einfach“ statt C? | ja |
| V-2 | Sender ohne Display, nur 3 LEDs? (mit OLED ca. +3 €) | ja, LEDs |
| V-3 | Platine bestellen oder Lochraster? | Platine |
| V-4 | Platz für eine zweite Spule auf der Platine vorsehen, erstmal nicht bestücken? (später Links/Rechts möglich) | ja |
