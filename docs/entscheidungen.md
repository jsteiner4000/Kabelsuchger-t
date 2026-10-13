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

---

## Vorgeschlagen (wartet auf Freigabe)

### E-005 Keine passive 50-Hz-Ortung von Netzleitungen
- **Datum:** 13.10.2026 · **Status:** vorgeschlagen
- **Inhalt:** Das Handgerät bekommt keinen Modus, der Netzleitungen über ihr 50-Hz-Feld findet
  („Bohrschutz“).
- **Begründung:** Eine abgeschaltete, geschirmte oder tief liegende Netzleitung erzeugt kein messbares
  Feld. „Kein Ausschlag“ würde dann falsche Sicherheit vor dem Bohren vermitteln. Das widerspricht dem
  Grundsatz aus dem Auftrag, lieber wegzulassen als falsche Sicherheit zu geben.
- **Alternative:** Modus mit deutlichem Warnhinweis. Verworfen, weil sich ein Warnhinweis im Alltag
  abnutzt.

---

## Offen (wird in Phase 2 entschieden)

| Thema | Lastenheft |
|---|---|
| Ortungsprinzip: elektrisches Feld, Magnetfeld oder beides | H-03 |
| Einspeisemodi und wie sie umgeschaltet werden | S-03 |
| Frequenz(en), Modulation/Kennung, Ausgangsleistung | S-01, S-04, S-11, S-17 |
| Auswertung im Handgerät (Filter, Synchrondemodulation, Goertzel …) | H-05 |
| Mikrocontroller beider Geräte | E-02 |
| Aufbauart (Module, Lochraster, eigene Platine, Herstellerbestückung) | E-01, E-07 |
| Fremdspannungswarnung ja/nein | S-13 |
| Durchgangs-/Schleifentest ja/nein | S-14 |
| Batterietyp und Anzahl | S-07, H-08 |
| Zielwerte der Abnahmetests | Kap. 9 |
