# Machbarkeitstest (Phase 3) – Mini-Test Variante „Einfach“

| | |
|---|---|
| **Version** | 2.0 |
| **Stand** | 13.10.2026 |
| **Grundlage** | Variante „Einfach“, freigegeben am 13.10.2026 ([`variante-einfach.md`](variante-einfach.md), E-034 bis E-041) |
| **Visuelle Übersicht** | [`machbarkeitstest-uebersicht.html`](machbarkeitstest-uebersicht.html) (Schaltpläne, Link siehe README) |
| **Protokoll zum Ausfüllen** | [`messprotokoll-phase3.md`](messprotokoll-phase3.md) |

---

## 0. Worum es geht

Du kaufst die Teile für **einen Satz** einmal, steckst beide Geräte auf dem Steckbrett zusammen und prüfst
die Punkte, die neu oder unsicher sind. Danach wandern dieselben Teile auf die Lochrasterplatine. Es gibt keinen
eigenen Testeinkauf mehr (E-041).

| Nr. | Frage | Messung | Pflicht? |
|---|---|---|---|
| 1 | Arbeitet die Endstufe sauber und stabil, stimmt die Strommessung, greift die Strombegrenzung? | M1 | ja |
| 2 | Läuft der eingebaute Wandler des ESP32-S3 sauber (Abtastrate, Rauschen, keine Aussetzer)? | M2 | ja |
| 3 | Wie empfindlich ist die Spule, hilft der Schirm, schaltet die Bereichswahl sauber um? | M3 | ja |
| 4 | Wie weit muss die Spule vom Digitalteil weg? Stört der Piezo? | M4 | wenn möglich |
| 5 | Wie weit reicht die Ortung an einer freien Leitung? | M5 | ja |
| 6 | Wand, Unterputz, Trasse, Vergleich mit dem IP-Kamera-Tester | M6 | wenn möglich |
| 7 | Wie zuverlässig ist die Stromrichtung mit einer Spule? | M7 | ja |
| 8 | Wie gut werden Störquellen unterdrückt? | M8 | wenn möglich |

**Zeitbedarf:** ein Abend Aufbau, ein Abend M1–M4 am Tisch, M5–M8 dann, wenn du an einer passenden Leitung
bist.

**Was sich gegenüber Version 1.0 geändert hat:** kein PCM1808, keine zweite Spule, keine E-Spitze, kein
Zangen-Modus. Der Sender hat jetzt Batterie, Display, Strommessung und Schutzdioden. Den Strom liest du
direkt am Sender ab.

---

## 1. Sicherheit und Masse – bitte zuerst lesen

1. **Nur an spannungsfreien Leitungen** und am Testaufbau.
2. **Der Sender muss „schwimmen“.** Die Endstufe ist eine Brückenschaltung: Beide Ausgänge A und E
   schwingen gegen die Schaltungsmasse. Deshalb:
   - Sender aus seinen **eigenen Batterien** betreiben. Zum Programmieren USB anstecken, dabei Schalter S1
     **AUS** (sonst speist die Batterie in den Laptop) und nichts an A/E angeschlossen.
   - **Oszilloskop nur auf dem Tisch** (M1) an den Sender klemmen, mit Masse an Sender-GND, und nur, wenn A
     und E an nichts Geerdetem hängen (nur Testlast oder Kalibrierschleife).
   - Im Feld (Klemme an PE) **kein Oszilloskop und kein Laptop am Sender**. Den Strom zeigt das Display.
3. Am Sender liegen 24 V Gleichspannung und bis ca. 17 V Spitze an den Klemmen. Das ist Schutzkleinspannung und
   ungefährlich, trotzdem: erst verdrahten, dann einschalten.
4. **Pegel MAX nur in Modus G** (gegen Erde). Hängt in Modus A ein Gerät am Ende, höchstens NORMAL.
5. Das Handgerät darf am Laptop hängen (S1 AUS, Versorgung dann über USB). Für Rauschmessungen den Laptop besser
   auf Akku betreiben.

---

## 2. Einkaufsliste (ein Satz)

Preise geschätzt (AliExpress ≈ 2–3 Wochen Lieferzeit, Reichelt ≈ 2–3 Tage). Alles landet später im Gerät,
außer den markierten Reserve- und Steckbrettteilen.

### 2.1 Module und ICs

| Teil | Menge | Bezug | ca. € |
|---|---|---|---|
| ESP32-S3 SuperMini (mit Stiftleisten) | 1 | AliExpress | 3,50 |
| OLED 1,3″ SH1106, I²C, 4 Pins | 2 | AliExpress | 6,00 |
| MT3608-Step-up-Modul (5er-Pack) | 1 Pack | AliExpress | 2,00 |
| TL072 (DIP-8) | 1 | Reichelt | 0,40 |
| MCP6002-I/P (DIP-8) | 2 (1 Reserve) | Reichelt | 1,00 |
| BD139 + BD140 | je 2 | Reichelt | 1,00 |
| ESP32-S2 mini | 1 | vorhanden | – |

### 2.2 Spule

| Teil | Menge | Bezug | ca. € |
|---|---|---|---|
| Ferritstab Ø 10 × 80 mm | 2 (1 Reserve) | AliExpress („ferrite rod 10x80“) | 2,00 |
| Kupferlackdraht 0,15 mm, **lötbar** (Polyurethan), ca. 50 g | 1 | AliExpress / Amazon | 4,00 |
| Kupferfolienband 10 mm, **leitfähiger Kleber** | 1 Rolle | AliExpress / Amazon | 3,00 |
| Kaptonband (oder Isolierband) | 1 | vorhanden? | – |

### 2.3 Bauteile (Reichelt oder Sortiment)

| Teil | Menge |
|---|---|
| Metallschicht 0,25 W: 10 Ω ×2, 100 Ω ×3, 1 kΩ ×2, 2,2 kΩ ×2, 3,3 kΩ ×1, 10 kΩ ×9, 18 kΩ ×1, 22 kΩ ×1, 47 kΩ ×3, 100 kΩ ×3, 390 kΩ ×1 | |
| 47 Ω 2 W ×2 · 1 kΩ 2 W ×1 (Testlast und Kalibrierwiderstand) | |
| Keramik **C0G**: 22 pF ×1, 100 pF ×2 · Keramik oder Folie: 1 nF ×2, 2,2 nF ×2 | |
| Folie: 47 nF ×2, 1 µF ×1 · Keramik 100 nF ×6 (eins davon als Testlast) | |
| Elko: 4,7 µF/16 V ×1, 10 µF/25 V ×2, 47 µF/25 V ×1, 100 µF/10 V ×1, 220 µF/35 V ×1 | |
| 10 µF **bipolar** (BP/NP) **63 V** ×2 – oder je zwei 22 µF/63 V antiseriell (Minus an Minus) | |
| Dioden: 1N5817 ×2, 1N4148 ×4, TVS **P6KE30A** ×1 | |
| Rückstellende Sicherung (PTC) 0,1 A Haltestrom, ≥ 60 V (z. B. Bourns MF-R010) ×2 | |

Sortiment-Preis ca. 6–8 €.

### 2.4 Bedienteile, Batterie, Aufbau

| Teil | Menge | ca. € |
|---|---|---|
| Batteriehalter 4 × AA mit Kabel | 2 | 1,50 |
| Schiebeschalter (für Leiterplatte oder Gehäuse) | 2 | 0,60 |
| Taster 6 × 6 mm | 3 | 0,30 |
| Piezo-Summer **passiv** (ohne eingebaute Elektronik), 12 mm | 1 | 0,30 |
| Silikonlitze 0,5 mm², rot und grün-gelb, je 1 m | 1 | 2,00 |
| Isolierte Krokodilklemmen rot und grün (oder gelb) | je 1 | 1,00 |
| Lochrasterplatine einseitig, 2,54 mm, Lötaugen, ca. 50 × 70 mm (5er-Pack) | 1 Pack | 2,00 |
| Stiftleisten und Buchsenleisten 2,54 mm (für Module und Stecker) | je 1 Streifen | 1,00 |
| Steckbrett 830 Kontakte (Steckbrett-Material) | 2 | 4,00 |
| Steckbrücken-Set (Steckbrett-Material) | 1 | 3,00 |
| WAGO 221-415, Elektro-Isolierrohr M20 (1 m), J-Y(St)Y-Reststück 2×2×0,6 (1,5 m), Kabelbinder | vorhanden | – |

**Summe ca. 46 €.** Davon landen ca. 28 € später im Gerät. Der Rest sind Packungsreste (Draht,
Kupferfolie, MT3608-Fünferpack, Lochraster-Fünferpack), Reserveteile und das Steckbrett-Material. Die
Lochrasterplatinen kommen gleich mit in die Bestellung, gelötet wird erst nach dem Test.

**Tipp:** Die AliExpress-Teile jetzt bestellen. In der Wartezeit kannst du den Spulenkörper drucken und die
Spule wickeln.

---

## 3. Druckteile

| Teil | Datei | Hinweis |
|---|---|---|
| Spulenkörper (Flansch 22 mm) | `hardware/handgeraet/spule/spulenkoerper_H.stl` | stehend, 3 Wände |
| Wickeldorn (1/4″-Sechskant für den Akkuschrauber) | `hardware/handgeraet/spule/wickeldorn.stl` | stehend, **Brim an**, 100 % Füllung |
| Messkopf (hält die Spule, nimmt das M20-Rohr als Griff auf) | `hardware/handgeraet/testaufbau/messkopf.stl` | flach |

Alle Teile: PETG, 0,4-mm-Düse, 0,2 mm Schicht, **kein Stützmaterial**. Den Spulenkörper V brauchst du nicht mehr.

---

## 4. Spule wickeln

1. **Spulenkörper auf den Wickeldorn** schieben (stramm, bis zur Schulter), Dorn ins Bohrfutter.
2. **Drahtanfang** durch den Schlitz im unteren Flansch fädeln, 15 cm überstehen lassen und mit Klebeband am
   Dorn fixieren. Das ist der **Anfang**, mit Edding markieren.
3. **Wickeln** auf kleinster Stufe. Den Draht durch ein Tuch zwischen Daumen und Zeigefinger laufen lassen
   (leichte Spannung) und gleichmäßig hin und her führen. Ziel: **ca. 1000 Windungen**, das sind 4–5 Lagen
   bzw. etwa 1 mm Wicklungshöhe. Zählhilfe: Klebefähnchen am Bohrfutter, in Zehnern zählen (Strichliste).
   Exakt muss es nicht sein, die Spule wird in M3 kalibriert.
4. **Ende** durch den oberen Schlitz führen, 15 cm überstehen lassen.
5. **Prüfen:** Drahtenden 1 cm verzinnen (lötbarer Lack schmilzt bei ca. 380 °C im Zinn), Widerstand messen.
   **Windungszahl ≈ 25 × Widerstand in Ω** (0,15-mm-Draht, Ø 12 mm). 35–45 Ω sind richtig.
6. **Isolieren:** eine Lage Kaptonband über die Wicklung.
7. **Schirm:** Kupferfolienband längs über die Wicklung kleben, **ringsum, aber mit 2 mm Spalt** in
   Längsrichtung. Ein geschlossener Ring wäre eine Kurzschlusswindung und würde das Signal schlucken. An einer
   Stelle eine dünne Litze als **Schirmanschluss** anlöten.
8. **Ferritstab** mittig einschieben, mit einem Tropfen Heißkleber sichern.
9. **Anschluss:** J-Y(St)Y-Reststück (1,5 m) als Spulenkabel, ein Paar an die Spule. Spulenschirm und
   Kabelschirm am Steckbrett an GND. Notieren, welche Ader an „Anfang“ hängt.

**Messkopf:** Spule waagerecht mit Kabelbindern unter die Platte, M20-Rohr als Griff in die Hülse, Kabel am Rohr
festkleben. Die Spule liegt damit wie später im Gerät: quer, parallel zur Abtastfläche.

---

## 5. Aufbau auf dem Steckbrett

Die Schaltpläne findest du als Zeichnung in der visuellen Übersicht und unter
`hardware/sender/testaufbau/sender_steckbrett.svg` bzw. `hardware/handgeraet/testaufbau/empfaenger_steckbrett.svg`.
Hier stehen sie als Verdrahtungsliste, Zeile für Zeile abzuhaken.

### 5.1 Sender (ESP32-S2 mini, TL072, BD139/BD140, OLED)

**Reihenfolge:** zuerst nur Batterie, Schalter, D1, S2 mini und MT3608 verbinden. Den MT3608 **mit dem
Multimeter auf 24,0 V einstellen** (Mehrgang-Poti, die Drehrichtung ist je nach Modul unterschiedlich). Erst
dann den Rest aufbauen.

| # | Von | Über | Nach | Zweck |
|---|---|---|---|---|
| S1 | Batterie + | Schalter S1, dann D1 1N5817 (Ring zur Schiene) | Schiene **VBAT** | Versorgung, Verpolschutz |
| S2 | Batterie − | – | Schiene **GND** | Masse |
| S3 | VBAT | – | S2 mini **VBUS** (5-V-Pin) | Versorgung Controller |
| S4 | S2 mini GND | – | GND | |
| S5 | VBAT / GND | – | MT3608 VIN+ / VIN− | Hochsetzsteller |
| S6 | MT3608 VOUT+ | – | Schiene **+24 V** | Endstufe |
| S7 | +24 V | C1 220 µF/35 V (+), C2 100 nF, D6 P6KE30A (Ring an +24 V) | GND | Puffer, Überspannungsschutz |
| S8 | +24 V | R1 10 kΩ | Knoten **VMID** | Mittenspannung |
| S9 | VMID | R2 10 kΩ und C3 47 µF (+ an VMID) | GND | ergibt 12 V |
| S10 | S2 mini GPIO18 (DAC) | R3 2,2 kΩ | Knoten F1 | Tiefpass Stufe 1 |
| S11 | F1 | C4 2,2 nF | GND | |
| S12 | F1 | R4 2,2 kΩ | Knoten F2 (TP2) | Tiefpass Stufe 2 |
| S13 | F2 | C5 2,2 nF | GND | |
| S14 | F2 | C6 1 µF Folie | U1 Pin 3 (IN+ A) | Koppelkondensator |
| S15 | U1 Pin 3 | R5 100 kΩ | VMID | Arbeitspunkt 12 V |
| S16 | U1 Pin 8 / Pin 4 | – | +24 V / GND | TL072 Versorgung |
| S17 | U1 Pin 1 (OUT A) | R6 100 Ω | Basis Q1 (BD139) und Basis Q2 (BD140) | Treiber A |
| S18 | Q1 Kollektor / Q2 Kollektor | – | +24 V / **PGND** (nicht GND!) | |
| S19 | Q1 Emitter + Q2 Emitter | – | Knoten **OA** (TP3) | Ausgang A |
| S20 | OA | R7 18 kΩ | U1 Pin 2 (IN− A) | Gegenkopplung, Verstärkung 6,45 |
| S21 | U1 Pin 2 | R8 3,3 kΩ, dann C7 10 µF (+ zu R8) | GND | |
| S22 | OA | R9 10 kΩ | U1 Pin 6 (IN− B) | invertierender Zweig |
| S23 | U1 Pin 5 (IN+ B) | – | VMID | |
| S24 | U1 Pin 7 (OUT B) | R11 100 Ω | Basis Q3 (BD139) und Basis Q4 (BD140) | Treiber B |
| S25 | Q3 Kollektor / Q4 Kollektor | – | +24 V / **PGND** | |
| S26 | Q3 Emitter + Q4 Emitter | – | Knoten **OB** (TP4) | Ausgang B |
| S27 | OB | R10 10 kΩ | U1 Pin 6 | Verstärkung −1 |
| S28 | OA | C8 10 µF bipolar, R12 47 Ω 2 W, PTC1 | **Klemme A** (rot) | Ausgang A |
| S29 | OB | C9 10 µF bipolar, R13 47 Ω 2 W, PTC2 | **Klemme E** (grün-gelb) | Ausgang E |
| S30 | OA | D2 1N4148 (Ring an +24 V) | +24 V | Klemmdiode |
| S31 | GND | D3 1N4148 (Ring an OA) | OA | Klemmdiode |
| S32 | OB | D4 / D5 wie S30 / S31 | +24 V / GND | Klemmdioden |
| S33 | PGND (TP5) | R14 10 Ω | GND | Messwiderstand |
| S34 | PGND | R15 10 kΩ | Knoten **I_MESS** → GPIO3 | Strommessung |
| S35 | I_MESS | C10 4,7 µF (+ an I_MESS) | GND | glättet |
| S36 | VBAT | R16 100 kΩ | Knoten **BATT** → GPIO5 | Batteriemessung |
| S37 | BATT | R17 47 kΩ und C11 100 nF | GND | |
| S38 | GPIO7 | Taster T1 PEGEL | GND | Bedienung |
| S39 | OLED VCC / GND / SDA / SCL | – | S2 mini 3V3 / GND / GPIO33 / GPIO35 | Anzeige |

BD139/BD140-Anschlüsse (Schrift zu dir, Beinchen nach unten): **E – C – B** von links nach rechts.
TL072: Pin 1 links oben neben der Kerbe/dem Punkt, gegen den Uhrzeigersinn weiterzählen.

**Messpunkte Sender:** TP1 = GPIO18 · TP2 = F2 · TP3 = OA · TP4 = OB · TP5 = PGND (Mittelwert des Stroms =
Spannung an TP5 ÷ 10 Ω).

### 5.2 Handgerät (ESP32-S3 SuperMini, MCP6002, Spule, OLED)

| # | Von | Über | Nach | Zweck |
|---|---|---|---|---|
| E1 | Batterie + | Schalter S1, dann D1 1N5817 (Ring zum Modul) | S3 **5V**-Pin | Versorgung |
| E2 | Batterie − | – | GND | |
| E3 | 5V-Pin | R1 100 kΩ | Knoten **BATT** → GPIO4 | Batteriemessung |
| E4 | BATT | R2 47 kΩ und C1 100 nF | GND | |
| E5 | S3 **3V3** | R3 10 Ω | Schiene **A3V3** | entkoppelt den Analogteil |
| E6 | A3V3 | C2 100 µF (+), C3 100 nF | GND | |
| E7 | A3V3 | R4 10 kΩ | Knoten **VREF** | |
| E8 | VREF | R5 10 kΩ und C4 10 µF (+ an VREF) | GND | 1,65 V Bezug |
| E9 | U1 Pin 8 / Pin 4 | – | A3V3 / GND; C5 100 nF direkt von Pin 8 nach GND | MCP6002 Versorgung |
| E10 | Spule „Anfang“ | – | VREF | |
| E11 | Spule „Ende“ | R6 22 kΩ | Spule „Anfang“ | dämpft die Spulenresonanz |
| E12 | Spule „Ende“ | R7 1 kΩ | U1 Pin 3 (IN+ A) | |
| E13 | U1 Pin 3 | C6 100 pF | VREF | HF-Filter |
| E14 | U1 Pin 1 (OUT A) | R8 47 kΩ ∥ C7 100 pF | U1 Pin 2 | Stufe 1: ×5,7 |
| E15 | U1 Pin 2 | R9 10 kΩ, dann C8 47 nF | VREF | Hochpass 340 Hz |
| E16 | U1 Pin 1 | – | U1 Pin 5 (IN+ B) | Stufe 1 → Stufe 2 |
| E17 | U1 Pin 7 (OUT B) | R11 390 kΩ ∥ C10 22 pF | U1 Pin 6 | Stufe 2: ×40 |
| E18 | U1 Pin 6 | R12 10 kΩ, dann C11 47 nF | VREF | |
| E19 | U1 Pin 1 | R10 1 kΩ | **GPIO1** (NIEDRIG); dort C9 1 nF nach GND | Wandlereingang |
| E20 | U1 Pin 7 | R13 1 kΩ | **GPIO2** (HOCH); dort C12 1 nF nach GND | Wandlereingang |
| E21 | OLED VCC / GND / SDA / SCL | – | S3 3V3 / GND / GPIO8 / GPIO9 | Anzeige |
| E22 | GPIO10 / GPIO11 | Taster T1 REF / T2 MODUS | GND | Bedienung |
| E23 | GPIO12 | R14 100 Ω, Piezo BZ1 | GND | Ton |
| E24 | Spulenschirm, Kabelschirm | – | GND | nur hier auflegen |

GPIO5 und GPIO6 bleiben frei (Reserve für eine zweite Spule).

**Messpunkte Handgerät:** TPE1 = VREF (1,65 V) · TPE2 = U1 Pin 1 (NIEDRIG) · TPE3 = U1 Pin 7 (HOCH) ·
TPE4 = BATT.

---

## 6. Software

1. **Arduino IDE 2** installieren, unter *Einstellungen → Zusätzliche Boardverwalter-URLs*
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json` eintragen, dann im Boardverwalter
   **„esp32“ von Espressif** installieren (**Version 3.x**, ältere 2.x gehen nicht).
2. Bibliotheksverwalter: **„U8g2“** von olikraus installieren (für die OLEDs).
3. **Sender:** `firmware/machbarkeit/sender_test/sender_test.ino` öffnen. Board **„LOLIN S2 Mini“**,
   *USB CDC On Boot: Enabled*. Erstes Hochladen: Taste **0** halten, **RST** kurz drücken, **0** loslassen,
   hochladen, danach RST drücken.
4. **Handgerät:** `firmware/machbarkeit/empfaenger_test/empfaenger_test.ino` öffnen. Board
   **„ESP32S3 Dev Module“**, *USB CDC On Boot: Enabled*. Bei Problemen beim Hochladen: BOOT halten, RST drücken,
   BOOT loslassen.
5. **Serieller Monitor / Plotter** auf 115200 Baud. Befehle tippst du in die Eingabezeile und schickst sie mit
   Enter. Beim Start misst das Handgerät 2 s lang seine Abtastrate und schreibt sie in den Monitor.

| Handgerät | Taste | Befehl | Wirkung |
|---|---|---|---|
| Frequenzsuche | REF lang | `s` | Gerät dabei nah an die Leitung halten |
| Referenz | REF kurz | `r` | Referenz für die Stromrichtung setzen (nahe am Sender, über der gesuchten Leitung) |
| Empfindlichkeit | MODUS kurz | `e` | AUTO → E1 (empfindlich) … E5 (grob) |
| Ton | MODUS lang | `m` | laut → leise → aus |
| Glättung | – | `t` | schnell (2 × 0,06 s) / ruhig (2 × 0,3 s) |
| Bereich | – | `b` | AUTO / nur NIEDRIG / nur HOCH (für M3) |
| Kalibrieren | – | `c10 10` | 10 mA im Draht, Spulenmitte 10 cm entfernt |
| sonst | – | `k17.3` `a` `v` `?` | Kalibrierfaktor setzen / AFC an-aus / ausführliche Zeile / Hilfe |

| Sender | Taste | Befehl | Wirkung |
|---|---|---|---|
| Pegel | PEGEL kurz | `1` `2` `3` | FEIN / NORMAL / MAX (ca. 5 / 9 / 17 V Spitze) |
| Ausgang | PEGEL lang | `o` | aus / ein |
| nur Ortungston | – | `c` | ohne Richtungston (reiner Sinus 8333 Hz) |
| Prüfton | – | `p` | 1041,7 Hz reiner Sinus, zum Nachmessen mit dem Multimeter |

**Plotter-Kurven Handgerät** (dB bezogen auf 1 µV an der Spule): `Pegel` = Ortungston, `Rauschen` = Störpegel
neben der Frequenz, `CD` = Richtungston, `Richtung` = +20 HIN / −20 ZURÜCK / 0 unklar, `Bereich` = 30 HOCH /
10 NIEDRIG. **Abstand Pegel – Rauschen = SNR.** Ab 10 dB gilt die Leitung als erkannt. Die SNR-Anzeige sättigt
bei ca. 60 dB; das ist normal.

**Stromrichtung mit einer Spule:** Der Pfeil hängt davon ab, wie herum du das Gerät hältst. Regel: **Display
immer zu dir**, beim Referenz-Setzen und beim Suchen gleich halten.

Die Signalverarbeitung (`lockin.h`) ist auf dem PC mit einem Modell der ganzen Kette getestet (Spule,
Verstärker mit Rauschen, 12-bit-Wandler mit Rauschen und Anschlag, `firmware/machbarkeit/test_host/build.sh`):
Frequenzsuche auf 0,01 Hz, Pegel auf 0,1 %, Bereichswechsel ohne Pegelsprung, Stromrichtung richtig, bei 0,1 nT
noch 44 dB Abstand zum Elektronikrauschen. Kompilieren für den ESP32 konnte ich hier nicht; falls die Arduino IDE
einen Fehler meldet, schick mir die Meldung.

---

## 7. Messprogramm

Alle Ergebnisse trägst du in [`messprotokoll-phase3.md`](messprotokoll-phase3.md) ein (oder schickst mir Fotos
und Screenshots vom Plotter).

### M1 – Sender auf dem Tisch (Pflicht)

**Aufbau:** Sender aus seinen Batterien, Last **1 kΩ 2 W** zwischen A und E, sonst nichts angeschlossen.
Oszi-Masse an Sender-GND.

| Schritt | Messpunkt | Einstellung | Erwartung |
|---|---|---|---|
| a | +24 V, VMID | Multimeter | 24,0 V · 12,0 V ±0,3 V |
| b | TP3, TP4 | Ausgang aus (PEGEL lang) | je ca. 12 V Gleichspannung |
| c | TP1 (DAC) | NORMAL | Periode 240 µs, zwei ungleiche Buckel je Periode, ca. 1,6 Vss |
| d | TP3 und TP4 gleichzeitig | MAX | gegenphasig, je ca. 17 Vss, **keine abgeflachten Spitzen** |
| e | TP3, Zeitbasis 1 µs/div | Last 100 nF statt 1 kΩ, MAX | sauber, **keine Schwingung** (kein „Gras“ auf der Kurve) |
| f | Strom | Prüfton (`p`), NORMAL, Last 1 kΩ | Multimeter (AC) über dem 1-kΩ-Widerstand: Strom = Spannung ÷ 1 kΩ, ca. 7 mA. **Display auf ±10 %** |
| g | Batterie | – | Display gegen Multimeter an VBAT, ±3 % |
| h | Kurzschluss | A und E direkt verbinden, MAX | Display „STROMBEGRENZUNG“, Pegel geht auf FEIN, Strom ≤ 60 mA; nach 5 min nichts heiß (R12/R13 handwarm ist ok) |

Zu f: Viele Multimeter messen Wechselspannung nur bis ca. 400 Hz–1 kHz richtig, deshalb der Prüfton. Mit einem
Oszi geht es auch bei 8 kHz: Spannung über dem 1-kΩ-Widerstand (Math CH1−CH2), I_eff in mA = Vss ÷ 2,83.

### M2 – Handgerät in Betrieb nehmen (Pflicht)

| Schritt | Was | Erwartung |
|---|---|---|
| a | TPE1 VREF, TPE2 und TPE3 Gleichanteil | 1,65 V · 1,65 V · 1,65 V (±0,1 V) |
| b | serieller Monitor nach dem Start | „Abtastrate gesamt … Hz“ notieren (Soll 81 000, Abweichung bis einige % ist ok) |
| c | serieller Monitor | keine Warnung „Wandler-Puffer übergelaufen“ oder „keine Daten“ |
| d | Spule durch eine Drahtbrücke ersetzen (Ende an VREF), Plotter `Rauschen` | ca. −20 … −10 dBµV (Grundrauschen der Elektronik) |
| e | Spule dran, Sender aus | `Rauschen` notieren; Laptop auf Akku und am Netzteil vergleichen |
| f | Display, Tasten, Ton | REF und MODUS reagieren; ein eingeschaltetes Netzkabel in der Nähe gibt keinen Ausschlag (50 Hz wird unterdrückt) |

### M3 – Spule kalibrieren, Bereichswahl, Haltung (Pflicht)

**Aufbau:** Kalibrierschleife aus 10 m Litze (z. B. 0,75 mm²) als Rechteck **3 m × 2 m** auf dem Boden.
Sender-Ausgang A über **1 kΩ 2 W** an das eine Ende, E an das andere Ende, nichts geerdet. Sender `c` (nur
Ortungston) und MAX. Strom vom Sender-Display ablesen (oder mit dem Oszi wie bei M1 f).

1. Messkopf mittig über eine lange Seite halten, **Spulenmitte 10 cm über dem Draht**, Spule quer zum Draht.
   REF lang (Frequenzsuche), dann im Monitor `c<Strom in mA> 10`, z. B. `c8.8 10`.
2. Der Empfänger zeigt den Kalibrierfaktor `calK`. Theorie: ca. 17 µV/(nT·kHz). Wert notieren.
3. **Bereichswahl:** `v` (ausführlich) einschalten, Messkopf langsam von 30 cm bis auf 1 cm an den Draht.
   `Bereich` springt von 30 (HOCH) auf 10 (NIEDRIG), **der Pegel springt dabei nicht** (< 1 dB). Den Wert
   `Verh` notieren (Erwartung 35–42).
4. **Schirm-Test:** Ein offenes Drahtende (1 m) an A hängen, E bleibt frei. Spule 1 cm neben das Drahtende, so
   dass fast nur ein E-Feld wirkt. `Pegel` mit angeschlossenem und mit abgeklemmtem Spulenschirm vergleichen.
   **Erwartung: mit Schirm ≥ 20 dB weniger.**
5. **Haltung:** Wieder an die Schleife, Sender `c` aus (Zweiton). REF kurz (Referenz), Pfeil zeigt HIN. Gerät
   um 180° drehen (Display von dir weg): Pfeil zeigt ZURÜCK. Damit ist die Regel „Display zu dir“ bestätigt.

### M4 – Abstand zum Digitalteil (wenn möglich)

Sender aus. Messkopf auf den Tisch, das Handgerät-Steckbrett (ESP32-S3 + OLED) schrittweise **3 / 6 / 10 / 15 /
25 cm** davon entfernt. Jeweils `Rauschen` notieren. Die Halslänge ist der Abstand, ab dem sich der Wert nicht
mehr ändert. Dann bei 6 cm einmal mit Ton laut und mit Ton aus vergleichen.

### M5 – Freie Leitung (Pflicht, Vorab T-01 / T-02)

**Aufbau:** Testleitung **vollständig abgewickelt** und möglichst gerade auslegen (Flur, Hof, mind. 30 m). Eine
aufgewickelte Trommel wirkt wie eine Spule und verfälscht alles. Sender am Leitungsanfang in **Modus G**: alle
Adern und den Schirm in eine WAGO, A daran. E an eine Erde (PE-Klemme eines Verteilers, Heizungsrohr).
Alternativ führt man E über einen zweiten Draht mindestens 5 m seitlich weg.

1. Strom am Sender-Display notieren (MAX).
2. In der **Leitungsmitte**: Kopf quer zur Leitung. REF lang (Suche), dann Höhe über der Leitung **0,1 / 0,25 /
   0,5 / 1 / 1,5 / 2 m**: `Pegel` und `Rauschen` notieren. **Erkennungsabstand** = größte Höhe mit
   SNR ≥ 10 dB, bei der man das Maximum über der Leitung noch eindeutig findet.
3. Gleiches **mit geerdetem fernem Ende** (Bündel am Ende an Erde oder an den Rückleitungsdraht).
4. Zum Vergleich einmal **Ader gegen Ader**: A an Ader 1, E an Ader 2, Brücke am Ende. Erkennungsabstand?
   (Erwartung: wenige cm, Konzept Kap. 1.2. Das bestätigt, warum wir gegen Erde einspeisen.)

### M6 – Wand, Unterputz, Trasse (wenn möglich, Vorab T-03 / T-05 / T-13)

1. Eine bekannte Leitung unter Putz: Erkennungsabstand senkrecht zur Wand. **Maximum-Ortung:** Kopf quer zur
   Leitung langsam darüber schwenken, Stelle mit dem höchsten Pegel markieren und mit der echten Lage
   vergleichen (± cm). Dann den Kopf um 90° drehen: Der Pegel fällt stark ab, wenn der Kopf längs zur Leitung
   steht. So siehst du, in welche Richtung sie läuft.
2. **Vergleich IP-Kamera-Tester:** gleiche Leitung, dessen Tongenerator und Prüfkopf laut Anleitung. Wie weit
   hörst du ihn noch? Dann unser System mit Modus G.
3. **Trasse** (Kunststoff/Gitter und, falls vorhanden, Blech): von unten (Boden) und direkt an der Trasse. SNR
   und Erkennbarkeit notieren.

### M7 – Stromrichtung im Bündel (Pflicht, Vorab T-05)

**Aufbau:** 5–10 Kabel (Reststücke ≥ 5 m) parallel zusammengebunden, an 3 Stellen auf ca. 30 cm Länge
auseinandergespreizt (≥ 5 cm Abstand). Gesuchte Leitung in Modus G. Alle anderen am **Sender-Ende** mit E (Erde)
verbinden, am fernen Ende offen.

1. Kopf nahe am Sender über die gesuchte Leitung, **Display zu dir**, REF kurz (Referenz).
2. An den 3 gespreizten Stellen jedes Kabel einzeln abtasten, Gerät gleich herum halten. Notieren: Pegel und
   Richtung (HIN / ZURÜCK / ?). **Trefferquote** = Anteil richtig erkannter Kabel.
3. Variante: Nachbarn am **fernen** Ende erden. (Erwartung: Nachbarn zeigen dann teils auch HIN; bekannte
   Grenze, Konzept Kap. 4.2.)

### M8 – Störquellen (wenn möglich, Vorab T-07)

Sender aus. Kopf 30 cm neben LED-Treiber, Schaltnetzteil, Leuchtstofflampe, Frequenzumrichter (falls
vorhanden): `Pegel` und `Rauschen` notieren. Dann Sender an einer Testleitung: Wie stark ändert sich der Pegel,
wenn die Störquelle ein- und ausgeschaltet wird? (Ziel ≤ 3 dB.)

---

## 8. Erfolgskriterien (Go / No-Go für Phase 4)

| Nr. | Kriterium | Aus |
|---|---|---|
| K1 | Wandler läuft stabil (Abtastrate gemessen, keine Überläufe), Grundrauschen ≤ 0 dBµV | M2 |
| K2 | Kalibrierfaktor innerhalb ±50 % der Theorie (8–26 µV/(nT·kHz)); Schirm ≥ 20 dB; Bereichswechsel ohne Sprung (< 1 dB) | M3 |
| K3 | Freie Leitung 30–50 m, Ende offen: Erkennungsabstand ≥ 1 m in der Mitte – oder Feld pro mA passt auf ±50 % zum Modell | M5 |
| K4 | Ader gegen Ader klar schlechter als Modus G (bestätigt die Grundidee) | M5 |
| K5 | Stromrichtung ≥ 80 % richtig an gespreizten Kabeln (Nachbarn am Sender geerdet) | M7 |
| K6 | Endstufe ≥ 15 Vss je Ausgang ohne Begrenzung, stabil an 100 nF; Strommessung ±10 %; Strombegrenzung greift | M1 |

Fällt ein Kriterium durch, passen wir die Schaltung an, bevor Schaltplan und Lochraster-Aufbauplan (Phase 4/5)
entstehen. Die
gemessenen Werte bestimmen außerdem die Halslänge (M4).
