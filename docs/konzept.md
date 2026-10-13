# Konzept Kabelsuchgerät (Phase 2)

| | |
|---|---|
| **Version** | 1.0 – zur Freigabe |
| **Stand** | 13.10.2026 |
| **Grundlage** | [Lastenheft v1.0](lastenheft.md) (freigegeben) |
| **Rechnung** | [`berechnungen/feldmodell.py`](berechnungen/feldmodell.py) – alle Zahlen in diesem Dokument sind damit nachvollziehbar |
| **Visuelle Übersicht** | [`konzept-uebersicht.html`](konzept-uebersicht.html) (Link siehe README) |

---

## 0. Kurzfassung

1. **Wir orten über das Magnetfeld.** Es geht ungehindert durch Putz, Mauerwerk, Beton und
   (mit Verzerrung) Stahlbeton. Das elektrische Feld, mit dem einfache Tonsucher arbeiten, wird
   von feuchten Baustoffen, Bewehrung und geerdeten Schirmen geschluckt. Wir nutzen es nur noch
   dort, wo es stark ist: mit einer feinen Spitze im Bündel und an abisolierten Adern.
2. **Ein Magnetfeld nach außen gibt es nur, wenn der Strom nicht im selben Kabel zurückfließt.**
   Deshalb speisen wir im Hauptmodus **alle Adern und den Schirm gemeinsam gegen Erde** ein
   („Modus G“). Der Rückweg läuft über die Gebäudeerde, weit weg vom Kabel. Bei Ader/Ader heben
   sich Hin- und Rückstrom auf; das Feld ist nach rund 10 cm praktisch verschwunden (Kap. 1.2).
3. In Modus G **schrumpfen die sechs Fälle aus dem Lastenheft auf zwei**: Hat das ferne Ende eine
   Verbindung zur Erde (Schirm dort geerdet, geerdetes Gerät, selbst gesetzte Erde) oder nicht?
   Gebrückt oder offen spielt dann keine Rolle mehr, und **angeschlossene Geräte sehen keine
   Spannung**, weil alle Adern dasselbe Signal tragen.
4. **Signal:** zwei fest gekoppelte Töne (Standard ≈ 8,2 kHz zum Orten + ≈ 4,1 kHz für die
   Richtung), bis 15 V eff, Strom bis 30 mA (Stufe „Normal“). Zwei weitere Frequenzsätze
   (≈ 1 kHz und ≈ 33 kHz) sind reine Firmware und werden in Phase 3 bewertet.
5. **Stromrichtung (wie bei Profigeräten):** Auf der gesuchten Leitung fließt der Strom „hin“, auf
   Nachbarkabeln, auf die das Signal übergesprungen ist, meist „zurück“. Das Handgerät zeigt das
   als Pfeil. Das ist der Schlüssel für volle Trassen (U-13).
6. **Handgerät:** zwei Ferritspulen (waagerecht und senkrecht) plus E-Feld-Spitze, ein
   24-bit-Audio-Wandler (PCM1808) und digitale Lock-in-Auswertung mit 2–10 Hz Bandbreite. Daraus
   ergeben sich Signal, Links/Rechts, Null-Ortung, Tiefe (45°-Methode) und Stromrichtung. Die
   Anzeige erfolgt in echten Einheiten (nT).
7. **Sender:** misst Spannung, Strom und Phase am Ausgang. Er zeigt damit, ob das Ende offen oder
   geerdet ist, schätzt die Leitungslänge, misst den Durchgang in Ω und sperrt bei Fremdspannung.
8. **Mikrocontroller:** Sender mit deinem ESP32-S2 mini (hat den nötigen D/A-Wandler), Handgerät mit
   ESP32-S3 (Gleitkomma-Rechenwerk, zwei Kerne). Alles THT oder Modul, eigene Platinen.
9. **Reichweite (realistisch):** Unterputz und Hohlwand sicher ab ≥ 50 cm Abstand, durch 20 cm
   Stahlbeton, in Kunststoff- und Gittertrassen von unten aus 1–2 m über den größten Teil der
   Länge. Blech-Kabelrinnen schirmen stark ab; dort arbeitet man direkt an der Rinne.
10. **Budget:** Die empfohlene Variante kostet geschätzt **ca. 50 € pro Satz, also rund 10 € über
    dem Ziel**. Die abgespeckte Variante wäre nur ca. 5 € billiger, kann aber deutlich weniger.
    **Hier brauche ich deine Entscheidung** (Kap. 11).

---

## 1. Die Physik in einfachen Worten

### 1.1 Zwei Felder, zwei Werkzeuge

| | Elektrisches Feld (E-Feld) | Magnetfeld (H-Feld) |
|---|---|---|
| Entsteht durch | **Spannung** auf der Leitung | **Strom** in der Leitung |
| Sensor im Handgerät | Elektrode (Metallfläche, Spitze) | Spule auf Ferritstab |
| Braucht am Leitungsende | nichts, geht auch bei offenem Ende | einen Stromkreis (Erde am Ende oder genug Kapazität) |
| Wird blockiert von | geerdetem Schirm, Bewehrung, feuchtem Putz und Beton, Metallrinnen | praktisch nichts außer dicken Stahlblechen; Bewehrung verzerrt nur |
| Stark bei | Bündel, abisolierte Adern, trockene Hohlwand, in Luft | Tiefe, Beton, Stahlbeton, Trassen von unten |
| Liefert Richtungsinformation | nein | ja: links/rechts, Tiefe, Stromrichtung |

Warum der Putz das E-Feld schluckt: Bei einigen kHz verhalten sich feuchter Putz und Beton wie ein
schlechter Leiter. Sie schirmen das elektrische Feld ab, ähnlich wie ein Schirm, nur schwächer. Das
Magnetfeld interessiert das nicht: Seine Eindringtiefe in Beton liegt bei 8 kHz im Bereich von
Metern. **Das ist der Hauptgrund, warum Billig-Tonsucher in der Tiefe versagen.**

### 1.2 Warum sich die Felder bei zwei benachbarten Adern aufheben

Fließt der Strom auf Ader A hin und auf Ader B zurück, laufen zwei gleich große, entgegengesetzte
Ströme im Abstand von ca. 1,5 mm nebeneinander. Von weitem sieht man die Differenz der beiden
Felder, und die ist winzig. Bei verdrillten Paaren dreht sich zusätzlich alle paar Zentimeter die
Lage, und das Restfeld fällt ab einigen Zentimetern exponentiell ab.

Gerechnet für 1 mA (`feldmodell.py`):

| Abstand | Einzelleiter (Rückweg über Erde) | Paar, unverdrillt | Paar, verdrillt (Schlag 10 cm) |
|---|---|---|---|
| 2 cm | 10 nT | 0,75 nT | 0,67 nT |
| 10 cm | 2 nT | 0,03 nT | 0,001 nT |
| 30 cm | 0,67 nT | 0,003 nT | ≈ 0 |
| 1 m | 0,2 nT | 0,0003 nT | ≈ 0 |

**Beispiel:** Ader/Ader mit Brücke am Ende und 30 mA Schleifenstrom ist bei 5 cm noch ortbar, bei
10 cm nicht mehr. Derselbe Sender in Modus G mit nur 3,5 mA ist rechnerisch noch in mehreren Metern
Abstand zu finden.

**Wie wir das umgehen:** Der Rückweg darf nicht im selben Kabel liegen. Also alle Adern und den
Schirm zusammenfassen und gegen Erde einspeisen. Das machen Profigeräte genauso (Einspeisung
gegen Erdspieß).

### 1.3 Was der Schirm macht

| Schirm | Wirkung bei Einspeisung auf eine Ader | Lösung |
|---|---|---|
| am Leitungsanfang geerdet (üblich bei EMA/BMA) | Der Schirm hält das E-Feld innen. Der kapazitive Strom fließt über den Schirm zurück zur Zentrale; Hin- und Rückstrom liegen koaxial ineinander, außen bleibt fast nichts. | **Schirm am Anfang lösen** und in die Einspeisung mit aufnehmen (Modus G). |
| am fernen Ende geerdet (beidseitig) | Mit Modus G ein Glücksfall: Der Strom fließt über den Schirm bis zum fernen Ende und dort in die Erde. Das ist ein geschlossener Kreis mit vollem Strom auf der ganzen Länge. | Modus G, Frequenz „Standard“ oder „Tief“. |
| ungeerdet | Der Schirm nimmt einen Teil der Spannung mit; in Luft viel, in feuchter Wand wenig. | Modus G nimmt ihn einfach mit. |

**Ader/Schirm**, also wie du es im Auftrag genannt hast, hält dagegen alles im Kabel: Der Schirm
umschließt die Ader, außen kommt kaum etwas an. Diese Schaltung ist nur zum Identifizieren am
abisolierten Ende gut und für den Durchgangstest, nicht zum Verfolgen.

### 1.4 Was das Leitungsende ausmacht

Mit Modus G liegen alle Adern auf demselben Signal. Ob am Ende eine Brücke sitzt, ist dann egal. Es
zählt nur noch, **ob vom Ende eine Verbindung zur Erde besteht**:

- **Ohne Erde am Ende:** Der Strom fließt kapazitiv in die Umgebung (Putz, Nachbarkabel, Trasse).
  Er ist am Anfang am größten und fällt zum Ende hin auf null. Das reicht trotzdem weit
  (Kap. 9), nur die letzten 10–20 % der Länge sind schwächer.
- **Mit Erde am Ende** (Schirm dort geerdet, geerdetes Gerät, selbst gesetzte Erde, Ringleitung mit
  geerdetem zweitem Ende): Der volle Strom fließt auf der ganzen Länge. Das ist die beste Ortung.

Der Sender erkennt den Fall selbst an der Phase zwischen Strom und Spannung: kapazitiv bedeutet
offen, ohmsch bedeutet geerdet (Kap. 6.4). Er zeigt das an.

**Tipp Ringleitung (BMA-Ring):** Beide Enden liegen in der Zentrale. Ein Ende einspeisen, das andere
Ende an PE legen. Dann fließt der volle Strom einmal um den ganzen Ring, und der Rückweg ist kurz
und liegt in der Zentrale. Besser geht es nicht.

### 1.5 Wände, Decken, Trassen

| Umgebung | E-Feld | Magnetfeld |
|---|---|---|
| Luft, trockener Gipskarton | gut | gut |
| Putz, Mauerwerk, Beton | stark gedämpft (leitet bei kHz) | ungehindert |
| Stahlbeton | praktisch abgeschirmt | geht durch; die Bewehrung verzerrt das Feld (Ortung ± einige cm) |
| Kunststoff-, Gittertrasse | Nachbarkabel koppeln mit | gut; Nachbarkabel tragen Teilströme zurück (Stromrichtung hilft) |
| Blech-Kabelrinne, Stahlrohr | abgeschirmt | stark gedämpft: Stahl lässt bei 8 kHz das Feld nur ca. 0,2 mm tief eindringen. Außerdem fließt Rückstrom in der Rinne direkt neben dem Kabel. Niedrige Frequenz hilft etwas. |

---

## 2. Ortungsprinzip: Entscheidung

**Magnetfeld als Hauptprinzip, E-Feld als Spezialwerkzeug.**

- Ortung (Unterputz, Wand, Decke, Trasse von unten): **Magnetfeld**, Spulen im Sensorkopf.
- Identifikation im Bündel und in der Trasse: **E-Feld-Spitze plus Magnetfeld mit Stromrichtung**.
- Ader am abisolierten Ende: **E-Feld-Spitze** (am offenen Ende fließt kein Strom, also gibt es dort
  kein Magnetfeld).

Verworfen: nur E-Feld (Variante A, Kap. 8). Das erfüllt U-09 (Beton, Stahlbeton) und U-13
(Trassen) nicht und wäre nur ein besserer Billigsucher.

---

## 3. Einspeisemodi (S-03)

Der Sender hat drei Klemmen: **A** (rot), **B** (schwarz) und **E** (grün-gelb, Erde). Zwei kleine
Relais schalten den Modus um. Den Modus wählst du per Taste; das Display zeigt ihn mit Anschlussbild.

| Modus | Anschluss | Wofür | Geräteschutz |
|---|---|---|---|
| **G – Bündel gegen Erde** (Hauptmodus) | Alle Adern + Schirm in eine WAGO, A daran. E an PE/PAS. Schirm am Anfang von der Erde lösen. | Verfolgen, Trasse, Unterputz, Stahlbeton, lange Leitungen | Alle Adern tragen dasselbe Signal, ein Gerät am Ende sieht **0 V** zwischen seinen Anschlüssen. |
| **A – Ader gegen Rest** | Gesuchte Ader an A, alle übrigen Adern + Schirm an B, E an Erde (B und E werden intern verbunden). | Ader am abisolierten Ende finden (E-Spitze) | Spannung A–B hardwareseitig auf ca. 12 V Spitze begrenzt, Strom ≤ 20 mA |
| **P – Paar / Durchgang** | Ader A und Ader B, E frei | Durchgang, Schleifenwiderstand, Länge über Kapazität, Paar am Ende finden | wie A; Durchgangsmessung mit ≤ 1 mA und ≤ 3,3 V |

Die sechs Fälle aus dem Lastenheft, gelöst:

| Ende ↓ / Schirm → | Schirm geerdet | Schirm nicht geerdet |
|---|---|---|
| **offen** | Schirm am Anfang lösen → G. Ist er nur am fernen Ende geerdet: wie „geerdet am Ende“, also beste Ortung. | G, kapazitiver Rückweg; letzte 10–20 % schwächer |
| **gebrückt** | wie oben (die Brücke ist in G egal) | wie oben |
| **Gerät am Ende** | G; das Gerät sieht keine Spannung. Ist es geerdet, schließt es sogar den Kreis. | G, wie oben |

Zum **Identifizieren am fernen Ende** wechselt man auf Modus A (Ader) oder P (Paar).

---

## 4. Signal

### 4.1 Frequenz

| Frequenz | Vorteil | Nachteil |
|---|---|---|
| tief (≈ 0,5–1 kHz) | wenig Übersprechen, dringt besser durch Stahl, Ortung genauer | braucht Erde am Ende; bei offenem Ende 36 dB schwächer als 8 kHz; mehr 50-Hz-Oberwellen in der Nähe |
| **mittel (≈ 8 kHz)** | guter Kompromiss, offenes Ende geht, Profi-Standard für allgemeine Ortung | – |
| hoch (≈ 33 kHz) | offenes Ende 24 dB stärker als 8 kHz (Strom und Spuleninduktion steigen beide mit f) | mehr Übersprechen; Stahl schirmt stärker |

Profigeräte empfehlen: so tief wie möglich, so hoch wie nötig. Unsere Spulen sind **nicht abgestimmt**
und der Wandler im Handgerät ist breitbandig. Die Frequenz ist deshalb reine Firmware. Wir bauen
drei Frequenzsätze ein und wählen in Phase 3 nach Messung:

| Satz | Ortungston | Richtungston | Einsatz |
|---|---|---|---|
| **Standard** | 8225 Hz | 4112,5 Hz | fast immer |
| **Tief** | 1025 Hz | 512,5 Hz | Erde am Ende vorhanden, Blechrinnen, lange Leitungen mit Erde |
| **Hoch** | 32,9 kHz | – (keine Stromrichtung) | lange Leitung, Ende offen, kein Zugang zum Ende |

Die genauen Werte liegen zwischen den 50-Hz-Oberwellen, sie sind ein Firmware-Parameter. Das
Handgerät **erkennt den Satz selbst**, weil es alle Töne gleichzeitig auswertet. Ein Umstellen am
Handgerät ist also nicht nötig.

### 4.2 Zweiton und Stromrichtung (U-13)

Der Sender erzeugt die Summe aus Ortungston f und Richtungston f/2 mit **fester Phasenlage**. Im
Handgerät wird verglichen, wie die beiden Töne zueinander stehen. Kehrt sich der Strom um, kippt
dieser Vergleich um 180°.

1. Man setzt einmal eine **Referenz**: Gerät in Sendernähe über die gesuchte Leitung halten,
   Taste drücken. Das macht Radiodetection genauso („CD Reset“).
2. Danach zeigt das Display **→ HIN**, wenn der Strom so fließt wie an der Referenz, und **← ZURÜCK**
   bei umgekehrter Richtung. Bei unklarer Lage zeigt es **?**.
3. Nachbarkabel, auf die das Signal übergesprungen ist, sind meist an der Zentrale geerdet. Ihr Strom
   fließt also zurück zur Zentrale und zeigt ← ZURÜCK. **Die gesuchte Leitung trägt außerdem immer den
   größten Strom.**

Grenzen, ehrlich: Hängt ein Nachbarkabel erst am fernen Ende an Erde, fließt sein Strom ebenfalls
„hin“. Über sehr lange Strecken verschiebt sich die Phase etwas. Deshalb gibt es „?“ statt eines
falschen Pfeils, und die Richtung ist ein Hilfsmittel neben Betrag und E-Spitze. Gleichzeitig ist der
Zweiton die **Kennung** (S-17): Ein Signal ohne die passende Tonpaarung wird nicht angezeigt.

Der Pfeil bezieht sich auf das Gerät, wie bei einem Kompass. Wer das Gerät umdreht, sieht den Pfeil
umgekehrt.

### 4.3 Ausgangsspannung, Strom, Stufen

| Stufe | Spannung max. | Strom max. | Wofür |
|---|---|---|---|
| Fein | 5 V eff | 5 mA | Bündel, kurze Leitungen, wenig Übersprechen |
| **Normal** | 15 V eff | 30 mA | Standard |
| Max | 15 V eff | 60 mA | Erde am Ende, große Entfernung |

15 V eff liegen sicher unter der Grenze von 25 V eff für berührungsunkritische Schutzkleinspannung
(S-04). Der Sender regelt den Strom auf die Stufe und begrenzt die Spannung. Bei offenem Ende liegt
der Strom meist unter der Grenze; der Sender zeigt dann einfach an, was fließt.

---

## 5. Auswertung im Handgerät

### 5.1 Sensoren

| Sensor | Aufbau | Aufgabe |
|---|---|---|
| **H-Spule** (waagerecht, quer zur Leitung) | Ferritstab 10 × 80 mm, ca. 1000 Windungen 0,15 mm CuL | Signal maximal direkt über der Leitung („Spitze“), Stromrichtung |
| **V-Spule** (senkrecht) | Ferritstab 8–10 × 60 mm, ca. 1000 Windungen | Null direkt über der Leitung, Vorzeichen zeigt links/rechts |
| **E-Spitze** | Metallstift vorn am Sensorkopf, hochohmiger Verstärker | Bündel, abisolierte Adern |

Die Spulen bekommen einen **elektrostatischen Schirm** (Kupferfolie mit Schlitz, auf Masse). Sonst
mischt sich das E-Feld ins Magnetsignal und verfälscht Links/Rechts und Stromrichtung. Profigeräte
machen das auch. Die Spulen werden mit dem Akkuschrauber auf einen gedruckten Spulenkörper
gewickelt (Anleitung kommt in Phase 3).

**Empfindlichkeit (gerechnet):** 1 nT ergibt ca. 140 µV an der Spule. Die Rauschgrenze liegt bei
4 Hz Bandbreite bei ca. 0,1 pT. Wir rechnen in Kap. 9 trotzdem mit einer praktischen Schwelle von
0,1 nT, also 1000-fache Reserve für Störungen, Rückströme und unruhige Hände.

### 5.2 Signalkette

```
H-Spule ─ Vorverstärker (×1 / ×30, Hochpass) ──────────────┐
V-Spule ─ Vorverstärker (×1 / ×30, Hochpass) ─┐            ├─ PCM1808 24 bit Stereo ─ I²S ─ ESP32-S3
E-Spitze ─ Impedanzwandler ───────────────────┴─ Umschalter ┘            (ca. 90 kHz Abtastrate)
```

- **PCM1808**: 24-bit-Audio-A/D-Wandler als fertiges Modul mit Stiftleisten. Er hat 99 dB Dynamik
  und eingebaute Anti-Alias-Filter. Der interne Wandler des ESP32 (12 bit, max. 83 kS/s, deutlich
  verrauscht) kann das nicht.
- **Verstärkung umschaltbar** ×1 / ×30 (Analogschalter CD4053): ×30 für Entfernung, ×1 direkt am Kabel.
- **Kanal 2** schaltet zwischen V-Spule (Ortung) und E-Spitze (Bündel / Ader) um.

### 5.3 Digitale Lock-in-Auswertung (statt Bandpass oder Goertzel)

| Verfahren | Wie | Bewertung |
|---|---|---|
| Analoger Bandpass + Gleichrichter (Billig-Tonsucher) | Filter einige 100 Hz breit, Pegel hören | Alles, was in den Bereich fällt, piept mit; keine Phase |
| Goertzel | eine einzelne Frequenz blockweise berechnen | gut, aber in Blöcken; Bandbreite an Blocklänge gebunden |
| **Lock-in (IQ-Demodulation)** | Signal fortlaufend mit Sinus und Kosinus der Sendefrequenz multiplizieren, Tiefpass 2–10 Hz | **gewählt:** Betrag und Phase fortlaufend, Bandbreite frei einstellbar, ca. 50 Messwerte pro Sekunde |

Mit 2–10 Hz Bandbreite statt einigen hundert Hz sinkt der Störpegel um 20–30 dB. Alles, was nicht
genau auf unserer Frequenz liegt, fällt raus: 50 Hz samt Oberwellen, Schaltnetzteile (20–150 kHz),
LED-Treiber. Die Quarze von Sender und Handgerät weichen höchstens ca. 0,2 Hz voneinander ab; das
liegt in der Bandbreite. Weil das Handgerät auch die Phase kennt, bekommen wir ohne Zusatzhardware:

- **Links/Rechts:** Vorzeichen von V gegenüber H.
- **Null-Ortung:** V wird direkt über der Leitung null. Das ist viel schärfer als das Maximum
  (Genauigkeit ±1 cm bei Unterputz).
- **Tiefe (45°-Methode, H-15):** Seitlich gehen, bis |V| = |H| (Display zeigt „45°“). Der Abstand
  zur Nullstelle ist dann die Tiefe. Das ist die bewährte Profi-Handmethode, ± ca. 15 %.
- **Stromrichtung:** Kap. 4.2.
- **Störcheck:** Pegel neben unseren Frequenzen messen; das Display warnt bei Störquellen.
  Bei Bedarf wechselt man den Frequenzsatz.
- **Anzeige in nT:** Nach einer einmaligen Kalibrierung mit dem Sender (bekannter Strom in einem
  Draht, bekannter Abstand) ist der Zahlenwert eine echte Flussdichte (A-01). Kennt man die Tiefe,
  kann das Gerät daraus sogar den **Strom in der Leitung in mA** schätzen.

---

## 6. Sender im Detail

### 6.1 Blockschaltbild

```
4×AA ─ Ein/Aus (selbsthaltend) ─┬─ 3,3 V ─ ESP32-S2 mini ─ OLED 1,3″, 3 Tasten
                                └─ Hochsetzsteller 24 V ─┐
ESP32-DAC ─ Tiefpass ─ Endstufe (L272, Brücke) ──────────┘─ Strommessung ─ Schutz ─ Relais (Modus) ─ A / B / E
                                                     │                │
                                     Spannung, Strom, Phase → ESP32-ADC     Durchgangsmessung (1 mA)
```

### 6.2 Signalerzeugung und Endstufe

- **ESP32-S2-DAC** (8 bit, per DMA): erzeugt die Tonsumme aus einer Tabelle. 8 bit reichen für einen
  Sender; die Stufen glättet ein Tiefpass.
- **Endstufe L272** (Doppel-Leistungs-OPV, DIP-8, bis 28 V, Ruhestrom ca. 8 mA) in Brückenschaltung an
  24 V aus einem Hochsetzsteller-Modul. Das ergibt max. ca. 42 V Spitze-Spitze = 15 V eff.
- **Ausgang:** Koppelkondensator (kein Gleichstrom auf der Leitung), Serienwiderstand,
  rückstellende Sicherung (PTC), Suppressordioden (TVS).

### 6.3 Schutz (S-05, S-12, U-07)

| Fall | Was schützt |
|---|---|
| Kurzschluss (dauerhaft) | Firmware regelt den Strom innerhalb weniger ms herunter. Dahinter Serienwiderstand + PTC, dahinter die thermische Abschaltung des L272. |
| Fremdspannung bis 60 V DC / 50 V AC | TVS-Dioden begrenzen, der PTC löst aus, die Endstufe ist über den Serienwiderstand entkoppelt. Der Ausgang bleibt gesperrt. |
| Gerät am Ende | G: keine Spannung am Gerät. A/P: Spannung per TVS hart auf ca. 12 V Spitze begrenzt, Strom ≤ 20 mA (auch bei Verpolung unkritisch). |

### 6.4 Was der Sender misst und anzeigt

Der Sender misst Spannung und Strom am Ausgang nach Betrag und Phase, mit Goertzel auf dem
ESP32-ADC. Daraus ergeben sich:

- **Strom in mA** (S-10): zeigt sofort, ob überhaupt etwas fließt.
- **Lastbild:** z. B. „2,1 kΩ ∠ −84° → kapazitiv → Ende offen, C ≈ 9 nF“ oder
  „35 Ω ∠ 0° → Erde am Ende“.
- **Längenschätzung** im Modus P bei offenem Paar: C ÷ 100 nF/km (J-Y(St)Y 0,8 mm laut Datenblatt)
  → „≈ 120 m“. Ehrlich gekennzeichnet als Schätzung mit Annahme, ± ca. 25 %.
- **Erde prüfen:** Ist der Strom in Modus G verdächtig klein (nur wenige pF), meldet der Sender
  „Erde prüfen“.

### 6.5 Fremdspannungswarnung (S-13): **ja, als Sperre**

- Vor jedem Einschalten des Ausgangs und bei jedem Moduswechsel misst der Sender DC und AC an den
  Klemmen. Ab 3 V sperrt er den Ausgang und zeigt „FREMDSPANNUNG 24,3 V DC“.
- **Er zeigt nie „spannungsfrei“ an.** Unter 3 V zeigt er einfach nichts dazu. Eine Klemme ohne
  Kontakt misst 0 V; daraus darf niemand Spannungsfreiheit ableiten. Das ersetzt keinen
  Spannungsprüfer, und so steht es auch in der Anleitung.
- Begründung: Die Messung ist zuverlässig, solange sie nur **warnt** und nie **bestätigt**. Sie
  schützt außerdem den Sender selbst. So vermittelt sie keine falsche Sicherheit.

### 6.6 Durchgangs-/Schleifentest (S-14): **ja**

- Gleichstrom-Messung mit ≤ 1 mA, ≤ 3,3 V, Bereich 0,1 Ω … 1 MΩ. Das ist für jedes Gerät am Ende
  harmlos.
- Anzeige: Wert in Ω und Deutung. „Gebrückt: 7,4 Ω ≈ 2 × 51 m bei 0,8 mm“ (73,2 Ω/km
  Schleife laut Datenblatt), „Abschlusswiderstand? 12,0 kΩ“, „offen“.
- Misst mit beiden Polaritäten. Weichen die Werte ab, steckt ein Halbleiter oder Gerät am Ende. Dann
  zeigt er „Gerät am Ende? 4,7 kΩ / 1,2 kΩ“ statt einer falschen Deutung.

### 6.7 Bedienung, Versorgung

- 3 Taster (Modus, Stufe, Messen/OK) und Ein/Aus. OLED 1,3″ (gleich wie Handgerät, A-03).
- 4 × AA. Rechnung: ca. 0,6–0,8 W im Mittel bei Stufe Normal ergibt ca. 12 h mit NiMH 2000 mAh
  (S-16 ≥ 8 h).
- Abschaltung 2 h nach der letzten Bedienung (30 min / 8 h wählbar). Eine Minute vorher piept er.

---

## 7. Handgerät im Detail

### 7.1 Blockschaltbild

```
Sensorkopf: H-Spule, V-Spule, E-Spitze ─ Vorverstärker ─ PCM1808 ─ I²S ─ ESP32-S3
ESP32-S3: Kern 1 = Lock-in-Rechnung, Kern 2 = Anzeige + Ton + Bedienung
          → OLED 1,3″ (SPI, flüssige Grafik)   → Ton: Sinus → Verstärker PAM8302 → Lautsprecher 28 mm
          ← Drehgeber mit Taster, 2 Taster (Modus, Referenz/Tiefe)
4×AA ─ Ein/Aus (selbsthaltend) ─ Hochsetzsteller ─ rauscharmer 5-V-Regler (Analogteil) ; 3,3 V vom Board
```

### 7.2 Bauform (F-17): kompakter Paddel-Stab, ca. 22 cm

- **Unten der Sensorkopf** (ca. 90 × 40 × 25 mm): H-Spule quer, V-Spule senkrecht in der Mitte,
  E-Spitze vorn mittig.
- **Darüber ein Hals** von ca. 6 cm: Abstand zwischen Spulen und Digitalteil. Display, Prozessor und
  Spannungswandler stören sonst die Spulen.
- **Griff** mit 4 × AA (2 × 2), oben Display schräg zum Auge, Drehgeber unter dem Daumen, Lautsprecher
  oben. Eine Metallfläche am Griff verbindet deine Hand mit der Schaltungsmasse. Das macht die
  E-Spitze deutlich empfindlicher, weil dein Körper als Gegenpol dient.
- Länge ca. 22 cm, passt auf 25,6 cm Bauraum, druckbar in zwei Halbschalen ohne Stützmaterial
  (Phase 7).
- **Bündel-Modus:** Gerät waagerecht halten und mit der Spitze die Kabel abtasten.

### 7.3 Anzeige (A-01 … A-03) und Ton

- **Ortung:** großer Wert in nT, Balken mit Spitzenwert-Halten, Links/Rechts-Pfeil, Stromrichtung,
  Verlaufsgraph der letzten 2 s beim Überstreichen (man sieht die „Beule“ über der Leitung).
- **Bündel:** E-Pegel in dB (als relativ gekennzeichnet), großer Stromrichtungspfeil.
- **Tiefe:** geführte 45°-Methode.
- **Ton:** Tonhöhe steigt mit dem Pegel. Der Ton ist ein sauberer Sinus; Rechteck-Oberwellen könnten
  in unseren Messbereich fallen.
- Mockups der Bildschirme: siehe visuelle Übersicht.

### 7.4 Versorgung

4 × AA (wie Sender, gleiche Ersatzzellen). Ca. 0,45 W ergibt ca. 20 h mit NiMH. Abschaltung nach
10 min ohne Bedienung **und** ohne Signal.

---

## 8. Varianten

| | A – „Verbesserter Tonsucher“ | B – „Magnetisch einfach“ | **C – „Profi-Prinzip“ (Empfehlung)** |
|---|---|---|---|
| Einspeisung | Ader gegen Erde, Rechteck ~1 kHz | Modus G, ein Ton 8,2 kHz | Modus G/A/P, Zweiton, 3 Frequenzsätze |
| Handgerät | E-Feld-Sonde, Analogfilter, ESP-ADC | 1 Spule + E-Spitze, ESP-ADC (12 bit) | 2 Spulen + E-Spitze, 24-bit-Wandler, Lock-in |
| Durch Beton/Stahlbeton | nein | ja | ja |
| Trasse (U-13) | „alles piept“ | nur Betrag | Betrag + Stromrichtung + E-Spitze |
| Links/Rechts, Null, Tiefe | nein | nein | ja |
| Anzeige in echten Einheiten | nein | eingeschränkt | ja (nT, mA, Ω, nF) |
| Sender misst Last / Länge | nein | Strom | Strom, Lastbild, Länge, Durchgang |
| Firmware-Aufwand | gering | mittel | hoch (übernehme ich) |
| **Kosten pro Satz (Schätzung)** | **ca. 37 €** | **ca. 45 €** | **ca. 50 €** |
| Erfüllt das Lastenheft | nein (H-03, U-09, U-13) | teilweise (U-13 schwach) | ja |

**Empfehlung: Variante C.** Der Aufpreis gegenüber B beträgt nur ca. 5 €. Den Großteil der Kosten
machen Displays, Platinen, Batteriehalter und Messleitungen aus, und die braucht jede Variante. Für
diese ca. 5 € bekommst du Stromrichtung, Links/Rechts, Null und Tiefe sowie Messwerte, denen man
trauen kann.

---

## 9. Realistische Reichweiten

Modus G, Frequenzsatz Standard, Stufe Normal. „Rechnung“ steht für das Feldmodell mit 0,1 nT Schwelle.
„Erwartung“ ist das, was ich dir in der Praxis zusage. Sie ist bewusst vorsichtiger, weil Rückströme in
Nachbarleitern und Metall das Feld schwächen und verzerren. Gemessen wird in Phase 3.

| Situation | Strom (Rechnung) | Rechnung | **Erwartung** | Einfacher Tonsucher (E-Feld, typisch) |
|---|---|---|---|---|
| Unterputz, 30 m, Ende offen | 7 mA am Anfang | > 1 m | **sicher ab ≥ 50 cm Abstand, ±1 cm genau** | 1–5 cm |
| Hohlwand / Gipskarton | ähnlich | > 1 m | **≥ 50 cm** | 5–15 cm |
| Beton, 20 cm | ähnlich | > 1 m | **durch, ±2 cm** | kaum |
| Stahlbeton, 20 cm Decke | ähnlich | > 1 m | **durch, ±3–5 cm (Verzerrung)** | nein |
| Kunststoff-/Gittertrasse, 100 m, Ende offen, von unten | 7,7 mA → 0 am Ende | 7,7 m (Mitte) / 1,5 m (90 %) | **1–2 m über ca. 80 % der Länge; das letzte Stück von der Leiter** | nur direkt an der Trasse |
| Trasse, Ende geerdet | 30 mA | sehr weit | **≥ 2 m auf ganzer Länge** | – |
| Blech-Kabelrinne von unten | – | – | **stark eingeschränkt (0–50 cm); an der Rinne arbeiten**, Satz „Tief“ testen | nur an der Rinne |
| 500 m, Ende offen | 19 mA am Anfang | 3,9 m bei 90 % der Länge | **≥ 1 m bis ca. 90 % der Länge** | – |
| Identifikation in voller Trasse | – | – | **richtige Leitung an der Trasse sicher** (Betrag + Richtung + E-Spitze) | Ratespiel |
| Ader am abisolierten Ende | – | – | **eindeutig** (Modus A, E-Spitze) | geht |

**Zu deinen 500 m:** Nicht die Länge ist das Problem, sondern der Rückweg. In Modus G treibt der
Sender bei 500 m sogar mehr Strom als bei 30 m, weil die Leitung mehr Kapazität hat. Rechnerisch ist
500 m kein Problem. Ich halte es für erreichbar; T-11 prüft das.

**Wo es nicht geht:** Leitungen im Stahlrohr; Blech-Kabelrinnen aus größerer Entfernung; Kabel, die
parallel zu einem geerdeten Leiter laufen, der den Rückstrom direkt daneben zurückführt.

---

## 10. Zielwerte für die Abnahmetests

Werden nach Freigabe ins Lastenheft (Kap. 9) übernommen.

| Test | Zielwert |
|---|---|
| T-01 Freie Leitung 50 m am Boden, Ende offen | Erkennungsabstand in Leitungsmitte **≥ 1,0 m** (Rechnung 1,5 m) |
| T-02 wie T-01, Ende geerdet | **≥ 2 m** |
| T-03 Unterputz | Erkennungsabstand **≥ 0,5 m** ab Wand; Ortungsgenauigkeit **±1 cm** bis 5 cm Tiefe (Null-Methode) |
| T-04 Stahlbeton | Leitung in 20 cm Decke ortbar, **±3 cm** |
| T-05 Trasse ≥ 10 Leitungen | richtige Leitung in **≥ 9 von 10** Versuchen; Kunststoff-/Gittertrasse von unten aus **≥ 1 m** über **≥ 80 %** der Länge |
| T-06 Ader am Ende | **10 von 10** |
| T-07 Störquellen in 30 cm | ohne Sender keine Anzeige über der Erkennungsschwelle; mit Sender Abweichung **≤ 3 dB** |
| T-08 Kurzschluss 1 h | kein Schaden, Anzeige „Kurzschluss“, Strom ≤ Stufengrenze |
| T-09 12-V-Gerät am Ende, auch verpolt | kein Schaden; G: **0 V** am Gerät; A/P: **≤ 12 V Spitze** |
| T-10 60 V DC / 50 V AC | kein Schaden; Warnung und Sperre **< 1 s** |
| T-11 300 m (Muss) / 500 m (Soll), Ende offen | Sender stabil; Leitung bei 90 % der Länge aus **≥ 0,5 m** ortbar |
| T-12 Laufzeit | Sender **≥ 8 h** (Normal, NiMH 2000 mAh), Handgerät **≥ 15 h** |
| T-13 Vergleich IPC-Tester | **≥ 3-facher** Erkennungsabstand an einer Unterputzleitung |
| T-14 Durchgang | offen > 1 MΩ; Brücke **±(2 % + 0,5 Ω)**; 1–50 kΩ **±2 %** |
| T-15 NYM | wie T-03 |
| T-16 Anzeige | Strom ±5 % ±0,1 mA; Spannung ±3 %; nT nach Kalibrierung ±20 %; Länge aus Kapazität ±25 % |

---

## 11. Budget: Entscheidung nötig

Schätzung nach Preisen chinesischer Händler (AliExpress, LCSC), ±20 %. Bei Reichelt liegen die
Preise ca. 30–50 % höher. Die genaue Stückliste folgt in Phase 4.

| Posten | Sender | Handgerät |
|---|---|---|
| Mikrocontroller-Modul | ESP32-S2 mini ≈ 3 € (hast du schon) | ESP32-S3 SuperMini ≈ 3,50 € |
| Display OLED 1,3″ | ≈ 3 € | ≈ 3 € |
| Wandler / Endstufe | L272 + Hochsetzsteller + OPV ≈ 2,50 € | PCM1808-Modul + 2 OPV + CD4053 ≈ 5 € |
| Sensorik | – | 2 Ferritstäbe + Kupferlackdraht ≈ 3,50 € |
| Schutz, Relais, Messung | ≈ 2,50 € | – |
| Ton | Summer ≈ 0,30 € | Verstärker + Lautsprecher ≈ 1,50 € |
| Messleitungen, Klemmen, 4-mm-Buchse | ≈ 3,50 € | – |
| Batteriehalter, Ein/Aus, Regler | ≈ 1,50 € | ≈ 2,50 € |
| Taster, Drehgeber | ≈ 0,50 € | ≈ 1 € |
| Kleinteile, Stiftleisten, Gewindeeinsätze | ≈ 3 € | ≈ 2,50 € |
| **Summe Bauteile** | **≈ 20 €** | **≈ 22,50 €** |
| Platinen (JLCPCB, beide Designs zusammen ≈ 9 € inkl. Versand) | ≈ 4,50 € | ≈ 4,50 € |
| **Summe je Gerät** | **≈ 24,50 €** | **≈ 27 €** |

**Gesamt ca. 50 € pro Satz (Variante C).** Davon ist der ESP32-S2 mini (≈ 3 €) schon bezahlt; deine
tatsächliche Ausgabe liegt also bei ca. 47 €.

Sparmöglichkeiten, falls du unter 45 € bleiben willst:

- 0,96″-OLED im Sender statt 1,3″ (−1 €; dann sieht er etwas anders aus als das Handgerät)
- 4-mm-Buchse weglassen (−0,50 €)
- Teile, die du ohnehin hast (Widerstände, Kondensatoren, Stiftleisten), sparen ca. 3–5 €

**Frage an dich: Ist Variante C mit ca. 50 € pro Satz für dich okay?**

---

## 12. Risiken und Programm für den Machbarkeitstest (Phase 3)

| Risiko | Was Phase 3 prüft |
|---|---|
| PCM1808 als Slave am ESP32-S3 (in Foren gab es Probleme mit dem Takt) | Modul zuerst allein in Betrieb nehmen, Abtastrate und Rauschen messen |
| Selbstgewickelte Spulen: Empfindlichkeit, E-Feld-Einstreuung | Spule wickeln, mit bekanntem Strom (Draht + Sender) kalibrieren, mit und ohne Schirm vergleichen |
| Display, Prozessor und Spannungswandler stören die Spulen | Störpegel bei verschiedenen Abständen messen → Mindestlänge des Halses |
| Reichweite und Übersprechen in echten Trassen | Messungen an deiner Testtrommel und in einer echten Trasse, Sätze Tief/Standard/Hoch im Vergleich |
| Zuverlässigkeit der Stromrichtung | Bündel aus 5–10 Kabeln, Nachbarn an einer Seite geerdet: Wie oft stimmt der Pfeil? |
| 8-bit-DAC als Signalquelle | Spektrum am Ausgang mit dem Oszilloskop (oder am Handgerät) prüfen |
| L272 erhältlich und stabil an kapazitiver Last | an 10–100 nF Last testen |

Für Phase 3 brauche ich keine Platine. Steckbrett, die Module und zwei gewickelte Spulen genügen. Die
Messwerte schaut man sich im **seriellen Plotter der Arduino IDE** an. Dein Oszilloskop braucht kein
FFT, das Handgerät rechnet selbst.

---

## 13. Was sich im Lastenheft ändert, wenn du freigibst

- **S-13** (Fremdspannungswarnung): umgesetzt als Sperre ohne „spannungsfrei“-Anzeige.
- **S-14** (Durchgang): umgesetzt, plus Lastbild und Längenschätzung.
- **H-14** (Links/Rechts) und **H-15** (Tiefe): umgesetzt ohne Zusatzhardware.
- **Kap. 9:** Zielwerte aus Kap. 10 werden eingetragen.
- **Neu:** Handgerät erkennt den Frequenzsatz selbst; Anzeige des Feldes in nT.

---

## Quellen

- Radiodetection, *Current Direction (CD)* und *Understanding CD*:
  <https://support.radiodetection.com/hc/en-gb/articles/360000382503-Current-Direction-CD>,
  <https://support.radiodetection.com/hc/en-gb/articles/360052920031-Understanding-CD>
- Radiodetection RD8100, Frequenzen und Frequenzwahl:
  <https://www.radiodetection.com/sites/default/files/product-downloads/rd8100-locator-specv4.pdf>,
  <https://www.manualslib.com/manual/2312544/Radiodetection-Rd8100.html>
- J-Y(St)Y-Kennwerte (Schleifenwiderstand 73,2 Ω/km, Betriebskapazität 100 nF/km bei 0,8 mm):
  <https://www.hardy-schmitz.de/HS/Downloadables/Documents/ProductMedia/HSMAN_KABEL_1114XXX_DATENBLATT_JYSTY_KABEL.pdf>
- ESP32-S2-Fähigkeiten (ADC max. 83 333 S/s, 12 bit; 1 × I²S; DAC vorhanden), ESP-IDF `soc_caps.h`:
  <https://github.com/espressif/esp-idf/blob/v5.3/components/soc/esp32s2/include/soc/soc_caps.h>
- ESP32-S2 DAC im DMA-Betrieb: <https://docs.espressif.com/projects/esp-idf/en/v5.3/esp32s2/api-reference/peripherals/dac.html>
- PCM1808 am ESP32 (Takt, Slave-Betrieb): <https://esp32.com/viewtopic.php?p=121953>
