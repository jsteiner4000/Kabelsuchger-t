# Platine oder Lochraster?

**Stand:** 13.10.2026 · **Status:** Entscheidungshilfe. Beschlossen ist zurzeit Lochraster (E-042); Julius möchte
anhand des KiCad-Entwurfs noch einmal entscheiden.

## 1. Der Platinen-Entwurf

Das KiCad-Projekt liegt in [`hardware/kicad/`](../hardware/kicad/) (Anleitung dort im README). Beide Platinen sind
vollständig geroutet und vom Prüfprogramm (DRC) ohne elektrische Fehler: keine Kurzschlüsse, keine zu kleinen
Abstände, keine offenen Verbindungen. Die restlichen Meldungen betreffen nur den Bestückungsdruck (überlappende
Beschriftungen).

| | Handgerät | Sender |
|---|---|---|
| Größe | 34 × 70 mm (passt in den Griff) | 80 × 60 mm, 4 Löcher M3 |
| Lagen | 2, Massefläche oben und unten | 2, Massefläche oben und unten |
| Bauteile | 35, nur bedrahtet; ESP32-S3 auf Buchsenleisten | 49, nur bedrahtet; S2 mini auf Buchsenleisten |
| Lötpunkte | 99 | 124 |
| Anschlüsse | Stiftleisten für Spule, Batterie, Tasten, Piezo, OLED, Reserve 2. Spule | Stiftleisten für Batterie, OLED, Taste, MT3608; Schraubklemme A/E |

Bilder: `hardware/kicad/handgeraet/bilder/` und `hardware/kicad/sender/bilder/` (oben, unten, Bestückung).

## 2. Vergleich

| | Platine (bestellt) | Lochraster |
|---|---|---|
| Kosten | ca. 9–12 € für 5 Stück je Gerät inkl. Versand | ca. 2 € |
| Wartezeit | 1–3 Wochen Lieferung | keine |
| Lötarbeit | 223 Lötpunkte, keine Drähte; ca. ein Abend für beide Geräte | dieselben 223 Lötpunkte und dazu ca. 155 Verbindungen von Hand (Draht oder Lötbrücke); ca. 2–3 Abende |
| Fehlerrisiko | gering; ein Fehler im Entwurf steckt aber in allen 5 Platinen (dann Drahtbrücke) | jede der 155 Verbindungen kann falsch sein; Fehlersuche mühsamer |
| Größe Handgerät | 34 × 70 mm, passt sicher in den Griff | realistisch ca. 40 × 80 mm, der Griff wird dicker |
| Analogteil Handgerät | Massefläche und kurze Wege, wenig Einstreuung | ohne Massefläche, empfindlicher für Störungen |
| Änderung nach dem Test | Drahtbrücke oder neue Bestellung | einfach umlöten |
| Kosten pro Satz gesamt | ca. 38–41 € | ca. 31 € |

## 3. Meine Empfehlung

**Platine, aber erst nach dem Mini-Test bestellen.**

- Beim **Handgerät** bringt die Platine am meisten. Sie ist klein genug für den Griff, und die Massefläche schützt
  den empfindlichen Analogteil. Auf Lochraster ist genau dieser Teil am schwierigsten.
- Beim **Sender** ist Lochraster gut machbar. Eine Platine nur für das Handgerät spart aber kaum etwas, weil der
  Versand den größten Teil der Kosten ausmacht. Deshalb würde ich beide zusammen bestellen.
- **Budget:** Mit Platinen liegt ein Satz bei ca. 38–41 €, also genau an deiner 40-€-Grenze (B-02).
- **Lochraster** ist die richtige Wahl, wenn dir die 7–10 € oder die Wartezeit wichtiger sind. Dann zeichne ich
  in Phase 5 einen Aufbauplan zum Abhaken.

## 4. Vor einer Bestellung prüfen

Die Module sind im Netz nicht einheitlich dokumentiert. Bevor Platinen bestellt werden, prüfen wir am gelieferten
Teil:

1. **ESP32-S3 SuperMini:** 2 × 9 Pins, Reihenabstand 15,24 mm. Reihenfolge laut Footprint: eine Seite 5V, GND, 3V3,
   13, 12, 11, 10, 9, 8, die andere TX, RX, 1, 2, 3, 4, 5, 6, 7, jeweils vom USB-Ende aus.
2. **LOLIN S2 mini:** nur die beiden äußeren Reihen (22,86 mm Abstand). Links EN, 3, 5, 7, 9, 11, 12, 3V3, rechts
   39, 37, 35, 33, 18, 16, GND, VBUS, jeweils von oben (USB unten).
3. **OLED:** Pinreihenfolge GND, VCC, SCL, SDA. Manche Module haben VCC und GND vertauscht.
4. **MT3608:** Das Modul wird mit kurzen Drähten oder Stiften an J5 (Eingang) und J6 (Ausgang) angeschlossen.
5. **Ergebnis des Mini-Tests:** Jede Schaltungsänderung kommt vor der Bestellung in die Netzliste
   (`hardware/kicad/netze.py`), dann wird neu erzeugt.

Eine Änderung aus der Platinenarbeit gilt schon jetzt: Der Signalausgang des Senders liegt auf **GPIO18**
(D/A-Kanal 2, äußere Stiftreihe) statt GPIO17. Firmware, Steckbrett-Schaltplan und Anleitung sind umgestellt.
