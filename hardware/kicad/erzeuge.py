#!/usr/bin/env python3
"""Erzeugt die KiCad-7-Projekte für Sender und Handgerät (Variante „Einfach“, Entwurf).

Ablauf je Gerät:
  1. eigene Bibliothek (Module ESP32-S3 SuperMini, LOLIN S2 mini) schreiben
  2. Schaltplan (.kicad_sch) aus der Netzliste in netze.py, Verbindungen über Netznamen
  3. Platine (.kicad_pcb) mit pcbnew: Footprints laden, Netze zuweisen, nach platzierung.py setzen
  4. Routen mit Freerouting (Specctra DSN/SES), Massefläche oben und unten, Füllen
  5. DRC-Bericht, Gerber, Bohrdaten, Bilder (SVG)

Aufruf:  python3 erzeuge.py [sender|handgeraet] [--kein-routing]
Umgebung: KiCad 7 (pcbnew-Python-Modul, kicad-cli), Freerouting über FREEROUTING_CP und FREEROUTING_JAVA.
"""
import json
import os
import re
import shutil
import subprocess
import sys
import uuid

import pcbnew

import netze
import platzierung
from sexpr import Q, dump, find, find_all, parse, q

HIER = os.path.dirname(os.path.abspath(__file__))
SYMDIR = '/usr/share/kicad/symbols'
FPDIR = '/usr/share/kicad/footprints'
LIBDIR = os.path.join(HIER, 'lib')
EIGEN = 'Kabelsucher'
DATUM = '2026-10-13'


def uid():
    return str(uuid.uuid4())


def mm(v):
    return pcbnew.FromMM(v)


# =============================================================== eigene Bibliothek
MODULE = {
    # Name: (Pins links [(Nr, Name)], Pins rechts, Footprint-Beschreibung)
    'ESP32-S3_SuperMini': (
        [('1', '5V'), ('2', 'GND'), ('3', '3V3'), ('4', 'GPIO13'), ('5', 'GPIO12'), ('6', 'GPIO11'),
         ('7', 'GPIO10'), ('8', 'GPIO9'), ('9', 'GPIO8')],
        [('10', 'TX/GPIO43'), ('11', 'RX/GPIO44'), ('12', 'GPIO1'), ('13', 'GPIO2'), ('14', 'GPIO3'),
         ('15', 'GPIO4'), ('16', 'GPIO5'), ('17', 'GPIO6'), ('18', 'GPIO7')],
        'ESP32-S3 SuperMini, 2 x 9 Pins, Reihenabstand 15,24 mm, USB-C am Pin-1-Ende. '
        'BELEGUNG UND REIHENABSTAND VOR DER BESTELLUNG AM GELIEFERTEN MODUL PRUEFEN.'),
    'LOLIN_S2_mini': (
        [('1', 'EN'), ('2', 'GPIO3'), ('3', 'GPIO5'), ('4', 'GPIO7'), ('5', 'GPIO9'), ('6', 'GPIO11'),
         ('7', 'GPIO12'), ('8', '3V3')],
        [('9', 'GPIO39'), ('10', 'GPIO37'), ('11', 'GPIO35'), ('12', 'GPIO33'), ('13', 'GPIO18'),
         ('14', 'GPIO16'), ('15', 'GND'), ('16', 'VBUS')],
        'LOLIN S2 mini, nur die beiden aeusseren Reihen (D1-mini-kompatibel, 22,86 mm), USB-C unten. '
        'BELEGUNG VOR DER BESTELLUNG AM GELIEFERTEN MODUL PRUEFEN.'),
}


def eff(size=1.27, hide=False, justify=None):
    e = ['effects', ['font', ['size', size, size]]]
    if justify:
        e.append(['justify'] + justify.split())
    if hide:
        e.append('hide')
    return e


def modul_symbol(name):
    links, rechts, beschr = MODULE[name]
    n = max(len(links), len(rechts))
    h = (n + 1) * 2.54 / 2
    h = round(h / 1.27) * 1.27
    pins = []
    for side, liste in ((-1, links), (1, rechts)):
        for i, (nr, pname) in enumerate(liste):
            y = h - 2.54 - i * 2.54
            x = side * 10.16
            pins.append(['pin', 'passive', 'line', ['at', x, y, 0 if side < 0 else 180], ['length', 2.54],
                         ['name', q(pname), eff()], ['number', q(nr), eff()]])
    return ['symbol', q(name), ['pin_names', ['offset', 1.016]], ['in_bom', 'yes'], ['on_board', 'yes'],
            ['property', q('Reference'), q('A'), ['at', 0, h + 1.27, 0], eff()],
            ['property', q('Value'), q(name), ['at', 0, -h - 1.27, 0], eff()],
            ['property', q('Footprint'), q(f'{EIGEN}:{name}'), ['at', 0, 0, 0], eff(hide=True)],
            ['property', q('Datasheet'), q(''), ['at', 0, 0, 0], eff(hide=True)],
            ['property', q('ki_description'), q(beschr), ['at', 0, 0, 0], eff(hide=True)],
            ['symbol', q(f'{name}_0_1'),
             ['rectangle', ['start', -7.62, h], ['end', 7.62, -h], ['stroke', ['width', 0.254], ['type', 'default']],
              ['fill', ['type', 'background']]]],
            ['symbol', q(f'{name}_1_1')] + pins]


def modul_footprint(name):
    links, rechts, beschr = MODULE[name]
    if name == 'ESP32-S3_SuperMini':
        dx, aussen = 15.24, (-1.38, -1.1, 16.62, 21.42)
        usb = (7.62, 0.6)
    else:
        dx, aussen = 22.86, (-1.27, -2.0, 24.13, 32.3)
        usb = (11.43, 30.6)
    items = [['footprint', q(name), ['version', 20221018], ['generator', 'pcbnew'], ['layer', q('F.Cu')],
              ['descr', q(beschr)], ['tags', q(name.replace('_', ' '))], ['attr', 'through_hole']]]
    fp = items[0]
    x0, y0, x1, y1 = aussen
    fp.append(['fp_text', 'reference', q('REF**'), ['at', (x0 + x1) / 2, y0 - 1.6], ['layer', q('F.SilkS')],
               ['effects', ['font', ['size', 1, 1], ['thickness', 0.15]]]])
    fp.append(['fp_text', 'value', q(name), ['at', (x0 + x1) / 2, y1 + 1.6], ['layer', q('F.Fab')],
               ['effects', ['font', ['size', 1, 1], ['thickness', 0.15]]]])
    fp.append(['fp_text', 'user', q('USB-C'), ['at', usb[0], usb[1]], ['layer', q('F.SilkS')],
               ['effects', ['font', ['size', 1, 1], ['thickness', 0.15]]]])
    for lay, w in (('F.SilkS', 0.12), ('F.Fab', 0.1)):
        fp.append(['fp_rect', ['start', x0, y0], ['end', x1, y1], ['stroke', ['width', w], ['type', 'solid']],
                   ['fill', 'none'], ['layer', q(lay)]])
    n = max(len(links), len(rechts))
    ylast = (n - 1) * 2.54
    # Sperrfläche nur um die beiden Stiftreihen: unter dem Modul (auf Buchsenleisten) dürfen flache Teile sitzen
    for x in (0, dx):
        fp.append(['fp_rect', ['start', x - 1.6, -1.6], ['end', x + 1.6, ylast + 1.6],
                   ['stroke', ['width', 0.05], ['type', 'solid']], ['fill', 'none'], ['layer', q('F.CrtYd')]])
    for col, liste in ((0, links), (dx, rechts)):
        for i, (nr, _) in enumerate(liste):
            form = 'rect' if nr == '1' else 'oval'
            fp.append(['pad', q(nr), 'thru_hole', form, ['at', col, i * 2.54], ['size', 1.7, 1.7], ['drill', 1.0],
                       ['layers', q('*.Cu'), q('*.Mask')]])
    return fp


def schreibe_bibliothek():
    os.makedirs(os.path.join(LIBDIR, f'{EIGEN}.pretty'), exist_ok=True)
    lib = ['kicad_symbol_lib', ['version', 20220914], ['generator', 'kicad_symbol_editor']]
    for name in MODULE:
        lib.append(modul_symbol(name))
        with open(os.path.join(LIBDIR, f'{EIGEN}.pretty', f'{name}.kicad_mod'), 'w') as f:
            f.write(dump(modul_footprint(name)) + '\n')
    with open(os.path.join(LIBDIR, f'{EIGEN}.kicad_sym'), 'w') as f:
        f.write(dump(lib) + '\n')


# =============================================================== Symbole laden
_libs = {}


def _lib(name):
    if name not in _libs:
        path = os.path.join(LIBDIR, f'{EIGEN}.kicad_sym') if name == EIGEN else os.path.join(SYMDIR, f'{name}.kicad_sym')
        _libs[name] = parse(open(path, encoding='utf-8').read())
    return _libs[name]


def _sym_roh(lib, name):
    for e in _lib(lib):
        if isinstance(e, list) and e[:1] == ['symbol'] and e[1] == name:
            return e
    raise KeyError(f'{lib}:{name}')


def lib_symbol(lib_id):
    """Symbol für den Abschnitt lib_symbols, abgeleitete Symbole (extends) aufgelöst."""
    lib, name = lib_id.split(':')
    s = _sym_roh(lib, name)
    ext = find(s, 'extends')
    if ext:
        parent = _sym_roh(lib, ext[1])
        props = find_all(s, 'property')
        rest = [e for e in parent[2:] if not (isinstance(e, list) and e[0] in ('property', 'symbol'))]
        subs = []
        for sub in find_all(parent, 'symbol'):
            neu = list(sub)
            neu[1] = Q(sub[1].replace(ext[1], name, 1))
            subs.append(neu)
        s = ['symbol', name] + rest + props + subs
    out = list(s)
    out[1] = Q(lib_id)
    return out


def pins_von(lib_id):
    """{Pinnummer: (x, y, Winkel, Einheit)} in Symbolkoordinaten (y nach oben)."""
    s = lib_symbol(lib_id)
    res = {}
    for sub in find_all(s, 'symbol'):
        m = re.search(r'_(\d+)_(\d+)$', sub[1])
        unit = int(m.group(1)) if m else 0
        for p in find_all(sub, 'pin'):
            at = find(p, 'at')
            res[str(find(p, 'number')[1])] = (float(at[1]), float(at[2]), int(float(at[3])), unit)
    return res


def einheiten(lib_id):
    return max([u for (_, _, _, u) in pins_von(lib_id).values()] + [1])


# =============================================================== Schaltplan
def _drehe(vx, vy, rot):
    """Vektor im Schaltplan (y nach unten) um rot Grad gegen den Uhrzeigersinn drehen."""
    for _ in range((rot // 90) % 4):
        vx, vy = vy, -vx
    return vx, vy


def _pin_lage(px, py, pang, X, Y, rot):
    """Anschlusspunkt und Richtung nach außen im Schaltplan."""
    dx, dy = _drehe(px, -py, rot)
    dirs = {0: (1, 0), 90: (0, 1), 180: (-1, 0), 270: (0, -1)}
    bx, by = dirs[pang % 360]
    bx, by = _drehe(bx, -by, rot)
    return X + dx, Y + dy, -bx, -by


def _label(name, x, y, ox, oy):
    ang = {(1, 0): 0, (-1, 0): 180, (0, -1): 90, (0, 1): 270}[(ox, oy)]
    just = 'left bottom' if ang in (0, 90) else 'right bottom'
    return ['label', q(name), ['at', round(x, 2), round(y, 2), ang], ['fields_autoplaced'],
            ['effects', ['font', ['size', 1.27, 1.27]], ['justify'] + just.split()], ['uuid', uid()]]


def schreibe_schaltplan(projekt, titel, teile, pfad):
    root = uid()
    items = []
    used = {}
    sym_uuid = {}
    # Anordnung: Spalten, Teile in Listenreihenfolge, 2-polige Teile waagerecht
    x, y, spalte_b = 30.48, 30.48, 0
    ymax = 205
    for ref, wert, lib_id, fp, pins in teile:
        used[lib_id] = True
        pv = pins_von(lib_id)
        nunits = einheiten(lib_id)
        su = uid()
        sym_uuid[ref] = su
        for unit in range(1, nunits + 1):
            upins = {n: v for n, v in pv.items() if v[3] in (0, unit)}
            # Zweipole mit senkrechten Anschlüssen (R, C, Sicherung) waagerecht legen
            zwei = len(pv) == 2 and len({v[0] for v in pv.values()}) == 1
            rot = 90 if zwei else 0
            ys = [v[1] for v in upins.values()] or [0]
            xs = [v[0] for v in upins.values()] or [0]
            hoehe = (max(ys) - min(ys)) if not zwei else 2.54
            breite = (max(xs) - min(xs)) if not zwei else 7.62
            hoehe = max(hoehe, 5.08) + 10.16
            if y + hoehe > ymax:
                x += max(spalte_b, 55) + 15.24
                y = 30.48
                spalte_b = 0
            X = round((x + 25.4) / 2.54) * 2.54
            Y = round((y + hoehe / 2) / 2.54) * 2.54
            spalte_b = max(spalte_b, breite + 40)
            y += hoehe
            inst = ['symbol', ['lib_id', q(lib_id)], ['at', X, Y, rot], ['unit', unit], ['in_bom', 'yes'],
                    ['on_board', 'yes'], ['dnp', 'no'], ['uuid', su if nunits == 1 else uid()]]
            if nunits > 1 and unit == 1:
                inst[-1] = ['uuid', su]
            top = Y - (max(ys) if not zwei else 2.54) - 2.54
            inst.append(['property', q('Reference'), q(ref), ['at', X, round(top, 2), rot], eff()])
            inst.append(['property', q('Value'), q(wert), ['at', X, round(Y + (-min(ys) if not zwei else 2.54) + 2.54, 2), rot], eff()])
            inst.append(['property', q('Footprint'), q(fp), ['at', X, Y, 0], eff(hide=True)])
            inst.append(['property', q('Datasheet'), q('~'), ['at', X, Y, 0], eff(hide=True)])
            for n in upins:
                inst.append(['pin', q(n), ['uuid', uid()]])
            inst.append(['instances', ['project', q(projekt), ['path', q('/' + root), ['reference', q(ref)], ['unit', unit]]]])
            items.append(inst)
            for n, (px, py, pang, _) in upins.items():
                cx, cy, ox, oy = _pin_lage(px, py, pang, X, Y, rot)
                net = pins.get(n)
                if net is None:
                    items.append(['no_connect', ['at', round(cx, 2), round(cy, 2)], ['uuid', uid()]])
                    continue
                ex, ey = cx + ox * 2.54, cy + oy * 2.54
                items.append(['wire', ['pts', ['xy', round(cx, 2), round(cy, 2)], ['xy', round(ex, 2), round(ey, 2)]],
                              ['stroke', ['width', 0], ['type', 'default']], ['uuid', uid()]])
                items.append(_label(net, ex, ey, ox, oy))
    # Versorgungsmarken (PWR_FLAG) für die Netze, die von außen kommen
    used['power:PWR_FLAG'] = True
    netze_alle = {n for t in teile for n in t[4].values() if n}
    flags = [n for n in ('GND', 'VBAT_SW', '+5V', 'VBAT', '+3V3', 'A3V3', '+24V', 'PGND') if n in netze_alle]
    fx = 30.48
    for i, n in enumerate(flags):
        X, Y = fx + i * 25.4, 279.4
        items.append(['symbol', ['lib_id', q('power:PWR_FLAG')], ['at', X, Y, 0], ['unit', 1], ['in_bom', 'yes'],
                      ['on_board', 'yes'], ['dnp', 'no'], ['uuid', uid()],
                      ['property', q('Reference'), q(f'#FLG0{i + 1:02d}'), ['at', X, Y - 1.905, 0], eff(hide=True)],
                      ['property', q('Value'), q('PWR_FLAG'), ['at', X, Y - 3.81, 0], eff()],
                      ['property', q('Footprint'), q(''), ['at', X, Y, 0], eff(hide=True)],
                      ['property', q('Datasheet'), q('~'), ['at', X, Y, 0], eff(hide=True)],
                      ['pin', q('1'), ['uuid', uid()]],
                      ['instances', ['project', q(projekt), ['path', q('/' + root), ['reference', q(f'#FLG0{i + 1:02d}')], ['unit', 1]]]]])
        items.append(['wire', ['pts', ['xy', X, Y], ['xy', X, Y + 2.54]], ['stroke', ['width', 0], ['type', 'default']], ['uuid', uid()]])
        items.append(_label(n, X, Y + 2.54, 0, 1))
    items.append(['text', q('Verbindungen über Netznamen. Lesbare Zeichnung: hardware/*/testaufbau/*.svg'),
                  ['at', 30.48, 20.32, 0], ['effects', ['font', ['size', 2, 2]], ['justify', 'left', 'bottom']], ['uuid', uid()]])
    sch = ['kicad_sch', ['version', 20230121], ['generator', 'eeschema'], ['uuid', root], ['paper', q('A3')],
           ['title_block', ['title', q(titel)], ['date', q(DATUM)], ['rev', q('Entwurf 1')],
            ['company', q('Kabelsuchgerät (Variante Einfach)')],
            ['comment', 1, q('Entwurf vor dem Mini-Test. Modulbelegung vor der Bestellung am echten Modul prüfen.')]],
           ['lib_symbols'] + [lib_symbol(l) for l in sorted(used)]]
    sch += items
    sch.append(['sheet_instances', ['path', q('/'), ['page', q('1')]]])
    with open(pfad, 'w', encoding='utf-8') as f:
        f.write(dump(sch) + '\n')
    return root, sym_uuid


# =============================================================== Projektdateien
def schreibe_projekt(ordner, projekt, root, power):
    klassen = [{'name': 'Default', 'clearance': 0.25, 'track_width': 0.3, 'via_diameter': 0.8, 'via_drill': 0.4,
                'microvia_diameter': 0.3, 'microvia_drill': 0.1, 'diff_pair_gap': 0.25, 'diff_pair_via_gap': 0.25,
                'diff_pair_width': 0.2, 'line_style': 0, 'pcb_color': 'rgba(0, 0, 0, 0.000)',
                'schematic_color': 'rgba(0, 0, 0, 0.000)', 'wire_width': 6, 'bus_width': 12}]
    muster = []
    for breite in sorted(set(power.values())):
        name = f'Strom_{breite:.1f}mm'.replace('.', '_')
        k = dict(klassen[0])
        k.update({'name': name, 'track_width': breite, 'via_diameter': 1.0, 'via_drill': 0.5})
        klassen.append(k)
        for net, b in power.items():
            if b == breite:
                muster.append({'netclass': name, 'pattern': net})
    pro = {
        'board': {'design_settings': {
            'defaults': {'board_outline_line_width': 0.1, 'copper_line_width': 0.2, 'copper_text_size_h': 1.5,
                         'copper_text_size_v': 1.5, 'copper_text_thickness': 0.3, 'silk_line_width': 0.15,
                         'silk_text_size_h': 1.0, 'silk_text_size_v': 1.0, 'silk_text_thickness': 0.15},
            'rules': {'min_clearance': 0.2, 'min_track_width': 0.2, 'min_via_diameter': 0.6, 'min_through_hole_diameter': 0.3,
                      'min_hole_clearance': 0.25, 'min_hole_to_hole': 0.25, 'min_copper_edge_clearance': 0.4,
                      'min_silk_clearance': 0.0, 'min_text_height': 0.8, 'min_text_thickness': 0.08},
            'track_widths': [0.0, 0.3, 0.5, 0.8], 'via_dimensions': [{'diameter': 0.0, 'drill': 0.0}, {'diameter': 0.8, 'drill': 0.4}],
            'rule_severities': {'silk_overlap': 'ignore', 'silk_over_copper': 'ignore', 'silk_edge_clearance': 'ignore',
                                'lib_footprint_issues': 'ignore', 'lib_footprint_mismatch': 'ignore'}}},
        'meta': {'filename': f'{projekt}.kicad_pro', 'version': 1},
        'net_settings': {'classes': klassen, 'meta': {'version': 3}, 'net_colors': None,
                         'netclass_assignments': None, 'netclass_patterns': muster},
        'pcbnew': {'page_layout_descr_file': ''},
        'schematic': {'legacy_lib_dir': '', 'legacy_lib_list': []},
        'sheets': [[root, '']],
        'text_variables': {},
    }
    with open(os.path.join(ordner, f'{projekt}.kicad_pro'), 'w') as f:
        json.dump(pro, f, indent=2)
    rel = '${KIPRJMOD}/../lib'
    with open(os.path.join(ordner, 'sym-lib-table'), 'w') as f:
        f.write(f'(sym_lib_table\n  (lib (name "{EIGEN}")(type "KiCad")(uri "{rel}/{EIGEN}.kicad_sym")(options "")(descr "Module Kabelsuchgerät"))\n)\n')
    with open(os.path.join(ordner, 'fp-lib-table'), 'w') as f:
        f.write(f'(fp_lib_table\n  (lib (name "{EIGEN}")(type "KiCad")(uri "{rel}/{EIGEN}.pretty")(options "")(descr "Module Kabelsuchgerät"))\n)\n')


# =============================================================== Platine
def lade_fp(fpid):
    lib, name = fpid.split(':')
    pfad = os.path.join(LIBDIR, f'{EIGEN}.pretty') if lib == EIGEN else os.path.join(FPDIR, f'{lib}.pretty')
    fp = pcbnew.FootprintLoad(pfad, name)
    if fp is None:
        raise RuntimeError(f'Footprint fehlt: {fpid}')
    fp.SetFPID(pcbnew.LIB_ID(lib, name))
    return fp


def crt_box(fp):
    """Umriss (Sperrfläche) relativ zur Footprint-Position, in mm: (x0, y0, x1, y1)."""
    xs, ys = [], []
    for it in fp.GraphicalItems():
        if it.GetLayer() == pcbnew.F_CrtYd:
            bb = it.GetBoundingBox()
            xs += [bb.GetX(), bb.GetRight()]
            ys += [bb.GetY(), bb.GetBottom()]
    for p in fp.Pads():
        bb = p.GetBoundingBox()
        xs += [bb.GetX(), bb.GetRight()]
        ys += [bb.GetY(), bb.GetBottom()]
    pos = fp.GetPosition()
    return (pcbnew.ToMM(min(xs) - pos.x), pcbnew.ToMM(min(ys) - pos.y),
            pcbnew.ToMM(max(xs) - pos.x), pcbnew.ToMM(max(ys) - pos.y))


def setze(fp, x, y, rot):
    fp.SetOrientationDegrees(rot)
    fp.SetPosition(pcbnew.VECTOR2I(mm(x), mm(y)))


def text(board, s, x, y, groesse=1.0, layer=pcbnew.F_SilkS, rot=0):
    t = pcbnew.PCB_TEXT(board)
    t.SetText(s)
    t.SetPosition(pcbnew.VECTOR2I(mm(x), mm(y)))
    t.SetLayer(layer)
    t.SetTextSize(pcbnew.VECTOR2I(mm(groesse), mm(groesse)))
    t.SetTextThickness(mm(groesse * 0.15))
    if rot:
        t.SetTextAngleDegrees(rot)
    if layer == pcbnew.B_SilkS:
        t.SetMirrored(True)
    board.Add(t)


def baue_platine(projekt, teile, sym_uuid, plan, pfad):
    board = pcbnew.BOARD()
    board.SetCopperLayerCount(2)
    nets = {}
    for t in teile:
        for n in t[4].values():
            if n and n not in nets:
                ni = pcbnew.NETINFO_ITEM(board, n)
                board.Add(ni)
                nets[n] = ni
    fps = {}
    for ref, wert, lib_id, fpid, pins in teile:
        fp = lade_fp(fpid)
        fp.SetReference(ref)
        fp.SetValue(wert)
        fp.SetPath(pcbnew.KIID_PATH('/' + sym_uuid[ref]))
        board.Add(fp)
        for pad in fp.Pads():
            n = pins.get(pad.GetNumber())
            if n:
                pad.SetNet(nets[n])
        fps[ref] = fp

    W, H = plan['groesse']
    # Platzierung: feste Teile, dann Zonen (Reihen von links nach rechts)
    for ref, x, y, rot in plan.get('fest', []):
        setze(fps[ref], x, y, rot)
    for zone in plan.get('zonen', []):
        (zx0, zy0, zx1, zy1), liste = zone['rechteck'], zone['teile']
        luecke = zone.get('luecke', 0.6)
        cx, cy, zeile_h = zx0, zy0, 0
        for eintrag in liste:
            if eintrag == '|':                       # erzwungener Zeilenumbruch
                cx, cy, zeile_h = zx0, cy + zeile_h + luecke, 0
                continue
            ref, rot = eintrag if isinstance(eintrag, tuple) else (eintrag, 0)
            fp = fps[ref]
            setze(fp, 0, 0, rot)
            b = crt_box(fp)
            w, h = b[2] - b[0], b[3] - b[1]
            if cx + w > zx1 + 1e-6 and cx > zx0:
                cx, cy, zeile_h = zx0, cy + zeile_h + luecke, 0
            setze(fp, cx - b[0], cy - b[1], rot)
            cx += w + luecke
            zeile_h = max(zeile_h, h)
            if cy + h > zy1 + 1e-6:
                print(f'  WARNUNG: {ref} ragt aus Zone {zone["name"]} heraus (unten {cy + h:.1f} > {zy1})')
    # Umriss und Löcher
    r = pcbnew.PCB_SHAPE(board)
    r.SetShape(pcbnew.SHAPE_T_RECT)
    r.SetStart(pcbnew.VECTOR2I(0, 0))
    r.SetEnd(pcbnew.VECTOR2I(mm(W), mm(H)))
    r.SetLayer(pcbnew.Edge_Cuts)
    r.SetWidth(mm(0.1))
    board.Add(r)
    for i, (x, y) in enumerate(plan.get('loecher', [])):
        fp = lade_fp('MountingHole:MountingHole_3.2mm_M3')
        fp.SetReference(f'H{i + 1}')
        fp.SetValue('M3')
        fp.SetBoardOnly(True)
        fp.SetExcludedFromBOM(True)
        setze(fp, x, y, 0)
        board.Add(fp)
    for s, x, y, g, *rest in plan.get('texte', []):
        text(board, s, x, y, g, rot=rest[0] if rest else 0)
    pcbnew.SaveBoard(pfad, board)
    return board


def crt_rechtecke(fp):
    """Einzelne Sperrflächen-Rechtecke in Platinenkoordinaten (mm)."""
    r = []
    for it in fp.GraphicalItems():
        if it.GetLayer() == pcbnew.F_CrtYd:
            bb = it.GetBoundingBox()
            r.append((pcbnew.ToMM(bb.GetX()), pcbnew.ToMM(bb.GetY()), pcbnew.ToMM(bb.GetRight()), pcbnew.ToMM(bb.GetBottom())))
    if fp.GetFPID().GetLibNickname() == EIGEN:
        return r
    if r:   # Standardteile: Umriss aus allen Linien
        return [(min(a[0] for a in r), min(a[1] for a in r), max(a[2] for a in r), max(a[3] for a in r))]
    bb = fp.GetBoundingBox(False, False)
    return [(pcbnew.ToMM(bb.GetX()), pcbnew.ToMM(bb.GetY()), pcbnew.ToMM(bb.GetRight()), pcbnew.ToMM(bb.GetBottom()))]


def pruefe_ueberlappung(board):
    boxen = [(fp.GetReference(), r) for fp in board.GetFootprints() if not fp.GetReference().startswith('H')
             for r in crt_rechtecke(fp)]
    fehler = set()
    for i in range(len(boxen)):
        for j in range(i + 1, len(boxen)):
            (ra, a), (rb, b) = boxen[i], boxen[j]
            if ra != rb and a[0] < b[2] - 0.05 and b[0] < a[2] - 0.05 and a[1] < b[3] - 0.05 and b[1] < a[3] - 0.05:
                fehler.add(tuple(sorted((ra, rb))))
    return sorted(fehler), boxen


def vorschau(ordner, projekt):
    pcb = os.path.join(ordner, f'{projekt}.kicad_pcb')
    out = os.path.join(ordner, f'{projekt}-vorschau.svg')
    subprocess.run(['kicad-cli', 'pcb', 'export', 'svg', '-o', out, '--layers', 'F.Cu,B.Cu,F.SilkS,F.CrtYd,Edge.Cuts',
                    '--page-size-mode', '2', '--exclude-drawing-sheet', pcb], capture_output=True)
    return out


def masseflaechen(board, W, H):
    gnd = board.FindNet('GND')
    for layer in (pcbnew.F_Cu, pcbnew.B_Cu):
        z = pcbnew.ZONE(board)
        z.SetLayer(layer)
        z.SetNetCode(gnd.GetNetCode())
        z.SetLocalClearance(mm(0.4))
        z.SetMinThickness(mm(0.3))
        z.SetPadConnection(pcbnew.ZONE_CONNECTION_THERMAL)
        z.SetThermalReliefGap(mm(0.5))
        z.SetThermalReliefSpokeWidth(mm(0.5))
        o = z.Outline()
        o.NewOutline()
        for x, y in ((0.3, 0.3), (W - 0.3, 0.3), (W - 0.3, H - 0.3), (0.3, H - 0.3)):
            o.Append(mm(x), mm(y))
        board.Add(z)
    pcbnew.ZONE_FILLER(board).Fill(board.Zones())


def route(ordner, projekt, power):
    pcb = os.path.join(ordner, f'{projekt}.kicad_pcb')
    dsn = os.path.join(ordner, f'{projekt}.dsn')
    ses = os.path.join(ordner, f'{projekt}.ses')
    board = pcbnew.LoadBoard(pcb)
    if not pcbnew.ExportSpecctraDSN(board, dsn):
        raise RuntimeError('DSN-Export fehlgeschlagen')
    # Leiterbahnbreiten der Stromnetze sicher in die DSN schreiben (Einheit dort: um)
    txt = open(dsn).read()
    extra = ''
    for breite in sorted(set(power.values())):
        netzliste = ' '.join(f'"{n}"' for n, b in power.items() if b == breite)
        extra += f'    (class kabel_{int(breite * 1000)} {netzliste}\n      (circuit (use_via Via[0-1]_800:400_um))\n      (rule (width {breite * 1000:.0f}) (clearance 250)))\n'
    txt = re.sub(r'(\(network\b.*?)(\n  \)\n  \(wiring)', lambda m: m.group(1) + '\n' + extra.rstrip('\n') + m.group(2), txt, count=1, flags=re.S)
    open(dsn, 'w').write(txt)
    cp = os.environ.get('FREEROUTING_CP')
    java = os.environ.get('FREEROUTING_JAVA', 'java')
    if not cp:
        raise RuntimeError('FREEROUTING_CP nicht gesetzt')
    cmd = [java, '-cp', cp, 'app.freerouting.Freerouting', '-de', dsn, '-do', ses, '-mp', '40', '-mt', '1',
           '--gui.enabled=false', '--usage_and_diagnostic_data.disable_analytics=true']
    # Freerouting arbeitet nicht deterministisch: bis zu 5 Läufe, bis alles verbunden ist
    for lauf in range(1, 6):
        if os.path.exists(ses):
            os.remove(ses)
        log = subprocess.run(cmd, capture_output=True, text=True, timeout=1800)
        ausgabe = log.stdout + '\n' + log.stderr
        with open(os.path.join(ordner, 'freerouting.log'), 'w') as f:
            f.write(ausgabe[-30000:])
        if not os.path.exists(ses):
            raise RuntimeError('Freerouting hat keine SES-Datei geschrieben, siehe freerouting.log')
        m = re.findall(r'final score: [\d.]+ \((\d+) unrouted and (\d+) violations\)', ausgabe)
        offen, verstoss = (int(m[-1][0]), int(m[-1][1])) if m else (99, 99)
        print(f'  Freerouting Lauf {lauf}: {offen} offen, {verstoss} Verstöße')
        if offen == 0 and verstoss == 0:
            break
    lies_ses(board, ses)
    return board


def lies_ses(board, ses):
    """Leiterbahnen und Durchkontaktierungen aus der Freerouting-Sitzung übernehmen.
    (pcbnew.ImportSpecctraSES geht in KiCad 7 nur im Editor.) Koordinaten: 1/10 um, y nach oben."""
    s = parse(open(ses).read())
    routes = find(s, 'routes')
    res = find(routes, 'resolution')
    teiler = float(res[2]) * 1000.0          # Einheiten pro mm
    lagen = {'F.Cu': pcbnew.F_Cu, 'B.Cu': pcbnew.B_Cu}
    vias = {}
    lib = find(routes, 'library_out')
    for ps in find_all(lib, 'padstack') if lib else []:
        m = re.search(r'_(\d+):(\d+)_um', ps[1])
        if m:
            vias[ps[1]] = (int(m.group(1)) / 1000.0, int(m.group(2)) / 1000.0)
    n_bahn = n_via = 0
    for net in find_all(find(routes, 'network_out'), 'net'):
        ni = board.FindNet(str(net[1]))
        for w in find_all(net, 'wire'):
            path = find(w, 'path')
            lage, breite = lagen[str(path[1])], float(path[2]) / teiler
            pts = [(float(path[i]) / teiler, -float(path[i + 1]) / teiler) for i in range(3, len(path) - 1, 2)]
            for (x0, y0), (x1, y1) in zip(pts, pts[1:]):
                t = pcbnew.PCB_TRACK(board)
                t.SetStart(pcbnew.VECTOR2I(mm(x0), mm(y0)))
                t.SetEnd(pcbnew.VECTOR2I(mm(x1), mm(y1)))
                t.SetWidth(mm(breite))
                t.SetLayer(lage)
                t.SetNet(ni)
                board.Add(t)
                n_bahn += 1
        for v in find_all(net, 'via'):
            d, bohr = vias.get(str(v[1]), (0.8, 0.4))
            via = pcbnew.PCB_VIA(board)
            via.SetPosition(pcbnew.VECTOR2I(mm(float(v[2]) / teiler), mm(-float(v[3]) / teiler)))
            via.SetWidth(mm(d))
            via.SetDrill(mm(bohr))
            via.SetLayerPair(pcbnew.F_Cu, pcbnew.B_Cu)
            via.SetNet(ni)
            board.Add(via)
            n_via += 1
    print(f'  übernommen: {n_bahn} Leiterbahnstücke, {n_via} Durchkontaktierungen')


def drc(board, ordner, projekt):
    rep = os.path.join(ordner, f'{projekt}-drc.txt')
    pcbnew.WriteDRCReport(board, rep, pcbnew.EDA_UNITS_MILLIMETRES, True)
    txt = open(rep).read()
    zahlen = re.findall(r'\*\* Found (\d+) ([^*]+?) \*\*', txt)
    return rep, zahlen


def exporte(ordner, projekt):
    pcb = os.path.join(ordner, f'{projekt}.kicad_pcb')
    sch = os.path.join(ordner, f'{projekt}.kicad_sch')
    fert = os.path.join(ordner, 'fertigung')
    bild = os.path.join(ordner, 'bilder')
    shutil.rmtree(fert, ignore_errors=True)
    os.makedirs(fert)
    os.makedirs(bild, exist_ok=True)
    run = lambda *a: subprocess.run(['kicad-cli'] + list(a), capture_output=True, text=True)
    run('pcb', 'export', 'gerbers', '-o', fert + '/', '--layers',
        'F.Cu,B.Cu,F.SilkS,B.SilkS,F.Mask,B.Mask,Edge.Cuts', '--subtract-soldermask', pcb)
    run('pcb', 'export', 'drill', '-o', fert + '/', '--format', 'excellon', '--excellon-separate-th', pcb)
    shutil.make_archive(os.path.join(ordner, f'{projekt}-gerber'), 'zip', fert)
    run('pcb', 'export', 'svg', '-o', os.path.join(bild, f'{projekt}-oben.svg'), '--layers',
        'F.Cu,F.SilkS,F.Mask,Edge.Cuts', '--page-size-mode', '2', '--exclude-drawing-sheet', pcb)
    run('pcb', 'export', 'svg', '-o', os.path.join(bild, f'{projekt}-unten.svg'), '--layers',
        'B.Cu,B.SilkS,Edge.Cuts', '--page-size-mode', '2', '--exclude-drawing-sheet', '--mirror', pcb)
    run('pcb', 'export', 'svg', '-o', os.path.join(bild, f'{projekt}-bestueckung.svg'), '--layers',
        'F.SilkS,Edge.Cuts', '--page-size-mode', '2', '--exclude-drawing-sheet', '--black-and-white', pcb)
    run('sch', 'export', 'pdf', '-o', os.path.join(ordner, f'{projekt}-schaltplan.pdf'), sch)


GERAETE = {
    'handgeraet': ('Handgerät', netze.HANDGERAET, netze.HANDGERAET_POWER, platzierung.HANDGERAET),
    'sender': ('Sender', netze.SENDER, netze.SENDER_POWER, platzierung.SENDER),
}


def main():
    wahl = [a for a in sys.argv[1:] if not a.startswith('--')] or list(GERAETE)
    routen = '--kein-routing' not in sys.argv
    schreibe_bibliothek()
    for projekt in wahl:
        titel, teile, power, plan = GERAETE[projekt]
        ordner = os.path.join(HIER, projekt)
        os.makedirs(ordner, exist_ok=True)
        print(f'== {titel}')
        root, sym_uuid = schreibe_schaltplan(projekt, f'Kabelsuchgerät – {titel}', teile,
                                             os.path.join(ordner, f'{projekt}.kicad_sch'))
        schreibe_projekt(ordner, projekt, root, power)
        pcb = os.path.join(ordner, f'{projekt}.kicad_pcb')
        board = baue_platine(projekt, teile, sym_uuid, plan, pcb)
        fehler, _ = pruefe_ueberlappung(board)
        print('  Überlappungen:', fehler or 'keine')
        if not routen:
            print('  Vorschau:', vorschau(ordner, projekt))
        if routen:
            board = route(ordner, projekt, power)
            W, H = plan['groesse']
            masseflaechen(board, W, H)
            pcbnew.SaveBoard(pcb, board)
            board = pcbnew.LoadBoard(pcb)
            pcbnew.ZONE_FILLER(board).Fill(board.Zones())
            pcbnew.SaveBoard(pcb, board)
            rep, zahlen = drc(board, ordner, projekt)
            print('  DRC:', zahlen)
            exporte(ordner, projekt)
        for f in ('dsn', 'ses'):
            p = os.path.join(ordner, f'{projekt}.{f}')
            if os.path.exists(p):
                os.remove(p)


if __name__ == '__main__':
    main()
