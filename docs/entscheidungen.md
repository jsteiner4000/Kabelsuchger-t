# Entscheidungen

Hier steht jede Entscheidung mit Begründung, damit wir später nachvollziehen können,
warum etwas so ist, wie es ist.

**Format:** Nummer, Datum, Status (*vorgeschlagen*, *beschlossen*, *verworfen*,
*ersetzt durch E-xxx*), Inhalt, Begründung und, wo sinnvoll, verworfene Alternativen.
Verweise wie S-13 oder F-09 beziehen sich auf das [Lastenheft](lastenheft.md).

---

## Beschlossen

### E-001 Projektstruktur und Arbeitsweise
- **Datum:** 13.10.2026 · **Status:** beschlossen (Vorgabe aus dem Auftrag)
- **Inhalt:** Ordner `docs/`, `hardware/sender/`, `hardware/handgeraet/`, `firmware/sender/`,
  `firmware/handgeraet/`, `gehaeuse/`, Datei `stueckliste.csv`. Arbeit in 8 Phasen; nach jeder
  Phase gibt Julius frei.
- **Begründung:** Klare Trennung von Dokumentation, Hardware, Firmware und Mechanik. Die Freigaben
  verhindern, dass auf einer falschen Annahme weitergebaut wird.

### E-002 Lastenheft vor dem Konzept
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Inhalt:** Zuerst ein Lastenheft (*was* das Gerät können muss), getrennt vom Konzept (*wie*).
  Lastenheft v1.0 wird vor Phase 2 freigegeben. Dazu gibt es eine visuelle Übersicht.
- **Begründung:** Julius' Wunsch, nicht aneinander vorbeizureden. Das Lastenheft ist die Messlatte
  für alle späteren Entscheidungen und die Grundlage der Abnahmetests.

### E-003 Das Repository ist die einzige verlässliche Quelle
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Inhalt:** Anforderungen, Entscheidungen, Messergebnisse und Abweichungen werden im Repository
  festgehalten, nicht nur im Chat.
- **Begründung:** Chat-Verläufe gehen verloren, Arbeitssitzungen wechseln. Wer später weiterarbeitet,
  muss alles Nötige im Repo finden.

### E-004 Stückliste als CSV mit Semikolon
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Inhalt:** `stueckliste.csv` nutzt `;` als Trennzeichen und `,` als Dezimalzeichen.
- **Begründung:** Öffnet sich in deutschem Excel und LibreOffice direkt richtig.

### E-005 Keine passive 50-Hz-Ortung von Netzleitungen
- **Datum:** 13.10.2026 · **Status:** beschlossen (vorgeschlagen in v0.1, keine Einwände)
- **Inhalt:** Das Handgerät bekommt keinen Modus, der Netzleitungen über ihr 50-Hz-Feld findet
  („Bohrschutz“).
- **Begründung:** Eine abgeschaltete, geschirmte oder tief liegende Netzleitung erzeugt kein messbares
  Feld. „Kein Ausschlag“ würde dann falsche Sicherheit vor dem Bohren vermitteln. Das widerspricht dem
  Grundsatz aus dem Auftrag, lieber wegzulassen als falsche Sicherheit zu geben.
- **Alternative:** Modus mit deutlichem Warnhinweis. Verworfen, weil sich ein Warnhinweis im Alltag
  abnutzt.

### E-006 Kein SMD-Handlöten: THT und fertige Module
- **Datum:** 13.10.2026 · **Status:** beschlossen (F-03)
- **Inhalt:** Alles, was Julius selbst lötet, ist THT oder ein fertiges Modul mit Stiftleisten.
  SMD kommt nur in Frage, wenn der Platinenhersteller bestückt.
- **Begründung:** Julius lötet kein SMD. Herstellerbestückung kostet bei einem einzigen Satz
  Rüstkosten, die das Budget stark belasten; der Kostenvergleich folgt in Phase 4.

### E-007 OLED-Anzeige in beiden Geräten, nur echte Messwerte
- **Datum:** 13.10.2026 · **Status:** beschlossen (F-15)
- **Inhalt:** Sender und Handgerät bekommen ein OLED. Die Anzeige soll schick sein, aber jeder
  Zahlenwert ist ein echter Messwert mit Einheit oder nachvollziehbarer Skala (Lastenheft Kap. 5a).
- **Begründung:** Julius' Wunsch. Beim Sender macht das OLED eindeutige Werte möglich (Strom in mA,
  Fremdspannung in V, Schleifenwiderstand in Ω), die mit LEDs nur grob gingen.

### E-008 Kein Kopfhöreranschluss
- **Datum:** 13.10.2026 · **Status:** beschlossen (F-18)
- **Inhalt:** H-13 entfällt.
- **Begründung:** Ausdrücklich nicht gewünscht.

### E-009 Versorgung mit AA-Zellen, Abschaltzeiten
- **Datum:** 13.10.2026 · **Status:** beschlossen (F-16)
- **Inhalt:** Beide Geräte mit AA, Alkaline und NiMH. Sender schaltet 2 h nach der letzten Bedienung
  ab (umschaltbar 30 min / 8 h), Handgerät nach 10 min ohne Bedienung und ohne Signal.
- **Begründung:** AA hat etwa die doppelte Kapazität von AAA. Gleiche Zellen in beiden Geräten
  vereinfachen den Alltag. Der Sender läuft unbeaufsichtigt und braucht deshalb eine lange Zeit.

### E-010 Leitungslänge: 300 m Muss, 500 m Soll
- **Datum:** 13.10.2026 · **Status:** beschlossen (F-08)
- **Inhalt:** U-03 auf 300 m (Muss) und 500 m (Soll) festgelegt.
- **Begründung:** Julius hält 500 m für unrealistisch. Ob es geht, hängt vor allem davon ab, wie der
  Strom zurückfließt, weniger von der Länge an sich. Das Konzept rechnet es nach, bevor wir es
  versprechen.

### E-011 Durchgangstest auf Soll, NYM als Kann
- **Datum:** 13.10.2026 · **Status:** geändert durch E-034/E-038 (Durchgangstest entfällt, NYM-Ortung bleibt Kann)
- **Inhalt:** S-14 (Durchgangs-/Schleifentest) von Kann auf Soll. NYM wird als Kann aufgenommen,
  nur freigeschaltet und gesichert (U-02b, N-04).
- **Begründung:** Julius möchte Leitungen „durchmessen“ können, auch NYM. Eine Widerstandsmessung
  mit kleinem Gleichstrom liefert einen eindeutigen Messwert; wo die Deutung unsicher ist
  (z. B. Gerät am Ende), wird nur der Wert angezeigt.

### E-012 Fremdspannung: 60 V DC / 50 V AC, kein 230-V-Schutz gefordert
- **Datum:** 13.10.2026 · **Status:** beschlossen (F-13)
- **Inhalt:** Der Sender übersteht 60 V DC / 50 V AC dauerhaft. Ein Schutz gegen 230 V ist nicht
  gefordert; ob eine billige Eingangssicherung trotzdem hineinkommt, entscheidet Phase 4.
- **Begründung:** Laut Julius kommt 230 V nie vor. Ein parallel angeklemmter, laufender Ringbus
  oder eine Zentrale mit bis zu ca. 40 V ist dagegen denkbar.

### E-013 Schwerpunkt Trasse und Unterputz, Selektivität als Muss
- **Datum:** 13.10.2026 · **Status:** beschlossen (F-07)
- **Inhalt:** Neue Anforderung U-13: In Trassen muss sich die gesuchte Leitung eindeutig von
  Nachbarleitungen abheben, auf die Signal übergesprochen hat. Metall- und Gitterrinnen sind Muss.
- **Begründung:** Julius sucht meistens in Trassen und unter Putz. In vollen Trassen ist nicht die
  Reichweite das Hauptproblem, sondern dass Nachbarkabel mitleuchten.

### E-014 KiCad als Ziel für die Platine
- **Datum:** 13.10.2026 · **Status:** entfällt mit E-042 (Lochraster statt Platine)
- **Inhalt:** Die Platinen werden als KiCad-Projekt geliefert, möglichst automatisiert erstellt
  (Wunsch: per KiCad-MCP).
- **Stand der Werkzeugfrage:** In der aktuellen Cloud-Umgebung ist KiCad noch nicht installiert, und
  die KiCad-Paketquellen sind dort nicht erreichbar. Bis Phase 5 klären wir, ob KiCad per Setup-Skript
  in die Umgebung kommt oder ein KiCad-MCP auf Julius' Rechner genutzt wird. Ausweichlösung:
  eindeutige Netzliste mit Beschreibung.

---

## Beschlossen in Phase 2 (Konzept freigegeben am 13.10.2026)

Begründungen ausführlich in [`konzept.md`](konzept.md); die Kapitelnummern beziehen sich darauf.
Mit der Freigabe hat Julius auch das Budget von ca. 50 € pro Satz (E-027) und das Selbstwickeln der
Spulen bestätigt.

### E-015 Magnetfeld als Hauptprinzip, E-Feld als Spezialwerkzeug
- **Datum:** 13.10.2026 · **Status:** Magnetfeld beschlossen; E-Feld-Spitze entfällt (E-036)
- **Inhalt:** Ortung über das Magnetfeld (Ferritspulen). E-Feld-Spitze nur für Bündel und abisolierte Adern.
- **Begründung:** Bei kHz schirmen feuchter Putz, Beton, Bewehrung und geerdete Schirme das E-Feld ab,
  das Magnetfeld nicht (Kap. 1.1, 1.5). Am offenen Ende fließt kein Strom, dort braucht man das E-Feld.
- **Verworfen:** nur E-Feld (Variante A). Erfüllt U-09 und U-13 nicht.

### E-016 Hauptmodus „Bündel gegen Erde“ (G), dazu Modi A und P
- **Datum:** 13.10.2026 · **Status:** G und A beschlossen, als Anklemm-Anleitung ohne Relais; P entfällt (E-037)
- **Inhalt:** G: alle Adern + Schirm gemeinsam (WAGO) gegen Erde, Schirm am Anfang gelöst. A: eine Ader gegen
  den Rest (Identifikation am Ende). P: Paar (Durchgang, Länge, Paar finden). Umschaltung mit 2 Relais.
- **Begründung:** Hin- und Rückstrom im selben Kabel heben sich auf (Ader/Ader: bei 10 cm ca. 1/70 des Feldes,
  verdrillt nahezu null; Kap. 1.2). Mit Rückweg über Erde bleibt das volle Feld. In G sieht ein Gerät am Ende
  keine Spannung (U-07), und die sechs Fälle des Lastenhefts reduzieren sich auf „Erde am Ende ja/nein“.
- **Verworfen:** Ader/Ader und Ader/Schirm als Ortungsmodus (Feld bleibt im Kabel).

### E-017 Zweiton-Signal und drei Frequenzsätze
- **Datum:** 13.10.2026 · **Status:** Zweiton beschlossen; nur Satz Standard (E-029, E-037)
- **Inhalt:** Standard 8225 Hz + 4112,5 Hz (Stromrichtung), Tief 1025 Hz + 512,5 Hz, Hoch 32,9 kHz.
  Frequenzen zwischen den 50-Hz-Oberwellen; Handgerät erkennt den Satz selbst.
- **Begründung:** 8 kHz ist der Profi-Kompromiss. Tief: weniger Übersprechen, besser bei Stahl, braucht Erde am
  Ende. Hoch: offenes Ende +24 dB. Spulen sind unabgestimmt, die Frequenz ist reine Firmware (Kap. 4.1).

### E-018 Stromrichtung nach dem CD-Prinzip mit Referenztaste
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Inhalt:** Phasenvergleich von Ortungs- und Richtungston; Referenz einmal in Sendernähe setzen; Anzeige
  → HIN / ← ZURÜCK / ?. Der Zweiton dient gleichzeitig als Kennung (S-17).
- **Begründung:** Löst U-13 (volle Trassen); bewährt bei Radiodetection („Current Direction“). „?“ statt
  falschem Pfeil bei unklarer Lage.

### E-019 Handgerät: 24-bit-Wandler PCM1808 und digitale Lock-in-Auswertung
- **Datum:** 13.10.2026 · **Status:** Lock-in beschlossen; PCM1808 ersetzt durch E-035
- **Inhalt:** H-Spule, V-Spule, E-Spitze → Vorverstärker (×1/×30) → PCM1808 (Stereo, ≈ 90 kHz) → ESP32-S3.
  IQ-Demodulation mit 2–10 Hz Bandbreite.
- **Begründung:** Der ESP32-interne ADC (12 bit, max. 83 kS/s, verrauscht) reicht für Dynamik und Phase nicht.
  Lock-in liefert Betrag und Phase fortlaufend; daraus folgen Links/Rechts, Null, Tiefe, Stromrichtung.
- **Verworfen:** analoger Bandpass (zu breit, keine Phase); Goertzel (geht, aber blockweise; Lock-in ist flexibler).

### E-020 Mikrocontroller: Sender ESP32-S2 mini, Handgerät ESP32-S3
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Begründung:** Der S2 hat einen D/A-Wandler für die Signalerzeugung, und Julius hat schon einen. Der S3 hat ein
  Gleitkomma-Rechenwerk und zwei Kerne (Rechnen und Anzeige getrennt), dadurch ist die Firmware lesbarer.
  Beide lassen sich über USB in der Arduino IDE programmieren.

### E-021 Sender-Endstufe L272 in Brücke an 24 V, max. 15 V eff
- **Datum:** 13.10.2026 · **Status:** ersetzt durch E-028
- **Inhalt:** Stufen Fein (5 V / 5 mA), Normal (15 V / 30 mA), Max (15 V / 60 mA). Schutz: Serienwiderstand,
  PTC, TVS, Firmware-Strombegrenzung. In A/P Spannung hart auf ca. 12 V Spitze begrenzt.
- **Begründung:** THT (DIP-8), geringer Ruhestrom, genug Spannung für kapazitive Last; 15 V eff liegen sicher
  unter 25 V eff (S-04).

### E-022 Fremdspannungswarnung (S-13): ja, als Sperre ohne „spannungsfrei“-Anzeige
- **Datum:** 13.10.2026 · **Status:** ersetzt durch E-038 (entfällt)
- **Begründung:** Zuverlässig, solange sie nur warnt und nie bestätigt (Kap. 6.5).

### E-023 Durchgang (S-14): ja, plus Lastbild und Längenschätzung
- **Datum:** 13.10.2026 · **Status:** ersetzt durch E-038 (entfällt)
- **Inhalt:** DC-Messung ≤ 1 mA / ≤ 3,3 V in beiden Polaritäten; AC-Lastbild aus Spannung, Strom und Phase;
  Länge offener Paare aus der Kapazität (≈ 100 nF/km), gekennzeichnet als Schätzung.

### E-024 Bauform Handgerät: Paddel-Stab ca. 22 cm mit Hals ≥ 6 cm
- **Datum:** 13.10.2026 · **Status:** geändert durch E-033 und E-036 (Kopf oben, eine Spule); Abstand Spule – Digitalteil ≥ 6 cm bleibt (Messung M4)
- **Begründung:** Spulen weit weg vom Digitalteil (Störungen); Kopf 90 mm breit für 80-mm-Ferritstab; passt auf
  25,6 cm Bauraum. Handkontakt am Griff verbessert die E-Spitze.

### E-025 Anzeige: OLED 1,3″ in beiden Geräten, Feld in nT
- **Datum:** 13.10.2026 · **Status:** OLED 1,3″ in beiden Geräten bleibt; Schnittstelle I²C (E-030); dB statt nT (E-039)
- **Inhalt:** Handgerät über SPI für flüssige Grafik. Feld nach Kalibrierung mit dem Sender in nT (A-01).

### E-026 Aufbau: eigene Platinen (KiCad, JLCPCB), nur THT und Module
- **Datum:** 13.10.2026 · **Status:** ersetzt durch E-042 (Lochraster); nur THT und Module bleibt
- **Begründung:** E-01/E-006; Analogteil und Spulenumfeld brauchen ein sauberes Layout. Spulen werden selbst
  gewickelt (gedruckter Spulenkörper, Akkuschrauber).

### E-027 Variante C, Kosten ca. 50 € pro Satz
- **Datum:** 13.10.2026 · **Status:** ersetzt durch E-034 (Variante „Einfach“, ca. 38 €)
- **Begründung:** Variante B wäre nur ca. 5 € billiger, kann aber keine Stromrichtung, kein Links/Rechts und
  keine Tiefe. Den Großteil der Kosten tragen Displays, Platinen, Batteriehalter und Messleitungen (Kap. 8, 11).

---

## Beschlossen in Phase 3

### E-028 Endstufe TL072 + BD139/BD140 statt L272
- **Datum:** 13.10.2026 · **Status:** beschlossen (ersetzt E-021)
- **Inhalt:** Je Brückenzweig ein Operationsverstärker (TL072) mit Gegentakt-Emitterfolger aus BD139 (NPN) und
  BD140 (PNP) in der Gegenkopplung; Versorgung 24 V aus MT3608. Ausgang über Koppelkondensator, 47 Ω und PTC.
- **Begründung:** Der L272 ist bei onsemi abgekündigt, die Lieferbarkeit beim ST-Original ist unsicher. TL072,
  BD139 und BD140 sind Standardteile, THT und dauerhaft lieferbar. Gleiche Funktion, Ruhestrom kleiner, Kosten
  gleich. Der 47-Ω-Widerstand entkoppelt die kapazitive Kabellast, damit die Endstufe nicht schwingt.
- **Prüfung:** M1 im Machbarkeitstest.

### E-029 Frequenzen exakt aus dem DAC-Takt
- **Datum:** 13.10.2026 · **Status:** beschlossen; seit E-037 nur noch Standard (und Prüfton 1041,7 Hz im Test)
- **Inhalt:** Standard 200 kHz/24 = 8333,3 Hz + 200 kHz/48 = 4166,7 Hz; Tief 50 kHz/48 = 1041,7 Hz +
  50 kHz/96 = 520,8 Hz; Hoch 400 kHz/12 = 33 333 Hz.
- **Begründung:** Ganzzahlige Teiler des D/A-Takts ergeben exakt das Verhältnis 2 : 1 für die Stromrichtung und
  eine saubere, periodische Tabelle. Alle Töne liegen mindestens 8 Hz neben einer 50-Hz-Oberwelle. Das Handgerät
  sucht die Frequenz zusätzlich selbst und führt sie nach.

### E-030 OLED über I²C
- **Datum:** 13.10.2026 · **Status:** beschlossen (ändert E-025)
- **Begründung:** 4 statt 7 Leitungen; mit 400 kHz Bustakt sind ca. 25–40 Bilder pro Sekunde möglich, das reicht
  für flüssige Balken und Verlaufsgraphen.

### E-031 Werkzeuge: Prüfung auf dem PC, KiCad und OpenSCAD in der Cloud-Umgebung
- **Datum:** 13.10.2026 · **Status:** beschlossen; KiCad entfällt mit E-042 (Lochraster-Aufbauplan als Zeichnung)
- **Inhalt:** Die Signalverarbeitung (`lockin.h`) wird auf dem PC mit simulierten Signalen getestet, beide Sketche
  werden gegen Ersatz-Header mit den Original-Signaturen auf Syntax geprüft (`firmware/machbarkeit/test_host/`).
  Druckteile entstehen in OpenSCAD, Schaltpläne der Testaufbauten mit schemdraw. KiCad 7 lässt sich in der
  Cloud-Umgebung über die Ubuntu-Paketquellen installieren; damit wird Phase 5 skriptgesteuert gemacht (statt
  KiCad-MCP).
- **Grenze:** Für den ESP32 kompilieren lässt sich hier nicht (die Arduino- und Espressif-Downloadquellen sind
  in der Umgebung gesperrt). Das erledigt Julius in der Arduino IDE.

### E-032 Messkopf und Spulenkabel aus Elektro-Material
- **Datum:** 13.10.2026 · **Status:** beschlossen; im Mini-Test nur mit der H-Spule (ein Paar des Kabels)
- **Inhalt:** Stiel aus M20-Isolierrohr, Spulenkabel aus J-Y(St)Y-Reststück (je Spule ein Paar, Schirm an GND).
- **Begründung:** Vorhandenes Material, abgeschirmt, definierte Geometrie für reproduzierbare Messungen.

## Neuausrichtung „Variante Einfach“ (Phase 3, freigegeben am 13.10.2026)

### E-033 Abtastfläche des Handgeräts oben am Kopf
- **Datum:** 13.10.2026 · **Status:** beschlossen (Vorgabe von Julius, ändert E-024)
- **Inhalt:** Das Handgerät ist ein Stab: oben der Sensorkopf mit der Abtastfläche auf der Stirnseite, darunter
  Display und Tasten, unten der Griff mit den Batterien. Die Spule liegt quer im Kopf, direkt unter der Fläche.
- **Begründung:** Man hält das Gerät wie eine Taschenlampe gegen Wand oder Decke und schaut dabei aufs Display.
  Eine Abtastfläche unten am Gerät passt nicht zu dieser Haltung.
- **Ansicht:** `gehaeuse/konzept/ansicht.scad` und die PNG-Bilder daneben.

### E-034 Variante „Einfach“ statt Variante C
- **Datum:** 13.10.2026 · **Status:** beschlossen (Freigabe durch Julius am 13.10.2026, ersetzt E-027)
- **Inhalt:** Hardware wie Variante B (eine Spule, Wandler im ESP32-S3), Software wie Variante C (Zweiton, Lock-in,
  Stromrichtung). Sender ohne Relais, Durchgangsprüfung und Lastmessung. Keine E-Feld-Spitze, nur der
  Standard-Frequenzsatz. Antworten auf die offenen Fragen aus `variante-einfach.md`:
  - V-1 Variante „Einfach“: **ja**
  - V-2 Display im Sender: **ja, OLED in beiden Geräten** („kosten ja nicht viel“) statt der vorgeschlagenen 3 LEDs
  - V-3 Platine oder Lochraster: zuerst „Platine“, kurz danach geändert auf **Lochraster** (E-042)
  - V-4 Platz für eine zweite Spule: **ja**, vorbereitet, aber nicht bestückt
- **Kosten:** ca. 31 € pro Satz mit Lochraster (davon ca. 3 € für das Sender-Display); Julius zahlt ca. 28 €,
  weil der ESP32-S2 mini vorhanden ist. Unter dem Ziel von 40 € (B-01).
- **Begründung:** Variante C ist Julius zu teuer und zu aufwendig. Die Reichweite hängt hauptsächlich an
  Einspeisung, Strom und Spule, und die bleiben gleich. Details: `docs/variante-einfach.md`.

### E-035 Handgerät: Wandler im ESP32-S3 mit zwei Verstärkerstufen statt PCM1808
- **Datum:** 13.10.2026 · **Status:** beschlossen (ersetzt E-019 für die Hardware; die Lock-in-Auswertung bleibt)
- **Inhalt:** Spule → Stufe 1 (MCP6002, ×5,7, Hochpass 340 Hz) → Stufe 2 (×40). Beide Ausgänge gehen an je einen
  Eingang des ESP32-S3 (ADC1, 12 bit). Der Wandler tastet reihum NIEDRIG, HOCH und die Batteriespannung ab,
  zusammen 81 000 Wandlungen pro Sekunde, also ca. 27 kHz je Kanal. Die Firmware misst die tatsächliche Rate beim
  Start, wertet beide Kanäle gleichzeitig per Lock-in aus und nimmt den Kanal, der nicht übersteuert
  (`lockin::DualRange`). Das genaue Verstärkungsverhältnis lernt sie im Betrieb nach.
- **Begründung:** In Phase 2 hatte ich den internen Wandler abgelehnt (E-019), weil Variante C drei Sensoren
  gleichzeitig mit großer Dynamik und genauer Phase brauchte (Links/Rechts, Tiefe, nT-Werte, E-Spitze). Das
  entfällt. Für eine Spule mit Pegel und Stromrichtung reicht er: Die Simulation mit Wandlerrauschen
  (3 LSB), Verstärkerrauschen, Anschlag und Zeitversatz zwischen den Kanälen ergibt eine Rauschgrenze von ca.
  0,001 nT. Die praktische Grenze von ca. 0,1 nT setzen Störfelder in Gebäuden, nicht die Elektronik. Die zwei
  Stufen geben zusammen gut 30 dB mehr Aussteuerbereich.
- **Risiko:** Der Wandler des S3 kann Störlinien oder ungleichmäßige Abtastung haben, die die Simulation nicht
  kennt. Das prüft der Mini-Test (M2, M3).
- **Verworfen:** PCM1808 (Kosten, Verdrahtung, Taktleitungen); eine Stufe mit fester Verstärkung (zu wenig
  Aussteuerbereich direkt an der Leitung).

### E-036 Eine Spule im Kopf, zweite Spule vorbereitet, keine E-Feld-Spitze
- **Datum:** 13.10.2026 · **Status:** beschlossen (V-4; ändert E-015 und E-024)
- **Inhalt:** Eine Ferritspule (1000 Wdg. auf 10 × 80 mm) liegt quer im Kopf, direkt unter der Abtastfläche. Der
  Kopf ist dafür ca. 92 mm breit. Auf der Lochrasterplatine sind Platz und zwei Wandlereingänge (GPIO5, GPIO6) für eine
  zweite Spule frei (später Links/Rechts möglich, oder eine kleine Tastspule für Bündel). Die E-Feld-Spitze
  entfällt.
- **Ersatz für die E-Spitze:** Die richtige Ader am abisolierten Ende findet man mit dem Multimeter (Ader am
  Anfang auf Erde legen, am Ende gegen Erde durchmessen).
- **Wichtig für die Stromrichtung:** Mit einer Spule hängt der Pfeil davon ab, wie herum man das Gerät hält.
  Regel: Display immer zu dir, beim Referenz-Setzen und beim Suchen gleich halten.

### E-037 Sender mit zwei Klemmen, ohne Umschaltung, ein Frequenzsatz
- **Datum:** 13.10.2026 · **Status:** beschlossen (ändert E-016 und E-017)
- **Inhalt:** Nur die Klemmen A (rot) und E (grün-gelb). Die Einspeisemodi entstehen durch das Anklemmen: Modus G
  = alle Adern + Schirm per WAGO an A, E an Erde; Modus A = eine Ader an A, Rest + Schirm per WAGO an E. Keine
  Relais. Modus P und die Frequenzsätze „Tief“ und „Hoch“ entfallen; Standard bleibt (8333,3 Hz + 4166,7 Hz). Ein
  Prüfton 1041,7 Hz (reiner Sinus) bleibt nur im Test, damit man mit dem Multimeter nachmessen kann.
- **Begründung:** Weniger Teile, keine Bedienfehler bei der Umschaltung. Andere Frequenzen lassen sich später per
  Software nachrüsten, die Hardware kann sie.

### E-038 Sender misst den Strom, Schutz ohne Fremdspannungssperre
- **Datum:** 13.10.2026 · **Status:** beschlossen (ersetzt E-022 und E-023; ändert E-011)
- **Inhalt:** Ein 10-Ω-Messwiderstand liegt im Rückweg der beiden PNP-Transistoren. Durch ihn fließt der Betrag
  des Ausgangsstroms; RC-Glied und ESP32-S2 messen den Mittelwert, die Firmware rechnet in mA (eff) um
  (ca. ±10 %). Das OLED zeigt den Strom, ab 60 mA nimmt die Firmware den Pegel zurück. Schutz: 10 µF bipolar
  (≥ 63 V) und 47 Ω 2 W und PTC 0,1 A je Klemme, Klemmdioden 1N4148 an beiden Brückenausgängen, TVS-Diode
  P6KE30A an der 24-V-Schiene.
- **Entfällt:** Durchgangs- und Schleifenmessung, Lastbild, Längenschätzung, Fremdspannungssperre.
- **Begründung:** Der Strom ist der wichtigste Wert („kommt überhaupt ein Stromkreis zustande?“) und kostet nur drei
  Bauteile. S-12 (60 V DC / 50 V AC ohne Schaden) bleibt Muss und wird durch die Koppelkondensatoren (kein
  Gleichstrom), Klemmdioden, TVS und PTC erfüllt.

### E-039 Anzeige: Pegel in dB statt nT
- **Datum:** 13.10.2026 · **Status:** beschlossen (ändert E-025)
- **Inhalt:** Das Handgerät zeigt den Pegel in dB, bezogen auf 1 µV an der Spule (ein echter Messwert, A-01), dazu
  Balken mit Spitzenwert, Stromrichtung, Empfindlichkeit, Ton und Batterie. nT gibt es nur in der seriellen
  Ausgabe zum Kalibrieren im Test. Der Sender zeigt Strom in mA, Pegel, Frequenz und Batterie.

### E-040 Versorgung und Bedienung
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Inhalt:** Beide Geräte: 4 × AA → Schiebeschalter → 1N5817 → 5-V-Pin des Moduls, Batteriespannung über Teiler
  100 kΩ / 47 kΩ gemessen. USB nur bei ausgeschaltetem Schalter anstecken (sonst speist die Batterie in den
  Laptop). Handgerät: Tasten REF (kurz Referenz, lang Frequenzsuche) und MODUS (kurz Empfindlichkeit AUTO/1–5,
  lang Ton laut/leise/aus), passiver Piezo. Sender: eine Taste PEGEL (kurz FEIN/NORMAL/MAX ≈ 5/9/17 V Spitze, lang
  Ausgang aus/ein).
- **Regel:** MAX nur in Modus G (gegen Erde). In Modus A mit angeschlossenem Gerät höchstens NORMAL
  (≤ 12 V Spitze, U-07).
- **Ton:** Die Tonhöhe ist auf Vielfache von 50 Hz gerundet. Ihre Oberwellen liegen dann wie die Netzoberwellen
  neben der Sendefrequenz und stören die Auswertung nicht.

### E-041 Phase 3 als Mini-Test mit den endgültigen Teilen
- **Datum:** 13.10.2026 · **Status:** beschlossen (ersetzt den Einkauf aus Machbarkeitstest v1.0)
- **Inhalt:** Statt eines eigenen Steckbrett-Einkaufs (ca. 57 €) werden die Teile für einen Satz einmal bestellt,
  auf dem Steckbrett getestet und danach auf die Lochrasterplatine übernommen. Geprüft werden nur die Punkte, die neu oder
  unsicher sind: Wandler des S3, Strommessung im Sender, Stromrichtung mit einer Spule, Reichweite im Vergleich.

### E-042 Aufbau auf Lochraster statt eigener Platine
- **Datum:** 13.10.2026 · **Status:** beschlossen (Julius, ändert E-034 V-3, ersetzt E-026 und E-014)
- **Inhalt:** Sender und Handgerät werden auf je einer Lochrasterplatine aufgebaut (einseitig, Raster 2,54 mm,
  Lötaugen). ESP32-Module und OLED sitzen auf Stift- bzw. Buchsenleisten, alle anderen Teile sind bedrahtet.
  Phase 5 liefert statt eines KiCad-Projekts einen **Lochraster-Aufbauplan**: Bauteilplatzierung, Verdrahtung
  (Ansicht von oben und gespiegelt von unten), Schnittstellen zum Gehäuse.
- **Kosten:** ca. 2 € statt ca. 9 €. Gesamt ca. 31 € pro Satz, Julius zahlt ca. 28 €.
- **Begründung:** Julius' Wunsch. Bei 8 kHz und den kleinen Strömen ist Lochraster technisch unkritisch. Eine
  Änderung nach dem Test ist mit dem Lötkolben gemacht, ohne neue Bestellung.
- **Worauf es ankommt:** Analogteil des Handgeräts kompakt und weg vom ESP32, eine durchgehende Masseleitung,
  kurze Wege um den MCP6002; im Sender die Leitungen der Endstufe und des Messwiderstands kräftig (0,5 mm²).
  Das steht im Aufbauplan.
- **Nachteil:** Mehr Lötarbeit (ca. 2–3 Abende für beide Geräte) und mehr Möglichkeiten für Verdrahtungsfehler.
  Der Aufbauplan zum Abhaken soll das abfangen.

### E-043 Signalausgang des Senders an GPIO18
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Inhalt:** Der D/A-Wandler gibt das Signal auf Kanal 2 (GPIO18) statt Kanal 1 (GPIO17) aus.
- **Begründung:** GPIO18 liegt beim LOLIN S2 mini auf der äußeren Stiftreihe. Dann reichen zwei Stiftleisten am
  Modul, auf dem Steckbrett, auf Lochraster und auf einer Platine. GPIO17 liegt auf der inneren Reihe.
- **Geändert:** `sender_test.ino` (DAC_CHANNEL_MASK_CH1), Steckbrett-Schaltplan, `machbarkeitstest.md`.

### E-044 KiCad-Entwurf als Entscheidungshilfe Platine oder Lochraster
- **Datum:** 13.10.2026 · **Status:** vorgeschlagen, Julius entscheidet (E-042 gilt bis dahin)
- **Inhalt:** Beide Geräte als KiCad-7-Projekt in `hardware/kicad/`: Schaltplan, Platine (Handgerät 34 × 70 mm,
  Sender 80 × 60 mm, 2 Lagen, nur bedrahtete Teile, Module auf Buchsenleisten), mit Freerouting vollständig
  geroutet, DRC ohne elektrische Fehler, Gerber-Dateien. Erzeugt per Skript aus `netze.py` und `platzierung.py`.
- **Empfehlung:** Platine, aber erst nach dem Mini-Test und nach Prüfung der Modulbelegung am gelieferten Teil
  bestellen. Mehrkosten ca. 7–10 € pro Satz (dann ca. 38–41 €). Begründung und Vergleich:
  [`platine-oder-lochraster.md`](platine-oder-lochraster.md).
- **Unsicher:** Pinbelegung und Reihenabstand des ESP32-S3 SuperMini sowie die inneren/äußeren Reihen des S2 mini
  sind im Netz nicht einheitlich beschrieben; vor der Bestellung am echten Modul nachmessen.

