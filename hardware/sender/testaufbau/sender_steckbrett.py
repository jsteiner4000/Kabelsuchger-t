#!/usr/bin/env python3
"""Schaltplan Sender-Steckbrett (Phase 3) mit schemdraw -> sender_steckbrett.svg
Aufruf: python3 sender_steckbrett.py   (pip install schemdraw)
Bauteilnummern wie in docs/machbarkeitstest.md, Abschnitt 5.1."""
import schemdraw
import schemdraw.elements as elm

schemdraw.use('svg')
OUT = __file__.replace('.py', '.svg')


def pin_no(d, xy, txt, dx=0.0, dy=0.25):
    d += elm.Label().at((xy[0] + dx, xy[1] + dy)).label(txt, fontsize=8)


with schemdraw.Drawing(file=OUT, show=False, transparent=True) as d:
    d.config(unit=2.5, fontsize=11, color='currentColor', font='sans-serif', lw=1.3)

    # ================= Versorgung (oben links)
    pb = d.add(elm.Ic(pins=[elm.IcPin(side='right', anchorname='out')], size=(2.4, 1.2),
                      label='Powerbank\n5 V').anchor('out').at((2.4, 12)))
    d += elm.Line().right().at((2.4, 12)).to((3.6, 12)).label('USB-C', loc='top', fontsize=9)
    s2 = d.add(elm.Ic(pins=[elm.IcPin(name='USB', side='left', anchorname='usb'),
                            elm.IcPin(name='VBUS', side='right', pos=0.78, anchorname='vbus'),
                            elm.IcPin(name='GPIO17', side='right', pos=0.22, anchorname='dac'),
                            elm.IcPin(name='GND', side='bot', anchorname='gnd')],
                      size=(4.2, 3.4), label='ESP32-S2\nmini').anchor('usb').at((3.6, 12)))
    d += elm.Ground().at(s2.gnd)
    d += elm.Line().right().at(s2.vbus).to((9.2, s2.vbus[1]))
    mt = d.add(elm.Ic(pins=[elm.IcPin(side='left', anchorname='inp'),
                            elm.IcPin(side='right', anchorname='outp'),
                            elm.IcPin(side='bot', anchorname='gnd')],
                      size=(3.0, 1.5), label='MT3608\n5 V → 24 V').anchor('inp').at((9.2, s2.vbus[1])))
    d += elm.Ground().at(mt.gnd)
    y24 = mt.outp[1]
    d += elm.Line().right().at(mt.outp).to((14.0, y24))
    d += elm.Dot()
    d += elm.Line().up(0.8).at((14.0, y24))
    d += elm.Vdd().up().label('+24 V')
    d += elm.Capacitor(polar=True).down().at((14.0, y24)).label('C1\n220 µF', loc='bottom')
    d += elm.Ground()
    d += elm.Line().right().at((14.0, y24)).to((15.6, y24))
    d += elm.Capacitor().down().label('C2\n100 nF', loc='bottom')
    d += elm.Ground()

    # ================= Mittenspannung VMID (oben rechts)
    xb = 20.5
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
    d += elm.Line().right().at(s2.dac).to((8.6, s2.dac[1]))
    d += elm.Line().down().to((8.6, y))
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
    d += elm.Line().down(0.4).at(q2.collector)
    d += elm.Ground()
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
    d += elm.Line().down(0.4).at(q4.collector)
    d += elm.Ground()
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

    d += elm.Label().at((0, yr10 - 4.2)).label(
        'U1 = TL072: Pin 8 an +24 V, Pin 4 an GND.  BD139/BD140 von vorn (Schrift zu dir): E – C – B.',
        loc='right', fontsize=10)
    d += elm.Label().at((0, yr10 - 5.0)).label(
        'Messpunkte: TP1 = GPIO17, TP2 = nach R4, TP3 = OA, TP4 = OB.  Strom = Spannung über R12 ÷ 47 Ω.',
        loc='right', fontsize=10)
print('geschrieben:', OUT)
