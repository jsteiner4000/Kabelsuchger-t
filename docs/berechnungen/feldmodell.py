#!/usr/bin/env python3
"""
Feldmodell für das Kabelsuchgerät (Phase 2, Konzept).

Rechnet nach, was im Konzept (docs/konzept.md) behauptet wird:
  1. Magnetfeld einer Leitung mit Rückweg über Erde (Einzelleiter)
     im Vergleich zu Hin- und Rückstrom in einem Aderpaar (unverdrillt und verdrillt).
  2. Strom auf der Leitung je Einsatzfall (Modus G = Bündel gegen Erde).
  3. Daraus der Abstand, bis zu dem das Handgerät die Leitung sicher findet.
  4. Empfindlichkeit und Rauschgrenze der Ferritspule.

Nur Python-Standardbibliothek. Aufruf:  python3 feldmodell.py
Alle Werte sind Abschätzungen; gemessen wird in Phase 3.
"""
import json
import math

MU0 = 4e-7 * math.pi


# ---------------------------------------------------------------- Bessel-Funktionen
def _k(nu, x, n=4000, tmax=12.0):
    """Modifizierte Besselfunktion K_nu(x) per numerischer Integration (nu = 0 oder 1)."""
    h = tmax / n
    s = 0.0
    for i in range(n + 1):
        t = i * h
        w = 0.5 if i in (0, n) else 1.0
        s += w * math.exp(-x * math.cosh(t)) * math.cosh(nu * t)
    return s * h


def k0(x):
    return _k(0, x)


def k1(x):
    return _k(1, x)


# ---------------------------------------------------------------- Felder (Betrag in Tesla)
def b_einzelleiter(i, r):
    """Strom i fließt auf der Leitung, der Rückweg ist weit weg (Erde)."""
    return MU0 * i / (2 * math.pi * r)


def b_paar(i, r, d):
    """Hin- und Rückstrom in zwei Adern im Abstand d, unverdrillt (Linien-Dipol)."""
    return MU0 * i * d / (2 * math.pi * r * r)


def b_paar_verdrillt(i, r, d, schlag):
    """Verdrilltes Paar mit Schlaglänge 'schlag' (rotierender Linien-Dipol).
    Für r << schlag geht das in b_paar über, für r >> schlag fällt es exponentiell."""
    k = 2 * math.pi / schlag
    x = k * r
    K1 = k1(x)
    K1s = -k0(x) - K1 / x  # Ableitung K1'(x)
    amp = math.sqrt(K1s ** 2 + (K1 / x) ** 2 + K1 ** 2) / math.sqrt(2)
    return MU0 * i * d * k * k / (2 * math.pi) * amp


def abstand_fuer(i, b_min):
    """Abstand, bei dem ein Einzelleiter mit Strom i noch b_min erzeugt."""
    return MU0 * i / (2 * math.pi * b_min)


# ---------------------------------------------------------------- Spule
def spule(f, windungen=1000, durchm=0.010, mu_eff=35.0, r_draht=34.0,
          en_opv=8e-9, bandbreite=4.0):
    """Empfindlichkeit (V/T) und Rauschgrenze (T) einer Ferritstab-Spule.
    mu_eff ~ 35 gilt für einen Stab 10 x 80 mm (Entmagnetisierung begrenzt)."""
    a = math.pi * (durchm / 2) ** 2
    s = 2 * math.pi * f * windungen * a * mu_eff
    en_spule = math.sqrt(4 * 1.38e-23 * 300 * r_draht)
    en = math.sqrt(en_opv ** 2 + en_spule ** 2) * math.sqrt(bandbreite)
    return s, en / s


# ---------------------------------------------------------------- Einsatzfälle
U_EFF = 15.0          # V, Ausgangsspannung Modus G (Obergrenze Schutzkleinspannung 25 V)
I_GRENZ = 0.030       # A, Strombegrenzung Stufe "Normal"
B_MIN = 1e-10         # T, praktische Schwelle "sicher ortbar" (0,1 nT, ca. 1000x über Rauschen)

FAELLE = [
    # name, Länge m, C' pF/m gegen Umgebung, Ende geerdet?, Bemerkung
    ("Unterputz, 30 m, Ende offen", 30, 300, False, "Putz koppelt stark kapazitiv"),
    ("Unterputz, 30 m, Ende geerdet", 30, 300, True, "z. B. Schirm am Ende auf PE"),
    ("Trasse Kunststoff, 100 m, Ende offen", 100, 100, False, "Nachbarkabel koppeln mit"),
    ("Trasse, 100 m, Ende geerdet", 100, 100, True, "Rückweg über Gebäudeerde"),
    ("Lange Leitung, 300 m, Ende offen", 300, 50, False, "gemischte Verlegung"),
    ("Lange Leitung, 500 m, Ende offen", 500, 50, False, "gemischte Verlegung"),
    ("Freie Leitung am Boden, 50 m, Ende offen", 50, 40, False, "Testaufbau T-01"),
]


def strom_am_anfang(laenge, c_pf_m, geerdet, f):
    """Strom, den der Sender in Modus G treibt (Spannung bis U_EFF, Strom bis I_GRENZ)."""
    if geerdet:
        return I_GRENZ  # niederohmiger Kreis über Erde: Sender begrenzt auf den Sollstrom
    c = laenge * c_pf_m * 1e-12
    i = U_EFF * 2 * math.pi * f * c
    return min(i, I_GRENZ)


def main():
    f = 8200.0
    out = {"f_hz": f, "b_min_T": B_MIN}

    s, b_rausch = spule(f)
    print(f"Spule 1000 Wdg. auf Ferritstab 10x80 mm bei {f/1000:.1f} kHz:")
    print(f"  Empfindlichkeit {s/1e6*1e-9*1e9:.3f} µV/pT  -> 1 nT ergibt {s*1e-9*1e6:.0f} µV")
    print(f"  Rauschgrenze bei 4 Hz Bandbreite: {b_rausch*1e12:.2f} pT")
    print(f"  praktische Schwelle {B_MIN*1e9:.1f} nT = {B_MIN/b_rausch:.0f}-fache Reserve\n")
    out["spule"] = {"V_pro_T": s, "rauschgrenze_T": b_rausch}

    # 1) Feld über Abstand, gleicher Strom 1 mA
    d = 1.5e-3
    rs = [0.01 * (10 ** (k / 20)) for k in range(0, 47)]  # 1 cm ... 2 m, logarithmisch
    kurve = []
    for r in rs:
        kurve.append({
            "r_m": round(r, 4),
            "einzel_nT": b_einzelleiter(1e-3, r) * 1e9,
            "paar_nT": b_paar(1e-3, r, d) * 1e9,
            "verdrillt10_nT": b_paar_verdrillt(1e-3, r, d, 0.10) * 1e9,
            "verdrillt5_nT": b_paar_verdrillt(1e-3, r, d, 0.05) * 1e9,
        })
    out["kurve_1mA"] = kurve
    print("Feld bei 1 mA (nT):   r      Einzelleiter   Paar unverdr.   verdrillt 10 cm   verdrillt 5 cm")
    for r in (0.02, 0.05, 0.10, 0.30, 1.0):
        print(f"                   {r*100:5.0f} cm   {b_einzelleiter(1e-3, r)*1e9:9.3f}   "
              f"{b_paar(1e-3, r, d)*1e9:12.5f}   {b_paar_verdrillt(1e-3, r, d, 0.10)*1e9:14.2e}   "
              f"{b_paar_verdrillt(1e-3, r, d, 0.05)*1e9:14.2e}")

    # Ader/Ader mit Brücke am Ende: 30 mA im Paar. Wie weit reicht das?
    print("\nAder/Ader, Ende gebrückt, 30 mA Schleifenstrom (verdrillt, Schlag 10 cm):")
    for r in (0.01, 0.02, 0.05, 0.10):
        b = b_paar_verdrillt(0.030, r, d, 0.10)
        print(f"  {r*100:4.0f} cm: {b*1e9:9.4f} nT  ({'ortbar' if b >= B_MIN else 'zu schwach'})")

    # 2) Einsatzfälle in Modus G
    print(f"\nModus G (Bündel gegen Erde), {U_EFF:.0f} V eff, Strombegrenzung {I_GRENZ*1000:.0f} mA, {f/1000:.1f} kHz")
    print("  Fall                                       I Anfang   I bei 50 %   I bei 90 %   Reichweite 50 % / 90 %")
    tab = []
    for name, laenge, cpf, geerdet, bem in FAELLE:
        i0 = strom_am_anfang(laenge, cpf, geerdet, f)
        i50 = i0 if geerdet else i0 * 0.5
        i90 = i0 if geerdet else i0 * 0.1
        r50 = abstand_fuer(i50, B_MIN)
        r90 = abstand_fuer(i90, B_MIN)
        tab.append({"fall": name, "laenge_m": laenge, "c_pf_m": cpf, "geerdet": geerdet,
                    "i0_mA": i0 * 1e3, "i50_mA": i50 * 1e3, "i90_mA": i90 * 1e3,
                    "r50_m": r50, "r90_m": r90, "bemerkung": bem})
        print(f"  {name:42s} {i0*1e3:7.1f} mA  {i50*1e3:8.2f} mA  {i90*1e3:9.2f} mA   "
              f"{r50:6.1f} m / {r90:5.1f} m")
    out["faelle"] = tab

    # 3) Frequenzvergleich für offenes Ende (Strom ~ f, Spulenspannung ~ f)
    print("\nOffenes Ende: Signal an der Spule relativ zu 8,2 kHz (Strom und Induktion steigen beide mit f)")
    for fx in (1024, 4100, 8200, 32800):
        rel = (fx / f) ** 2
        print(f"  {fx/1000:5.2f} kHz: x{rel:6.2f}  ({20*math.log10(rel):+5.1f} dB)")

    with open(__file__.replace(".py", ".json"), "w", encoding="utf-8") as fh:
        json.dump(out, fh, indent=1, ensure_ascii=False)


if __name__ == "__main__":
    main()
