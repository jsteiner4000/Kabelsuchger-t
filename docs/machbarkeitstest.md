# Machbarkeitstest (Phase 3)

| | |
|---|---|
| **Version** | 1.0 |
| **Stand** | 13.10.2026 |
| **Grundlage** | [Konzept v1.0](konzept.md) (freigegeben) |
| **Visuelle Übersicht** | [`machbarkeitstest-uebersicht.html`](machbarkeitstest-uebersicht.html) (Schaltpläne, Link siehe README) |
| **Protokoll zum Ausfüllen** | [`messprotokoll-phase3.md`](messprotokoll-phase3.md) |

---

## 0. Worum es geht

Bevor eine Platine bestellt wird, prüfen wir auf dem Steckbrett die Punkte, an denen das Konzept
scheitern könnte (Konzept Kap. 12):

| Nr. | Frage | Messung |
|---|---|---|
| 1 | Läuft der 24-bit-Wandler PCM1808 sauber am ESP32-S3? | M2 |
| 2 | Wie empfindlich sind die selbstgewickelten Spulen, und hilft der Schirm gegen E-Feld-Einstreuung? | M3 |
| 3 | Wie weit müssen die Spulen vom Digitalteil weg (Halslänge)? | M4 |
| 4 | Wie weit reicht die Ortung wirklich: freie Leitung, Wand, Trasse? Welcher Frequenzsatz ist am besten? | M5, M6 |
| 5 | Wie zuverlässig ist die Stromrichtung im Bündel? | M7 |
| 6 | Findet die E-Spitze die Ader am Ende? | M8 |
| 7 | Arbeitet die Endstufe stabil an kapazitiver Last? | M1 |
| 8 | Wie gut werden Störquellen unterdrückt? | M9 |

Gemessen wird mit dem Handgerät-Prototyp selbst. Die Werte schaust du dir im **seriellen Plotter
der Arduino IDE** an; das Oszilloskop brauchst du für Spannungen, Takte und den Strom.

**Was sich gegenüber dem Konzept geändert hat** (Entscheidungen E-028 bis E-030):

- **Endstufe:** Der L272 ist bei onsemi abgekündigt, die Lieferbarkeit ist unsicher. Ersatz:
  Operationsverstärker TL072 mit Gegentakt-Transistoren BD139/BD140. Das ist dieselbe Funktion, alles
  THT, dauerhaft lieferbar und kostet gleich viel.
- **Frequenzen:** Sie ergeben sich exakt aus dem Takt des D/A-Wandlers. Standard 8333,3 Hz +
  4166,7 Hz, Tief 1041,7 Hz + 520,8 Hz, Hoch 33 333 Hz. Alle liegen mindestens 8 Hz neben einer
  50-Hz-Oberwelle.
- **OLED:** 1,3″ mit I²C (4 Pins). Das ist einfacher zu verdrahten; die Bildrate reicht.

---

## 1. Sicherheit und Masse – bitte zuerst lesen

1. **Nur an spannungsfreien Leitungen** und am Testaufbau. Der Prototyp hat noch keinen Schutz gegen
   Fremdspannung.
2. **Der Sender muss „schwimmen“.** Die Endstufe ist eine Brückenschaltung: Beide Ausgänge A und E
   schwingen gegen die Schaltungsmasse. Deshalb:
   - Sender **nur aus der Powerbank** versorgen, nie über einen Laptop am Netzteil.
   - **Oszilloskop nur auf dem Tisch** (M1, M3) an den Sender klemmen, mit Masse an Sender-GND, und nur,
     wenn A und E an nichts Geerdetem hängen (nur Testlast oder Kalibrierschleife).
   - Im Feld (Klemme an PE) **kein Oszilloskop an den Sender**. Den Strom misst du dort mit dem
     Zangen-Modus des Empfängers (Abschnitt 6, M5).
3. Am Sender liegen 24 V Gleichspannung und bis ca. 12 V eff Wechselspannung an den Klemmen. Das ist
   Schutzkleinspannung und ungefährlich, trotzdem: erst verdrahten, dann einschalten.
4. Der Empfänger darf am Laptop hängen. Für Rauschmessungen den Laptop besser auf Akku betreiben.

---

## 2. Einkaufsliste

Preise geschätzt (AliExpress ≈ 2–3 Wochen Lieferzeit, Reichelt ≈ 2–3 Tage). „Endgerät“ bedeutet,
dass das Teil später in Sender oder Handgerät eingebaut wird.

### 2.1 Module und ICs

| Teil | Menge | Bezug | ca. € | Endgerät |
|---|---|---|---|---|
| ESP32-S3 SuperMini (mit Stiftleisten) | 2 (1 Reserve) | AliExpress | 7,00 | ja |
| PCM1808-ADC-Modul (mit Stiftleisten) | 2 (1 Reserve) | AliExpress | 6,00 | ja |
| OLED 1,3″ SH1106, I²C, 4 Pins | 2 | AliExpress | 6,00 | ja |
| MT3608-Step-up-Modul (5er-Pack) | 1 Pack | AliExpress | 2,00 | ja |
| TL072 (DIP-8) | 2 | Reichelt | 0,80 | ja |
| MCP6022-I/P (DIP-8) | 2 | Reichelt / Mouser | 4,00 | ja |
| BD139 + BD140 | je 3 | Reichelt | 1,50 | ja |
| ESP32-S2 mini | 1 | vorhanden | – | ja |

### 2.2 Spulen

| Teil | Menge | Bezug | ca. € | Endgerät |
|---|---|---|---|---|
| Ferritstab Ø 10 × 80 mm (H-Spule) | 2 | AliExpress („ferrite rod 10x80“) | 2,00 | ja |
| Ferritstab Ø 10 × 60 mm (V-Spule) | 2 | AliExpress | 2,00 | ja |
| Kupferlackdraht 0,15 mm, **lötbar** (Polyurethan), ca. 50 g | 1 | AliExpress / Amazon | 4,00 | ja |
| Kupferfolienband 10 mm, **leitfähiger Kleber** | 1 Rolle | AliExpress / Amazon | 3,00 | ja |
| Kaptonband (oder Isolierband) | 1 | vorhanden? | – | ja |

### 2.3 Passive Bauteile (Reichelt oder Sortiment)

| Teil | Menge |
|---|---|
| Metallschicht 0,25 W: 10 Ω ×1, 100 Ω ×4, 1 kΩ ×4, 2,2 kΩ ×2, 3,3 kΩ ×1, 10 kΩ ×8, 18 kΩ ×1, 33 kΩ ×2, 100 kΩ ×1, 1 MΩ ×1, 10 MΩ ×1 |  |
| 47 Ω 2 W ×2 · 1 kΩ 2 W ×1 (Kalibrierwiderstand) |  |
| Keramik C0G oder Folie: 100 pF ×2, 1 nF ×2, 2,2 nF ×2 |  |
| Keramik 100 nF ×8 · Folie 470 nF ×2 · Folie 1 µF ×1 |  |
| Elko: 10 µF/25 V ×6, 47 µF/25 V ×1, 220 µF/35 V ×2 |  |
| 10 µF **bipolar** (BP/NP) 63 V ×2 – oder je zwei 22 µF/35 V antiseriell (Minus an Minus) |  |
| Rückstellende Sicherung (PTC) 0,1 A Haltestrom, ≥ 60 V (z. B. Bourns MF-R010) ×2 |  |

Sortiment-Preis ca. 8–10 €.

### 2.4 Aufbau

| Teil | Menge | ca. € |
|---|---|---|
| Steckbrett 830 Kontakte | 2 | 4,00 |
| Steckbrücken-Set (Stecker-Stecker) | 1 | 3,00 |
| Messleitungen mit Krokodilklemmen (Set) | 1 | 3,00 |
| WAGO 221-415, Elektro-Isolierrohr M20 (1 m), J-Y(St)Y-Reststück 2×2×0,6 (1,5 m), Kabelbinder, Powerbank, USB-C-Kabel | vorhanden | – |

**Summe ca. 55–60 €.** Davon landen ca. 30–35 € später direkt in den Geräten. Der Rest sind
Reserveteile (zweites PCM1808 und ESP32-S3, falls eins Ärger macht) und Steckbrett-Material.

**Tipp:** Die AliExpress-Teile jetzt bestellen. In der Wartezeit kannst du die Druckteile drucken und
die Spulen wickeln.

---

## 3. Druckteile

| Teil | Datei | Hinweis |
|---|---|---|
| Spulenkörper H (Flansch 22 mm) | `hardware/handgeraet/spule/spulenkoerper_H.stl` | stehend, 3 Wände |
| Spulenkörper V (Flansch 15,5 mm, passt ins M20-Rohr) | `hardware/handgeraet/spule/spulenkoerper_V.stl` | stehend, 3 Wände |
| Wickeldorn (1/4″-Sechskant für den Akkuschrauber) | `hardware/handgeraet/spule/wickeldorn.stl` | stehend, **Brim an**, 100 % Füllung |
| Messkopf (hält H-Spule, nimmt M20-Rohr auf) | `hardware/handgeraet/testaufbau/messkopf.stl` | flach |

Alle Teile: PETG, 0,4-mm-Düse, 0,2 mm Schicht, **kein Stützmaterial**. Die `.scad`-Dateien liegen
daneben; Maße (z. B. Stabdurchmesser) kannst du dort ändern, falls deine Ferritstäbe abweichen.

---

## 4. Spulen wickeln

Beide Spulen werden gleich gebaut, nur der Spulenkörper unterscheidet sich.

1. **Spulenkörper auf den Wickeldorn** schieben (stramm, bis zur Schulter), Dorn ins Bohrfutter.
2. **Drahtanfang** durch den Schlitz im unteren Flansch fädeln, 15 cm überstehen lassen und mit
   Klebeband am Dorn fixieren. Das ist der **Anfang**, mit Edding markieren (wichtig für Links/Rechts).
3. **Wickeln** auf kleinster Stufe. Den Draht durch ein Tuch zwischen Daumen und Zeigefinger laufen
   lassen (leichte Spannung) und gleichmäßig hin und her führen. Ziel: **ca. 1000 Windungen**, das sind
   4–5 Lagen bzw. etwa 1 mm Wicklungshöhe. Zählhilfe: Klebefähnchen am Bohrfutter, in Zehnern zählen
   (Strichliste). Exakt muss es nicht sein, die Spule wird in M3 kalibriert.
4. **Ende** durch den oberen Schlitz führen, 15 cm überstehen lassen.
5. **Prüfen:** Drahtenden 1 cm verzinnen (lötbarer Lack schmilzt bei ca. 380 °C im Zinn), Widerstand
   messen. **Windungszahl ≈ 25 × Widerstand in Ω** (0,15-mm-Draht, Ø 12 mm). 35–45 Ω sind richtig.
   Beide Spulen sollten auf ±10 % gleich sein.
6. **Isolieren:** eine Lage Kaptonband über die Wicklung.
7. **Schirm:** Kupferfolienband längs über die Wicklung kleben, **ringsum, aber mit 2 mm Spalt** in
   Längsrichtung. Ein geschlossener Ring wäre eine Kurzschlusswindung und würde das Signal schlucken.
   An einer Stelle eine dünne Litze als **Schirmanschluss** anlöten.
8. **Ferritstab** mittig einschieben, mit einem Tropfen Heißkleber sichern.
9. **Anschluss:** J-Y(St)Y-Reststück (1,5 m) als Spulenkabel. Paar 1 an die H-Spule, Paar 2 an die
   V-Spule, beide Spulenschirme und der Kabelschirm am Steckbrett an GND. Notieren, welche Ader an
   „Anfang“ hängt.

**Messkopf zusammensetzen:** H-Spule waagerecht mit Kabelbindern unter die Platte, V-Spule senkrecht
unten in das M20-Rohr stecken (Ferritstab steht auf der Platte), Rohr in die Hülse. Drähte der H-Spule
durch das Loch neben der Hülse nach oben führen und am Rohr festkleben.

---

## 5. Aufbau auf dem Steckbrett

Die Schaltpläne findest du als Zeichnung in der visuellen Übersicht. Hier stehen sie als
Verdrahtungsliste, Zeile für Zeile abzuhaken.

### 5.1 Sender-Steckbrett (ESP32-S2 mini, TL072, BD139/BD140)

**Reihenfolge:** zuerst nur Powerbank, S2 mini und MT3608 verbinden. Den MT3608 **mit dem Multimeter
auf 24,0 V einstellen** (Mehrgang-Poti, die Drehrichtung ist je nach Modul unterschiedlich). Erst dann
den Rest aufbauen.

| # | Von | Über | Nach | Zweck |
|---|---|---|---|---|
| S1 | S2 mini VBUS (5 V) | – | MT3608 VIN+ | Versorgung Hochsetzsteller |
| S2 | S2 mini GND | – | MT3608 VIN−, Masse-Schiene | Masse |
| S3 | MT3608 VOUT+ | – | Schiene **+24 V** | Endstufe |
| S4 | +24 V | C1 220 µF/35 V (+) und C2 100 nF | GND | Puffer, nah am TL072 |
| S5 | +24 V | R1 10 kΩ | Knoten **VMID** | Mittenspannung |
| S6 | VMID | R2 10 kΩ und C3 47 µF (+ an VMID) | GND | ergibt 12 V |
| S7 | S2 mini GPIO17 (DAC) | R3 2,2 kΩ | Knoten F1 | Tiefpass Stufe 1 |
| S8 | F1 | C4 2,2 nF | GND | |
| S9 | F1 | R4 2,2 kΩ | Knoten F2 | Tiefpass Stufe 2 |
| S10 | F2 | C5 2,2 nF | GND | |
| S11 | F2 | C6 1 µF Folie | U1 Pin 3 (IN+ A) | Koppelkondensator |
| S12 | U1 Pin 3 | R5 100 kΩ | VMID | Arbeitspunkt 12 V |
| S13 | U1 Pin 8 | – | +24 V | TL072 Versorgung |
| S14 | U1 Pin 4 | – | GND | |
| S15 | U1 Pin 1 (OUT A) | R6 100 Ω | Basis Q1 (BD139) und Basis Q2 (BD140) | Treiber A |
| S16 | Q1 Kollektor | – | +24 V | |
| S17 | Q2 Kollektor | – | GND | |
| S18 | Q1 Emitter + Q2 Emitter | – | Knoten **OA** | Ausgang A |
| S19 | OA | R7 18 kΩ | U1 Pin 2 (IN− A) | Gegenkopplung, Verstärkung 6,5 |
| S20 | U1 Pin 2 | R8 3,3 kΩ, dann C7 10 µF (+ zu R8) | GND | |
| S21 | OA | R9 10 kΩ | U1 Pin 6 (IN− B) | invertierender Zweig |
| S22 | U1 Pin 5 (IN+ B) | – | VMID | |
| S23 | U1 Pin 7 (OUT B) | R11 100 Ω | Basis Q3 (BD139) und Basis Q4 (BD140) | Treiber B |
| S24 | Q3 Kollektor / Q4 Kollektor | – | +24 V / GND | |
| S25 | Q3 Emitter + Q4 Emitter | – | Knoten **OB** | Ausgang B |
| S26 | OB | R10 10 kΩ | U1 Pin 6 | Verstärkung −1 |
| S27 | OA | C8 10 µF bipolar, R12 47 Ω 2 W, PTC1 | **Klemme A** (rot) | Ausgang A |
| S28 | OB | C9 10 µF bipolar, R13 47 Ω 2 W, PTC2 | **Klemme E** (grün-gelb) | Ausgang E (Rückleiter) |

BD139/BD140-Anschlüsse (Schrift zu dir, Beinchen nach unten): **E – C – B** von links nach rechts.
TL072: Pin 1 links oben neben der Kerbe/dem Punkt, gegen den Uhrzeigersinn weiterzählen.

Der Prototyp hat nur zwei Ausgänge: **A** und **E**. Für Modus P (Paar) klemmst du E einfach an Ader B
statt an Erde.

**Messpunkte Sender:** TP1 = GPIO17 · TP2 = F2 · TP3 = OA · TP4 = OB · TP5/TP6 = beide Enden von R12
(Strom = Spannung über R12 ÷ 47 Ω).

### 5.2 Empfänger-Steckbrett (ESP32-S3, PCM1808, MCP6022)

| # | Von | Über | Nach | Zweck |
|---|---|---|---|---|
| E1 | S3 5V-Pin | R20 10 Ω | Schiene **A5** (Analog-5-V) | entkoppelt USB-Störungen |
| E2 | A5 | C20 220 µF (+) und C21 100 nF | GND | |
| E3 | A5 | – | PCM1808 **5V** (VCC) | Analogversorgung |
| E4 | S3 3V3 | – | PCM1808 **3.3V** (VDD), OLED VCC | Digitalversorgung |
| E5 | S3 GND | – | PCM1808 GND, OLED GND, Masse-Schiene | |
| E6 | PCM1808 **FMT, MD0, MD1** (falls herausgeführt) | – | GND | Slave-Betrieb, I²S-Format |
| E7 | S3 GPIO4 | (kurz, < 10 cm) | PCM1808 **SCK** | Systemtakt 24,576 MHz |
| E8 | S3 GPIO5 | | PCM1808 **BCK** | Bittakt 6,144 MHz |
| E9 | S3 GPIO6 | | PCM1808 **LRC** | Kanaltakt 96 kHz |
| E10 | PCM1808 **OUT** | | S3 GPIO7 | Daten |
| E11 | S3 GPIO8 / GPIO9 | | OLED SDA / SCL | Anzeige |
| E12 | A5 | R21 10 kΩ | Knoten **VREF** | |
| E13 | VREF | R22 10 kΩ, C22 10 µF (+ an VREF), C23 100 nF | GND | 2,5 V Bezug |
| E14 | U2 Pin 8 / Pin 4 | – | A5 / GND | MCP6022 Versorgung |
| E15 | H-Spule „Anfang“ | – | VREF | |
| E16 | H-Spule „Ende“ | R23 100 Ω | U2 Pin 3 (IN+ L) | |
| E17 | U2 Pin 3 | C24 1 nF | VREF | HF-Filter |
| E18 | U2 Pin 1 (OUT L) | R24 33 kΩ ∥ C25 100 pF | U2 Pin 2 | Verstärkung 34 |
| E19 | U2 Pin 2 | R25 1 kΩ, dann C26 470 nF | VREF | Hochpass 340 Hz |
| E20 | U2 Pin 1 | C27 10 µF (+ zu U2) | PCM1808 **L**-Eingang | Kanal L |
| E21–E26 | V-Spule an U2 Pin 5/6/7 | wie E15–E20 | PCM1808 **R**-Eingang | Kanal R |
| E27 | Spulenschirme, Kabelschirm | – | GND | nur hier auflegen |

**Für M8 (E-Spitze)** die V-Spule abklemmen und stattdessen: Spitze (z. B. Prüfspitze einer alten
Messleitung) über abgeschirmtes Kabel → R28 10 kΩ → U2 Pin 5, dazu R29 10 MΩ von Pin 5 nach VREF.
Kabelschirm an GND.

**Verstärkung ×2 statt ×34** (falls „ÜBERSTEUERT“ direkt am Kabel): 1 kΩ parallel zu R24 stecken.

**Messpunkte Empfänger:** TPE1 = VREF (2,50 V) · TPE2 = U2 Pin 1 · TPE3 = SCK · TPE4 = BCK ·
TPE5 = LRC · TPE6 = OUT.

---

## 6. Software

1. **Arduino IDE 2** installieren, unter *Einstellungen → Zusätzliche Boardverwalter-URLs*
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json` eintragen, dann im
   Boardverwalter **„esp32“ von Espressif** installieren (Version 3.x oder neuer).
2. Bibliotheksverwalter: **„U8g2“** von olikraus installieren (für das OLED).
3. **Sender:** `firmware/machbarkeit/sender_test/sender_test.ino` öffnen. Board **„LOLIN S2 Mini“**,
   *USB CDC On Boot: Enabled*. Erstes Hochladen: Taste **0** halten, **RST** kurz drücken, **0**
   loslassen, hochladen, danach RST drücken.
4. **Empfänger:** `firmware/machbarkeit/empfaenger_test/empfaenger_test.ino` öffnen. Board
   **„ESP32S3 Dev Module“**, *USB CDC On Boot: Enabled*, *CPU 240 MHz*. Bei Problemen beim Hochladen:
   BOOT halten, RST drücken, BOOT loslassen.
5. **Serieller Monitor / Plotter** auf 115200 Baud. Befehle tippst du in die Eingabezeile und schickst
   sie mit Enter.

| Empfänger-Befehl | Wirkung |
|---|---|
| `s` (oder BOOT lang) | Frequenzsuche, Messkopf dabei nah an die Leitung halten |
| `r` (oder BOOT kurz) | Referenz für die Stromrichtung setzen (nahe am Sender, über der gesuchten Leitung) |
| `f` | nächster Frequenzsatz (Standard / Tief / Hoch). **Sender und Empfänger müssen gleich stehen.** |
| `t` | Glättung schnell / ruhig |
| `z` | Zangen-Modus: Strom einer einzelnen Leitung in 5 cm Abstand |
| `c10 10` | Kalibrieren: 10 mA im Draht, Spulenmitte 10 cm entfernt |
| `k17.3` | Kalibrierfaktor direkt setzen |
| `l` / `a` / `v` / `?` | Links/Rechts umdrehen / AFC an-aus / ausführliche Zeile / Hilfe |

| Sender-Befehl | Wirkung |
|---|---|
| `1` `2` `3` (oder Taste 0) | Pegel 25 / 50 / 90 % |
| `s` `t` `h` | Frequenzsatz Standard / Tief / Hoch |
| `c` | nur Ortungston (ohne Richtungston) |
| `o` | Ausgang aus |

**Plotter-Kurven** (dB bezogen auf 1 µV an der Spule): `H` = H-Spule, `V` = Kanal R, `CD` =
Richtungston, `Rauschen` = Störpegel neben der Frequenz, `Richtung` = +20 HIN / −20 ZURÜCK / 0 unklar,
`LR` = Links/Rechts × 20. **Abstand Signal – Rauschen = SNR.** Ab 10 dB gilt die Leitung als erkannt.

Die Signalverarbeitung (`lockin.h`) ist auf dem PC mit simulierten Signalen getestet
(`firmware/machbarkeit/test_host/build.sh`): Frequenzsuche auf 0,01 Hz genau, Pegel auf 0,1 %,
Stromrichtung und Links/Rechts korrekt, Netzoberwelle 16,7 Hz daneben ≤ 0,1 % Einfluss. Kompilieren
für den ESP32 konnte ich hier nicht; falls die Arduino IDE einen Fehler meldet, schick mir die Meldung.

---

## 7. Messprogramm

Alle Ergebnisse trägst du in [`messprotokoll-phase3.md`](messprotokoll-phase3.md) ein (oder schickst
mir Fotos und Screenshots vom Plotter).

### M1 – Sender auf dem Tisch (Oszilloskop)

**Aufbau:** Sender an der Powerbank, Oszi-Masse an Sender-GND, Last zwischen A und E, sonst nichts
angeschlossen.

| Schritt | Messpunkt | Einstellung | Erwartung |
|---|---|---|---|
| a | +24 V, VMID | Multimeter | 24,0 V · 12,0 V ±0,3 V |
| b | TP3, TP4 | Sender `o` (aus) | je ca. 12 V Gleichspannung |
| c | TP1 (DAC) | `s`, `2` | Periode 240 µs, zwei ungleiche Buckel je Periode, ca. 1,6 Vss |
| d | TP2 | wie c | ähnliche Form, etwas glatter |
| e | TP3 und TP4 gleichzeitig | `3` (90 %) | gegenphasig, je ca. 17 Vss, **keine abgeflachten Spitzen** |
| f | über R12 (Math CH1−CH2) | Last 1 kΩ, `3` | ca. 1,4 Vss über R12 → Strom ≈ 30 mA Spitze-Spitze (≈ 9 mA eff) |
| g | TP3, Zeitbasis 1 µs/div | Last 100 nF | sauber, **keine Schwingung** (kein „Gras“ auf der Kurve) |
| h | TP3 | `h` (Hoch) | Periode 30 µs, Amplitude notieren (der Tiefpass dämpft hier) |

### M2 – Empfänger in Betrieb nehmen

| Schritt | Was | Erwartung |
|---|---|---|
| a | TPE1 VREF, TPE2 Gleichanteil | 2,50 V · 2,5 V |
| b | TPE3 SCK / TPE4 BCK / TPE5 LRC | 24,6 MHz / 6,1 MHz / 96 kHz (Periode 10,4 µs). Bei einem 20-MHz-Oszi ist SCK nur klein zu sehen, Hauptsache vorhanden |
| c | serieller Monitor | keine Warnung „nur Nullen vom PCM1808“ |
| d | Spule abgeklemmt, U2 Pin 3 an VREF, Plotter `Rauschen` | ca. −40 … −25 dBµV (Grundrauschen) |
| e | Spulen dran, Sender aus, Laptop auf Akku | `Rauschen` notieren; dann Laptop am Netzteil: ändert es sich? |

### M3 – Spule kalibrieren

**Aufbau:** Kalibrierschleife aus 10 m Litze (z. B. 0,75 mm²) als Rechteck **3 m × 2 m** auf dem
Boden. Sender-Ausgang A über **1 kΩ 2 W** an das eine Ende, E an das andere Ende, Sender an der
Powerbank, nichts geerdet. Sender auf `c` (nur Ortungston, reiner Sinus) und `3`.
Strom mit dem Oszi über dem 1-kΩ-Widerstand messen (Math CH1−CH2): **I_eff in mA = Vss ÷ 2,83**
(bei 1 kΩ), z. B. 25 Vss → 8,8 mA.

1. Messkopf mittig über eine lange Seite halten, **Spulenmitte 10 cm über dem Draht**, H-Spule quer
   zum Draht. `s` (Frequenzsuche), dann `c<Strom in mA> 10` eingeben, z. B. `c8.8 10`.
2. Der Empfänger zeigt den Kalibrierfaktor `calK`. Theorie: ca. 17 µV/(nT·kHz). Wert notieren.
   Für später oben im Sketch bei `calK` eintragen.
3. **Schirm-Test:** Ein offenes Drahtende (1 m) an A hängen, E bleibt frei. Spule 1 cm neben das
   Drahtende, so dass fast nur ein E-Feld wirkt. `H` mit angeschlossenem und mit abgeklemmtem
   Spulenschirm vergleichen. **Erwartung: mit Schirm ≥ 20 dB weniger.**
4. Gleiches für die V-Spule (sie liegt auf Kanal R, Wert `V`).

### M4 – Abstand zum Digitalteil

Sender aus. Messkopf auf den Tisch, das Empfänger-Steckbrett (ESP32-S3 + OLED) schrittweise
**3 / 6 / 10 / 15 / 25 cm** davon entfernt. Jeweils `Rauschen` notieren. Die Halslänge ist der
Abstand, ab dem sich der Wert nicht mehr ändert. Zum Vergleich einmal mit `USE_OLED 0` flashen.

### M5 – Freie Leitung (Vorab T-01 / T-02)

**Aufbau:** Testleitung **vollständig abgewickelt** und möglichst gerade auslegen (Flur, Hof,
mind. 30 m). Eine aufgewickelte Trommel wirkt wie eine Spule und verfälscht alles. Sender am
Leitungsanfang in **Modus G**: alle Adern und den Schirm in eine WAGO, A daran. E an eine Erde
(PE-Klemme eines Verteilers, Heizungsrohr). Alternativ führt man E über einen zweiten Draht mindestens
5 m seitlich weg.

1. Strom am Anfang messen: Empfänger `z` (Zangen-Modus), Messkopf mit der H-Spule 5 cm neben die
   **A-Messleitung** halten. Die E-Leitung muss dabei ≥ 30 cm weg sein. Strom notieren.
2. In der **Leitungsmitte**: Messkopf senkrecht, H-Spule quer zur Leitung. `s`, dann Höhe über der
   Leitung **0,1 / 0,25 / 0,5 / 1 / 1,5 / 2 m**: `H` und `Rauschen` notieren.
   **Erkennungsabstand** = größte Höhe mit SNR ≥ 10 dB, bei der man das Maximum über der Leitung
   noch eindeutig findet.
3. Gleiches **mit geerdetem fernem Ende** (Bündel am Ende an Erde oder an den Rückleitungsdraht).
4. Gleiches mit **Satz Tief** und **Satz Hoch** (an beiden Geräten umschalten).
5. Zum Vergleich einmal **Modus P**: A an Ader 1, E an Ader 2, Brücke am Ende. Erkennungsabstand?
   (Erwartung: wenige cm, Konzept Kap. 1.2.)

### M6 – Wand, Unterputz, Trasse (Vorab T-03 / T-05 / T-13)

1. Eine bekannte Leitung unter Putz: Erkennungsabstand senkrecht zur Wand. **Null-Ortung:**
   V-Kanal-Minimum suchen (Plotter `V` fällt steil ab), Stelle markieren und mit der echten Lage
   vergleichen (± cm).
2. **Vergleich IP-Kamera-Tester:** gleiche Leitung, dessen Tongenerator und Prüfkopf laut Anleitung.
   Wie weit hörst du ihn noch? Dann unser System mit Modus G.
3. **Trasse** (Kunststoff/Gitter und, falls vorhanden, Blech): von unten (Boden) und direkt an der
   Trasse. SNR und Erkennbarkeit notieren, mit Satz Standard und Tief.

### M7 – Stromrichtung im Bündel (Vorab T-05)

**Aufbau:** 5–10 Kabel (Reststücke ≥ 5 m) parallel zusammengebunden. Gesuchte Leitung in Modus G.
Alle anderen am **Sender-Ende** mit E (Erde) verbinden, am fernen Ende offen.

1. Messkopf nahe am Sender über die gesuchte Leitung, `r` (Referenz).
2. An 3 Stellen entlang des Bündels jedes Kabel einzeln mit der H-Spule abtasten. Notieren: Pegel und
   Richtung (HIN / ZURÜCK / ?). **Trefferquote** = Anteil richtig erkannter Kabel.
3. Variante: Nachbarn am **fernen** Ende erden. (Erwartung: Nachbarn zeigen dann teils auch HIN;
   bekannte Grenze, Konzept Kap. 4.2.)

### M8 – E-Spitze: Ader am Ende (Vorab T-06)

E-Spitze statt V-Spule anschließen (Abschnitt 5.2). Sender **Modus A**: gesuchte Ader an A, alle
übrigen Adern und den Schirm in eine WAGO, daran E, und E zusätzlich an Erde. Am fernen Ende die Adern
abisolieren und auffächern. Spitze an jede Ader halten (1–5 mm Abstand), Kanal `V` im Plotter
notieren. **Erwartung: gesuchte Ader ≥ 10 dB über den anderen.**

### M9 – Störquellen (Vorab T-07)

Sender aus. Messkopf 30 cm neben LED-Treiber, Schaltnetzteil, Leuchtstofflampe, Frequenzumrichter
(falls vorhanden): `H` und `Rauschen` je Frequenzsatz notieren. Dann Sender an einer Testleitung: Wie
stark ändert sich der Pegel, wenn die Störquelle ein- und ausgeschaltet wird? (Ziel ≤ 3 dB.)

---

## 8. Erfolgskriterien (Go / No-Go für Phase 4)

| Nr. | Kriterium | Aus |
|---|---|---|
| K1 | PCM1808 läuft stabil, Grundrauschen ≤ −25 dBµV | M2 |
| K2 | Kalibrierfaktor innerhalb ±50 % der Theorie (8–26 µV/(nT·kHz)); Schirm ≥ 20 dB | M3 |
| K3 | Freie Leitung 30–50 m, Ende offen: Erkennungsabstand ≥ 1 m in der Mitte – oder Feld pro mA passt auf ±50 % zum Modell | M5 |
| K4 | Ader/Ader klar schlechter als Modus G (bestätigt die Grundidee) | M5 |
| K5 | Stromrichtung ≥ 80 % richtig (Nachbarn am Sender geerdet) | M7 |
| K6 | E-Spitze: gesuchte Ader ≥ 10 dB über den anderen | M8 |
| K7 | Endstufe ≥ 15 Vss je Ausgang ohne Begrenzung, stabil an 100 nF | M1 |

Fällt ein Kriterium durch, passen wir das Konzept an, bevor Schaltplan und Platine (Phase 4/5)
entstehen. Die gemessenen Werte bestimmen außerdem die Halslänge (M4) und den Standard-Frequenzsatz
(M5, M6).
