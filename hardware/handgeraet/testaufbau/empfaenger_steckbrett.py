#!/usr/bin/env python3
"""Schaltplan Handgerät-Steckbrett (Phase 3, Variante "Einfach") mit schemdraw -> empfaenger_steckbrett.svg
Aufruf: python3 empfaenger_steckbrett.py   (pip install schemdraw)
Bauteilnummern wie in docs/machbarkeitstest.md, Abschnitt 5.2."""
import schemdraw
import schemdraw.elements as elm

schemdraw.use('svg')
OUT = __file__.replace('.py', '.svg')


def net(d, xy, text, direction='down', length=0.8, fontsize=10):
    """Kurze Leitung mit Netzname am Ende."""
    ln = getattr(elm.Line(), direction)(length).at(xy)
    d += ln
    loc = {'down': 'bottom', 'up': 'top', 'right': 'right', 'left': 'left'}[direction]
    d += elm.Label().at(ln.end).label(text, loc=loc, fontsize=fontsize)
    return ln.end


def gnd(d, xy, length=0.5):
    d += elm.Line().down(length).at(xy)
    d += elm.Ground()


def pin_no(d, xy, txt, dx=0.0, dy=0.25):
    d += elm.Label().at((xy[0] + dx, xy[1] + dy)).label(txt, fontsize=8)


def stufe(d, x_in, y_plus, name, pins, rf, cf, rg, cg):
    """Nicht invertierender Verstärker, + Eingang oben bei (x_in, y_plus), Gegenkopplung unten.
    Gibt (Opamp, Ausgangsknoten) zurück."""
    op = d.add(elm.Opamp(leads=True).right().flip().anchor('in2').at((x_in, y_plus))
               .label(name, loc='center', ofst=(-0.35, 0), fontsize=9))
    pin_no(d, op.in2, pins[0], dx=0.25)
    pin_no(d, op.in1, pins[1], dx=0.25, dy=-0.55)
    pin_no(d, op.out, pins[2], dx=-0.2)
    d += elm.Line().right(0.8).at(op.out)
    d += elm.Dot()
    o = d.here
    # Gegenkopplung Rf || Cf unterhalb
    n1 = (op.in1[0] - 0.6, op.in1[1])
    d += elm.Line().left(0.6).at(op.in1)
    d += elm.Dot()
    yf = op.in1[1] - 1.4
    d += elm.Line().down().at(n1).to((n1[0], yf))
    d += elm.Resistor().right().to((o[0], yf)).label(rf, loc='bottom', fontsize=10)
    d += elm.Line().up().to(o)
    d += elm.Line().down(1.6).at((n1[0], yf))
    d += elm.Capacitor().right().to((o[0], yf - 1.6)).label(cf, loc='bottom', fontsize=10)
    d += elm.Line().up(1.6)
    # Rg + Cg vom Minus-Eingang nach VREF (senkrecht nach unten)
    d += elm.Line().left(1.0).at(n1)
    d += elm.Resistor().down().label(rg, loc='top', fontsize=10)
    d += elm.Capacitor().down().label(cg, loc='top', fontsize=10)
    net(d, d.here, 'VREF', length=0.4)
    return op, o


with schemdraw.Drawing(file=OUT, show=False, transparent=True) as d:
    d.config(unit=2.5, fontsize=11, color='currentColor', font='sans-serif', lw=1.3)

    # ================= Spule und Eingangsfilter
    y = 10.0
    k = (0.0, y)
    d += (lc := elm.Inductor2(loops=4).down().at(k))
    d += elm.Label().at((1.0, y - 1.25)).label('L1 Spule\n1000 Wdg.', loc='right', fontsize=10)
    d += elm.Label().at((0.25, y - 0.3)).label('Ende', loc='right', fontsize=9)
    d += elm.Label().at((0.25, lc.end[1] + 0.3)).label('Anfang', loc='right', fontsize=9)
    d += elm.Dot().at(lc.end)
    net(d, lc.end, 'VREF', length=0.6)
    d += elm.Dot().at(k)
    d += elm.Line().left(2.0).at(k)
    d += elm.Resistor().down().to((-2.0, lc.end[1])).label('R6\n22 kΩ', loc='top')
    d += elm.Line().right().to(lc.end)
    d += elm.Label().at((-2.6, y - 4.0)).label('Schirm von Spule und Kabel → GND', loc='right', fontsize=9)
    d += elm.Line().up(1.2).at(k)
    d += elm.Resistor().right().label('R7 1 kΩ', loc='top')
    d += elm.Line().right(1.6)
    d += elm.Line().down(1.2)
    d += elm.Dot()
    p = d.here
    d += (c6 := elm.Capacitor().down().at(p).label('C6\n100 pF', loc='bottom'))
    net(d, c6.end, 'VREF', length=0.5)

    # ================= Stufe 1 (U1A) und Stufe 2 (U1B), MCP6002
    d += elm.Line().right(4.0).at(p)
    opa, o1 = stufe(d, d.here[0], y, 'U1A', ('3', '2', '1'), 'R8 47 kΩ', 'C7 100 pF', 'R9 10 kΩ', 'C8 47 nF')
    xb = o1[0] + 6.5
    d += elm.Line().right().at(o1).to((xb, o1[1]))
    opb, o2 = stufe(d, xb, o1[1], 'U1B', ('5', '6', '7'), 'R11 390 kΩ', 'C10 22 pF', 'R12 10 kΩ', 'C11 47 nF')
    d += elm.Line().right(1.4).at(o2)

    # Ausgänge zu den Wandlereingängen, je mit RC gegen GND (Ladungsreserve für den Wandler)
    for (o, r, c, txt) in ((o1, 'R10\n1 kΩ', 'C9\n1 nF', 'NIEDRIG (GPIO1)'), (o2, 'R13\n1 kΩ', 'C12\n1 nF', 'HOCH (GPIO2)')):
        tp = (o[0] + 1.4, o[1])
        d += elm.Dot().at(tp)
        d += elm.Line().up(3.0).at(tp)
        d += elm.Resistor().right().label(r.replace('\n', ' '), loc='top', fontsize=10)
        d += elm.Dot()
        a = d.here
        d += elm.Capacitor().down(1.6).at(a).label(c, loc='right', fontsize=10)
        d += elm.Ground()
        d += elm.Line().right(0.6).at(a)
        d += elm.Label().at(d.here).label(txt, loc='right', fontsize=10)
    d += elm.Label().at((opa.out[0] - 1.2, y + 1.3)).label('Stufe 1: ×5,7', fontsize=10)
    d += elm.Label().at((opb.out[0] - 1.2, o1[1] + 1.3)).label('Stufe 2: ×40', fontsize=10)

    # ================= ESP32-S3 SuperMini (unten links)
    ys = -9.0
    s3 = d.add(elm.Ic(pins=[
        elm.IcPin(name='5V', side='left', pos=0.86, anchorname='v5'),
        elm.IcPin(name='3V3', side='left', pos=0.62, anchorname='v33'),
        elm.IcPin(name='GND', side='left', pos=0.38, anchorname='gnd'),
        elm.IcPin(name='USB-C', side='left', pos=0.12, anchorname='usb'),
        elm.IcPin(name='GPIO1', side='right', pos=0.93, anchorname='g1'),
        elm.IcPin(name='GPIO2', side='right', pos=0.81, anchorname='g2'),
        elm.IcPin(name='GPIO4', side='right', pos=0.69, anchorname='g4'),
        elm.IcPin(name='GPIO8', side='right', pos=0.55, anchorname='g8'),
        elm.IcPin(name='GPIO9', side='right', pos=0.43, anchorname='g9'),
        elm.IcPin(name='GPIO10', side='right', pos=0.29, anchorname='g10'),
        elm.IcPin(name='GPIO11', side='right', pos=0.17, anchorname='g11'),
        elm.IcPin(name='GPIO12', side='right', pos=0.05, anchorname='g12')],
        size=(4.6, 10.0), label='ESP32-S3\nSuperMini').right().at((0.5, ys)))
    net(d, s3.v5, '5V', direction='left', length=0.8)
    net(d, s3.v33, '3V3', direction='left', length=0.8)
    d += elm.Line().left(0.8).at(s3.gnd)
    d += elm.Ground()
    d += elm.Line().left(0.8).at(s3.usb)
    d += elm.Label().at(d.here).label('Laptop', loc='left', fontsize=9)
    net(d, s3.g1, 'NIEDRIG', direction='right', length=1.0)
    net(d, s3.g2, 'HOCH', direction='right', length=1.0)
    net(d, s3.g4, 'BATT', direction='right', length=1.0)
    d += elm.Label().at((s3.g4[0] + 0.3, s3.g4[1] - 0.55)).label('GPIO5/6: Reserve 2. Spule', loc='right', fontsize=8)

    # OLED
    xo = s3.g8[0] + 4.2
    ol = d.add(elm.Ic(pins=[
        elm.IcPin(name='SDA', side='left', pos=0.72, anchorname='sda'),
        elm.IcPin(name='SCL', side='left', pos=0.38, anchorname='scl'),
        elm.IcPin(name='VCC', side='right', pos=0.72, anchorname='vcc'),
        elm.IcPin(name='GND', side='right', pos=0.38, anchorname='gnd')],
        size=(5.2, 2.4), label='OLED 1,3″\nI²C').right().anchor('sda').at((xo, s3.g8[1])))
    d += elm.Line().at(s3.g8).to(ol.sda)
    d += elm.Line().right(1.2).at(s3.g9)
    d += elm.Line().to((s3.g9[0] + 1.2, ol.scl[1]))
    d += elm.Line().to(ol.scl)
    net(d, ol.vcc, '3V3', direction='right', length=0.6)
    d += elm.Line().right(0.6).at(ol.gnd)
    d += elm.Ground()

    # Tasten und Piezo an einer gemeinsamen GND-Schiene rechts
    xr = s3.g10[0] + 4.6
    for (pin, name) in ((s3.g10, 'T1 REF'), (s3.g11, 'T2 MODUS')):
        d += elm.Line().right(1.0).at(pin)
        d += elm.Button().right().label(name, loc='top', fontsize=9)
        d += elm.Line().right().to((xr, pin[1]))
        d += elm.Dot()
    d += elm.Resistor().right().at(s3.g12).label('R14 100 Ω', loc='top', fontsize=9)
    d += elm.Crystal().right().to((xr, s3.g12[1])).label('BZ1 Piezo', loc='bottom', fontsize=9)
    d += elm.Line().up().at((xr, s3.g12[1])).to((xr, s3.g10[1]))
    gnd(d, (xr, s3.g12[1]), length=0.5)

    # ================= Versorgung (unten rechts)
    xv = xb - 1.0
    yv = ys + 0.5
    d += elm.Battery().up().at((xv, yv - 2.5)).label('4 × AA\n4,8–6,4 V', loc='bottom')
    d += elm.Line().down(0.4).at((xv, yv - 2.5))
    d += elm.Ground()
    d += elm.Switch().right().at((xv, yv)).label('S1 EIN', loc='top', fontsize=9)
    d += elm.Diode().right().label('D1 1N5817', loc='top', fontsize=9)
    d += elm.Dot()
    vb = d.here
    net(d, vb, '5V', direction='up', length=0.8)
    # Batteriemessung: Teiler 100k / 47k
    d += elm.Line().right(1.4).at(vb)
    d += elm.Resistor().down().label('R1\n100 kΩ', loc='bottom')
    d += elm.Dot()
    bt = d.here
    d += elm.Resistor().down().label('R2\n47 kΩ', loc='bottom')
    d += elm.Ground()
    d += elm.Line().right(1.4).at(bt)
    d += elm.Capacitor().down().label('C1\n100 nF', loc='bottom')
    d += elm.Ground()
    net(d, (bt[0] + 1.4, bt[1]), 'BATT', direction='right', length=0.6)

    # Analog-3,3 V und VREF
    xa3 = xv
    ya3 = yv - 6.6
    net(d, (xa3, ya3), '3V3', direction='left', length=0.6)
    d += elm.Resistor().right().at((xa3, ya3)).label('R3 10 Ω', loc='top')
    d += elm.Dot()
    a3 = d.here
    d += elm.Label().at((a3[0], a3[1] + 0.35)).label('A3V3', loc='top', fontsize=10)
    d += elm.Capacitor(polar=True).down().at(a3).label('C2\n100 µF', loc='bottom')
    d += elm.Ground()
    d += elm.Line().right(1.6).at(a3)
    d += elm.Dot()
    a3b = d.here
    d += elm.Capacitor().down().at(a3b).label('C3\n100 nF', loc='bottom')
    d += elm.Ground()
    d += elm.Line().right(1.6).at(a3b)
    d += elm.Resistor().down().label('R4\n10 kΩ', loc='bottom')
    d += elm.Dot()
    vr = d.here
    d += elm.Resistor().down().label('R5\n10 kΩ', loc='bottom')
    d += elm.Ground()
    d += elm.Line().right(1.6).at(vr)
    d += elm.Capacitor(polar=True).down().label('C4\n10 µF', loc='bottom')
    d += elm.Ground()
    net(d, (vr[0] + 1.6, vr[1]), 'VREF 1,65 V', direction='right', length=0.6)
    d += elm.Line().right(1.6).at(a3b)
    d += elm.Line().right(1.8)
    net(d, d.here, 'U1 Pin 8', direction='right', length=0.3, fontsize=9)
    d += elm.Label().at((a3[0] + 5.6, a3[1] - 0.6)).label('U1 Pin 4 → GND,\nC5 100 nF an Pin 8', loc='right', fontsize=9)

    d += elm.Label().at((-2.0, ys - 7.0)).label(
        'U1 = MCP6002 (DIP-8). Spule über geschirmtes Kabel (Schirm nur hier an GND). Ruhepegel an Pin 1 und 7: 1,65 V.',
        loc='right', fontsize=10)
    d += elm.Label().at((-2.0, ys - 7.8)).label(
        'Messpunkte: TPE1 = VREF · TPE2 = U1 Pin 1 (NIEDRIG) · TPE3 = U1 Pin 7 (HOCH) · TPE4 = BATT.  '
        'USB nur bei S1 = AUS anstecken.',
        loc='right', fontsize=10)
print('geschrieben:', OUT)
