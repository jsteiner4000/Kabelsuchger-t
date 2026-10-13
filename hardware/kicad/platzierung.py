"""Lage der Bauteile auf den Platinen (mm, Ursprung oben links, y nach unten).

'fest':  (Referenz, x, y, Drehung) – Position des Footprint-Ursprungs (meist Pin 1)
'zonen': Rechteck (x0, y0, x1, y1) und Reihenfolge; die Teile werden zeilenweise von links nach rechts
         gepackt. '|' erzwingt eine neue Zeile. (Referenz, Drehung) dreht ein Teil.
'texte': Bestückungsdruck (Text, x, y, Größe[, Drehung])
"""

# ------------------------------------------------------------------ Handgerät 34 x 70 mm
# Oben die Spule (J1), darunter der Analogteil, unten das ESP32-S3-Modul auf Buchsenleisten.
# Unter dem Modul sitzen nur flache Teile (Höhe < 8 mm).
HANDGERAET = {
    'groesse': (34, 70),
    'fest': [
        ('J1', 12.46, 2.3, 90),
        ('A1', 24.62, 68.0, 180),
        ('J3', 2.2, 47.0, 0),
        ('J4', 2.2, 54.0, 0),
        ('J5', 2.2, 63.4, 0),
        ('J2', 31.8, 43.6, 0),
        ('J6', 31.8, 55.0, 0),
    ],
    'zonen': [
        {'name': 'analog', 'rechteck': (0.7, 5.0, 33.3, 41.0), 'luecke': 0.5,
         'teile': ['R6', 'R7', 'C6', 'R9', 'C8', '|',
                   'R8', 'C7', 'U1', 'R11', 'C10', '|',
                   'R12', 'C11', 'R10', 'C9', 'R13', '|',
                   'C12', 'R3', 'C3', 'C5', 'R4', 'R5', '|', 'C2', 'C4']},
        {'name': 'unter-modul', 'rechteck': (11.3, 47.0, 22.7, 67.5), 'luecke': 0.5,
         'teile': [('D1', 0), '|', 'R1', 'R2', '|', 'C1', '|', 'R14']},
    ],
    'texte': [
        ('SPULE: A E S', 25.0, 2.3, 0.8),
        ('BAT + -', 5.4, 48.3, 0.8, 90),
        ('REF MOD G', 5.4, 56.5, 0.8, 90),
        ('PIEZO', 5.4, 64.7, 0.8, 90),
        ('OLED G V C D', 28.6, 47.4, 0.8, 90),
        ('2.SPULE 5 6 A V G', 28.6, 60.1, 0.8, 90),
    ],
}

# ------------------------------------------------------------------ Sender 80 x 60 mm
# Links das ESP32-S2-Modul (USB-C unten), oben links die Anschlüsse, rechts die Endstufe,
# unten rechts die Schraubklemme A/E. Kleine Widerstände und Dioden stehen senkrecht.
SENDER = {
    'groesse': (80, 60),
    'fest': [
        ('A1', 4.0, 21.5, 0),
        ('J4', 73.0, 47.5, 90),
    ],
    'loecher': [(3.5, 3.5), (76.5, 3.5), (76.5, 56.5), (3.5, 56.5)],   # H4 liegt unter dem USB-Ende des Moduls (Schraubenkopf < 8 mm)
    'zonen': [
        {'name': 'oben-links', 'rechteck': (7.4, 1.0, 27.5, 19.5), 'luecke': 0.6,
         'teile': [('J1', 90), ('J5', 90), ('J6', 90), '|', ('J2', 90), ('J3', 90), '|', 'D1', 'R16', 'R17', 'C11']},
        {'name': 'unter-modul', 'rechteck': (6.6, 22.5, 24.0, 53.5), 'luecke': 0.8,
         'teile': ['R3', 'C4', 'R4', 'C5', '|', 'R14', 'R15', 'C10']},
        {'name': 'leistung', 'rechteck': (29.0, 2.2, 72.0, 59.4), 'luecke': 0.6,
         'teile': ['C1', 'C2', 'D6', 'C12', 'R1', 'R2', 'C3', '|',
                   'R5', 'C6', 'U1', 'R7', 'R8', 'C7', '|',
                   'R6', 'Q1', 'Q2', ('D2', 90), ('D3', 90), 'R9', 'R10', '|',
                   'R11', 'Q3', 'Q4', ('D4', 90), ('D5', 90), '|',
                   'C8', 'R12', 'F1', '|', 'C9', 'R13', 'F2']},
    ],
    'texte': [
        ('A', 66.2, 47.5, 1.2),
        ('E', 66.2, 42.4, 1.2),
    ],
}
