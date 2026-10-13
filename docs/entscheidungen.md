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
- **Datum:** 13.10.2026 · **Status:** beschlossen (F-08)
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
- **Datum:** 13.10.2026 · **Status:** beschlossen (F-02), Werkzeug offen
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
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Inhalt:** Ortung über das Magnetfeld (Ferritspulen). E-Feld-Spitze nur für Bündel und abisolierte Adern.
- **Begründung:** Bei kHz schirmen feuchter Putz, Beton, Bewehrung und geerdete Schirme das E-Feld ab,
  das Magnetfeld nicht (Kap. 1.1, 1.5). Am offenen Ende fließt kein Strom, dort braucht man das E-Feld.
- **Verworfen:** nur E-Feld (Variante A). Erfüllt U-09 und U-13 nicht.

### E-016 Hauptmodus „Bündel gegen Erde“ (G), dazu Modi A und P
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Inhalt:** G: alle Adern + Schirm gemeinsam (WAGO) gegen Erde, Schirm am Anfang gelöst. A: eine Ader gegen
  den Rest (Identifikation am Ende). P: Paar (Durchgang, Länge, Paar finden). Umschaltung mit 2 Relais.
- **Begründung:** Hin- und Rückstrom im selben Kabel heben sich auf (Ader/Ader: bei 10 cm ca. 1/70 des Feldes,
  verdrillt nahezu null; Kap. 1.2). Mit Rückweg über Erde bleibt das volle Feld. In G sieht ein Gerät am Ende
  keine Spannung (U-07), und die sechs Fälle des Lastenhefts reduzieren sich auf „Erde am Ende ja/nein“.
- **Verworfen:** Ader/Ader und Ader/Schirm als Ortungsmodus (Feld bleibt im Kabel).

### E-017 Zweiton-Signal und drei Frequenzsätze
- **Datum:** 13.10.2026 · **Status:** beschlossen, Feinabstimmung nach Phase 3
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
- **Datum:** 13.10.2026 · **Status:** beschlossen
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
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Begründung:** Zuverlässig, solange sie nur warnt und nie bestätigt (Kap. 6.5).

### E-023 Durchgang (S-14): ja, plus Lastbild und Längenschätzung
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Inhalt:** DC-Messung ≤ 1 mA / ≤ 3,3 V in beiden Polaritäten; AC-Lastbild aus Spannung, Strom und Phase;
  Länge offener Paare aus der Kapazität (≈ 100 nF/km), gekennzeichnet als Schätzung.

### E-024 Bauform Handgerät: Paddel-Stab ca. 22 cm mit Hals ≥ 6 cm
- **Datum:** 13.10.2026 · **Status:** beschlossen, Halslänge nach Messung in Phase 3
- **Begründung:** Spulen weit weg vom Digitalteil (Störungen); Kopf 90 mm breit für 80-mm-Ferritstab; passt auf
  25,6 cm Bauraum. Handkontakt am Griff verbessert die E-Spitze.

### E-025 Anzeige: OLED 1,3″ in beiden Geräten, Feld in nT
- **Datum:** 13.10.2026 · **Status:** beschlossen, Schnittstelle geändert durch E-030
- **Inhalt:** Handgerät über SPI für flüssige Grafik. Feld nach Kalibrierung mit dem Sender in nT (A-01).

### E-026 Aufbau: eigene Platinen (KiCad, JLCPCB), nur THT und Module
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Begründung:** E-01/E-006; Analogteil und Spulenumfeld brauchen ein sauberes Layout. Spulen werden selbst
  gewickelt (gedruckter Spulenkörper, Akkuschrauber).

### E-027 Variante C, Kosten ca. 50 € pro Satz
- **Datum:** 13.10.2026 · **Status:** beschlossen (Julius hat mit der Freigabe zugestimmt, B-02)
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
- **Datum:** 13.10.2026 · **Status:** beschlossen
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
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Inhalt:** Die Signalverarbeitung (`lockin.h`) wird auf dem PC mit simulierten Signalen getestet, beide Sketche
  werden gegen Ersatz-Header mit den Original-Signaturen auf Syntax geprüft (`firmware/machbarkeit/test_host/`).
  Druckteile entstehen in OpenSCAD, Schaltpläne der Testaufbauten mit schemdraw. KiCad 7 lässt sich in der
  Cloud-Umgebung über die Ubuntu-Paketquellen installieren; damit wird Phase 5 skriptgesteuert gemacht (statt
  KiCad-MCP).
- **Grenze:** Für den ESP32 kompilieren lässt sich hier nicht (die Arduino- und Espressif-Downloadquellen sind
  in der Umgebung gesperrt). Das erledigt Julius in der Arduino IDE.

### E-032 Messkopf und Spulenkabel aus Elektro-Material
- **Datum:** 13.10.2026 · **Status:** beschlossen
- **Inhalt:** Stiel aus M20-Isolierrohr, Spulenkabel aus J-Y(St)Y-Reststück (je Spule ein Paar, Schirm an GND).
- **Begründung:** Vorhandenes Material, abgeschirmt, definierte Geometrie für reproduzierbare Messungen.
