#!/usr/bin/env python3
"""Schaltplan Sender-Steckbrett (Phase 3, Variante "Einfach") mit schemdraw -> sender_steckbrett.svg
Aufruf: python3 sender_steckbrett.py   (pip install schemdraw)
Bauteilnummern wie in docs/machbarkeitstest.md, Abschnitt 5.1."""
import schemdraw
import schemdraw.elements as elm

schemdraw.use('svg')
OUT = __file__.replace('.py', '.svg')


def net(d, xy, text, direction='right', length=0.8, fontsize=10):
    """Kurze Leitung mit Netzname am Ende."""
    ln = getattr(elm.Line(), direction)(length).at(xy)
    d += ln
    loc = {'down': 'bottom', 'up': 'top', 'right': 'right', 'left': 'left'}[direction]
    d += elm.Label().at(ln.end).label(text, loc=loc, fontsize=fontsize)
    return ln.end


def pin_no(d, xy, txt, dx=0.0, dy=0.25):
    d += elm.Label().at((xy[0] + dx, xy[1] + dy)).label(txt, fontsize=8)


with schemdraw.Drawing(file=OUT, show=False, transparent=True) as d:
    d.config(unit=2.5, fontsize=11, color='currentColor', font='sans-serif', lw=1.3)

    # ================= Batterie, ESP32-S2 mini, OLED, Taste (oben links)
    yb0 = 15.0
    d += elm.Battery().up().at((-8.0, yb0 - 2.5)).label('4 × AA\n4,8–6,4 V', loc='bottom')
    d += elm.Line().down(0.4).at((-8.0, yb0 - 2.5))
    d += elm.Ground()
    d += elm.Switch().right().at((-8.0, yb0)).label('S1 EIN', loc='top', fontsize=9)
    d += elm.Diode().right().label('D1 1N5817', loc='top', fontsize=9)
    d += elm.Dot()
    vbat = d.here
    d += elm.Label().at((vbat[0], vbat[1] + 0.4)).label('VBAT', loc='top', fontsize=10)
    # Batteriemessung: Teiler 100k / 47k
    d += elm.Resistor().down().at(vbat).label('R16\n100 kΩ', loc='bottom')
    d += elm.Dot()
    bt = d.here
    d += elm.Resistor().down().label('R17\n47 kΩ', loc='bottom')
    d += elm.Ground()
    d += elm.Line().left(1.4).at(bt)
    d += elm.Capacitor().down().label('C11\n100 nF', loc='top')
    d += elm.Ground()
    net(d, bt, 'BATT', direction='right', length=1.2)

    s2 = d.add(elm.Ic(pins=[elm.IcPin(name='VBUS', side='left', pos=0.88, anchorname='vbus'),
                            elm.IcPin(name='3V3', side='left', pos=0.66, anchorname='v33'),
                            elm.IcPin(name='GND', side='left', pos=0.44, anchorname='gnd'),
                            elm.IcPin(name='USB-C', side='left', pos=0.18, anchorname='usb'),
                            elm.IcPin(name='GPIO33', side='right', pos=0.92, anchorname='g33'),
                            elm.IcPin(name='GPIO35', side='right', pos=0.79, anchorname='g35'),
                            elm.IcPin(name='GPIO7', side='right', pos=0.56, anchorname='g7'),
                            elm.IcPin(name='GPIO5', side='right', pos=0.37, anchorname='g5'),
                            elm.IcPin(name='GPIO3', side='right', pos=0.24, anchorname='g3'),
                            elm.IcPin(name='GPIO17', side='right', pos=0.08, anchorname='dac')],
                      size=(4.4, 7.0), label='ESP32-S2\nmini').anchor('vbus').at((vbat[0] + 5.0, vbat[1])))
    d += elm.Line().at(vbat).to(s2.vbus)
    d += elm.Line().left(0.6).at(s2.gnd)
    d += elm.Ground()
    d += elm.Line().left(0.6).at(s2.usb)
    d += elm.Label().at(d.here).label('Laptop', loc='left', fontsize=9)
    # OLED
    ol = d.add(elm.Ic(pins=[
        elm.IcPin(name='SDA', side='left', pos=0.75, anchorname='sda'),
        elm.IcPin(name='SCL', side='left', pos=0.45, anchorname='scl'),
        elm.IcPin(name='VCC', side='right', pos=0.75, anchorname='vcc'),
        elm.IcPin(name='GND', side='right', pos=0.45, anchorname='gnd')],
        size=(5.0, 2.4), label='OLED 1,3″\nI²C').right().anchor('sda').at((s2.g33[0] + 3.0, s2.g33[1])))
    d += elm.Line().at(s2.g33).to(ol.sda)
    d += elm.Line().right(1.2).at(s2.g35)
    d += elm.Line().to((s2.g35[0] + 1.2, ol.scl[1]))
    d += elm.Line().to(ol.scl)
    net(d, ol.vcc, '3V3', length=0.6)
    d += elm.Line().right(0.6).at(ol.gnd)
    d += elm.Ground()
    net(d, s2.v33, '3V3', direction='left', length=0.6)
    d += elm.Line().right(0.8).at(s2.g7)
    d += elm.Button().right().label('T1 PEGEL', loc='top', fontsize=9)
    d += elm.Line().down(0.3)
    d += elm.Ground()
    net(d, s2.g5, 'BATT', length=1.0)
    net(d, s2.g3, 'I_MESS', length=1.0)

    # ================= Hochsetzsteller und 24-V-Schiene (oben Mitte)
    xm = 21.5
    ym = 14.0
    mt = d.add(elm.Ic(pins=[elm.IcPin(side='left', anchorname='inp'),
                            elm.IcPin(side='right', anchorname='outp'),
                            elm.IcPin(side='bot', anchorname='gnd')],
                      size=(3.0, 1.5), label='MT3608\n→ 24,0 V').anchor('inp').at((xm, ym)))
    d += elm.Ground().at(mt.gnd)
    net(d, mt.inp, 'VBAT', direction='left', length=1.0)
    y24 = mt.outp[1]
    x24 = mt.outp[0] + 1.6
    d += elm.Line().right().at(mt.outp).to((x24, y24))
    d += elm.Dot()
    d += elm.Line().up(0.8).at((x24, y24))
    d += elm.Vdd().up().label('+24 V')
    d += elm.Capacitor(polar=True).down().at((x24, y24)).label('C1\n220 µF', loc='bottom')
    d += elm.Ground()
    d += elm.Line().right(1.6).at((x24, y24))
    d += elm.Dot()
    d += elm.Capacitor().down().label('C2\n100 nF', loc='bottom')
    d += elm.Ground()
    d += elm.Line().right(1.6).at((x24 + 1.6, y24))
    d += elm.Zener().down().reverse().label('D6\nP6KE30A', loc='bottom')
    d += elm.Ground()

    # ================= Mittenspannung VMID (oben rechts)
    xb = 34.0
    d += elm.Vdd().up().at((xb, 14.6)).label('+24 V')
    d += elm.Resistor().down().at((xb, 14.6)).label('R1\n10 kΩ', loc='bottom')
    d += elm.Dot()
    vm = d.here
    d += elm.Resistor().down().label('R2\n10 kΩ', loc='bottom')
    d += elm.Ground()
    d += elm.Line().right().at(vm).to((xb + 1.6, vm[1]))
    d += elm.Capacitor(polar=True).down().label('C3\n47 µF', loc='bottom')
    d += elm.Ground()
    d += elm.Line().left().at(vm).to((xb - 0.8, vm[1]))
    d += elm.Label().label('VMID = 12 V', loc='left')

    # ================= Tiefpass, Kanal A (Zeile y = 6)
    y = 6.0
    xd = s2.dac[0] + 1.6
    d += elm.Line().right().at(s2.dac).to((xd, s2.dac[1]))
    d += elm.Line().down().to((xd, y))
    d += elm.Resistor().right().label('R3 2,2 kΩ')
    d += elm.Dot()
    f1 = d.here
    d += elm.Capacitor().down().at(f1).label('C4\n2,2 nF', loc='bottom')
    d += elm.Ground()
    d += elm.Resistor().right().at(f1).label('R4 2,2 kΩ')
    d += elm.Dot()
    f2 = d.here
    d += elm.Label().at((f2[0], f2[1] + 0.45)).label('TP2', fontsize=9)
    d += elm.Capacitor().down().at(f2).label('C5\n2,2 nF', loc='bottom')
    d += elm.Ground()
    d += elm.Capacitor().right().at(f2).label('C6 1 µF')
    d += elm.Dot()
    ina = d.here
    d += elm.Resistor().up().at(ina).label('R5\n100 kΩ', loc='bottom')
    d += elm.Vdd().up().label('VMID')
    d += elm.Line().right().at(ina).to((ina[0] + 1.0, ina[1]))
    ua = d.add(elm.Opamp(leads=True).right().flip().anchor('in2').label('U1A', loc='center', ofst=(-0.4, 0), fontsize=9))
    pin_no(d, ua.in2, '3', dx=0.2)
    pin_no(d, ua.in1, '2', dx=0.2, dy=-0.55)
    pin_no(d, ua.out, '1', dx=-0.2)
    d += elm.Resistor().right().at(ua.out).label('R6\n100 Ω')
    d += elm.Dot()
    b1 = d.here
    q1 = d.add(elm.BjtNpn(circle=True).right().anchor('base').at((b1[0] + 0.9, b1[1] + 1.2)).label('Q1\nBD139', loc='right'))
    q2 = d.add(elm.BjtPnp(circle=True).right().anchor('base').at((b1[0] + 0.9, b1[1] - 1.2)).label('Q2\nBD140', loc='right'))
    d += elm.Line().at(b1).to((b1[0], q1.base[1]))
    d += elm.Line().to(q1.base)
    d += elm.Line().at(b1).to((b1[0], q2.base[1]))
    d += elm.Line().to(q2.base)
    d += elm.Line().up(0.5).at(q1.collector)
    d += elm.Vdd().up().label('+24 V')
    net(d, q2.collector, 'PGND', direction='down', length=0.4, fontsize=10)
    d += elm.Line().at(q1.emitter).to((q1.emitter[0], b1[1]))
    d += elm.Dot()
    oa = d.here
    d += elm.Line().at(q2.emitter).to((q2.emitter[0], b1[1]))
    d += elm.Line().right().at(oa).to((oa[0] + 1.6, oa[1])).label('OA (TP3)', loc='top', fontsize=9)
    d += elm.Dot()
    oa2 = d.here
    d += elm.Capacitor().right().label('C8 10 µF\nbipolar')
    d += elm.Resistor().right().label('R12\n47 Ω 2 W')
    d += elm.Fuse().right().label('PTC1\n0,1 A')
    d += elm.Dot(open=True).label('A (rot)', loc='right')

    # Gegenkopplung A: OA -> R7 -> Pin 2 ; Pin 2 -> R8 -> C7 -> GND
    yfb = 1.4
    d += elm.Line().down().at(oa2).to((oa2[0], yfb))
    d += elm.Resistor().left().to((ua.in1[0] + 2.0, yfb)).label('R7 18 kΩ', loc='bottom')
    d += elm.Line().left().to((ua.in1[0], yfb))
    d += elm.Line().up().to(ua.in1)
    d += elm.Dot().at(ua.in1)
    d += elm.Line().left().at(ua.in1).to((ua.in1[0] - 0.9, ua.in1[1]))
    d += elm.Resistor().down(2.0).label('R8\n3,3 kΩ', loc='bottom')
    d += elm.Capacitor(polar=True).down(1.6).label('C7\n10 µF', loc='bottom')
    d += elm.Ground()

    # ================= Kanal B (invertierend, Verstärkung -1), Zeile y = -2
    yb = -4.2
    d += elm.Line().down().at((oa2[0], yfb)).to((oa2[0], yb + 0.55))
    d += elm.Dot()
    d += elm.Resistor().left().to((oa2[0] - 3.0, yb + 0.55)).label('R9 10 kΩ', loc='top')
    d += elm.Dot()
    n6 = d.here
    ub = d.add(elm.Opamp(leads=True).left().anchor('in1').at((n6[0] - 0.5, n6[1])).label('U1B', loc='center', ofst=(0.4, 0), fontsize=9))
    d += elm.Line().at(n6).to(ub.in1)
    pin_no(d, ub.in1, '6', dx=-0.2)
    pin_no(d, ub.in2, '5', dx=-0.2, dy=-0.55)
    pin_no(d, ub.out, '7', dx=0.2)
    d += elm.Line().right().at(ub.in2).to((n6[0] + 0.3, ub.in2[1]))
    d += elm.Vdd().right().label('VMID', loc='right')
    d += elm.Resistor().left().at(ub.out).label('R11\n100 Ω')
    d += elm.Dot()
    b2 = d.here
    q3 = d.add(elm.BjtNpn(circle=True).left().flip().anchor('base').at((b2[0] - 0.9, b2[1] + 1.2)).label('Q3\nBD139', loc='left'))
    q4 = d.add(elm.BjtPnp(circle=True).left().flip().anchor('base').at((b2[0] - 0.9, b2[1] - 1.2)).label('Q4\nBD140', loc='left'))
    d += elm.Line().at(b2).to((b2[0], q3.base[1]))
    d += elm.Line().to(q3.base)
    d += elm.Line().at(b2).to((b2[0], q4.base[1]))
    d += elm.Line().to(q4.base)
    d += elm.Line().up(0.5).at(q3.collector)
    d += elm.Vdd().up().label('+24 V')
    net(d, q4.collector, 'PGND', direction='down', length=0.4, fontsize=10)
    d += elm.Line().at(q3.emitter).to((q3.emitter[0], b2[1]))
    d += elm.Dot()
    ob = d.here
    d += elm.Line().at(q4.emitter).to((q4.emitter[0], b2[1]))
    d += elm.Line().left().at(ob).to((ob[0] - 1.2, ob[1])).label('OB (TP4)', loc='bottom', fontsize=9)
    d += elm.Dot()
    ob2 = d.here
    # R10: OB -> Pin 6 (unten herum)
    yr10 = yb - 3.6
    d += elm.Line().down().at(ob2).to((ob2[0], yr10))
    d += elm.Resistor().right().to((n6[0], yr10)).label('R10 10 kΩ', loc='bottom')
    d += elm.Line().up().to(n6)
    # Ausgang E
    d += elm.Line().down().at(ob2).to((ob2[0], yr10 - 1.6))
    d += elm.Line().right().to((oa2[0] + 0.0, yr10 - 1.6))
    d += elm.Capacitor().right().label('C9 10 µF\nbipolar', loc='bottom')
    d += elm.Resistor().right().label('R13\n47 Ω 2 W', loc='bottom')
    d += elm.Fuse().right().label('PTC2\n0,1 A', loc='bottom')
    d += elm.Dot(open=True).label('E (grün-gelb)', loc='right')

    # ================= Strommessung (unten links)
    xs = -6.0
    ysm = yr10 - 1.0
    d += elm.Label().at((xs, ysm)).label('PGND', loc='left', fontsize=10)
    d += elm.Dot().at((xs, ysm))
    d += elm.Resistor().down().at((xs, ysm)).label('R14\n10 Ω', loc='bottom')
    d += elm.Ground()
    d += elm.Resistor().right().at((xs, ysm)).label('R15 10 kΩ')
    d += elm.Dot()
    im = d.here
    d += elm.Capacitor(polar=True).down().at(im).label('C10\n4,7 µF', loc='bottom')
    d += elm.Ground()
    net(d, im, 'I_MESS', length=1.0)

    # ================= Schutzdioden an OA und OB (unten Mitte)
    xp = 2.0
    for i, (nd, da, db) in enumerate((('OA', 'D2', 'D3'), ('OB', 'D4', 'D5'))):
        x0 = xp + i * 6.0
        d += elm.Vdd().up().at((x0, ysm + 0.3)).label('+24 V')
        d += elm.Diode().down().at((x0, ysm + 0.3)).reverse().label(da + '\n1N4148', loc='bottom', fontsize=9)
        d += elm.Dot()
        k = d.here
        d += elm.Diode().down().reverse().label(db + '\n1N4148', loc='bottom', fontsize=9)
        d += elm.Ground()
        net(d, k, nd, direction='left', length=1.2)

    d += elm.Label().at((-8.0, ysm - 7.0)).label(
        'U1 = TL072: Pin 8 an +24 V, Pin 4 an GND.  BD139/BD140 von vorn (Schrift zu dir): E – C – B.  '
        'MT3608 vor dem Aufbau auf 24,0 V einstellen.',
        loc='right', fontsize=10)
    d += elm.Label().at((-8.0, ysm - 7.8)).label(
        'Messpunkte: TP1 = GPIO17, TP2 = nach R4, TP3 = OA, TP4 = OB, TP5 = PGND (Strom: Mittelwert = Spannung ÷ 10 Ω).  '
        'USB nur bei S1 = AUS anstecken.',
        loc='right', fontsize=10)
print('geschrieben:', OUT)
