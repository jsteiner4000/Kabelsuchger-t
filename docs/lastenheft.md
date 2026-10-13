# Lastenheft Kabelsuchgerät (Sender + Handgerät)

| | |
|---|---|
| **Version** | 1.0 |
| **Stand** | 13.10.2026 |
| **Status** | Freigegeben durch Julius am 13.10.2026 |
| **Auftraggeber** | Julius |
| **Bearbeitung** | Claude (Entwicklungspartner) |
| **Visuelle Übersicht** | [`lastenheft-uebersicht.html`](lastenheft-uebersicht.html) (Link zur veröffentlichten Seite siehe README) |

---

## 0. Wozu dieses Dokument

Das Lastenheft beschreibt, **was** das Gerät können soll, nicht **wie**. Das Wie
(Ortungsprinzip, Frequenz, Schaltung) kommt im Konzept in Phase 2.

Wenn wir beide dieses Dokument freigegeben haben, ist es die Messlatte für alles Weitere:
Jede spätere Entscheidung muss eine Anforderung hier erfüllen, und am Ende prüfen wir das
Gerät gegen die Abnahmetests in Kapitel 9. Änderungen sind jederzeit möglich, werden
aber hier eingetragen (Änderungsverlauf am Ende).

**Prioritäten**

| Prio | Bedeutung |
|---|---|
| **Muss** | Ohne das ist das Projekt nicht erfüllt. |
| **Soll** | Wird umgesetzt, wenn Technik und Budget es sauber hergeben. Wegfall nur mit Begründung in `entscheidungen.md`. |
| **Kann** | Nur, wenn es sauber geht. Lösungen, die nur halb funktionieren oder falsche Sicherheit vermitteln, lassen wir weg und begründen das. |
| **entfällt** | Bewusst gestrichen; ID bleibt zur Nachverfolgung stehen. |

Verweise wie (F-07) zeigen auf die Antworten aus Phase 1 in Kapitel 14.

---

## 1. Ausgangslage und Ziel

Julius ist Servicetechniker für Einbruch- und Brandmeldeanlagen (EMA/BMA) und Video. Im
Alltag müssen Leitungen verfolgt und identifiziert werden, deren Verlauf unbekannt ist,
meistens in **Kabeltrassen** und **unter Putz** (F-07). Einfache Tonsucher (Tongenerator +
kapazitiver Prüfkopf) versagen dabei oft, vor allem bei geschirmten Leitungen, in Beton, bei
größerer Tiefe und in vollen Trassen.

**Ziel:** Ein selbst gebautes System aus **Sender** und **Handgerät**, das spürbar besser
ortet als einfache Tonsucher, vor allem in der Tiefe und in Trassen. Es soll sich mit
Lötkolben (ohne SMD), Bambu-Lab-Drucker und Arduino IDE bauen lassen und ca. 40 € pro Satz
kosten.

---

## 2. Begriffe

Damit wir dieselben Wörter für dieselben Dinge benutzen:

| Begriff | Bedeutung in diesem Projekt |
|---|---|
| **Sender** | Gerät, das am Leitungsanfang angeklemmt wird und ein Suchsignal einspeist. |
| **Handgerät** | Empfänger, mit dem man das Signal von außen aufspürt. |
| **Einspeisemodus** | Zwischen welchen Leitern der Sender sein Signal anlegt, z. B. Ader/Ader, Ader/Schirm, Ader/Erde. |
| **Ortung** (Trassenverfolgung) | Den Verlauf einer Leitung von außen verfolgen, z. B. durch Putz, Wand oder Decke. |
| **Identifikation** | Die richtige Leitung unter mehreren finden: (a) in einem Bündel oder einer Trasse, Mantel geschlossen; (b) am abisolierten Ende die richtige Ader. |
| **Trasse** | Kabelrinne, Gitterrinne oder Kabelbündel, in dem viele Leitungen parallel liegen. |
| **Übersprechen** | Signal, das von der gesuchten Leitung auf Nachbarleitungen überkoppelt, sodass diese ebenfalls „leuchten“. |
| **Leitungsanfang** | Dort, wo der Sender angeklemmt wird (meist Zentrale oder Verteiler). |
| **Leitungsende** | Das ferne Ende. Zustände: **offen**, **gebrückt** (auch über Abschlusswiderstand), **Gerät angeschlossen** (Melder, Kamera, Zentrale), **geerdet**. |
| **Schirm** | Statischer Schirm aus Alufolie und Beidraht, z. B. bei J-Y(St)Y. Kann **beidseitig geerdet**, **einseitig geerdet** oder **ungeerdet** sein. |
| **Erde** | Großflächiges Bezugspotenzial: PE bzw. Potentialausgleichsschiene (PAS), Heizungs- oder Wasserrohr, metallische Trasse. |
| **Erkennungsabstand** | Größter Abstand zwischen Sensorkopf und Leitung, bei dem das Signal noch eindeutig über dem Störpegel liegt. Genaue Messvorschrift in Phase 2. |
| **Ortungsgenauigkeit** | Wie genau sich die Lage der Leitung beim Überstreichen bestimmen lässt (± cm). |
| **Fremdspannung** | Spannung, die beim Anklemmen schon auf der Leitung liegt, z. B. weil doch noch etwas angeschlossen ist. |
| **Schutzkleinspannung** | Spannungsbereich nach DIN VDE 0100-410, in dem Berühren ungefährlich ist. |

---

## 3. Einsatzumfeld und Randbedingungen

### 3.1 Leitungen

| ID | Prio | Anforderung |
|---|---|---|
| U-01 | Muss | Hauptsächlich Fernmelde- und Signalleitungen der EMA/BMA: J-Y(St)Y n×2×0,6/0,8 (paarverseilt, statischer Schirm), LIYY, LIYCY. |
| U-02a | Soll | Cat-Netzwerkleitungen und Koax (Video). |
| U-02b | Kann | NYM, nur **freigeschaltet und gegen Wiedereinschalten gesichert** (siehe N-04): Ortung und Durchgangsmessung (F-08). |
| U-03 | Muss / Soll | Leitungslänge von wenigen Metern bis **300 m (Muss)** bzw. **500 m (Soll)**. Ob 500 m erreichbar sind, rechnet das Konzept nach (F-08). |
| U-04 | Muss | Leitungen sind beim Suchen spannungsfrei. Der Sender ist **nicht** für Netzspannung ausgelegt. |

### 3.2 Zustand von Leitungsanfang, Leitungsende und Schirm

| ID | Prio | Anforderung |
|---|---|---|
| U-05 | Muss | Leitungsende **offen**, **gebrückt** (auch über Abschlusswiderstand) oder **mit angeschlossenem Gerät**: Alle Fälle kommen etwa gleich oft vor (F-12). Das System muss in allen nutzbar sein. Die erreichbare Leistung darf sich unterscheiden, muss aber dokumentiert sein (Bedienungsanleitung: welcher Modus wann). |
| U-06 | Muss | Schirm geerdet (ein- oder beidseitig) oder ungeerdet. |
| U-07 | Muss | Angeschlossene Geräte dürfen durch das Suchsignal nicht beschädigt werden, auch nicht durch Verpolung. Annahme für das empfindlichste Gerät: 12-V-Melder bzw. 12-V-Kamera (F-12). |
| U-14 | Muss | Das ferne Ende ist mal zugänglich, mal unbekannt (F-11). Das System muss **ohne** Eingriff am fernen Ende funktionieren; ein Eingriff dort (Brücke, Erde) darf die Ortung verbessern. |
| U-15 | – | Am Leitungsanfang gilt (F-09, F-10): Die Leitung ist zum Suchen komplett abgeklemmt, der Schirm lässt sich vorübergehend von der Erde lösen, alle Adern dürfen gemeinsam angeschlossen werden, und meist ist eine Erde (PAS, Heizungsrohr, Trasse) erreichbar. |

### 3.3 Verlegung und Baustoffe

| ID | Prio | Anforderung |
|---|---|---|
| U-08 | Muss | Aufputz, Unterputz, Hohlwand, abgehängte Decke, Kabeltrasse, Leerrohr. **Schwerpunkt: Kabeltrasse und Unterputz** (F-07). |
| U-09 | Muss | Wandbaustoffe: Gipskarton, Mauerwerk (Ziegel, Kalksandstein, Porenbeton), Beton, Stahlbeton. Realistische Reichweiten je Baustoff schätzen wir in Phase 2 ab und messen sie in Phase 3. |
| U-10 | Muss / Kann | Kabeltrassen aus Metall und Gitterrinnen: **Muss**. Stahlpanzerrohr: **Kann**. |
| U-13 | Muss | **Selektivität in Trassen:** Die gesuchte Leitung muss sich eindeutig von Nachbarleitungen abheben, auch wenn Signal auf diese übergesprochen hat (R-5). |

### 3.4 Umgebung

| ID | Prio | Anforderung |
|---|---|---|
| U-11 | Muss | Innenräume, auch Baustellen und Technikräume mit Störquellen: 50-Hz-Netz, Schaltnetzteile, LED-Treiber, Frequenzumrichter, Leuchtstofflampen. |
| U-12 | Soll | Betriebstemperatur 0 … 40 °C (Annahme). |

---

## 4. Anforderungen an den Sender

| ID | Prio | Anforderung |
|---|---|---|
| S-01 | Muss | Speist ein Suchsignal in die Leitung ein, das das Handgerät eindeutig erkennt. |
| S-02 | Muss | Drei Anschlüsse: Ader A, Ader B, Schirm/Erde. |
| S-03 | Muss | Umschaltbare Einspeisemodi, mindestens Ader/Ader, Ader/Schirm, Ader/Erde. Welche Modi es endgültig gibt (auch Kombinationen, z. B. alle Adern gemeinsam), entscheidet das Konzept danach, was physikalisch am meisten bringt. |
| S-04 | Muss | Ausgangsspannung im berührungsunkritischen Schutzkleinspannungsbereich (Obergrenze 25 V AC eff bzw. 60 V DC nach DIN VDE 0100-410). Der tatsächliche Wert wird in Phase 2 so festgelegt, dass U-07 erfüllt ist. |
| S-05 | Muss | Dauerhaft kurzschlussfest, an jeder Klemmenkombination. |
| S-06 | Muss | Arbeitet stabil an Leitungen bis zur Maximallänge (U-03), in jedem Einspeisemodus. Die Leitungskapazität darf den Sender weder beschädigen noch zum Schwingen bringen; bei Überlast begrenzt er selbst. |
| S-07 | Muss | Versorgung mit **AA**-Zellen. Alkaline und NiMH-Akkus müssen funktionieren. |
| S-08 | Muss | Automatische Abschaltung **2 h** nach der letzten Bedienung, umschaltbar auf 30 min oder 8 h. |
| S-09 | Muss | **OLED-Anzeige**: Betrieb, Einspeisemodus, Batteriezustand, Fehler (Kurzschluss/Überlast, Fremdspannung). Gestaltung siehe A-01. |
| S-10 | Soll | Anzeige des eingespeisten Stroms in mA. Zeigt sofort, ob überhaupt ein Stromkreis zustande kommt. |
| S-11 | Soll | Mindestens zwei Leistungsstufen (wenig Übersprechen ↔ große Reichweite). |
| S-12 | Muss | Übersteht Fremdspannung an den Klemmen bis **60 V DC / 50 V AC** dauerhaft ohne Schaden (F-13). |
| S-13 | Kann | **Fremdspannungswarnung:** misst vor dem Einspeisen und warnt. Nur, wenn sie zuverlässig ist und nicht als Spannungsprüfer missverstanden werden kann (eine Anzeige „0 V“ ist kein Feststellen der Spannungsfreiheit). Entscheidung mit Begründung in Phase 2. |
| S-14 | Soll | **Durchgangs-/Schleifentest:** zeigt den Schleifenwiderstand in Ω und ob das Ende offen, gebrückt oder mit Abschlusswiderstand beschaltet ist. Nur, wenn das Ergebnis eindeutig ist; sonst wird nur der Messwert angezeigt. Entscheidung mit Begründung in Phase 2. |
| S-15 | Muss | Feste Messleitungen mit Zugentlastung: 3 × ca. 1 m Silikonlitze mit isolierten Krokodilklemmen in Rot (A), Schwarz (B), Grün-Gelb (Schirm/Erde) (F-14). |
| S-16 | Soll | Batterielaufzeit im Dauerbetrieb mindestens ein Arbeitstag (8 h) bei normaler Leistungsstufe. |
| S-17 | Soll | Signal mit Kennung, damit das Handgerät nur auf den eigenen Sender reagiert und nicht auf Störquellen oder fremde Tonsucher. |
| S-18 | Soll | Mehrere Adern lassen sich einfach gemeinsam anschließen, z. B. über eine WAGO-Klemme. Zusätzliche 4-mm-Buchse für eine optionale lange Erdleitung (F-14). |

---

## 5. Anforderungen an das Handgerät

| ID | Prio | Anforderung |
|---|---|---|
| H-01 | Muss | Akustische Anzeige über Lautsprecher: Der Ton ändert sich mit der Signalstärke (Tonhöhe und/oder Taktrate). |
| H-02 | Muss | **OLED-Anzeige** mit Balken und Zahlenwert der Signalstärke. Gestaltung siehe A-01. |
| H-03 | Muss | Ortungsprinzip nach Konzept (magnetisch, elektrisch oder beides). Der aktive Sensor bzw. Modus ist im Display erkennbar. |
| H-04 | Muss | **Bündel-Modus:** feine Spitze für die Identifikation in Bündel/Trasse und am abisolierten Ende; Empfindlichkeit regelbar. |
| H-05 | Muss | Selektive Auswertung: zeigt nur das Sendersignal an. Unterdrückt 50 Hz mit Oberwellen, Schaltnetzteile und LED-Treiber. |
| H-06 | Muss | Empfindlichkeit von Hand einstellbar. |
| H-07 | Soll | Automatische Empfindlichkeit als zusätzliche Betriebsart. |
| H-08 | Muss | Versorgung mit **AA**-Zellen, NiMH-tauglich. Automatische Abschaltung nach **10 min ohne Bedienung und ohne Signal**, damit sie beim Suchen nicht stört. |
| H-09 | Muss | Batterieanzeige. |
| H-10 | Soll | Lautstärke einstellbar, auch stumm. |
| H-11 | Soll | **So kompakt, wie es technisch sinnvoll ist** (F-17): Die Bauform legt das Konzept fest, unter anderem nach Abstand zwischen Sensor und Digitalteil und nach Lage der Sensorspule(n). Einhändig bedienbar, Display beim Suchen ablesbar. |
| H-12 | Soll | Ruhige Anzeige: Wert springt nicht durch Rauschen, wenn das Gerät stillgehalten wird. |
| H-13 | entfällt | ~~Kopfhöreranschluss~~ – ausdrücklich nicht gewünscht (F-18). |
| H-14 | Kann | Richtungsanzeige links/rechts. Prüfung in Phase 2; Platine wird vorbereitet. |
| H-15 | Kann | Tiefenschätzung. Geht nur bei Magnetfeld-Ortung und nur als Schätzung. Prüfung in Phase 2. |

---

## 5a. Anzeige in beiden Geräten

| ID | Prio | Anforderung |
|---|---|---|
| A-01 | Muss | Jeder angezeigte Zahlenwert ist ein echter Messwert mit klarer Einheit oder nachvollziehbarer Skala (z. B. mA, V, Ω, dB). Keine geschönten oder erfundenen Werte, keine Prozentangaben ohne Bezug. |
| A-02 | Soll | Grafisch hochwertig („schick“, F-15): z. B. Balken mit Spitzenwert-Halten, Verlaufsgraph beim Überstreichen, große gut lesbare Ziffern, flüssige Darstellung. |
| A-03 | Soll | Einheitliche Gestaltung von Sender und Handgerät (gleiche Symbole, gleiche Schrift). |

Displaygröße und -typ (z. B. 0,96″, 1,3″ oder 2,42″ OLED) legt das Konzept nach Lesbarkeit und Budget fest.

---

## 6. Anforderungen an die Gehäuse

| ID | Prio | Anforderung |
|---|---|---|
| G-01 | Muss | Druck in PETG auf Bambu Lab, 0,4-mm-Düse, Teile bis 25 cm Länge (F-01). |
| G-02 | Soll | Ohne Stützmaterial druckbar. |
| G-03 | Muss | Kompakt, normal robust (übersteht z. B. einen Fall vom Tisch), nicht klobig. |
| G-04 | Muss | Batteriefach; Batteriewechsel, ohne an die Elektronik zu müssen. |
| G-05 | Muss | Zugentlastung für die Messleitungen (Sender). |
| G-06 | Muss | Sinnvolle Verschraubung (z. B. Gewindeeinsätze), mehrfach zu öffnen. |
| G-07 | Muss | Parametrischer Quellcode (OpenSCAD oder CadQuery), fertige STL-Dateien und Druckeinstellungen. |
| G-08 | Muss | Konstruktion erst, wenn Platinenmaße und Bauteile feststehen. |
| G-09 | Soll | Hinweis „Nur an spannungsfreien Leitungen“ am Sender, direkt mitgedruckt. |

---

## 7. Elektronik, Firmware, Fertigung

| ID | Prio | Anforderung |
|---|---|---|
| E-01 | Muss | **Julius lötet kein SMD** (F-03). Erlaubt sind THT-Bauteile und fertige Module mit Stiftleisten. SMD-Bauteile nur, wenn der Platinenhersteller sie bestückt; ob sich das lohnt, zeigt der Kostenvergleich in Phase 4. |
| E-02 | Muss | Mikrocontroller mit der Arduino IDE programmierbar. Ein ESP32-S2 mini ist vorhanden (F-04) und kann für den Steckbrett-Test dienen; die endgültige Wahl trifft das Konzept. |
| E-03 | Soll | Programmieren über USB, ohne zusätzlichen Programmer. |
| E-04 | Muss | Abgleich und Test mit Multimeter und Oszilloskop möglich; beschriftete Messpunkte. |
| E-05 | Muss | Firmware gut kommentiert; Parameter (Frequenz, Abschaltzeit usw.) zentral einstellbar. |
| E-06 | Muss | Vor der Platinenbestellung ein Machbarkeitstest auf dem Steckbrett (Phase 3). |
| E-07 | Muss | Aufbauunterlagen als **KiCad-Projekt** mit Fertigungsdaten (F-02). Wunsch: Erstellung automatisiert, z. B. per KiCad-MCP. Das Werkzeug wird bis Phase 5 geklärt; Ausweichlösung ist eine eindeutige Netzliste mit Beschreibung. |

---

## 8. Sicherheit und Abgrenzung

### 8.1 Sicherheit

| ID | Prio | Anforderung |
|---|---|---|
| N-01 | Muss | Sender nur für spannungsfreie Leitungen. Hinweis am Gerät (G-09) und in der Anleitung. |
| N-02 | Muss | Ausgang begrenzt (S-04) und kurzschlussfest (S-05). |
| N-03 | – | Anschluss an 230 V kommt laut Julius nie vor (F-13). Ein Schutz dagegen ist nicht gefordert. Ob trotzdem eine billige Eingangssicherung hineinkommt, entscheidet Phase 4. |
| N-04 | Muss | Für NYM (U-02b) gilt: nur nach den fünf Sicherheitsregeln (freischalten, gegen Wiedereinschalten sichern, Spannungsfreiheit feststellen …). Die Bedienungsanleitung weist darauf hin. |

### 8.2 Bewusst nicht Teil des Projekts

- **Keine** Suche an Leitungen unter Spannung, **keine** Einspeisung in Netzspannung.
- **Keine** passive Ortung von Netzleitungen über ihr 50-Hz-Feld („Bohrschutz“). Begründung:
  Eine abgeschaltete, geschirmte oder tief liegende Netzleitung zeigt nichts an; „kein Ausschlag“
  würde dann falsche Sicherheit vermitteln (E-005).
- **Kein** Kopfhöreranschluss (F-18).
- **Kein** Funk, keine App.
- **Kein** CE-Konformitätsverfahren (Eigenbau für den eigenen Gebrauch). Wir bleiben trotzdem bei
  kleiner Sendeleistung und in Frequenzbereichen, die auch professionelle Leitungsortungsgeräte nutzen.

---

## 9. Abnahmetests

Die Zielwerte tragen wir in Phase 2 (Konzept) mit Begründung ein. Gemessen wird in Phase 3
(Steckbrett, soweit möglich) und Phase 8 (fertiges Gerät). Testleitung und ein einfacher
Tonsucher als Vergleich sind vorhanden (F-05).

| ID | Szenario | Prüft | Zielwert |
|---|---|---|---|
| T-01 | Freie Leitung (≥ 50 m, ausgelegt), Ende offen: Erkennungsabstand in Luft, je Einspeisemodus | U-05, H-03 | Phase 2 |
| T-02 | Wie T-01, Ende gebrückt bzw. geerdet | U-05, S-03 | Phase 2 |
| T-03 | Leitung unter Putz bzw. in Mauerwerk: Erkennungsabstand und Ortungsgenauigkeit | U-08, U-09 | Phase 2 |
| T-04 | Leitung in bzw. unter Stahlbeton (reales Objekt) | U-09 | Phase 2 |
| T-05 | Trasse mit ≥ 10 Leitungen (auch Metall-/Gitterrinne): gesuchte Leitung eindeutig finden und verfolgen, obwohl Nachbarleitungen übersprechen | U-10, U-13, H-04 | Phase 2 |
| T-06 | Richtige Ader am abisolierten Ende finden | H-04 | Phase 2 |
| T-07 | LED-Treiber, Schaltnetzteil, Leuchtstofflampe in 30 cm Abstand | H-05 | keine Fehlanzeige |
| T-08 | Kurzschluss am Ausgang, 1 h, jede Klemmenkombination | S-05 | kein Schaden, Fehleranzeige |
| T-09 | Endgerät angeschlossen (12-V-Melder, 12-V-Kamera), alle Einspeisemodi, auch verpolt | U-07 | kein Schaden, Gerät funktioniert danach |
| T-10 | 60 V DC und 50 V AC an jeder Klemmenkombination | S-12, S-13 | kein Schaden; Warnung, falls S-13 umgesetzt |
| T-11 | Lange Leitung (300 m bzw. 500 m oder verfügbare Trommel): Sender stabil, Signal am Ende ortbar | U-03, S-06 | Phase 2 |
| T-12 | Batterielaufzeit und automatische Abschaltung, beide Geräte | S-08, S-16, H-08 | Phase 2 |
| T-13 | Vergleich mit dem vorhandenen Tonsucher unter gleichen Bedingungen | Ziel aus Kap. 1 | deutlich größerer Erkennungsabstand (Wert Phase 2) |
| T-14 | Durchgangstest: Ende offen, gebrückt, mit Abschlusswiderstand | S-14 | richtig erkannt bzw. richtiger Ω-Wert |
| T-15 | NYM, freigeschaltet: Ortung und Durchgangsmessung | U-02b | Phase 2 |
| T-16 | Anzeige: Stichprobe der angezeigten Werte gegen Multimeter bzw. Oszilloskop | A-01 | Abweichung im Rahmen der Angabe aus Phase 2 |

---

## 10. Budget

| ID | Anforderung |
|---|---|
| B-01 | Ziel ca. 40 € pro Satz (Sender + Handgerät). Funktion geht vor Preis. |
| B-02 | Bevor die Stückliste 40 € überschreitet, sprechen wir darüber. |
| B-03 | In die 40 € zählt alles, was im fertigen Gerät steckt (inkl. Messleitungen, Schrauben, Gewindeeinsätze), plus Platinen und Versand anteilig. Nicht: Batterien, Filament, Werkzeug, Verbrauchsmaterial (F-19). |
| B-04 | Stückzahl: zunächst ein Satz, kein Termin (F-20). |

---

## 11. Lieferumfang

```
docs/
  lastenheft.md              dieses Dokument
  lastenheft-uebersicht.html visuelle Übersicht
  entscheidungen.md          Entscheidungen mit Begründung
  konzept.md                 Phase 2
  machbarkeitstest.md        Phase 3
  inbetriebnahme.md          Phase 8 (Aufbau, Abgleich, Tests)
  bedienungsanleitung.md     Phase 8 (welcher Modus wann)
hardware/sender/             Schaltplan, KiCad-Projekt
hardware/handgeraet/         Schaltplan, KiCad-Projekt
firmware/sender/             Arduino-Sketch
firmware/handgeraet/         Arduino-Sketch
gehaeuse/                    OpenSCAD- bzw. CadQuery-Quellcode, STL, Druckeinstellungen
stueckliste.csv              Stückliste mit Bezugsquelle und Preis
```

---

## 12. Vorgehen und Freigaben

Nach jeder Phase gibt Julius frei, erst dann geht es weiter.

| Phase | Inhalt | Ergebnis |
|---|---|---|
| 1 | Rückfragen, Lastenheft | Lastenheft v1.0 + visuelle Übersicht |
| 2 | Konzept | Ortungsprinzip, Blockschaltbild, 2–3 Varianten mit Empfehlung, Reichweiten-Einschätzung, Zielwerte für Kap. 9, visuelle Konzeptübersicht |
| 3 | Machbarkeitstest | Steckbrett-Aufbau, Messanleitung fürs Oszilloskop, Messergebnisse |
| 4 | Schaltung und Stückliste | Schaltpläne beider Geräte, Stückliste mit Bezugsquelle und Preis, Budget-Check |
| 5 | Platine | KiCad-Projekt mit Fertigungsdaten |
| 6 | Firmware | Sketche für beide Geräte, gut kommentiert |
| 7 | Gehäuse | Quellcode, STL, Druckeinstellungen |
| 8 | Inbetriebnahme | Aufbau-, Abgleich- und Testanleitung mit Messpunkten, Bedienungsanleitung |

---

## 13. Bekannte physikalische Grenzen (Vorschau)

Ausführlich erklärt und gelöst werden diese Punkte im Konzept (Phase 2).

| ID | Grenze |
|---|---|
| R-1 | **Stahlbeton:** Bewehrung und feuchter Beton schirmen das elektrische Feld stark ab. Durch Stahlbeton kommt praktisch nur ein Magnetfeld, und das entsteht nur, wo Strom fließt. |
| R-2 | **Magnetfeld braucht Strom:** Bei offenem Ende fließt nur ein kapazitiver Ladestrom. Er ist am Anfang am größten und fällt zum Ende hin auf null. |
| R-3 | **Feldauslöschung:** Fließt der Strom auf einer Ader hin und auf der Nachbarader zurück, heben sich die Felder außen fast vollständig auf, bei verdrillten Paaren besonders stark. |
| R-4 | **Geerdeter Schirm:** hält das elektrische Feld der Adern zurück und führt zusätzlich Strom zurück, der das Magnetfeld schwächt. |
| R-5 | **Übersprechen:** Je höher die Frequenz und je länger Leitungen parallel liegen, desto mehr Signal springt auf Nachbarleitungen über. In Trassen „leuchten“ dann auch Nachbarkabel. Wichtig wegen U-13. |
| R-6 | **Metall in der Nähe:** Stahlrohr und Metalltrassen dämpfen und verbiegen die Felder. |

---

## 14. Antworten aus Phase 1

| Frage | Antwort von Julius | Auswirkung |
|---|---|---|
| F-01 Drucker | Vorschlag ok: 0,4-mm-Düse, Teile bis 25 cm | G-01 |
| F-02 KiCad | KiCad gern, Erstellung per MCP | E-07; Werkzeug bis Phase 5 klären |
| F-03 Löten | kann kein SMD | E-01: THT + Module; SMD nur vom Hersteller bestückt |
| F-04 Bastelkiste | ESP32-S2 mini vorhanden | E-02; Kandidat für den Steckbrett-Test |
| F-05 Testmaterial | Testleitung und Tonsucher vorhanden | T-11, T-13 durchführbar |
| F-06 Aufgaben | alle drei | Ortung, Bündel/Trasse und Ader am Ende sind Muss |
| F-07 Einsatz | meistens Trasse und Unterputz | U-08, U-10, U-13 |
| F-08 Länge, Leitungstypen | 500 m wäre super, aber wohl unrealistisch; NYM durchmessen wäre cool | U-03 (300 m Muss, 500 m Soll), U-02b, S-14 auf Soll |
| F-09 Leitungsanfang | ja / ja / ja | U-15 |
| F-10 Erde | ja | U-15 |
| F-11 Fernes Ende | unterschiedlich | U-14 |
| F-12 Geräte am Ende | unterschiedlich, mal offen, mal gebrückt | U-05; empfindlichstes Gerät als Annahme (U-07) |
| F-13 Fremdspannung | niemals 230 V | S-12 (60 V DC / 50 V AC), N-03 |
| F-14 Messleitungen | Vorschlag, evtl. auch mit WAGO | S-15, S-18 |
| F-15 Anzeige | OLED in beiden Geräten, schick, aber technisch korrekt | S-09, H-02, A-01 … A-03 |
| F-16 Batterien, Abschaltzeit | Vorschlag | S-07, S-08, H-08 |
| F-17 Bauform | so kompakt wie technisch sinnvoll, Entscheidung bei Claude | H-11 |
| F-18 Extras | **kein** Kopfhöreranschluss | H-13 entfällt; H-14, H-15 Prüfung in Phase 2 |
| F-19 Budget-Umfang | Vorschlag | B-03 |
| F-20 Stückzahl, Termin | erst einmal ein Satz, kein Termin | B-04 |

### 14.1 Annahmen (bitte melden, falls falsch)

| ID | Annahme |
|---|---|
| AN-1 | Bauraum des Druckers mindestens 256 × 256 mm (Modell nicht genannt). |
| AN-2 | Bezugsquellen: Platinen bei JLCPCB, Bauteile bei Reichelt, Module (Display, Controller) auch AliExpress. |
| AN-3 | Empfindlichstes Gerät am Leitungsende: 12-V-Melder bzw. 12-V-Kamera. |
| AN-4 | Bei Zielkonflikten hat die Ortung in Trasse und unter Putz Vorrang vor den anderen Aufgaben. |

### 14.2 Nachfragen (beantwortet zur Freigabe)

| ID | Frage | Antwort | Auswirkung |
|---|---|---|---|
| O-1 | Welches Oszilloskop, mit FFT? | „ein ganz normales“, Modell unbekannt | Messanleitung Phase 3 setzt kein FFT voraus; Spektren zeigt das Handgerät selbst |
| O-2 | Vergleichs-Tonsucher? | Kabelsuchfunktion eines China-IP-Kamera-Testers, eigentlich für Netzwerkkabel, reicht nicht weit | Referenz für T-13 |
| O-3 | Trassen: von unten oder von der Leiter? | unterschiedlich | Konzept rechnet mit beidem (Reichweite von unten 1–2 m, an der Trasse direkt) |

---

## Änderungsverlauf

| Version | Datum | Änderung |
|---|---|---|
| 0.1 | 13.10.2026 | Erster Entwurf aus dem Auftrag; offene Fragen F-01 … F-20 |
| 1.0.1 | 13.10.2026 | Freigabe durch Julius; Antworten O-1 … O-3 eingetragen |
| 1.0 | 13.10.2026 | Antworten F-01 … F-20 eingearbeitet: kein SMD-Löten (E-01), OLED in beiden Geräten mit Anzeige-Grundsätzen (Kap. 5a), Schwerpunkt Trasse/Unterputz mit Selektivität (U-13), Länge 300 m Muss / 500 m Soll, NYM als Kann, Durchgangstest auf Soll, kein Kopfhöreranschluss, Fremdspannung 60 V DC / 50 V AC; neue Tests T-14 … T-16 |
