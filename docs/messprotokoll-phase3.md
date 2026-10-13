# Messprotokoll Machbarkeitstest (Phase 3)

Zum Ausfüllen. Anleitung: [`machbarkeitstest.md`](machbarkeitstest.md), Abschnitt 7.
Fotos vom Oszi-Bildschirm und Screenshots vom seriellen Plotter sind genauso gut wie Zahlen.

| | |
|---|---|
| Datum | |
| Spule H: Widerstand / ≈ Windungen (25 × R) | Ω / |
| Spule V: Widerstand / ≈ Windungen | Ω / |
| Ferritstäbe (Maße, Material falls bekannt) | |
| Oszilloskop (Bandbreite) | |

## M1 – Sender auf dem Tisch

| Schritt | Messwert | Bemerkung |
|---|---|---|
| a +24 V / VMID | V / V | |
| b TP3 / TP4 (Ausgang aus) | V / V | |
| c TP1 Periode / Vss | µs / V | |
| e TP3 / TP4 bei 90 % (Vss, abgeflacht?) | V / V | |
| f Last 1 kΩ: Vss über R12 → Strom | V → mA eff | |
| g Last 100 nF: Schwingung sichtbar? | ja / nein | |
| h Hoch-Satz: Periode / Vss an TP3 | µs / V | |

## M2 – Empfänger

| Schritt | Messwert | Bemerkung |
|---|---|---|
| a VREF / TPE2 | V / V | |
| b SCK / BCK / LRC | MHz / MHz / kHz | |
| c Warnung „nur Nullen“? | ja / nein | |
| d Grundrauschen (Eingang kurz) | dBµV | |
| e Rauschen mit Spulen, Laptop Akku / Netzteil | dBµV / dBµV | |

## M3 – Kalibrierung

| | H-Spule | V-Spule |
|---|---|---|
| Strom in der Schleife | mA eff | mA eff |
| Kalibrierfaktor calK | µV/(nT·kHz) | µV/(nT·kHz) |
| Schirm-Test: H mit / ohne Schirm | dBµV / dBµV | dBµV / dBµV |

## M4 – Abstand zum Digitalteil

| Abstand | 3 cm | 6 cm | 10 cm | 15 cm | 25 cm |
|---|---|---|---|---|---|
| Rauschen (dBµV), OLED an | | | | | |
| Rauschen (dBµV), OLED aus | | | | | |

## M5 – Freie Leitung

Leitung: Typ ______, Länge ____ m, Verlegung ______________, Erde: ______________

| | Standard | Tief | Hoch |
|---|---|---|---|
| Strom am Anfang (Zangen-Modus), Ende offen | mA | mA | mA |
| Strom am Anfang, Ende geerdet | mA | mA | mA |

Ende offen, Leitungsmitte, H-Pegel / Rauschen (dBµV):

| Höhe | Standard | Tief | Hoch |
|---|---|---|---|
| 0,1 m | / | / | / |
| 0,25 m | / | / | / |
| 0,5 m | / | / | / |
| 1 m | / | / | / |
| 1,5 m | / | / | / |
| 2 m | / | / | / |
| **Erkennungsabstand** | m | m | m |

Ende geerdet, Erkennungsabstand: Standard ____ m · Tief ____ m · Hoch ____ m

Modus P (Ader/Ader, Brücke am Ende), Erkennungsabstand: ____ cm

## M6 – Wand, Unterputz, Trasse

| Objekt | Erkennungsabstand | Null-Ortung (Abweichung) | IP-Kamera-Tester (Reichweite) |
|---|---|---|---|
| Unterputz: ______ | cm | ± cm | cm |
| Trasse von unten: ______ | m | | |
| an der Trasse: ______ | | | |
| Blechrinne (falls vorhanden) | | | |

## M7 – Stromrichtung im Bündel

Anzahl Kabel ____, Länge ____ m, Nachbarn geerdet am: Sender-Ende / fernen Ende

| Stelle | gesuchte Leitung (Pegel, Richtung) | Nachbarn (Pegel, Richtung) |
|---|---|---|
| 1 | | |
| 2 | | |
| 3 | | |
| **Trefferquote** | | % |

## M8 – E-Spitze am Ende

| Ader | Pegel V (dBµV) |
|---|---|
| gesuchte Ader | |
| Nachbar 1 | |
| Nachbar 2 | |
| Nachbar 3 | |

## M9 – Störquellen (30 cm, Sender aus)

| Quelle | H Standard (dBµV) | H Tief | H Hoch | Änderung mit Sender (dB) |
|---|---|---|---|---|
| LED-Treiber | | | | |
| Schaltnetzteil | | | | |
| Leuchtstofflampe | | | | |
| | | | | |

## Bemerkungen

-
