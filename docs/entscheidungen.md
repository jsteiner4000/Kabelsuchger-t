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

## Offen (wird in Phase 2 entschieden)

| Thema | Lastenheft |
|---|---|
| Ortungsprinzip: elektrisches Feld, Magnetfeld oder beides | H-03 |
| Einspeisemodi und wie sie umgeschaltet werden | S-03 |
| Frequenz(en), Modulation/Kennung, Ausgangsleistung | S-01, S-04, S-11, S-17 |
| Auswertung im Handgerät (Filter, Synchrondemodulation, Goertzel …) | H-05 |
| Wie die gesuchte Leitung in der Trasse von übersprechenden Nachbarn unterschieden wird | U-13 |
| Mikrocontroller beider Geräte | E-02 |
| Aufbauart (Module, Lochraster, eigene Platine, Herstellerbestückung) | E-01, E-07 |
| Displaygröße und -typ | A-02 |
| Bauform des Handgeräts | H-11 |
| Fremdspannungswarnung ja/nein | S-13 |
| Durchgangs-/Schleifentest: Messverfahren und Grenzen | S-14 |
| Richtungsanzeige und Tiefenschätzung | H-14, H-15 |
| Zielwerte der Abnahmetests | Kap. 9 |
