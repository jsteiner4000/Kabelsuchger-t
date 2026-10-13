#!/usr/bin/env python3
"""Schaltplan Empfänger-Steckbrett (Phase 3) mit schemdraw -> empfaenger_steckbrett.svg
Aufruf: python3 empfaenger_steckbrett.py   (pip install schemdraw)
Bauteilnummern wie in docs/machbarkeitstest.md, Abschnitt 5.2."""
import schemdraw
import schemdraw.elements as elm

schemdraw.use('svg')
OUT = __file__.replace('.py', '.svg')


def flag(d, xy, text, direction='up', loc=None):
    """Netz-Fahne (Versorgungs- oder Bezugsnetz)."""
    e = elm.Vdd()
    e = getattr(e, direction)()
    d += e.at(xy).label(text, loc=loc) if loc else e.at(xy).label(text)


def net(d, xy, text, direction='down', length=0.8):
    """Kurze Leitung mit Netzname am Ende (statt Versorgungsfahne)."""
    ln = getattr(elm.Line(), direction)(length).at(xy)
    d += ln
    end = ln.end
    loc = {'down': 'bottom', 'up': 'top', 'right': 'right', 'left': 'left'}[direction]
    d += elm.Label().at(end).label(text, loc=loc, fontsize=10)


def pin_no(d, xy, txt, dx=0.0, dy=0.25):
    d += elm.Label().at((xy[0] + dx, xy[1] + dy)).label(txt, fontsize=8)


with schemdraw.Drawing(file=OUT, show=False, transparent=True) as d:
    d.config(unit=2.5, fontsize=11, color='currentColor', font='sans-serif', lw=1.3)

    H = 6.4
    # ================= ESP32-S3
    s3 = d.add(elm.Ic(pins=[
        elm.IcPin(name='5V', side='left', pos=0.88, anchorname='v5'),
        elm.IcPin(name='3V3', side='left', pos=0.66, anchorname='v33'),
        elm.IcPin(name='GND', side='left', pos=0.44, anchorname='gnd'),
        elm.IcPin(name='USB', side='left', pos=0.12, anchorname='usb'),
        elm.IcPin(name='GPIO4', side='right', pos=0.88, anchorname='g4'),
        elm.IcPin(name='GPIO5', side='right', pos=0.74, anchorname='g5'),
        elm.IcPin(name='GPIO6', side='right', pos=0.60, anchorname='g6'),
        elm.IcPin(name='GPIO7', side='right', pos=0.46, anchorname='g7'),
        elm.IcPin(name='GPIO8', side='right', pos=0.24, anchorname='g8'),
        elm.IcPin(name='GPIO9', side='right', pos=0.10, anchorname='g9')],
        size=(4.6, H), label='ESP32-S3\nSuperMini').right().at((0, 0)))
    d += elm.Line().left(0.6).at(s3.v5)
    flag(d, d.here, '5V')
    d += elm.Line().left(0.6).at(s3.v33)
    flag(d, d.here, '3V3')
    d += elm.Line().left(0.6).at(s3.gnd)
    d += elm.Ground()
    d += elm.Line().left(1.6).at(s3.usb).label('USB → Laptop', loc='top', fontsize=9)

    # ================= PCM1808-Modul (gleiche Höhe, Pins auf gleicher Linie)
    xp = s3.g4[0] + 6.0
    pcm = d.add(elm.Ic(pins=[
        elm.IcPin(name='SCK', side='left', pos=0.88, anchorname='sck'),
        elm.IcPin(name='BCK', side='left', pos=0.74, anchorname='bck'),
        elm.IcPin(name='LRC', side='left', pos=0.60, anchorname='lrc'),
        elm.IcPin(name='OUT', side='left', pos=0.46, anchorname='dout'),
        elm.IcPin(name='L', side='right', pos=0.74, anchorname='inl'),
        elm.IcPin(name='R', side='right', pos=0.30, anchorname='inr'),
        elm.IcPin(name='5V', side='bot', pos=0.10, anchorname='v5'),
        elm.IcPin(name='3.3V', side='bot', pos=0.27, anchorname='v33'),
        elm.IcPin(name='GND', side='bot', pos=0.44, anchorname='gnd'),
        elm.IcPin(name='FMT', side='bot', pos=0.61, anchorname='fmt'),
        elm.IcPin(name='MD0', side='bot', pos=0.77, anchorname='md0'),
        elm.IcPin(name='MD1', side='bot', pos=0.93, anchorname='md1')],
        size=(7.2, H), label='PCM1808-\nModul').right().anchor('sck').at((xp, s3.g4[1])))
    d += elm.Line().at(s3.g4).to(pcm.sck).label('MCLK 24,6 MHz', loc='top', fontsize=9)
    d += elm.Line().at(s3.g5).to(pcm.bck).label('BCK 6,1 MHz', loc='top', fontsize=9)
    d += elm.Line().at(s3.g6).to(pcm.lrc).label('LRCK 96 kHz', loc='top', fontsize=9)
    d += elm.Line().at(pcm.dout).to(s3.g7).label('DATEN', loc='top', fontsize=9)
    net(d, pcm.v5, 'A5')
    net(d, pcm.v33, '3V3')
    for a in (pcm.gnd, pcm.fmt, pcm.md0, pcm.md1):
        d += elm.Line().down(0.8).at(a)
        d += elm.Ground()

    # ================= OLED
    ol = d.add(elm.Ic(pins=[
        elm.IcPin(name='SDA', side='left', pos=0.75, anchorname='sda'),
        elm.IcPin(name='SCL', side='left', pos=0.45, anchorname='scl'),
        elm.IcPin(name='VCC', side='bot', pos=0.3, anchorname='vcc'),
        elm.IcPin(name='GND', side='bot', pos=0.7, anchorname='gnd')],
        size=(4.2, 2.6), label='OLED 1,3″\nI²C').right().anchor('sda').at((s3.g8[0] + 2.4, s3.g8[1] - 3.0)))
    d += elm.Line().right(1.0).at(s3.g8)
    d += elm.Line().down().to((s3.g8[0] + 1.0, ol.sda[1]))
    d += elm.Line().right().to(ol.sda)
    d += elm.Line().right(0.6).at(s3.g9)
    d += elm.Line().down().to((s3.g9[0] + 0.6, ol.scl[1]))
    d += elm.Line().right().to(ol.scl)
    net(d, ol.vcc, '3V3')
    d += elm.Line().down(0.6).at(ol.gnd)
    d += elm.Ground()

    # ================= Vorverstärker Kanal L (rechts vom PCM1808, Signal läuft nach links)
    d += elm.Capacitor(polar=True).right().at(pcm.inl).label('C27\n10 µF', loc='top')
    d += elm.Dot()
    ol_node = d.here
    ua = d.add(elm.Opamp(leads=True).left().flip().anchor('out').at((ol_node[0] + 3.0, ol_node[1])).label('U2A', loc='center', ofst=(0.4, 0), fontsize=9))
    d += elm.Line().at(ol_node).to(ua.out)
    pin_no(d, ua.out, '1', dx=0.2)
    pin_no(d, ua.in1, '2', dx=-0.2)
    pin_no(d, ua.in2, '3', dx=-0.2, dy=-0.55)
    # Gegenkopplung R24 || C25 über dem OPV
    yf = ua.in1[1] + 1.6
    d += elm.Line().up().at(ol_node).to((ol_node[0], yf))
    d += elm.Resistor().right().to((ua.in1[0] + 0.6, yf)).label('R24 33 kΩ', loc='top')
    d += elm.Line().down().to((ua.in1[0] + 0.6, ua.in1[1]))
    d += elm.Line().left().to(ua.in1)
    d += elm.Line().up().at((ol_node[0] + 0.0, yf)).to((ol_node[0], yf + 1.3))
    d += elm.Capacitor().right().to((ua.in1[0] + 0.6, yf + 1.3)).label('C25 100 pF', loc='top')
    d += elm.Line().down().to((ua.in1[0] + 0.6, yf))
    # R25 + C26 vom Minus-Eingang nach VREF
    d += elm.Dot().at((ua.in1[0] + 0.6, ua.in1[1]))
    d += elm.Line().right().at((ua.in1[0] + 0.6, ua.in1[1])).to((ua.in1[0] + 1.6, ua.in1[1]))
    d += elm.Resistor().right().label('R25 1 kΩ')
    d += elm.Capacitor().right().label('C26 470 nF')
    net(d, d.here, 'VREF', direction='right', length=0.4)
    # Plus-Eingang: R23 von der Spule, C24 nach VREF
    d += elm.Line().right().at(ua.in2).to((ua.in2[0] + 0.6, ua.in2[1]))
    d += elm.Dot()
    pn = d.here
    d += (c24 := elm.Capacitor().down().at(pn).label('C24\n1 nF', loc='bottom'))
    net(d, c24.end, 'VREF', length=0.4)
    d += elm.Resistor().right().at(pn).label('R23 100 Ω', loc='bottom')
    d += elm.Line().right(0.6)
    d += elm.Dot()
    kn = d.here
    d += (lc := elm.Inductor2(loops=4).down().at(kn).label('H-Spule', loc='bottom').label('Ende', loc='top', fontsize=9))
    net(d, lc.end, 'VREF (Anfang)', length=0.4)
    d += elm.Label().at((kn[0] + 1.4, kn[1] - 2.2)).label('Schirm → GND', fontsize=9)

    # ================= Versorgung Analogteil (unten rechts)
    xv = xp + 1.0
    yv = s3.gnd[1] - 8.0
    flag(d, (xv, yv), '5V')
    d += elm.Resistor().right().at((xv, yv)).label('R20 10 Ω')
    d += elm.Dot()
    a5 = d.here
    net(d, a5, 'A5 (Analog-5 V)', direction='up', length=1.4)
    d += elm.Capacitor(polar=True).down().at(a5).label('C20\n220 µF', loc='bottom')
    d += elm.Ground()
    d += elm.Line().right(1.6).at(a5)
    d += elm.Capacitor().down().label('C21\n100 nF', loc='bottom')
    d += elm.Ground()
    d += elm.Line().right(1.6).at((a5[0] + 1.6, a5[1]))
    d += elm.Resistor().right().label('R21 10 kΩ')
    d += elm.Dot()
    vr = d.here
    net(d, vr, 'VREF 2,5 V', direction='up', length=1.4)
    d += elm.Resistor().down().at(vr).label('R22\n10 kΩ', loc='bottom')
    d += elm.Ground()
    d += elm.Line().right(1.6).at(vr)
    d += elm.Capacitor(polar=True).down().label('C22\n10 µF', loc='bottom')
    d += elm.Ground()

print('geschrieben:', OUT)
