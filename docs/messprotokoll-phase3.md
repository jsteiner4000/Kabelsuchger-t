# Messprotokoll Mini-Test (Phase 3, Variante „Einfach“)

Zum Ausfüllen. Anleitung: [`machbarkeitstest.md`](machbarkeitstest.md), Abschnitt 7.
Fotos vom Oszi-Bildschirm, Fotos der Displays und Screenshots vom seriellen Plotter sind genauso gut wie Zahlen.

| | |
|---|---|
| Datum | |
| Spule: Widerstand / ≈ Windungen (25 × R) | Ω / |
| Ferritstab (Maße, Material falls bekannt) | |
| Multimeter (True RMS? bis welche Frequenz?) | |
| Oszilloskop (Bandbreite), falls benutzt | |

## M1 – Sender auf dem Tisch

| Schritt | Messwert | Bemerkung |
|---|---|---|
| a +24 V / VMID | V / V | |
| b TP3 / TP4 (Ausgang aus) | V / V | |
| c TP1 Periode / Vss (NORMAL) | µs / V | |
| d TP3 / TP4 bei MAX (Vss, abgeflacht?) | V / V | |
| e Last 100 nF: Schwingung sichtbar? | ja / nein | |
| f Prüfton, NORMAL, 1 kΩ: Spannung über 1 kΩ → Strom / Display | V → mA / mA | |
| g Batterie: Multimeter / Display | V / Symbol | |
| h Kurzschluss A–E: Anzeige, Strom, Temperatur nach 5 min | / mA / | |

## M2 – Handgerät in Betrieb nehmen

| Schritt | Messwert | Bemerkung |
|---|---|---|
| a VREF / TPE2 / TPE3 (Gleichanteil) | V / V / V | |
| b Abtastrate gesamt laut Monitor | Hz | |
| c Warnung „Puffer übergelaufen“ oder „keine Daten“? | ja / nein | |
| d Grundrauschen (Spule durch Brücke ersetzt) | dBµV | |
| e Rauschen mit Spule, Laptop Akku / Netzteil | dBµV / dBµV | |
| f Tasten, Display, Ton in Ordnung? | ja / nein | |

## M3 – Kalibrierung, Bereichswahl, Haltung

| | Wert |
|---|---|
| Strom in der Schleife (Display / Oszi) | mA / mA |
| Kalibrierfaktor calK | µV/(nT·kHz) |
| Bereichswahl: Abstand beim Umschalten auf NIEDRIG | cm |
| Pegelsprung beim Umschalten | dB |
| Verhältnis `Verh` | |
| Schirm-Test: Pegel mit / ohne Schirm | dBµV / dBµV |
| Haltung: Pfeil normal / Gerät um 180° gedreht | HIN / |

## M4 – Abstand zum Digitalteil

| Abstand | 3 cm | 6 cm | 10 cm | 15 cm | 25 cm |
|---|---|---|---|---|---|
| Rauschen (dBµV) | | | | | |

Bei 6 cm: Rauschen mit Ton laut ____ dBµV, Ton aus ____ dBµV.

## M5 – Freie Leitung

Leitung: Typ ______, Länge ____ m, Verlegung ______________, Erde: ______________

| | Ende offen | Ende geerdet |
|---|---|---|
| Strom am Sender-Display (MAX) | mA | mA |

Leitungsmitte, Pegel / Rauschen (dBµV):

| Höhe | 0,1 m | 0,25 m | 0,5 m | 1 m | 1,5 m | 2 m |
|---|---|---|---|---|---|---|
| Ende offen | | | | | | |
| Ende geerdet | | | | | | |

Erkennungsabstand: Ende offen ____ m, Ende geerdet ____ m.
Ader gegen Ader (Brücke am Ende): Erkennungsabstand ____ cm.

## M6 – Wand, Unterputz, Trasse

| Messung | Ergebnis |
|---|---|
| Unterputz: Wandmaterial, Tiefe der Leitung | |
| Erkennungsabstand senkrecht zur Wand | m |
| Maximum-Ortung: Abweichung zur echten Lage | cm |
| IP-Kamera-Tester: Erkennungsabstand | m |
| Unser System, Modus G: Erkennungsabstand | m |
| Trasse von unten: Höhe, SNR | m, dB |
| Trasse direkt: SNR | dB |

## M7 – Stromrichtung im Bündel

Anzahl Kabel ____, Länge ____ m.

| Stelle | gesuchte Leitung | Nachbarn richtig (ZURÜCK oder ?) | Nachbarn falsch (HIN) |
|---|---|---|---|
| 1 | | | |
| 2 | | | |
| 3 | | | |

Trefferquote: ____ %. Variante „Nachbarn am fernen Ende geerdet“: ____ %.

## M8 – Störquellen

| Störquelle | Pegel / Rauschen ohne Sender (dBµV) | Pegeländerung mit Sender, Störer an/aus (dB) |
|---|---|---|
| LED-Treiber | | |
| Schaltnetzteil | | |
| Leuchtstofflampe | | |
| Frequenzumrichter | | |

## Ergebnis

| Kriterium | erfüllt? | Bemerkung |
|---|---|---|
| K1 Wandler | | |
| K2 Kalibrierung, Schirm, Bereichswahl | | |
| K3 Erkennungsabstand freie Leitung | | |
| K4 Ader/Ader schlechter als G | | |
| K5 Stromrichtung | | |
| K6 Sender | | |
