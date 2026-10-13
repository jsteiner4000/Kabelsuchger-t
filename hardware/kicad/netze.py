"""Netzlisten von Sender und Handgerät (Variante „Einfach“), gleiche Bauteilnummern wie in den
Steckbrett-Schaltplänen (hardware/*/testaufbau/*.py) und in docs/machbarkeitstest.md.

Jedes Bauteil: (Referenz, Wert, Symbol, Footprint, {Pin: Netz}). Netz None = nicht angeschlossen.
"""

R_V = 'Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical'
R_H = 'Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal'
R_2W = 'Resistor_THT:R_Axial_DIN0617_L17.0mm_D6.0mm_P5.08mm_Vertical'
C_S = 'Capacitor_THT:C_Disc_D3.0mm_W2.0mm_P2.50mm'
C_D5 = 'Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P5.00mm'
C_F47 = 'Capacitor_THT:C_Rect_L7.2mm_W2.5mm_P5.00mm_FKS2_FKP2_MKS2_MKP2'
C_F1U = 'Capacitor_THT:C_Rect_L7.2mm_W5.5mm_P5.00mm_FKS2_FKP2_MKS2_MKP2'
CP5 = 'Capacitor_THT:CP_Radial_D5.0mm_P2.00mm'
CP63 = 'Capacitor_THT:CP_Radial_D6.3mm_P2.50mm'
CP8 = 'Capacitor_THT:CP_Radial_D8.0mm_P3.50mm'
C_BP8 = 'Capacitor_THT:C_Radial_D8.0mm_H11.5mm_P3.50mm'
PTC = 'Capacitor_THT:C_Disc_D7.5mm_W5.0mm_P5.00mm'
DIP8 = 'Package_DIP:DIP-8_W7.62mm'
TO126 = 'Package_TO_SOT_THT:TO-126-3_Vertical'
DO41_V = 'Diode_THT:D_DO-41_SOD81_P5.08mm_Vertical_KathodeUp'
DO41_H = 'Diode_THT:D_DO-41_SOD81_P10.16mm_Horizontal'
DO35 = 'Diode_THT:D_DO-35_SOD27_P2.54mm_Vertical_KathodeUp'
DO15 = 'Diode_THT:D_DO-15_P5.08mm_Vertical_KathodeUp'
TB2 = 'TerminalBlock_Phoenix:TerminalBlock_Phoenix_MKDS-1,5-2-5.08_1x02_P5.08mm_Horizontal'


def hdr(n):
    return f'Connector_PinHeader_2.54mm:PinHeader_1x{n:02d}_P2.54mm_Vertical'


def con(n):
    return f'Connector:Conn_01x{n:02d}_Pin'


R, C, CP, DS, DZ = 'Device:R', 'Device:C', 'Device:C_Polarized', 'Device:D_Schottky', 'Device:D_Zener'
D = 'Device:D'
OPV2 = 'Amplifier_Operational:MCP6002-xP'
TL072 = 'Amplifier_Operational:TL072'


def two(a, b):
    return {'1': a, '2': b}


# ------------------------------------------------------------------ Handgerät
HANDGERAET = [
    # Versorgung und Batteriemessung
    ('J3', 'Batterie (nach S1)', con(2), hdr(2), two('VBAT_SW', 'GND')),
    ('D1', '1N5817', DS, DO41_V, two('+5V', 'VBAT_SW')),
    ('R1', '100k', R, R_V, two('+5V', 'BATT')),
    ('R2', '47k', R, R_V, two('BATT', 'GND')),
    ('C1', '100n', C, C_S, two('BATT', 'GND')),
    # Analogversorgung und VREF
    ('R3', '10', R, R_V, two('+3V3', 'A3V3')),
    ('C2', '100u/10V', CP, CP5, two('A3V3', 'GND')),
    ('C3', '100n', C, C_S, two('A3V3', 'GND')),
    ('R4', '10k', R, R_V, two('A3V3', 'VREF')),
    ('R5', '10k', R, R_V, two('VREF', 'GND')),
    ('C4', '10u', CP, CP5, two('VREF', 'GND')),
    ('C5', '100n', C, C_S, two('A3V3', 'GND')),
    # Spule und Eingang
    ('J1', 'Spule (Anfang, Ende, Schirm)', con(3), hdr(3), {'1': 'VREF', '2': 'COIL', '3': 'GND'}),
    ('R6', '22k', R, R_V, two('COIL', 'VREF')),
    ('R7', '1k', R, R_V, two('COIL', 'INP')),
    ('C6', '100p C0G', C, C_S, two('INP', 'VREF')),
    # MCP6002: Stufe 1 (A) und Stufe 2 (B)
    ('U1', 'MCP6002-I/P', OPV2, DIP8, {'1': 'OUT_A', '2': 'INN_A', '3': 'INP', '4': 'GND',
                                        '5': 'OUT_A', '6': 'INN_B', '7': 'OUT_B', '8': 'A3V3'}),
    ('R8', '47k', R, R_V, two('OUT_A', 'INN_A')),
    ('C7', '100p C0G', C, C_S, two('OUT_A', 'INN_A')),
    ('R9', '10k', R, R_V, two('INN_A', 'RG_A')),
    ('C8', '47n Folie', C, C_F47, two('RG_A', 'VREF')),
    ('R11', '390k', R, R_V, two('OUT_B', 'INN_B')),
    ('C10', '22p C0G', C, C_S, two('OUT_B', 'INN_B')),
    ('R12', '10k', R, R_V, two('INN_B', 'RG_B')),
    ('C11', '47n Folie', C, C_F47, two('RG_B', 'VREF')),
    ('R10', '1k', R, R_V, two('OUT_A', 'ADC_LO')),
    ('C9', '1n', C, C_S, two('ADC_LO', 'GND')),
    ('R13', '1k', R, R_V, two('OUT_B', 'ADC_HI')),
    ('C12', '1n', C, C_S, two('ADC_HI', 'GND')),
    # Bedienung, Anzeige, Ton, Reserve
    ('R14', '100', R, R_V, two('PIEZO_DRV', 'PIEZO')),
    ('J5', 'Piezo', con(2), hdr(2), two('PIEZO', 'GND')),
    ('J2', 'OLED (GND VCC SCL SDA)', con(4), hdr(4), {'1': 'GND', '2': '+3V3', '3': 'SCL', '4': 'SDA'}),
    ('J4', 'Tasten (REF MODUS GND)', con(3), hdr(3), {'1': 'BTN_REF', '2': 'BTN_MOD', '3': 'GND'}),
    ('J6', 'Reserve 2. Spule', con(5), hdr(5), {'1': 'RES1', '2': 'RES2', '3': 'A3V3', '4': 'VREF', '5': 'GND'}),
    ('A1', 'ESP32-S3 SuperMini', 'Kabelsucher:ESP32-S3_SuperMini', 'Kabelsucher:ESP32-S3_SuperMini',
     {'1': '+5V', '2': 'GND', '3': '+3V3', '4': None, '5': 'PIEZO_DRV', '6': 'BTN_MOD', '7': 'BTN_REF',
      '8': 'SCL', '9': 'SDA', '10': None, '11': None, '12': 'ADC_LO', '13': 'ADC_HI', '14': None,
      '15': 'BATT', '16': 'RES1', '17': 'RES2', '18': None}),
]
HANDGERAET_POWER = {'GND': 0.5, '+5V': 0.5, 'VBAT_SW': 0.5, '+3V3': 0.4, 'A3V3': 0.4}

# ------------------------------------------------------------------ Sender
SENDER = [
    # Batterie, Controller, Bedienung
    ('J1', 'Batterie (nach S1)', con(2), hdr(2), two('VBAT_SW', 'GND')),
    ('D1', '1N5817', DS, DO41_V, two('VBAT', 'VBAT_SW')),
    ('R16', '100k', R, R_V, two('VBAT', 'BATT')),
    ('R17', '47k', R, R_V, two('BATT', 'GND')),
    ('C11', '100n', C, C_S, two('BATT', 'GND')),
    ('A1', 'LOLIN S2 mini', 'Kabelsucher:LOLIN_S2_mini', 'Kabelsucher:LOLIN_S2_mini',
     {'1': None, '2': 'I_MESS', '3': 'BATT', '4': 'BTN', '5': None, '6': None, '7': None, '8': '+3V3',
      '9': None, '10': None, '11': 'SCL', '12': 'SDA', '13': 'DAC', '14': None, '15': 'GND', '16': 'VBAT'}),
    ('J2', 'OLED (GND VCC SCL SDA)', con(4), hdr(4), {'1': 'GND', '2': '+3V3', '3': 'SCL', '4': 'SDA'}),
    ('J3', 'Taste PEGEL', con(2), hdr(2), two('BTN', 'GND')),
    # Hochsetzsteller (Modul MT3608 über zwei Stiftleisten) und 24-V-Schiene
    ('J5', 'MT3608 Eingang (IN+ IN-)', con(2), hdr(2), two('VBAT', 'GND')),
    ('J6', 'MT3608 Ausgang (OUT+ OUT-)', con(2), hdr(2), two('+24V', 'GND')),
    ('C1', '220u/35V', CP, CP8, two('+24V', 'GND')),
    ('C2', '100n', C, C_S, two('+24V', 'GND')),
    ('D6', 'P6KE30A', DZ, DO15, two('+24V', 'GND')),
    ('C12', '100n', C, C_S, two('+24V', 'GND')),
    # Mittenspannung
    ('R1', '10k', R, R_V, two('+24V', 'VMID')),
    ('R2', '10k', R, R_V, two('VMID', 'GND')),
    ('C3', '47u/25V', CP, CP63, two('VMID', 'GND')),
    # Tiefpass und Eingang
    ('R3', '2k2', R, R_V, two('DAC', 'F1N')),
    ('C4', '2n2', C, C_S, two('F1N', 'GND')),
    ('R4', '2k2', R, R_V, two('F1N', 'F2N')),
    ('C5', '2n2', C, C_S, two('F2N', 'GND')),
    ('C6', '1u Folie', C, C_F1U, two('F2N', 'INP_A')),
    ('R5', '100k', R, R_V, two('INP_A', 'VMID')),
    # Brücke
    ('U1', 'TL072', TL072, DIP8, {'1': 'DRV_A', '2': 'INN_A', '3': 'INP_A', '4': 'GND',
                                   '5': 'VMID', '6': 'INN_B', '7': 'DRV_B', '8': '+24V'}),
    ('R6', '100', R, R_V, two('DRV_A', 'BASE_A')),
    ('Q1', 'BD139', 'Transistor_BJT:BD139', TO126, {'1': 'OA', '2': '+24V', '3': 'BASE_A'}),
    ('Q2', 'BD140', 'Transistor_BJT:BD140', TO126, {'1': 'OA', '2': 'PGND', '3': 'BASE_A'}),
    ('R7', '18k', R, R_V, two('OA', 'INN_A')),
    ('R8', '3k3', R, R_V, two('INN_A', 'RG_A')),
    ('C7', '10u/25V', CP, CP5, two('RG_A', 'GND')),
    ('R9', '10k', R, R_V, two('OA', 'INN_B')),
    ('R10', '10k', R, R_V, two('OB', 'INN_B')),
    ('R11', '100', R, R_V, two('DRV_B', 'BASE_B')),
    ('Q3', 'BD139', 'Transistor_BJT:BD139', TO126, {'1': 'OB', '2': '+24V', '3': 'BASE_B'}),
    ('Q4', 'BD140', 'Transistor_BJT:BD140', TO126, {'1': 'OB', '2': 'PGND', '3': 'BASE_B'}),
    ('D2', '1N4148', D, DO35, two('+24V', 'OA')),
    ('D3', '1N4148', D, DO35, two('OA', 'GND')),
    ('D4', '1N4148', D, DO35, two('+24V', 'OB')),
    ('D5', '1N4148', D, DO35, two('OB', 'GND')),
    # Ausgänge
    ('C8', '10u bipolar 63V', C, C_BP8, two('OA', 'A_C')),
    ('R12', '47 2W', R, R_2W, two('A_C', 'A_R')),
    ('F1', 'PTC 0,1A 60V', 'Device:Polyfuse', PTC, two('A_R', 'OUT_A')),
    ('C9', '10u bipolar 63V', C, C_BP8, two('OB', 'E_C')),
    ('R13', '47 2W', R, R_2W, two('E_C', 'E_R')),
    ('F2', 'PTC 0,1A 60V', 'Device:Polyfuse', PTC, two('E_R', 'OUT_E')),
    ('J4', 'Ausgang A (rot), E (gruen-gelb)', 'Connector:Screw_Terminal_01x02', TB2, two('OUT_A', 'OUT_E')),
    # Strommessung
    ('R14', '10', R, R_V, two('PGND', 'GND')),
    ('R15', '10k', R, R_V, two('PGND', 'I_MESS')),
    ('C10', '4u7 Keramik', C, C_D5, two('I_MESS', 'GND')),
]
SENDER_POWER = {'GND': 0.8, '+24V': 0.8, 'VBAT': 0.8, 'VBAT_SW': 0.8, 'OA': 0.8, 'OB': 0.8, 'PGND': 0.8,
                'A_C': 0.8, 'A_R': 0.8, 'OUT_A': 0.8, 'E_C': 0.8, 'E_R': 0.8, 'OUT_E': 0.8, '+3V3': 0.4}
