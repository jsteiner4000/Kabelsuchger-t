# Lastenheft Kabelsuchgerät (Sender + Handgerät)

| | |
|---|---|
| **Version** | 0.1 – Entwurf |
| **Stand** | 13.10.2026 |
| **Status** | Wartet auf Antworten zu Kapitel 14, danach v1.0 zur Freigabe |
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

**Offen (F-xx)** heißt: Die Anforderung hängt von einer Rückfrage in Kapitel 14 ab.

---

## 1. Ausgangslage und Ziel

Julius ist Servicetechniker für Einbruch- und Brandmeldeanlagen (EMA/BMA) und Video. Im
Alltag müssen Leitungen verfolgt und identifiziert werden, deren Verlauf unbekannt ist.
Einfache Tonsucher (Tongenerator + kapazitiver Prüfkopf) versagen dabei oft, vor allem bei
geschirmten Leitungen, in Beton und bei größerer Tiefe.

**Ziel:** Ein selbst gebautes System aus **Sender** und **Handgerät**, das spürbar besser
ortet als einfache Tonsucher, vor allem in der Tiefe. Es soll sich mit Lötkolben,
Bambu-Lab-Drucker und Arduino IDE bauen lassen und ca. 40 € pro Satz kosten.

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
| U-02 | offen (F-08) | Weitere Leitungstypen: Cat-Netzwerkleitungen, Koax (Video), spannungsfreie NYM. |
| U-03 | Muss | Leitungslänge von wenigen Metern bis mehrere hundert Meter. Obergrenze: F-08 (Vorschlag 500 m). |
| U-04 | Muss | Leitungen sind beim Suchen spannungsfrei. Der Sender ist **nicht** für Netzspannung ausgelegt. |

### 3.2 Zustand von Leitungsende und Schirm

| ID | Prio | Anforderung |
|---|---|---|
| U-05 | Muss | Leitungsende **offen**, **gebrückt** (auch über Abschlusswiderstand) oder **mit angeschlossenem Gerät**: Das System muss in allen drei Fällen nutzbar sein. Die erreichbare Leistung darf sich unterscheiden, muss aber dokumentiert sein (Bedienungsanleitung: welcher Modus wann). |
| U-06 | Muss | Schirm geerdet (ein- oder beidseitig) oder ungeerdet. |
| U-07 | Muss | Angeschlossene Geräte (Melder, Kamera, Zentralen-Eingänge) dürfen durch das Suchsignal nicht beschädigt werden. Empfindlichstes Gerät: F-12. |

### 3.3 Verlegung und Baustoffe

| ID | Prio | Anforderung |
|---|---|---|
| U-08 | Muss | Aufputz, Unterputz, Hohlwand, abgehängte Decke, Kabeltrasse, Leerrohr. |
| U-09 | Muss | Wandbaustoffe: Gipskarton, Mauerwerk (Ziegel, Kalksandstein, Porenbeton), Beton, Stahlbeton. Realistische Reichweiten je Baustoff schätzen wir in Phase 2 ab und messen sie in Phase 3. |
| U-10 | offen (F-07) | Metallische Kabeltrassen und Stahlpanzerrohr. |

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
| S-03 | Muss | Umschaltbare Einspeisemodi, mindestens Ader/Ader, Ader/Schirm, Ader/Erde. Welche Modi es endgültig gibt (auch Kombinationen, z. B. mehrere Adern gemeinsam), entscheidet das Konzept danach, was physikalisch am meisten bringt. |
| S-04 | Muss | Ausgangsspannung im berührungsunkritischen Schutzkleinspannungsbereich (Obergrenze 25 V AC eff bzw. 60 V DC nach DIN VDE 0100-410). Der tatsächliche Wert wird in Phase 2 so festgelegt, dass U-07 erfüllt ist. |
| S-05 | Muss | Dauerhaft kurzschlussfest, an jeder Klemmenkombination. |
| S-06 | Muss | Arbeitet stabil an Leitungen bis zur Maximallänge (F-08), in jedem Einspeisemodus. Die Leitungskapazität darf den Sender weder beschädigen noch zum Schwingen bringen; bei Überlast begrenzt er selbst. |
| S-07 | Muss | Versorgung mit AA- oder AAA-Batterien (Typ: F-16). NiMH-Akkus müssen funktionieren. |
| S-08 | Muss | Automatische Abschaltung. Weil der Sender unbeaufsichtigt läuft, während man sucht, darf sie nicht zu früh kommen (Zeit: F-16). |
| S-09 | Muss | Anzeige von Betrieb, Einspeisemodus, Batteriezustand und Fehlern (Kurzschluss/Überlast, Fremdspannung). Ausführung: F-15. |
| S-10 | Soll | Anzeige des eingespeisten Stroms. Zeigt sofort, ob überhaupt ein Stromkreis zustande kommt. |
| S-11 | Soll | Mindestens zwei Leistungsstufen (wenig Übersprechen auf Nachbarleitungen ↔ große Reichweite). |
| S-12 | Muss | Übersteht Fremdspannung an den Klemmen dauerhaft ohne Schaden. Wert: F-13 (Vorschlag 60 V DC / 50 V AC). |
| S-13 | Kann | **Fremdspannungswarnung:** misst vor dem Einspeisen und warnt. Nur, wenn sie zuverlässig ist. Entscheidung mit Begründung in Phase 2. |
| S-14 | Kann | **Durchgangs-/Schleifentest:** zeigt an, ob das Leitungsende gebrückt ist (z. B. als Widerstand in Ω). Nur, wenn das Ergebnis eindeutig ist. Entscheidung mit Begründung in Phase 2. |
| S-15 | Muss | Messleitungen mit Krokodilklemmen (Ausführung: F-14), Zugentlastung am Gehäuse. |
| S-16 | Soll | Batterielaufzeit im Dauerbetrieb mindestens ein Arbeitstag (8 h) bei normaler Leistungsstufe. |
| S-17 | Soll | Signal mit Kennung, damit das Handgerät nur auf den eigenen Sender reagiert und nicht auf Störquellen oder fremde Tonsucher. |

---

## 5. Anforderungen an das Handgerät

| ID | Prio | Anforderung |
|---|---|---|
| H-01 | Muss | Akustische Anzeige über Lautsprecher: Der Ton ändert sich mit der Signalstärke (Tonhöhe und/oder Taktrate). |
| H-02 | Muss | OLED-Anzeige mit Balken und Zahlenwert der Signalstärke. |
| H-03 | Muss | Ortungsprinzip nach Konzept (magnetisch, elektrisch oder beides). Der aktive Sensor bzw. Modus ist im Display erkennbar. |
| H-04 | Muss | **Bündel-Modus:** feine Spitze für die Identifikation in Bündel/Trasse und am abisolierten Ende; Empfindlichkeit regelbar. |
| H-05 | Muss | Selektive Auswertung: zeigt nur das Sendersignal an. Unterdrückt 50 Hz mit Oberwellen, Schaltnetzteile und LED-Treiber. |
| H-06 | Muss | Empfindlichkeit von Hand einstellbar. |
| H-07 | Soll | Automatische Empfindlichkeit als zusätzliche Betriebsart. |
| H-08 | Muss | Versorgung mit AA- oder AAA-Batterien (F-16), NiMH-tauglich. Automatische Abschaltung, die beim Suchen nicht stört (Vorschlag: 10 min ohne Bedienung **und** ohne Signal). |
| H-09 | Muss | Batterieanzeige. |
| H-10 | Soll | Lautstärke einstellbar, auch stumm. |
| H-11 | Soll | Einhändig bedienbar; Display beim Suchen ablesbar. Bauform: F-17. |
| H-12 | Soll | Ruhige Anzeige: Wert springt nicht durch Rauschen, wenn das Gerät stillgehalten wird. |
| H-13 | Kann | Kopfhöreranschluss (F-18). |
| H-14 | Kann | Richtungsanzeige links/rechts (F-18). |
| H-15 | Kann | Tiefenschätzung (F-18). Geht nur bei Magnetfeld-Ortung und nur als Schätzung. |

---

## 6. Anforderungen an die Gehäuse

| ID | Prio | Anforderung |
|---|---|---|
| G-01 | Muss | Druck in PETG auf Bambu Lab (Modell und Bauraum: F-01). |
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
| E-01 | Muss | Nur handlötbare Bauformen (THT, SMD ab 0805 bzw. SOIC) oder Bestückung durch den Platinenhersteller. Nichts, was Heißluft braucht (z. B. QFN, BGA). Grenze: F-03. |
| E-02 | Muss | Mikrocontroller mit der Arduino IDE programmierbar. |
| E-03 | Soll | Programmieren über USB, ohne zusätzlichen Programmer. |
| E-04 | Muss | Abgleich und Test mit Multimeter und Oszilloskop möglich; beschriftete Messpunkte. |
| E-05 | Muss | Firmware gut kommentiert; Parameter (Frequenz, Abschaltzeit usw.) zentral einstellbar. |
| E-06 | Muss | Vor der Platinenbestellung ein Machbarkeitstest auf dem Steckbrett (Phase 3). |
| E-07 | Muss | Aufbauunterlagen: KiCad-Projekt, oder eindeutige Netzliste mit Beschreibung, oder Lochrasterplan. |

---

## 8. Sicherheit und Abgrenzung

### 8.1 Sicherheit

| ID | Prio | Anforderung |
|---|---|---|
| N-01 | Muss | Sender nur für spannungsfreie Leitungen. Hinweis am Gerät (G-09) und in der Anleitung. |
| N-02 | Muss | Ausgang begrenzt (S-04) und kurzschlussfest (S-05). |
| N-03 | Muss | Verhalten bei versehentlichem Anschluss an 230 V: F-13. Vorschlag: nicht dafür ausgelegt, aber sicher ausfallen (Sicherung, kein Brand); danach Reparatur. |

### 8.2 Bewusst nicht Teil des Projekts

- **Keine** Suche an Leitungen unter Spannung, **keine** Einspeisung in Netzspannung.
- **Keine** passive Ortung von Netzleitungen über ihr 50-Hz-Feld („Bohrschutz“). Begründung:
  Eine abgeschaltete, geschirmte oder tief liegende Netzleitung zeigt nichts an; „kein Ausschlag“
  würde dann falsche Sicherheit vermitteln. *(Vorschlag – du kannst widersprechen.)*
- **Kein** Funk, keine App.
- **Kein** CE-Konformitätsverfahren (Eigenbau für den eigenen Gebrauch). Wir bleiben trotzdem bei
  kleiner Sendeleistung und in Frequenzbereichen, die auch professionelle Leitungsortungsgeräte nutzen.

---

## 9. Abnahmetests

Die Zielwerte tragen wir in Phase 2 (Konzept) mit Begründung ein. Gemessen wird in Phase 3
(Steckbrett, soweit möglich) und Phase 8 (fertiges Gerät).

| ID | Szenario | Prüft | Zielwert |
|---|---|---|---|
| T-01 | Freie Leitung (≥ 50 m, ausgelegt), Ende offen: Erkennungsabstand in Luft, je Einspeisemodus | U-05, H-03 | Phase 2 |
| T-02 | Wie T-01, Ende gebrückt bzw. geerdet | U-05, S-03 | Phase 2 |
| T-03 | Leitung in bzw. hinter Mauerwerk: Erkennungsabstand und Ortungsgenauigkeit | U-09 | Phase 2 |
| T-04 | Leitung in bzw. unter Stahlbeton (reales Objekt) | U-09 | Phase 2 |
| T-05 | Richtige Leitung in einem Bündel von ≥ 10 Leitungen eindeutig finden | H-04 | Phase 2 |
| T-06 | Richtige Ader am abisolierten Ende finden | H-04 | Phase 2 |
| T-07 | LED-Treiber, Schaltnetzteil, Leuchtstofflampe in 30 cm Abstand: keine Fehlanzeige | H-05 | keine Fehlanzeige |
| T-08 | Kurzschluss am Ausgang, 1 h, jede Klemmenkombination | S-05 | kein Schaden, Fehleranzeige |
| T-09 | Endgerät angeschlossen (z. B. Melder, 12-V-Kamera), alle Einspeisemodi | U-07 | kein Schaden, Gerät funktioniert danach |
| T-10 | Fremdspannung (Wert aus F-13) an den Klemmen | S-12, S-13 | kein Schaden; Warnung, falls S-13 umgesetzt |
| T-11 | Lange Leitung (Maximallänge oder verfügbare Trommel): Sender stabil, Signal am Ende ortbar | S-06 | Phase 2 |
| T-12 | Batterielaufzeit und automatische Abschaltung, beide Geräte | S-08, S-16, H-08 | Phase 2 |
| T-13 | Vergleich mit Referenz-Tonsucher (F-05) unter gleichen Bedingungen | Ziel aus Kap. 1 | deutlich größerer Erkennungsabstand (Wert Phase 2) |

---

## 10. Budget

| ID | Anforderung |
|---|---|
| B-01 | Ziel ca. 40 € pro Satz (Sender + Handgerät). Funktion geht vor Preis. |
| B-02 | Bevor die Stückliste 40 € überschreitet, sprechen wir darüber. |
| B-03 | Was in die 40 € zählt: F-19. |

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
hardware/sender/             Schaltplan, KiCad-Projekt bzw. Aufbauplan
hardware/handgeraet/         Schaltplan, KiCad-Projekt bzw. Aufbauplan
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
| 5 | Platine bzw. Aufbauplan | KiCad-Projekt oder Netzliste/Lochrasterplan |
| 6 | Firmware | Sketche für beide Geräte, gut kommentiert |
| 7 | Gehäuse | Quellcode, STL, Druckeinstellungen |
| 8 | Inbetriebnahme | Aufbau-, Abgleich- und Testanleitung mit Messpunkten, Bedienungsanleitung |

---

## 13. Bekannte physikalische Grenzen (Vorschau)

Diese Punkte sind der Grund für einige Rückfragen in Kapitel 14. Ausführlich erklärt und
gelöst werden sie im Konzept (Phase 2).

| ID | Grenze |
|---|---|
| R-1 | **Stahlbeton:** Bewehrung und feuchter Beton schirmen das elektrische Feld stark ab. Durch Stahlbeton kommt praktisch nur ein Magnetfeld, und das entsteht nur, wo Strom fließt. |
| R-2 | **Magnetfeld braucht Strom:** Bei offenem Ende fließt nur ein kapazitiver Ladestrom. Er ist am Anfang am größten und fällt zum Ende hin auf null. |
| R-3 | **Feldauslöschung:** Fließt der Strom auf einer Ader hin und auf der Nachbarader zurück, heben sich die Felder außen fast vollständig auf, bei verdrillten Paaren besonders stark. |
| R-4 | **Geerdeter Schirm:** hält das elektrische Feld der Adern zurück und führt zusätzlich Strom zurück, der das Magnetfeld schwächt. |
| R-5 | **Übersprechen:** Je höher die Frequenz und je länger Leitungen parallel liegen, desto mehr Signal springt auf Nachbarleitungen über. In Trassen „leuchten“ dann auch Nachbarkabel. |
| R-6 | **Metall in der Nähe:** Stahlrohr und Metalltrassen dämpfen und verbiegen die Felder. |

---

## 14. Offene Fragen (Phase 1)

Zu jeder Frage steht mein Vorschlag. Wenn er passt, reicht „ok“. Nach deinen Antworten
trage ich alles ein und lege dir v1.0 zur Freigabe vor.

### A. Werkstatt und Beschaffung

**F-01 Drucker.** Welches Bambu-Lab-Modell hast du (A1 mini, A1, P1S, X1C, H2D …) und welche Düse?
*Warum:* Der Bauraum begrenzt die Länge des Handgeräts (A1 mini: 18 cm).
*Vorschlag:* 0,4-mm-Düse, Teile bis 25 cm Länge.
*Antwort:* –

**F-02 KiCad.** Nutzt du KiCad? Wenn ja, welche Version, und wie sicher bist du darin?
*Vorschlag:* Ich liefere ein komplettes KiCad-Projekt mit Fertigungsdaten; du musst nur bestellen.
*Antwort:* –

**F-03 Bezugsquellen und Löten.** Wo bestellst du (Reichelt, Conrad, LCSC/JLCPCB, Mouser/DigiKey,
AliExpress, Amazon)? Sind Platinen aus China mit 1–3 Wochen Lieferzeit okay? Wäre es okay, wenn
der Platinenhersteller kleine SMD-Teile vorbestückt? Traust du dir SMD in 0805 und SOIC mit dem
Lötkolben zu?
*Vorschlag:* Platinen bei JLCPCB, Rest bei Reichelt, Module (Display, Controller-Board) evtl.
AliExpress. Handlöten bis 0805/SOIC.
*Antwort:* –

**F-04 Messgeräte und Bastelkiste.** Welches Oszilloskop (Modell, hat es FFT)? Hast du einen
Funktionsgenerator? Was liegt schon da (Arduino Nano/Uno, ESP32, Raspberry Pi Pico, OLED,
Ferritstäbe, Operationsverstärker, Steckbrett)?
*Warum:* Für den Steckbrett-Test nehme ich möglichst, was du schon hast.
*Antwort:* –

**F-05 Testmaterial.** Hast du eine längere Leitung für Tests (Resttrommel J-Y(St)Y, ≥ 50–100 m)?
Und einen einfachen Tonsucher als Vergleich, welches Modell?
*Warum:* Die Trommel brauchen wir für Tests mit langer Leitung; der Billigsucher ist unsere Messlatte
für „spürbar besser“ (T-13).
*Antwort:* –

### B. Einsatz

**F-06 Suchaufgaben.** Wie verteilen sich deine Aufgaben?
a) Verlauf durch Wand oder Decke verfolgen,
b) richtige Leitung in Trasse oder Bündel finden (Mantel geschlossen),
c) richtige Ader am abisolierten Ende finden (Verteiler, Dose).
*Hinweis:* Eine einzelne Ader in einem geschlossenen Kabel lässt sich von außen nicht von ihren
Nachbaradern unterscheiden. Das geht nur am offenen Ende (Fall c).
*Vorschlag (Annahme):* a 50 %, b 30 %, c 20 %.
*Antwort:* –

**F-07 Wo es heute hakt.** Wo versagt dein jetziger Tonsucher am häufigsten? Wie liegen die Leitungen
typischerweise (Unterputz, Hohlwand, Leerrohr in Betondecke, Metalltrasse, Stahlpanzerrohr) und wie tief?
*Warum:* Darauf optimieren wir das Konzept.
*Antwort:* –

**F-08 Länge und Leitungstypen.** Wie lang ist die längste Leitung, die du realistisch suchst? Sollen
außer J-Y(St)Y/LIYY auch Cat-Netzwerkleitungen, Koax (Video) oder spannungsfreie NYM gehen?
*Vorschlag:* bis 500 m; Cat und Koax als „Soll“, NYM spannungsfrei als „Kann“.
*Antwort:* –

**F-09 Leitungsanfang.** Klemmst du die Leitung zum Suchen komplett ab (alle Adern und Schirm frei)?
Kannst du den Schirm dort vorübergehend von der Erde lösen? Wäre es okay, alle Adern gemeinsam
(z. B. in einer WAGO-Klemme) an den Sender anzuschließen?
*Warum:* Ein am Anfang geerdeter Schirm hält das Signal der Adern weitgehend zurück (R-4). Alle Adern
gemeinsam einzuspeisen ist ein Kandidat für die stärkste Ortung; das prüfe ich in Phase 2.
*Vorschlag:* ja / ja / ja.
*Antwort:* –

**F-10 Erde am Einspeiseort.** Gibt es dort meist eine brauchbare Erde (PE/PAS in der Zentrale,
Heizungsrohr, Metalltrasse)?
*Warum:* Professionelle Ortungsgeräte führen den Strom oft über Erde zurück. Dann gibt es keine
Rückleitung im selben Kabel, die das Feld aufhebt (R-3). Das bringt meist die größte Reichweite.
*Antwort:* –

**F-11 Fernes Ende.** Kennst du das Ende meistens und kommst du hin, sodass du dort eine Brücke oder
Erde setzen könntest? Oder ist es typischerweise unbekannt? Wie oft hast du Ringleitungen (beide
Enden in der Zentrale)?
*Warum:* Ein Magnetfeld entsteht nur, wo Strom fließt (R-2). Ob sich der Stromkreis am Ende schließen
lässt, entscheidet mit über die Reichweite.
*Antwort:* –

**F-12 Geräte am Ende.** Was hängt typischerweise dran, und was davon ist am empfindlichsten? Zum
Beispiel Ringbus-Melder (BMA), EMA-Melder mit Abschlusswiderstand (welche Werte?), 12-V-Kameras,
PoE-Geräte, Zentralen-Eingänge.
*Warum:* Danach richtet sich, welche Spannung zwischen zwei Adern höchstens anliegen darf (U-07).
*Antwort:* –

**F-13 Fremdspannung.** Womit muss der Sender realistisch rechnen?
a) parallel an einer laufenden Zentrale oder einem Ringbus (ca. 12–40 V DC),
b) versehentlich an 230 V.
*Vorschlag:* a) dauerhaft ohne Schaden überstehen (bis 60 V DC / 50 V AC) und warnen;
b) nicht dafür ausgelegt, aber sicher ausfallen (Sicherung, kein Brand), danach Reparatur.
*Antwort:* –

### C. Sender

**F-14 Messleitungen.** Fest mit Zugentlastung oder 4-mm-Sicherheitsbuchsen mit abnehmbaren
Leitungen? Welche Länge?
*Vorschlag:* fest, 3 × 1 m Silikonlitze mit isolierten Krokodilklemmen: Rot (A), Schwarz (B),
Grün-Gelb (Schirm/Erde). Zusätzlich eine 4-mm-Buchse für eine optionale lange Erdleitung.
*Antwort:* –

**F-15 Anzeige am Sender.** Reichen LEDs, oder lieber ein kleines OLED?
*Vorschlag:* OLED 0,96″ (ca. 2–3 € mehr). Es zeigt den eingespeisten Strom in mA, die Fremdspannung
in V und den Schleifenwiderstand in Ω eindeutig an. Mit LEDs ginge das nur grob.
*Antwort:* –

**F-16 Batterien und Abschaltzeit.** AA oder AAA? Alkaline oder NiMH-Akkus? Gleicher Typ in beiden
Geräten? Nach welcher Zeit soll der Sender abschalten?
*Vorschlag:* AA in beiden Geräten, NiMH-tauglich. Sender schaltet 2 h nach der letzten Bedienung ab
(umschaltbar 30 min / 8 h). Handgerät nach 10 min ohne Bedienung und ohne Signal.
*Antwort:* –

### D. Handgerät

**F-17 Bauform.** Stab, Pistole oder flach wie ein Multimeter?
*Vorschlag:* Stab, ca. 22–25 cm, wie die Empfänger professioneller Ortungsgeräte: Sensorkopf vorn,
Display und Lautsprecher am Griff, Batterien im Griff. Für den Bündel-Modus eine feine Spitze vorn.
*Antwort:* –

**F-18 Extras.** Was davon willst du?
Kopfhöreranschluss (laute Umgebung); Richtungsanzeige links/rechts (zweite Sensorspule, ca. 2–4 €);
Tiefenschätzung (nur bei Magnetfeld-Ortung, nur als Schätzung).
*Vorschlag:* Kopfhörer ja. Richtungsanzeige und Tiefe prüfe ich in Phase 2; die Platine wird darauf
vorbereitet.
*Antwort:* –

### E. Rahmen

**F-19 Budget-Umfang.** Was zählt in die 40 €? Auch Platinen inkl. Versand, Messleitungen, Batterien,
Filament, Schrauben und Gewindeeinsätze?
*Vorschlag:* alles, was im fertigen Gerät steckt, plus Platinen und Versand anteilig. Nicht: Batterien,
Filament, Werkzeug, Verbrauchsmaterial.
*Antwort:* –

**F-20 Stückzahl und Termin.** Wie viele Sätze willst du bauen (nur für dich oder auch für Kollegen)?
Gibt es einen Termin?
*Vorschlag:* 1 Satz, keine feste Frist.
*Antwort:* –

---

## Änderungsverlauf

| Version | Datum | Änderung |
|---|---|---|
| 0.1 | 13.10.2026 | Erster Entwurf aus dem Auftrag; offene Fragen F-01 … F-20 |
