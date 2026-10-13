# KiCad-Projekte (Entwurf 1)

Sender und Handgerät der Variante „Einfach“ als KiCad-7-Projekte: Schaltplan, Platine (2 Lagen, nur bedrahtete
Teile), Gerber-Dateien. **Entwurf vor dem Mini-Test – noch nicht bestellen.** Was vorher zu prüfen ist, steht in
[`docs/platine-oder-lochraster.md`](../../docs/platine-oder-lochraster.md).

| Ordner | Inhalt |
|---|---|
| `handgeraet/` | `handgeraet.kicad_pro` (öffnen), Schaltplan, Platine 34 × 70 mm, `handgeraet-gerber.zip`, `handgeraet-schaltplan.pdf`, `handgeraet-drc.txt`, Bilder |
| `sender/` | dasselbe für den Sender, Platine 80 × 60 mm |
| `lib/` | eigene Symbole und Footprints für ESP32-S3 SuperMini und LOLIN S2 mini |
| `netze.py` | Netzliste beider Geräte (gleiche Bauteilnummern wie die Steckbrett-Schaltpläne) |
| `platzierung.py` | Lage der Bauteile |
| `erzeuge.py` | erzeugt alles neu: Schaltplan, Platine, Routing (Freerouting), DRC, Gerber, Bilder |

## Öffnen

KiCad 7 oder neuer installieren und die `.kicad_pro`-Datei öffnen. Die eigene Bibliothek wird über die
Projekt-Bibliothekstabellen automatisch gefunden. KiCad 8/9 fragen beim Öffnen, ob das Format umgestellt werden soll;
das ist in Ordnung.

Der Schaltplan verbindet über Netznamen (keine Linien). Die gut lesbaren Zeichnungen sind die
Steckbrett-Schaltpläne in `hardware/sender/testaufbau/` und `hardware/handgeraet/testaufbau/`.

## Prüfung (DRC)

Beide Platinen: **0 offene Verbindungen, keine Abstands- oder Kurzschlussfehler.** Die übrigen Meldungen in
`*-drc.txt` sind:

- `silk_overlap`, `silk_over_copper`: Beschriftungen überlappen sich oder liegen auf Lötaugen (nur Optik)
- `lib_footprint_issues`: In der Erzeugungsumgebung war keine globale Bibliothekstabelle eingerichtet; auf einem
  normalen KiCad-Rechner entfällt das
- `starved_thermal`: An 1–2 Massepads hat die Wärmefalle nur einen statt zwei Stege; die Verbindung besteht trotzdem

## Bestellen (erst nach dem Mini-Test)

Bei JLCPCB oder einem ähnlichen Anbieter die beiden `*-gerber.zip` hochladen: 2 Lagen, 1,6 mm, HASL (bleifrei
optional), Farbe beliebig, 5 Stück. Ca. 9–12 € für beide inkl. Versand, 1–3 Wochen Lieferzeit.

## Neu erzeugen

Voraussetzungen: KiCad 7 (Python-Modul `pcbnew`, `kicad-cli`), Java 25 und Freerouting 2.4 mit Abhängigkeiten
(Maven: `app.freerouting:freerouting:2.4.1`).

```
export FREEROUTING_CP="/pfad/zu/freerouting/lib/*"
export FREEROUTING_JAVA=/usr/lib/jvm/java-25-openjdk-amd64/bin/java
python3 erzeuge.py                 # beide Geräte
python3 erzeuge.py sender          # nur der Sender
python3 erzeuge.py --kein-routing  # nur Platzierung prüfen
```

Freerouting arbeitet nicht deterministisch; das Skript wiederholt bis zu fünfmal, bis alles verbunden ist. Die
Leiterbahnen sehen deshalb nach jedem Lauf etwas anders aus.
