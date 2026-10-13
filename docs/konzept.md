# Konzept Kabelsuchgerät (Phase 2)

| | |
|---|---|
| **Version** | 2.0 – Variante „Einfach“ (Änderungen: Kap. 14); v1.0 (Variante C) freigegeben am 13.10.2026 |
| **Stand** | 13.10.2026 |
| **Grundlage** | [Lastenheft v2.0](lastenheft.md); Freigabe der Variante „Einfach“ durch Julius am 13.10.2026 (E-034, [`variante-einfach.md`](variante-einfach.md)) |
| **Rechnung** | [`berechnungen/feldmodell.py`](berechnungen/feldmodell.py) – Feld- und Reichweitenzahlen; [`../firmware/machbarkeit/test_host/sim_test.cpp`](../firmware/machbarkeit/test_host/sim_test.cpp) – Simulation der Auswertung im Handgerät |
| **Visuelle Übersicht** | [`konzept-uebersicht.html`](konzept-uebersicht.html) (Link siehe README; zeigt noch den Stand v1.0) |

> **Was sich mit v2.0 geändert hat**
>
> 1. **Variante „Einfach“ statt Variante C** (E-034): ca. 31 € statt ca. 50 € pro Satz (auf Lochraster). Hardware wie
>    Variante B, Software wie Variante C.
> 2. **Handgerät: eine Spule** quer im Kopf, direkt unter der Abtastfläche oben (E-033, E-036). Zweite
>    Spule und E-Feld-Spitze entfallen; Platz für eine zweite Spule ist vorbereitet.
> 3. **Wandler im ESP32-S3** mit zwei Verstärkerstufen und automatischer Bereichswahl statt PCM1808
>    (E-035). Zweiton, Lock-in und Stromrichtung bleiben.
> 4. **Anzeige in dB** (bezogen auf 1 µV an der Spule) statt nT (E-039). Links/Rechts, Null-Methode und
>    Tiefenführung entfallen; geortet wird über das Maximum.
> 5. **Sender mit zwei Klemmen A und E**, ohne Relais (E-037). Modi G und A entstehen durch das
>    Anklemmen, Modus P entfällt. Nur noch der Frequenzsatz Standard.
> 6. **Sender misst nur den Strom** (ca. ±10 %) und nimmt ab 60 mA den Pegel zurück (E-038).
>    Fremdspannungssperre, Durchgang, Lastbild und Längenschätzung entfallen. Der Schutz nach S-12 bleibt.
> 7. **Bedienung:** Sender eine Taste PEGEL (FEIN/NORMAL/MAX), Handgerät zwei Tasten REF und MODUS, Piezo
>    statt Lautsprecher (E-040).
> 8. **Phase 3 ist ein Mini-Test mit den endgültigen Teilen** (E-041), Messungen M1–M8 in
>    [`machbarkeitstest.md`](machbarkeitstest.md).
> 9. **Aufbau auf Lochraster** statt eigener Platine (E-042). Ein fertig gerouteter KiCad-Entwurf liegt als
>    Alternative bereit (E-044, [`platine-oder-lochraster.md`](platine-oder-lochraster.md)); Julius entscheidet.
> 10. **Signalausgang des Senders an GPIO18** statt GPIO17 (E-043): äußere Stiftreihe des S2 mini.

---

## 0. Kurzfassung

1. **Wir orten über das Magnetfeld.** Es geht ungehindert durch Putz, Mauerwerk, Beton und
   (mit Verzerrung) Stahlbeton. Das elektrische Feld, mit dem einfache Tonsucher arbeiten, wird
   von feuchten Baustoffen, Bewehrung und geerdeten Schirmen geschluckt. Das Handgerät hat deshalb
   nur eine Spule; eine E-Feld-Spitze gibt es seit v2.0 nicht mehr.
2. **Ein Magnetfeld nach außen gibt es nur, wenn der Strom nicht im selben Kabel zurückfließt.**
   Deshalb speisen wir im Hauptmodus **alle Adern und den Schirm gemeinsam gegen Erde** ein
   („Modus G“). Der Rückweg läuft über die Gebäudeerde, weit weg vom Kabel. Bei Ader/Ader heben
   sich Hin- und Rückstrom auf; das Feld ist nach rund 10 cm praktisch verschwunden (Kap. 1.2).
3. In Modus G **schrumpfen die sechs Fälle aus dem Lastenheft auf zwei**: Hat das ferne Ende eine
   Verbindung zur Erde (Schirm dort geerdet, geerdetes Gerät, selbst gesetzte Erde) oder nicht?
   Gebrückt oder offen spielt dann keine Rolle mehr, und **angeschlossene Geräte sehen keine
   Spannung**, weil alle Adern dasselbe Signal tragen.
4. **Signal:** zwei fest gekoppelte Töne, 8333,3 Hz zum Orten und 4166,7 Hz für die Richtung. Drei
   Pegel FEIN, NORMAL und MAX mit ca. 5, 9 und 17 V Spitze. Der Sender misst den Strom und nimmt ab
   60 mA den Pegel zurück. Weitere Frequenzsätze lassen sich später per Software nachrüsten.
5. **Stromrichtung (wie bei Profigeräten):** Auf der gesuchten Leitung fließt der Strom „hin“, auf
   Nachbarkabeln, auf die das Signal übergesprungen ist, meist „zurück“. Das Handgerät zeigt das
   als Pfeil. Das ist der Schlüssel für volle Trassen (U-13). Mit einer Spule gilt: Display immer
   zu dir (Kap. 4.2).
6. **Handgerät:** ein Stab mit dem Sensorkopf oben. Eine Ferritspule liegt quer direkt unter der
   Abtastfläche. Zwei Verstärkerstufen gehen auf zwei Eingänge des Wandlers im ESP32-S3. Die Firmware
   wertet beide per Lock-in aus und wählt den Bereich selbst. Anzeige: Pegel in dB (bezogen auf 1 µV an
   der Spule), Balken, Stromrichtung. Geortet wird über das Maximum.
7. **Sender:** zwei Klemmen A und E, keine Relais. Er zeigt Strom in mA, Pegel und Batterie und
   übersteht 60 V DC / 50 V AC an den Klemmen. Durchgangsmessung, Lastbild und Fremdspannungssperre gibt
   es nicht mehr.
8. **Mikrocontroller:** Sender mit deinem ESP32-S2 mini (hat den nötigen D/A-Wandler), Handgerät mit
   ESP32-S3 SuperMini (Gleitkomma-Rechenwerk, zwei Kerne). Alles THT oder Modul, je Gerät eine
   Lochrasterplatine (E-042); ein KiCad-Entwurf für bestellte Platinen liegt als Alternative bereit (E-044).
9. **Reichweite (realistisch):** Unterputz und Hohlwand sicher ab ≥ 50 cm Abstand, durch 20 cm
   Stahlbeton, in Kunststoff- und Gittertrassen von unten aus 1–2 m über den größten Teil der
   Länge. Blech-Kabelrinnen schirmen stark ab; dort arbeitet man direkt an der Rinne. Die Rechnung
   dahinter nimmt mehr Sendespannung an, als die Endstufe liefert; Kap. 9 zeigt die Folgen, der
   Mini-Test misst nach.
10. **Budget:** ca. 31 € pro Satz auf Lochraster, du zahlst ca. 28 €, weil der ESP32-S2 mini vorhanden ist
    (E-034, E-042). Mit bestellten Platinen wären es ca. 38–41 €. Das Ziel von 40 € ist eingehalten (Kap. 11).

---

## 1. Die Physik in einfachen Worten

### 1.1 Zwei Felder, zwei Werkzeuge

| | Elektrisches Feld (E-Feld) | Magnetfeld (H-Feld) |
|---|---|---|
| Entsteht durch | **Spannung** auf der Leitung | **Strom** in der Leitung |
| Sensor im Handgerät | Elektrode (Metallfläche, Spitze); entfällt in v2.0 | Spule auf Ferritstab |
| Braucht am Leitungsende | nichts, geht auch bei offenem Ende | einen Stromkreis (Erde am Ende oder genug Kapazität) |
| Wird blockiert von | geerdetem Schirm, Bewehrung, feuchtem Putz und Beton, Metallrinnen | praktisch nichts außer dicken Stahlblechen; Bewehrung verzerrt nur |
| Stark bei | Bündel, abisolierte Adern, trockene Hohlwand, in Luft | Tiefe, Beton, Stahlbeton, Trassen von unten |
| Liefert Richtungsinformation | nein | ja: links/rechts, Tiefe, Stromrichtung (v2.0 nutzt die Stromrichtung) |

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
| am fernen Ende geerdet (beidseitig) | Mit Modus G ein Glücksfall: Der Strom fließt über den Schirm bis zum fernen Ende und dort in die Erde. Das ist ein geschlossener Kreis mit vollem Strom auf der ganzen Länge. | Modus G. |
| ungeerdet | Der Schirm nimmt einen Teil der Spannung mit; in Luft viel, in feuchter Wand wenig. | Modus G nimmt ihn einfach mit. |

**Ader/Schirm**, also wie du es im Auftrag genannt hast, hält dagegen alles im Kabel: Der Schirm
umschließt die Ader, außen kommt kaum etwas an. Zum Verfolgen taugt diese Schaltung nicht.

### 1.4 Was das Leitungsende ausmacht

Mit Modus G liegen alle Adern auf demselben Signal. Ob am Ende eine Brücke sitzt, ist dann egal. Es
zählt nur noch, **ob vom Ende eine Verbindung zur Erde besteht**:

- **Ohne Erde am Ende:** Der Strom fließt kapazitiv in die Umgebung (Putz, Nachbarkabel, Trasse).
  Er ist am Anfang am größten und fällt zum Ende hin auf null. Das reicht trotzdem weit
  (Kap. 9), nur die letzten 10–20 % der Länge sind schwächer.
- **Mit Erde am Ende** (Schirm dort geerdet, geerdetes Gerät, selbst gesetzte Erde, Ringleitung mit
  geerdetem zweitem Ende): Der volle Strom fließt auf der ganzen Länge. Das ist die beste Ortung.

Der Sender zeigt den Strom in mA. Mit Erde am Ende fließt meist deutlich mehr als bei offenem Ende.
Eine Deutung „offen“ oder „geerdet“ zeigt er seit v2.0 nicht mehr an (Lastbild entfällt, E-038).
Fließt kaum Strom, erscheint „kaum Strom: Erde?“.

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
| Blech-Kabelrinne, Stahlrohr | abgeschirmt | stark gedämpft: Stahl lässt bei 8 kHz das Feld nur ca. 0,2 mm tief eindringen. Außerdem fließt Rückstrom in der Rinne direkt neben dem Kabel. Niedrige Frequenz hilft etwas (Satz „Tief“ entfällt in v2.0, per Software nachrüstbar). |

---

## 2. Ortungsprinzip: Entscheidung

**Magnetfeld.** Die E-Feld-Spitze aus v1.0 entfällt (E-036).

- Ortung (Unterputz, Wand, Decke, Trasse von unten): **Magnetfeld**, eine Spule im Sensorkopf.
- Identifikation im Bündel und in der Trasse: **Magnetfeld mit Stromrichtung** und
  Empfindlichkeitsstufen.
- Ader am abisolierten Ende: **Multimeter.** Ader am Anfang auf Erde legen, am Ende gegen Erde
  durchmessen. Am offenen Ende fließt kein Strom, also gibt es dort kein Magnetfeld; dafür war in
  v1.0 die E-Feld-Spitze gedacht.

Verworfen: nur E-Feld (Variante A, Kap. 8). Das erfüllt U-09 (Beton, Stahlbeton) und U-13
(Trassen) nicht und wäre nur ein besserer Billigsucher.

---

## 3. Einspeisemodi (S-03)

Der Sender hat zwei Klemmen: **A** (rot) und **E** (grün-gelb, Erde bzw. Rückleiter). Es gibt keine
Umschaltung und keine Relais. Der Modus entsteht dadurch, wie du anklemmst (E-037). Der Sender weiß
deshalb nicht, welcher Modus anliegt; die Bedienungsanleitung beschreibt beide.

| Modus | Anschluss | Wofür | Geräteschutz |
|---|---|---|---|
| **G – Bündel gegen Erde** (Hauptmodus) | Alle Adern + Schirm in eine WAGO, A daran. E an PE/PAS. Schirm am Anfang von der Erde lösen. | Verfolgen, Trasse, Unterputz, Stahlbeton, lange Leitungen, Identifikation in der Trasse | Alle Adern tragen dasselbe Signal, ein Gerät am Ende sieht **0 V** zwischen seinen Anschlüssen. Alle Pegel erlaubt. |
| **A – Ader gegen Rest** | Gesuchte Ader an A. Alle übrigen Adern + Schirm in eine WAGO an E, E zusätzlich an Erde. | eine einzelne Ader gezielt ansteuern (Identifikation am Ende, E-016). Wie bei Ader/Schirm (Kap. 1.3) bleibt das Feld großteils im Kabel; zum Verfolgen ist G besser. | Ein Gerät am Ende sieht die Spannung A–E. Mit Gerät höchstens Pegel **NORMAL** (≤ 12 V Spitze, U-07). MAX nur in Modus G. |
| ~~P – Paar / Durchgang~~ | entfällt in v2.0 (E-037) | Durchgang misst man mit dem Multimeter. | – |

Die sechs Fälle aus dem Lastenheft, gelöst:

| Ende ↓ / Schirm → | Schirm geerdet | Schirm nicht geerdet |
|---|---|---|
| **offen** | Schirm am Anfang lösen → G. Ist er nur am fernen Ende geerdet: wie „geerdet am Ende“, also beste Ortung. | G, kapazitiver Rückweg; letzte 10–20 % schwächer |
| **gebrückt** | wie oben (die Brücke ist in G egal) | wie oben |
| **Gerät am Ende** | G; das Gerät sieht keine Spannung. Ist es geerdet, schließt es sogar den Kreis. | G, wie oben |

Die richtige **Ader am abisolierten Ende** findet man mit dem Multimeter (E-036).

---

## 4. Signal

### 4.1 Frequenz

| Frequenz | Vorteil | Nachteil |
|---|---|---|
| tief (≈ 0,5–1 kHz) | wenig Übersprechen, dringt besser durch Stahl, Ortung genauer | braucht Erde am Ende; bei offenem Ende 36 dB schwächer als 8 kHz; mehr 50-Hz-Oberwellen in der Nähe |
| **mittel (≈ 8 kHz)** | guter Kompromiss, offenes Ende geht, Profi-Standard für allgemeine Ortung | – |
| hoch (≈ 33 kHz) | offenes Ende 24 dB stärker als 8 kHz (Strom und Spuleninduktion steigen beide mit f) | mehr Übersprechen; Stahl schirmt stärker |

Profigeräte empfehlen: so tief wie möglich, so hoch wie nötig. Unsere Spule ist **nicht abgestimmt**,
die Frequenz ist deshalb Firmware. In v1.0 waren drei Frequenzsätze geplant. **Seit v2.0 gibt es nur
den Satz Standard** (E-037). Die Werte kommen exakt aus dem D/A-Takt (E-029):

| Satz | Ortungston | Richtungston | Stand v2.0 |
|---|---|---|---|
| **Standard** | 8333,3 Hz (200 kHz / 24) | 4166,7 Hz (200 kHz / 48) | **gilt** |
| Prüfton | 1041,7 Hz (50 kHz / 48), reiner Sinus | – | nur im Test, zum Nachmessen mit dem Multimeter |
| Tief | 1041,7 Hz | 520,8 Hz | entfällt, per Software nachrüstbar |
| Hoch | 33 333 Hz | – | entfällt (siehe unten) |

Die Töne liegen zwischen den 50-Hz-Oberwellen; die nächste liegt 16,7 Hz neben dem Ortungston
(8350 Hz). Der Sender kann alle Sätze erzeugen. Im Handgerät ginge „Tief“ ohne Umbau. „Hoch“ liegt
über der halben Abtastrate des Handgeräts (ca. 27 kHz je Kanal, also ca. 13,5 kHz) und ginge so nicht.
Das Handgerät sucht die Frequenz selbst (Taste REF lang) und führt sie nach (Kap. 5.3).

### 4.2 Zweiton und Stromrichtung (U-13)

Der Sender erzeugt die Summe aus Ortungston f und Richtungston f/2 mit **fester Phasenlage**. Im
Handgerät wird verglichen, wie die beiden Töne zueinander stehen. Kehrt sich der Strom um, kippt
dieser Vergleich um 180°.

1. Man setzt einmal eine **Referenz**: Gerät in Sendernähe über die gesuchte Leitung halten,
   Taste REF kurz drücken. Das macht Radiodetection genauso („CD Reset“).
2. Danach zeigt das Display **→ HIN**, wenn der Strom so fließt wie an der Referenz, und **← ZURÜCK**
   bei umgekehrter Richtung. Bei unklarer Lage zeigt es **?**, ohne Referenz **REF?**.
3. Nachbarkabel, auf die das Signal übergesprungen ist, sind meist an der Zentrale geerdet. Ihr Strom
   fließt also zurück zur Zentrale und zeigt ← ZURÜCK. **Die gesuchte Leitung trägt außerdem immer den
   größten Strom.**

Grenzen, ehrlich: Hängt ein Nachbarkabel erst am fernen Ende an Erde, fließt sein Strom ebenfalls
„hin“. Über sehr lange Strecken verschiebt sich die Phase etwas. Deshalb gibt es „?“ statt eines
falschen Pfeils, und die Richtung ist ein Hilfsmittel neben dem Betrag. Gleichzeitig ist der
Zweiton die **Kennung** (S-17): Ein Signal ohne die passende Tonpaarung soll nicht angezeigt werden.
Die Testfirmware zeigt ohne Richtungston nur keinen Pfeil; die Sperre setzt Phase 6 um.

**Haltung (wichtig mit einer Spule):** Der Pfeil bezieht sich auf das Gerät, wie bei einem Kompass.
Mit einer Spule hängt er davon ab, wie herum man das Gerät hält (E-036). Wer das Gerät umdreht,
sieht den Pfeil umgekehrt. Regel: **Display immer zu dir.** Beim Referenz-Setzen und beim Suchen
das Gerät gleich halten.

### 4.3 Ausgangsspannung, Strom, Pegel

| Pegel | Anteil am D/A-Bereich | Spannung A–E (Spitze, Tonsumme) | Wofür | Erlaubt |
|---|---|---|---|---|
| FEIN | 25 % | ≈ 5 V | Bündel, kurze Leitungen, wenig Übersprechen | G und A |
| **NORMAL** | 50 % | ≈ 9 V | Standard | G und A |
| MAX | 90 % | ≈ 17 V | Erde am Ende, große Entfernung | **nur G** |

Der Ortungston allein hat zwei Drittel der Spitzenspannung, weil sich die Spitzen von Ortungston und
halb so großem Richtungston addieren. Bei MAX sind das ca. 11 V Spitze, also ca. 8 V eff; bei NORMAL
ca. 4 V eff. Alle Pegel liegen weit unter der Grenze von 25 V eff für berührungsunkritische
Schutzkleinspannung (S-04).

Der Strom ergibt sich aus der Last. Der Sender regelt ihn nicht, er misst ihn (Kap. 6.4). Ab 60 mA
nimmt die Firmware den Pegel eine Stufe zurück, auf der kleinsten Stufe schaltet sie ab. Bei offenem
Ende fließt meist deutlich weniger; der Sender zeigt dann einfach an, was fließt.

---

## 5. Auswertung im Handgerät

### 5.1 Sensor

| Sensor | Aufbau | Aufgabe |
|---|---|---|
| **Spule** (quer im Kopf, direkt unter der Abtastfläche) | Ferritstab 10 × 80 mm, 1000 Windungen 0,15 mm CuL, Kupferfolienschirm mit Spalt | Signal maximal direkt über der Leitung, Stromrichtung |
| zweite Spule | nicht bestückt; Platz und Wandlereingänge GPIO5/GPIO6 sind vorbereitet (E-036) | später Links/Rechts oder eine kleine Tastspule für Bündel |
| ~~senkrechte Spule~~, ~~E-Feld-Spitze~~ | entfallen in v2.0 (E-036) | – |

Die Spule bekommt einen **elektrostatischen Schirm** (Kupferfolie mit Spalt, auf Masse). Sonst
mischt sich das E-Feld ins Magnetsignal und verfälscht Pegel und Stromrichtung. Profigeräte
machen das auch. Die Spule wird mit dem Akkuschrauber auf einen gedruckten Spulenkörper gewickelt
(Anleitung im Machbarkeitstest).

**Empfindlichkeit (gerechnet):** 1 nT ergibt bei 8,3 kHz ca. 140 µV an der Spule. Die Rauschgrenze
der Elektronik liegt laut Simulation (Kap. 5.3) bei ca. 0,001 nT; die Spule selbst rauscht noch
weniger. Wir rechnen in Kap. 9 trotzdem mit einer praktischen Schwelle von 0,1 nT. Die setzen
Störfelder in Gebäuden, Rückströme und unruhige Hände, nicht die Elektronik.

### 5.2 Signalkette

```
Spule ─┬─ R6 22 kΩ (Dämpfung, parallel zur Spule)
       └─ R7 1 kΩ + C6 100 pF ─ Stufe 1: MCP6002, ×5,7, Hochpass 340 Hz ─┬─ 1 kΩ + 1 nF ─ GPIO1 (NIEDRIG)
                                                                         └─ Stufe 2: ×40 ─ 1 kΩ + 1 nF ─ GPIO2 (HOCH)
Batterie ─ 100 kΩ / 47 kΩ ─ GPIO4 (BATT)
ESP32-S3: ADC1, 12 bit, reihum NIEDRIG / HOCH / BATT, zusammen 81 000 Wandlungen/s → ca. 27 kHz je Kanal
```

- **Zwei Bereiche:** NIEDRIG hat die Verstärkung ×5,7, HOCH zusammen ≈ ×228. Beide Kanäle laufen immer
  mit. Die Firmware nimmt HOCH, solange er nicht übersteuert, sonst NIEDRIG. Das genaue Verhältnis
  lernt sie im Betrieb nach (Bauteiltoleranzen), damit der Pegel beim Umschalten nicht springt. Das
  gibt gut 30 dB mehr Aussteuerbereich als eine Stufe allein, wichtig direkt an der Leitung.
- **Wandler:** ADC1 des ESP32-S3 mit 12 dB Dämpfung, Messbereich ca. 0–3,1 V. Die tatsächliche
  Abtastrate misst die Firmware beim Start und rechnet damit. Die RC-Glieder 1 kΩ + 1 nF vor den
  Eingängen sind die Ladungsreserve für den Wandler.
- **Analogversorgung:** 3,3 V vom Modul über 10 Ω + 100 µF. Ruhepegel VREF = 1,65 V.
- **Warum kein PCM1808 mehr** (E-035): Variante C brauchte drei Sensoren gleichzeitig mit großer
  Dynamik und genauer Phase (Links/Rechts, Tiefe, nT-Werte, E-Spitze). Das entfällt. Für eine Spule
  mit Pegel und Stromrichtung reicht der interne Wandler. Der PCM1808 kostete mehr und brauchte
  Verdrahtung und Taktleitungen.

### 5.3 Digitale Lock-in-Auswertung (statt Bandpass oder Goertzel)

| Verfahren | Wie | Bewertung |
|---|---|---|
| Analoger Bandpass + Gleichrichter (Billig-Tonsucher) | Filter einige 100 Hz breit, Pegel hören | Alles, was in den Bereich fällt, piept mit; keine Phase |
| Goertzel | eine einzelne Frequenz blockweise berechnen | gut, aber in Blöcken; Bandbreite an Blocklänge gebunden |
| **Lock-in (IQ-Demodulation)** | Signal fortlaufend mit Sinus und Kosinus der Sendefrequenz multiplizieren, dann glätten | **gewählt:** Betrag und Phase fortlaufend, Bandbreite frei einstellbar, ca. 50 Messwerte pro Sekunde |

Mit wenigen Hz Bandbreite statt einigen hundert Hz sinkt der Störpegel um 20 dB und mehr. Alles, was
nicht genau auf unserer Frequenz liegt, fällt raus: 50 Hz samt Oberwellen, Schaltnetzteile
(20–150 kHz), LED-Treiber.

So arbeitet die Firmware (`lockin.h`, getestet in der Testfirmware des Mini-Tests):

- **Blöcke von 20 ms**, danach eine zweistufige Glättung: 2 × 0,06 s (schnell) oder 2 × 0,3 s (ruhig).
  Zwei Stufen trennen eine Netzoberwelle 16,7 Hz neben dem Ortungston doppelt so stark (in dB) wie
  eine Stufe.
- **Beide Bereiche gleichzeitig:** NIEDRIG und HOCH laufen durch den Lock-in; die Bereichswahl
  (Kap. 5.2) nimmt den passenden.
- **Frequenzsuche und Nachführung:** Der Takt des Senders kann etwas neben dem Sollwert liegen. Das
  Handgerät sucht die Frequenz deshalb selbst (REF lang, Gerät nah an die Leitung) und führt sie
  danach fortlaufend nach.
- **Stromrichtung:** aus der Phase von Ortungs- und Richtungston (Kap. 4.2).
- **Rauschmessung:** Pegel auf einer leeren Frequenz 100 Hz neben dem Ortungston. Daraus ergeben sich
  der Signal-Rausch-Abstand (SNR) und „kein Signal“, wenn der Pegel weniger als 6 dB über dem Rauschen
  liegt.
- **Entfällt in v2.0:** Links/Rechts, Null-Ortung und die geführte Tiefenmessung (dafür fehlt die
  zweite Spule), Anzeige in nT (E-039). nT gibt die Testfirmware nur über die serielle Schnittstelle
  aus, zum Kalibrieren der Spule an einem bekannten Strom.

**Simulation** (`firmware/machbarkeit/test_host/sim_test.cpp`): Sie bildet die Kette Spule → Stufe 1 →
Stufe 2 → 12-bit-Wandler nach, mit Verstärker- und Wandlerrauschen (3 LSB), Anschlag und Zeitversatz
zwischen den reihum abgetasteten Kanälen. Die Rohwerte laufen durch denselben Code wie im Gerät. Alle
7 Tests sind bestanden:

| Prüfung | Ergebnis |
|---|---|
| Frequenzsuche, Signal 4,7 Hz neben dem Nennwert | auf 0,01 Hz genau |
| Pegel nach der Suche | ±0,1 % |
| Bereichswahl 10 nT → 300 nT → 10 nT | Wechsel auf NIEDRIG und zurück ohne Pegelsprung |
| Stromrichtung, Strom umgedreht | richtig, auch nach dem Bereichswechsel |
| Frequenzdrift 0,3 Hz/s über 10 s | Nachführung folgt |
| Netzoberwellen 8300/8350 Hz so stark wie das Signal | Pegel schwankt um höchstens 0,2 % |
| 0,1 nT, ruhige Glättung | SNR 44 dB |
| Rauschgrenze der Elektronik | ca. 0,001 nT |

Die SNR-Anzeige sättigt bei ca. 60 dB: Das Spiegelbild eines sehr starken Signals sickert etwa 60 dB
gedämpft in die Rauschmessung ein. Was die Simulation nicht kennt, sind Störlinien und ungleichmäßige
Abtastung des echten S3-Wandlers. Das prüft der Mini-Test (Kap. 12).

---

## 6. Sender im Detail

### 6.1 Blockschaltbild

```
4×AA ─ S1 Ein/Aus ─ 1N5817 ─ VBAT ─┬─ 5-V-Pin ESP32-S2 mini ─ OLED 1,3″ (I²C, GPIO33/35), Taste PEGEL (GPIO7)
                                    ├─ 100 kΩ / 47 kΩ ─ GPIO5 (Batterie)
                                    └─ MT3608 → 24,0 V ─ TVS P6KE30A
GPIO18 (D/A) ─ Tiefpass 2 × (2,2 kΩ / 2,2 nF) ─ Brücke TL072 + BD139/BD140 an 24 V
     ├─ Zweig A ─ 10 µF bipolar ─ 47 Ω 2 W ─ PTC 0,1 A ─ Klemme A (rot)
     └─ Zweig B ─ 10 µF bipolar ─ 47 Ω 2 W ─ PTC 0,1 A ─ Klemme E (grün-gelb)
     Klemmdioden 1N4148 an beiden Brückenausgängen gegen +24 V und GND
PNP-Kollektoren (PGND) ─ 10 Ω ─ GND;  RC 10 kΩ / 4,7 µF ─ GPIO3 (Strom)
```

Keine Relais, keine Umschaltung, keine Messung von Spannung oder Phase am Ausgang.

### 6.2 Signalerzeugung und Endstufe

- **D/A-Wandler des ESP32-S2** (8 bit, per DMA, Kanal 2 an GPIO18 auf der äußeren Stiftreihe, E-043): gibt die Tonsumme aus einer Tabelle aus, Takt
  200 kHz. Der Prüfton 1041,7 Hz (reiner Sinus, Takt 50 kHz) ist nur für den Test da: Viele Multimeter
  zeigen bei 8 kHz falsch an.
- **Tiefpass** 2 × (2,2 kΩ / 2,2 nF): glättet die Stufen des D/A-Wandlers.
- **Endstufe** (E-028): je Brückenzweig ein Operationsverstärker des TL072 mit einem
  Gegentakt-Emitterfolger aus BD139 (NPN) und BD140 (PNP) in der Gegenkopplung, Versorgung 24,0 V aus
  dem MT3608. Jeder Zweig liefert das 6,45-Fache, der zweite gegenphasig; an A–E liegt also das
  12,9-Fache. Daraus ergeben sich die Pegel aus Kap. 4.3.
- **Ausgang je Klemme:** 10 µF bipolar (≥ 63 V, kein Gleichstrom auf der Leitung), 47 Ω 2 W, PTC
  0,1 A. Der 47-Ω-Widerstand entkoppelt die kapazitive Kabellast, damit die Endstufe nicht schwingt.

### 6.3 Schutz (S-05, S-12, U-07)

| Fall | Was schützt |
|---|---|
| Kurzschluss (dauerhaft) | Zweimal 47 Ω begrenzen den Strom. Die Firmware misst ihn laufend; liegt er länger als ca. 0,3 s über 60 mA, nimmt sie den Pegel eine Stufe zurück („STROMBEGRENZUNG“). Auf der kleinsten Stufe schaltet sie den Ausgang ab („KURZSCHLUSS?“). Dahinter die PTC. |
| Fremdspannung bis 60 V DC / 50 V AC | Die Koppelkondensatoren sperren Gleichspannung. Bei Wechselspannung begrenzen 47 Ω und PTC den Strom, die Klemmdioden 1N4148 leiten ihn an den Brückenausgängen auf +24 V bzw. GND, die TVS-Diode P6KE30A hält die 24-V-Schiene. Keine Warnung, keine Sperre (E-038). |
| Gerät am Ende | G: keine Spannung am Gerät. A: Das Gerät sieht die Spannung A–E, deshalb mit Gerät höchstens NORMAL (≈ 9 V Spitze, ≤ 12 V). Kein Gleichstrom, Verpolung daher unkritisch. |

Anders als in v1.0 begrenzt in Modus A keine Hardware die Spannung auf 12 V. Der Sender kennt den
Modus nicht. Bei MAX zeigt er „MAX: nur gegen Erde!“. Phase 4 klärt, ob das reicht.

### 6.4 Was der Sender misst und anzeigt

- **Strom in mA** (S-10): Ein 10-Ω-Messwiderstand liegt im Rückweg der beiden PNP-Transistoren (PGND).
  Durch ihn fließt der Betrag des Ausgangsstroms. Das RC-Glied 10 kΩ / 4,7 µF glättet, GPIO3 misst
  den Mittelwert. Die Firmware rechnet ihn über den Formfaktor der Tonsumme in mA eff um. Den
  Nullpunkt misst sie beim Start mit stillem Ausgang. Genauigkeit ca. ±10 % (E-038).
- **Batteriespannung:** Teiler 100 kΩ / 47 kΩ an GPIO5.
- **Anzeige** (OLED 1,3″, SH1106, I²C): Frequenz, Strom in mA mit großen Ziffern, Strombalken
  logarithmisch 0,1–100 mA, Pegel, Batterie. Hinweiszeile: „kaum Strom: Erde?“, „MAX: nur gegen
  Erde!“, „STROMBEGRENZUNG“, „KURZSCHLUSS?“.
- **Entfällt (E-038):** Lastbild aus Betrag und Phase, Erkennung „offen/geerdet“, Längenschätzung.

### 6.5 Fremdspannungswarnung (S-13): entfällt (E-038)

In v1.0 als Sperre ohne „spannungsfrei“-Anzeige geplant. Mit der Variante „Einfach“ entfällt sie:
Die Spannungsfreiheit stellst du vor dem Anklemmen ohnehin fest. Der Schutz nach S-12 bleibt
(Kap. 6.3). Der Sender zeigt nie „spannungsfrei“ an.

### 6.6 Durchgangs-/Schleifentest (S-14): entfällt (E-038)

Durchgang und Schleifenwiderstand misst man mit dem Multimeter. Lastbild und Längenschätzung
entfallen mit.

### 6.7 Bedienung, Versorgung

- **Eine Taste PEGEL:** kurz = FEIN → NORMAL → MAX, lang = Ausgang aus/ein. Dazu der Schiebeschalter
  S1 (Ein/Aus). OLED 1,3″ wie im Handgerät (A-03).
- **Regel:** MAX nur in Modus G. In Modus A mit angeschlossenem Gerät höchstens NORMAL (E-040).
- **Versorgung:** 4 × AA → S1 → 1N5817 → VBAT → 5-V-Pin des S2 mini und MT3608 (auf 24,0 V
  eingestellt). USB nur bei ausgeschaltetem S1 anstecken, sonst speist die Batterie in den Laptop.
- **Laufzeit** (S-16, ≥ 8 h bei NORMAL): Phase 4 rechnet sie mit den Strömen aus dem Mini-Test (M1)
  nach.
- **Automatische Abschaltung** (S-08): kommt in Phase 4/6. Der ESP32 schaltet den 24-V-Wandler ab und
  geht in den Tiefschlaf. Details offen; im Testaufbau hängt der MT3608 noch fest an VBAT.

---

## 7. Handgerät im Detail

### 7.1 Blockschaltbild

```
Sensorkopf: Spule (quer, unter der Abtastfläche) ─ R6/R7/C6 ─ Stufe 1 ×5,7 ─┬─ GPIO1 (NIEDRIG)
                                                                          └─ Stufe 2 ×40 ─ GPIO2 (HOCH)
ESP32-S3 SuperMini: Kern 0 = Wandler + Lock-in, Kern 1 = Anzeige, Ton, Tasten
          → OLED 1,3″ (I²C, GPIO8/9)   → Piezo (GPIO12, über 100 Ω)
          ← Taste REF (GPIO10), Taste MODUS (GPIO11)
4×AA ─ S1 Ein/Aus ─ 1N5817 ─ 5-V-Pin;  Batterie über 100 kΩ / 47 kΩ ─ GPIO4
3,3 V vom Modul ─ 10 Ω + 100 µF ─ Analogteil (MCP6002), VREF 1,65 V
Reserve: GPIO5/GPIO6 für eine zweite Spule
```

### 7.2 Bauform (F-17, E-033): Stab mit Sensorkopf oben

- **Oben der Sensorkopf** mit der Abtastfläche auf der Stirnseite. Die Spule liegt quer direkt unter
  der Fläche. Der Kopf ist dafür ca. 92 mm breit (80-mm-Ferritstab). Platz für eine zweite Spule ist
  vorgesehen.
- **Darunter Display und Tasten**, **unten der Griff** mit 4 × AA.
- **Abstand Spule – Digitalteil ≥ 6 cm.** Display, Prozessor und Leitungen stören sonst die Spule.
  Den nötigen Abstand misst M4.
- Man hält das Gerät wie eine Taschenlampe gegen Wand oder Decke und schaut dabei aufs Display.
- Spule und Batterie kommen über Stecker auf die Platine. Die Maße legt Phase 7 fest, wenn die
  Platine feststeht (G-08). Ansicht: [`../gehaeuse/konzept/`](../gehaeuse/konzept/).
- Entfallen mit der E-Feld-Spitze: Metallfläche am Griff und Bündel-Modus mit Spitze.

**So wird gesucht:**

- **Ortung über das Maximum:** Kopf quer zur Leitung schwenken; der lauteste Punkt ist die Leitung.
- **Verlauf der Leitung:** Gerät auf der Stelle um 90° drehen. Liegt die Spule längs zur Leitung,
  wird das Signal minimal.
- **Tiefe nur als Handgriff** (45°-Methode, H-15): Gerät quer zur Leitung um 45° kippen und seitlich
  weggehen, bis das Signal minimal wird; der Abstand zur Leitung entspricht ungefähr der Tiefe. Die
  Software führt dabei nicht. Beschreibung in der Bedienungsanleitung (Phase 8).

### 7.3 Anzeige (A-01 … A-03), Ton und Bedienung

- **Anzeige** (OLED 1,3″, SH1106, I²C):
  - Kopfzeile: Frequenz, Empfindlichkeit AUTO oder E1–E5, Ton-Symbol, Batterie.
  - Groß: **Pegel in dB, bezogen auf 1 µV an der Spule** (echter Messwert, A-01). Doppelter Strom
    ergibt +6 dB.
  - Rechts: Stromrichtung HIN / ZURÜCK, „?“ bei unklarer Lage, „REF?“ ohne Referenz.
  - Balken: Fenster von 40 dB mit Spitzenwert-Strich. In AUTO folgt die obere Grenze dem Maximum
    sofort nach oben und langsam nach unten; E1 (empfindlich) bis E5 (grob) sind feste Fenster.
  - Fußzeile: SNR und aktiver Bereich NIEDRIG/HOCH, sonst „kein Signal“ oder „UEBERSTEUERT“.
  - nT nur in der seriellen Testausgabe zum Kalibrieren (M3). Weitere Grafik (z. B. Verlaufsgraph,
    A-02) klärt Phase 6.
- **Ton:** passiver Piezo an GPIO12 über 100 Ω. Die Tonhöhe steigt mit dem Balken von 300 bis 3000 Hz
  und ist auf Vielfache von 50 Hz gerundet. Seine Oberwellen liegen dann wie die Netzoberwellen neben
  der Sendefrequenz und stören die Auswertung nicht.
- **Tasten:**
  - REF: kurz = Referenz für die Stromrichtung setzen (nahe am Sender), lang = Frequenzsuche.
  - MODUS: kurz = Empfindlichkeit AUTO → E1 … E5, lang = Ton laut → leise → aus.
- Mockups der Bildschirme in der visuellen Übersicht zeigen noch den Stand v1.0.

### 7.4 Versorgung

4 × AA wie im Sender (gleiche Ersatzzellen): Schiebeschalter → 1N5817 → 5-V-Pin des Moduls. Die
Batteriespannung misst der Wandler über den Teiler 100 kΩ / 47 kΩ. USB nur bei ausgeschaltetem
Schalter anstecken. Die Laufzeit (T-12: ≥ 15 h) rechnet Phase 4 mit den gemessenen Strömen nach. Die
Abschaltung nach 10 min ohne Bedienung **und** ohne Signal (H-08) klärt Phase 4/6.

---

## 8. Varianten

| | A – „Verbesserter Tonsucher“ | B – „Magnetisch einfach“ | C – „Profi-Prinzip“ (Empfehlung v1.0, ersetzt durch E-034) | **„Einfach“ (beschlossen, E-034)** |
|---|---|---|---|---|
| Einspeisung | Ader gegen Erde, Rechteck ~1 kHz | Modus G, ein Ton 8,2 kHz | Modus G/A/P, Zweiton, 3 Frequenzsätze | Modus G/A durch Anklemmen, Zweiton, nur Satz Standard |
| Handgerät | E-Feld-Sonde, Analogfilter, ESP-ADC | 1 Spule + E-Spitze, ESP-ADC (12 bit) | 2 Spulen + E-Spitze, 24-bit-Wandler, Lock-in | 1 Spule, ESP-ADC (12 bit, 2 Bereiche), Lock-in |
| Durch Beton/Stahlbeton | nein | ja | ja | ja |
| Trasse (U-13) | „alles piept“ | nur Betrag | Betrag + Stromrichtung + E-Spitze | Betrag + Stromrichtung |
| Links/Rechts, Null, Tiefe | nein | nein | ja | nein (zweite Spule vorbereitet, Tiefe als Handgriff) |
| Anzeige in echten Einheiten | nein | eingeschränkt | ja (nT, mA, Ω, nF) | ja (dB bezogen auf 1 µV, mA) |
| Sender misst Last / Länge | nein | Strom | Strom, Lastbild, Länge, Durchgang | Strom |
| Firmware-Aufwand | gering | mittel | hoch (übernehme ich) | hoch (Software aus C) |
| **Kosten pro Satz (Schätzung)** | **ca. 37 €** | **ca. 45 €** | **ca. 50 €** | **ca. 31 €** (Lochraster), ca. 38–41 € mit Platinen |
| Erfüllt das Lastenheft | nein (H-03, U-09, U-13) | teilweise (U-13 schwach) | ja (v1.1) | ja (v2.0) |

**Empfehlung in v1.0: Variante C – durch E-034 ersetzt.** Begründung damals: Der Aufpreis gegenüber
B beträgt nur ca. 5 €, weil Displays, Platinen, Batteriehalter und Messleitungen den Großteil der
Kosten ausmachen.

**Beschlossen: Variante „Einfach“** (E-034). Julius war C zu teuer und zu aufwendig. „Einfach“ nimmt
die Hardware von B (eine Spule, Wandler im ESP32) und die Software von C (Zweiton, Lock-in,
Stromrichtung). Die Stromrichtung für volle Trassen bleibt, weil sie nur eine Spule braucht und reine
Software ist. Die Reichweite hängt vor allem an Einspeisung, Strom und Spule, und die bleiben gleich.
Was wegfällt und wie man es ersetzt: [`variante-einfach.md`](variante-einfach.md), Kap. 3.

---

## 9. Realistische Reichweiten

Modus G, Frequenzsatz Standard. „Rechnung“ steht für das Feldmodell mit 0,1 nT Schwelle und 15 V eff
am Ausgang (siehe Hinweis unter der Tabelle). „Erwartung“ ist das, was ich dir in der Praxis zusage.
Sie ist bewusst vorsichtiger, weil Rückströme in Nachbarleitern und Metall das Feld schwächen und
verzerren. Einspeisung und Spule sind in v2.0 gleich geblieben. Gemessen wird im Mini-Test (M5, M6).

| Situation | Strom (Rechnung) | Rechnung | **Erwartung** | Einfacher Tonsucher (E-Feld, typisch) |
|---|---|---|---|---|
| Unterputz, 30 m, Ende offen | 7 mA am Anfang | > 1 m | **sicher ab ≥ 50 cm Abstand, ±2 cm genau (Maximum)** | 1–5 cm |
| Hohlwand / Gipskarton | ähnlich | > 1 m | **≥ 50 cm** | 5–15 cm |
| Beton, 20 cm | ähnlich | > 1 m | **durch, ±2 cm** | kaum |
| Stahlbeton, 20 cm Decke | ähnlich | > 1 m | **durch, ±3–5 cm (Verzerrung)** | nein |
| Kunststoff-/Gittertrasse, 100 m, Ende offen, von unten | 7,7 mA → 0 am Ende | 7,7 m (Mitte) / 1,5 m (90 %) | **1–2 m über ca. 80 % der Länge; das letzte Stück von der Leiter** | nur direkt an der Trasse |
| Trasse, Ende geerdet | 30 mA | sehr weit | **≥ 2 m auf ganzer Länge** | – |
| Blech-Kabelrinne von unten | – | – | **stark eingeschränkt (0–50 cm); an der Rinne arbeiten** | nur an der Rinne |
| 500 m, Ende offen | 19 mA am Anfang | 3,9 m bei 90 % der Länge | **≥ 1 m bis ca. 90 % der Länge** | – |
| Identifikation in voller Trasse | – | – | **Betrag + Stromrichtung an einzeln greifbaren Kabeln** (T-05) | Ratespiel |
| Ader am abisolierten Ende | – | – | **mit dem Multimeter** (E-036) | geht |

**Hinweis zur Sendespannung:** `feldmodell.py` rechnet mit 15 V eff am Ausgang. Die Endstufe liefert
weniger (Kap. 4.3): Der Ortungston hat bei MAX ca. 8 V eff, bei NORMAL ca. 4 V eff. Das galt schon für
den Testaufbau von Variante C (gleiche Endstufe, gleiche Pegel) und ist keine Folge der Vereinfachung.
Bei offenem Ende ist der Strom proportional zur Spannung. Mit 8 V eff (MAX) ergibt dasselbe Modell:

| Fall, Ende offen | Strom am Anfang | Rechnung bei 50 % / 90 % der Länge |
|---|---|---|
| Unterputz, 30 m | 3,7 mA | 3,7 m / 0,7 m |
| Kunststoff-/Gittertrasse, 100 m | 4,1 mA | 4,1 m / 0,8 m (bei 80 %: ca. 1,6 m) |
| Lange Leitung, 300 m | 6,2 mA | 6,2 m / 1,2 m |
| Lange Leitung, 500 m | 10,3 mA | 10,3 m / 2,1 m |
| Freie Leitung am Boden, 50 m (T-01) | 0,8 mA | 0,8 m / 0,2 m |

Bei NORMAL halbieren sich diese Werte noch einmal ungefähr. Mit Erde am Ende ist der Strom meist
deutlich größer; der Sender zeigt ihn an. Folgen:

- Bei MAX sind die Erwartungen oben noch durch die Rechnung gedeckt, aber mit weniger Reserve.
- **T-01** (≥ 1,0 m in der Mitte einer freien 50-m-Leitung) liegt rechnerisch nicht mehr sicher drin.
  Ob die Schwelle von 0,1 nT im Freien niedriger liegt, zeigt M5.
- Phase 4 klärt, ob die Endstufe mehr Spannung liefern soll (S-04 erlaubt bis 25 V eff).

**Zu deinen 500 m:** Nicht die Länge ist das Problem, sondern der Rückweg. In Modus G treibt der
Sender bei 500 m sogar mehr Strom als bei 30 m, weil die Leitung mehr Kapazität hat. Rechnerisch ist
500 m kein Problem. Ich halte es für erreichbar; T-11 prüft das.

**Wo es nicht geht:** Leitungen im Stahlrohr; Blech-Kabelrinnen aus größerer Entfernung; Kabel, die
parallel zu einem geerdeten Leiter laufen, der den Rückstrom direkt daneben zurückführt.

---

## 10. Zielwerte für die Abnahmetests

Stand v2.0, gleich wie im Lastenheft (Kap. 9).

| Test | Zielwert |
|---|---|
| T-01 Freie Leitung 50 m am Boden, Ende offen | Erkennungsabstand in Leitungsmitte **≥ 1,0 m** (Rechnung 1,5 m bei 15 V eff; mit dem Ortungston bei MAX ca. 0,8 m, siehe Kap. 9) |
| T-02 wie T-01, Ende geerdet | **≥ 2 m** |
| T-03 Unterputz | Erkennungsabstand **≥ 0,5 m** ab Wand; Ortungsgenauigkeit **±2 cm** bis 5 cm Tiefe (Maximum-Methode) |
| T-04 Stahlbeton | Leitung in 20 cm Decke ortbar, **±3 cm** |
| T-05 Trasse ≥ 10 Leitungen | Kunststoff-/Gittertrasse von unten aus **≥ 1 m** über **≥ 80 %** der Länge; an der Trasse Stromrichtung an einzeln greifbaren (≥ 5 cm abgespreizten) Kabeln **≥ 9 von 10** richtig |
| T-06 Ader am Ende | entfällt: Multimeter (E-036) |
| T-07 Störquellen in 30 cm | ohne Sender keine Anzeige über der Erkennungsschwelle; mit Sender Abweichung **≤ 3 dB** |
| T-08 Kurzschluss A–E, 1 h | kein Schaden; Anzeige „STROMBEGRENZUNG“ bzw. „KURZSCHLUSS?“; Strom **≤ 60 mA eff** |
| T-09 12-V-Gerät am Ende, auch verpolt | kein Schaden; G: **0 V** am Gerät; A mit Pegel NORMAL: **≤ 12 V Spitze** |
| T-10 60 V DC / 50 V AC an A–E | kein Schaden (Warnung und Sperre entfallen) |
| T-11 300 m (Muss) / 500 m (Soll), Ende offen | Sender stabil; Leitung bei 90 % der Länge aus **≥ 0,5 m** ortbar |
| T-12 Laufzeit | Sender **≥ 8 h** (NORMAL, NiMH 2000 mAh), Handgerät **≥ 15 h** |
| T-13 Vergleich IPC-Tester | **≥ 3-facher** Erkennungsabstand an einer Unterputzleitung |
| T-14 Durchgang | entfällt mit S-14 (E-038) |
| T-15 NYM | nur Ortung, wie T-03 |
| T-16 Anzeige | Strom **±10 % ±0,3 mA** gegen Multimeter mit Prüfton 1 kHz; Batteriespannung **±3 %**; Pegelanzeige **+6 dB ±1 dB** bei doppeltem Strom |

Warum sich Zielwerte gegenüber v1.0 geändert haben:

- **T-03:** ±2 cm statt ±1 cm, weil die Null-Methode mit der zweiten Spule entfällt. Das Maximum ist
  breiter als die Null.
- **T-05:** Ohne feine Spitze lassen sich dicht liegende Kabel nicht einzeln abtasten. Der Zielwert
  für die Stromrichtung gilt deshalb für Kabel, die man ≥ 5 cm abspreizen kann.
- **T-08, T-09, T-10, T-16:** Strombegrenzung per Firmware statt Stufengrenze, kein Modus P, keine
  Fremdspannungssperre, Strommessung ±10 %, Anzeige in dB statt nT.

---

## 11. Budget

Entschieden mit E-034. Schätzung nach Preisen chinesischer Händler (AliExpress, LCSC), ±20 %. Bei
Reichelt liegen die Preise ca. 30–50 % höher. Die genaue Stückliste folgt in Phase 4.

| Posten | Sender | Handgerät |
|---|---|---|
| Mikrocontroller-Modul | ESP32-S2 mini ≈ 3 € (hast du schon) | ESP32-S3 SuperMini ≈ 3,50 € |
| Display OLED 1,3″ | ≈ 3 € | ≈ 3 € |
| Endstufe / Verstärker | TL072, BD139/BD140, MT3608 ≈ 2,50 € | MCP6002 ≈ 0,50 € |
| Sensor | – | Ferritstab + Kupferlackdraht ≈ 2 € |
| Schutz und Messung | PTC, Klemmdioden, TVS, 47 Ω, Kondensatoren, Messwiderstand ≈ 1,80 € | – |
| Ton | – | Piezo ≈ 0,30 € |
| Messleitungen mit Krokodilklemmen | ≈ 2,50 € | – |
| Batteriehalter, Schalter, Tasten | ≈ 1,80 € | ≈ 2,10 € |
| Kleinteile, Stiftleisten | ≈ 2 € | ≈ 1,50 € |
| **Summe Teile** | **≈ 16 €** | **≈ 13 €** |
| Lochrasterplatinen (2 Stück, 2,54 mm) | ≈ 2 € | |
| **Gesamt pro Satz** | **≈ 31 €** | |

**Gesamt ca. 31 € pro Satz** auf Lochraster (E-042). Davon ist der ESP32-S2 mini (≈ 3 €) schon bezahlt;
du zahlst also ca. 28 €. Das Ziel von 40 € (B-01) ist eingehalten. Filament ist nicht eingerechnet (B-03).
Das Sender-Display kostet ca. 3 € (V-2).

**Alternative bestellte Platinen** (KiCad-Entwurf, E-044): ca. 9–12 € für je 5 Stück inkl. Versand statt
ca. 2 €, also ca. 38–41 € pro Satz. Vergleich und Empfehlung: [`platine-oder-lochraster.md`](platine-oder-lochraster.md).

Zum Vergleich: Variante C lag in v1.0 bei ca. 50 € pro Satz (E-027, ersetzt durch E-034).

---

## 12. Risiken und Mini-Test (Phase 3)

Phase 3 ist ein Mini-Test mit den endgültigen Teilen (E-041): einmal bestellen, auf dem Steckbrett
testen, danach auf die Lochrasterplatine (oder bestellte Platine, E-044) übernehmen. Geprüft wird nur, was
neu oder unsicher ist. Messungen
M1–M8 und Erfolgskriterien K1–K6 stehen in [`machbarkeitstest.md`](machbarkeitstest.md).

| Risiko | Was der Mini-Test prüft |
|---|---|
| Wandler des ESP32-S3: Störlinien oder ungleichmäßige Abtastung, die die Simulation nicht kennt | M2 Handgerät in Betrieb nehmen (Abtastrate, Rauschen), M3 Spule kalibrieren und Bereichswahl |
| Strommessung im Sender nur ca. ±10 % (Formfaktor, Nullpunkt, Bauteile; bei rein kapazitiver Last weicht die Kurvenform des Stroms rechnerisch ca. 3 % ab) | M1 Sender auf dem Tisch: Strom gegen Multimeter mit Prüfton 1 kHz |
| Endstufe an kapazitiver Last, 8-bit-D/A als Signalquelle | M1 |
| Stromrichtung mit einer Spule hängt von der Haltung ab | M7 Stromrichtung im Bündel, mit der Regel „Display immer zu dir“ |
| Display, Prozessor und Leitungen stören die Spule | M4 Abstand Spule – Digitalteil |
| Sendespannung niedriger als in der Rechnung (Kap. 9) | M5 freie Leitung, M6 Wand, Unterputz, Trasse und Vergleich mit dem IP-Kamera-Tester |
| Störquellen in Gebäuden (LED-Treiber, Schaltnetzteile) | M8 Störquellen |
| Bedienfehler: MAX in Modus A mit Gerät am Ende (17 V Spitze statt ≤ 12 V) | kein Messpunkt; Hinweis im Display und in der Anleitung; Phase 4 klärt, ob eine Hardware-Begrenzung nötig ist |
| Automatische Abschaltung beider Geräte noch nicht entworfen | Phase 4 (Schaltung) und Phase 6 (Firmware) |

Die Messwerte schaut man sich im **seriellen Plotter der Arduino IDE** an. Dein Oszilloskop braucht
kein FFT, das Handgerät rechnet selbst.

---

## 13. Was sich im Lastenheft geändert hat

**Mit v1.0 des Konzepts** (Lastenheft v1.1, inzwischen teils überholt):

- **S-13** (Fremdspannungswarnung): umgesetzt als Sperre ohne „spannungsfrei“-Anzeige. Seit v2.0
  entfallen.
- **S-14** (Durchgang): umgesetzt, plus Lastbild und Längenschätzung. Seit v2.0 entfallen.
- **H-14** (Links/Rechts) und **H-15** (Tiefe): umgesetzt ohne Zusatzhardware. Seit v2.0: H-14 nur
  vorbereitet, H-15 nur als Anleitung.
- **Kap. 9:** Zielwerte aus Kap. 10 eingetragen.
- Handgerät erkennt den Frequenzsatz selbst; Anzeige des Feldes in nT. Seit v2.0: nur Satz Standard,
  Anzeige in dB.

**Mit v2.0** (Lastenheft v2.0):

- **S-02, S-03, S-15, S-18:** zwei Klemmen A und E, Modi G und A durch Anklemmen, WAGO, keine
  4-mm-Buchse.
- **S-09, S-10, S-11:** Anzeige von Strom, Pegel, Batterie und Warnungen; Strom ±10 %; Pegel
  FEIN/NORMAL/MAX.
- **S-13, S-14:** entfallen.
- **H-01, H-03, H-04, H-11:** Piezo, eine Spule, Identifikation über Stromrichtung, Stab mit Kopf oben.
- **H-14:** vorbereitet, nicht umgesetzt. **H-15:** nur als Anleitung.
- **Kap. 5a:** dB im Handgerät, mA im Sender, OLED 1,3″ in beiden Geräten.
- **E-02, E-06:** Controller festgelegt, Phase 3 als Mini-Test.
- **Tests:** T-03, T-05, T-08, T-09, T-10, T-15, T-16 angepasst; T-06 und T-14 entfallen.

---

## 14. Änderungen nach der Freigabe

| Datum | Änderung | Entscheidung |
|---|---|---|
| 13.10.2026 | Endstufe: TL072 + Gegentakt BD139/BD140 statt L272 (L272 bei onsemi abgekündigt, Lieferbarkeit unsicher). Funktion, Spannung (Brücke an 24 V) und Kosten bleiben gleich. | E-028 |
| 13.10.2026 | Frequenzen exakt aus dem DAC-Takt: Standard 8333,3 + 4166,7 Hz, Tief 1041,7 + 520,8 Hz, Hoch 33 333 Hz (statt 8225/4112,5 usw.). Alle ≥ 8 Hz neben einer 50-Hz-Oberwelle. | E-029 |
| 13.10.2026 | OLED über I²C (4 Pins) statt SPI: einfachere Verdrahtung, Bildrate reicht. | E-030 |
| 13.10.2026 | Handgerät als Stab mit der Abtastfläche oben am Kopf. | E-033 |
| 13.10.2026 | **v2.0:** Variante „Einfach“ statt Variante C, ca. 38 € pro Satz. | E-034 |
| 13.10.2026 | Handgerät: Wandler im ESP32-S3 mit zwei Verstärkerstufen und Bereichswahl statt PCM1808. | E-035 |
| 13.10.2026 | Eine Spule im Kopf, zweite vorbereitet; E-Feld-Spitze entfällt. | E-036 |
| 13.10.2026 | Sender mit zwei Klemmen A/E ohne Umschaltung; Modus P sowie Sätze „Tief“ und „Hoch“ entfallen. | E-037 |
| 13.10.2026 | Sender misst den Strom; Fremdspannungssperre, Durchgang, Lastbild und Längenschätzung entfallen. | E-038 |
| 13.10.2026 | Anzeige im Handgerät in dB statt nT. | E-039 |
| 13.10.2026 | Versorgung und Bedienung beider Geräte (Tasten, Piezo, Pegel-Regel). | E-040 |
| 13.10.2026 | Phase 3 als Mini-Test mit den endgültigen Teilen. | E-041 |
| 13.10.2026 | Aufbau auf Lochraster statt eigener Platine, ca. 31 € pro Satz. | E-042 |
| 13.10.2026 | Signalausgang des Senders an GPIO18 (äußere Stiftreihe) statt GPIO17. | E-043 |
| 13.10.2026 | KiCad-Entwurf beider Platinen als Entscheidungshilfe Platine oder Lochraster. | E-044 |

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
- PCM1808 am ESP32 (Takt, Slave-Betrieb), betraf nur v1.0: <https://esp32.com/viewtopic.php?p=121953>
