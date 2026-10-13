// =============================================================================
// Ansichtsmodell Kabelsuchgerät – Variante "Einfach" (Entwurf, KEIN Druckmodell)
// -----------------------------------------------------------------------------
// Handgerät in Stabform: Sensorkopf OBEN (vorn), darunter Display und Tasten,
// unten der Griff mit den Batterien. Sender: kleine Box mit LEDs, einer Taste
// und zwei Messleitungen (A rot, E grün-gelb). Maße vorläufig; das druckbare
// Gehäuse entsteht erst, wenn die Platinen feststehen.
//
//   teil = "hand" | "roentgen" | "sender" | "beide"
//   openscad -o hand.png --imgsize=1200,900 -D 'teil="hand"' ansicht.scad
// =============================================================================

teil = "beide";
$fn = 48;

C_BODY = "#3b4149"; C_RED = "#c41e2e"; C_SCREEN = "#07090b"; C_PIX = "#cfe9ff";
C_METAL = "#b9bfc7"; C_TEXT = "#d6dade"; C_COPPER = "#b8733a"; C_FERRIT = "#4a4a4a";
C_PCB = "#1f6b3a";

module rbox(x, y, z, r) {
  hull() for (ix = [-x/2 + r, x/2 - r]) for (iy = [-y/2 + r, y/2 - r]) for (iz = [r, z - r])
    translate([ix, iy, iz]) sphere(r);
}
module slab(x, y, r, h = 1) {
  hull() for (ix = [-x/2 + r, x/2 - r]) for (iy = [-y/2 + r, y/2 - r]) translate([ix, iy, 0]) cylinder(h = h, r = r);
}
module ftext(t, s, z, x = 0, y = -15, h = 0.4, halign = "center", font = "DejaVu Sans:style=Bold") {
  translate([x, y, z]) rotate([90, 0, 0]) linear_extrude(h) text(t, size = s, halign = halign, valign = "center", font = font);
}
module ttext(t, s, x, y, z, h = 0.4, halign = "center", font = "DejaVu Sans:style=Bold") {
  translate([x, y, z]) linear_extrude(h) text(t, size = s, halign = halign, valign = "center", font = font);
}

// ------------------------------------------------------------------ Handgerät
GW = 40; GD = 30;              // Griff/Körper Breite, Tiefe
BODY_H = 130;                  // Griff + Bedienteil (z 0 ... 130)
NECK_Z = 128; NECK_H = 48;     // Hals
HEAD_Z = 172; HEAD_W = 76; HEAD_D = 28; HEAD_H = 30;

module hand_form(a = 1) {
  color(C_BODY, a) {
    rbox(GW, GD, BODY_H, 8);
    hull() {
      translate([0, 0, NECK_Z]) slab(34, 26, 6, 2);
      translate([0, 0, HEAD_Z + 2]) slab(26, 20, 5, 2);
    }
    translate([0, 0, HEAD_Z]) rbox(HEAD_W, HEAD_D, HEAD_H, 7);
  }
}

module hand_details() {
  fy = -GD / 2;
  // Abtastfläche oben: rote Fläche + roter Ring knapp darunter
  sensor_markierung();
  // Kopf: Mittenmarke oben und Beschriftung
  color(C_TEXT) translate([0, 0, HEAD_Z + HEAD_H + 0.3]) linear_extrude(0.6) polygon([[-4, -5], [4, -5], [0, 3]]);
  color(C_TEXT) ftext("SENSOR", 3.2, HEAD_Z + 11, y = -HEAD_D / 2);
  color(C_RED) translate([0, -HEAD_D / 2 - 0.1, HEAD_Z + 3]) rotate([90, 0, 0]) linear_extrude(0.4) polygon([[-4, 0], [4, 0], [0, 6]]);
  // Display
  color("#15191d") translate([0, fy + 0.2, 110]) rotate([90, 0, 0]) linear_extrude(0.8) offset(r = 2) square([31, 21], center = true);
  color(C_SCREEN) translate([0, fy - 0.5, 110]) rotate([90, 0, 0]) linear_extrude(0.4) square([27, 14], center = true);
  color(C_PIX) {
    ftext("47", 4.6, 112.3, x = -6, y = fy - 0.9, font = "DejaVu Sans Condensed:style=Bold");
    ftext("dB", 1.8, 111.3, x = 0.5, y = fy - 0.9, halign = "left");
    translate([8, fy - 0.9, 112.3]) rotate([90, 0, 0]) linear_extrude(0.3) polygon([[-2, -2], [-2, 2], [2.5, 0]]);
    translate([-11.5, fy - 0.9, 106.4]) rotate([90, 0, 0]) linear_extrude(0.3) square([16, 1.4]);
  }
  // Tasten
  color(C_RED) translate([-9, fy, 86]) rotate([90, 0, 0]) cylinder(h = 2.5, d = 9);
  color("#8b9198") translate([9, fy, 86]) rotate([90, 0, 0]) cylinder(h = 2.5, d = 9);
  color(C_TEXT) {
    ftext("REF", 2.4, 76.5, x = -9, y = fy - 0.2);
    ftext("MODUS", 2.4, 76.5, x = 9, y = fy - 0.2);
    ftext("KABELSUCHER", 2.6, 66, y = fy - 0.2);
  }
  // Schiebeschalter seitlich, Summerlöcher
  color("#24282d") translate([GW / 2 - 0.5, -3, 95]) cube([2, 6, 10]);
  color("#15191d") for (z = [52, 56, 60]) for (x = [-6, 0, 6]) translate([x, fy + 0.1, z - 4]) rotate([90, 0, 0]) cylinder(h = 0.5, d = 1.6);
  // Batteriedeckel hinten (Fuge)
  color("#2a2f35") translate([0, GD / 2 - 0.2, 32]) rotate([-90, 0, 0]) linear_extrude(0.5)
    difference() { offset(r = 2) square([30, 56], center = true); offset(r = 1.4) square([29, 55], center = true); }
}

module hand() { hand_form(); hand_details(); }

module innen() {
  // Spule quer im Sensorkopf, dicht unter der Abtastfläche
  translate([0, 0, HEAD_Z + HEAD_H - 11]) rotate([0, 90, 0]) {
    color(C_FERRIT) cylinder(h = 60, d = 10, center = true);
    color(C_COPPER) cylinder(h = 40, d = 13, center = true);
  }
  // Kabel durch den Hals
  color("#9aa1a8") translate([6, 3, 118]) cylinder(h = 77, d = 3.5);
  // 4 x AA im Griff
  for (x = [-7.5, 7.5]) for (y = [-6.5, 7.5]) translate([x, y, 6]) {
    color("#2e6db0") cylinder(h = 47, d = 14.3);
    color(C_METAL) translate([0, 0, 47]) cylinder(h = 3, d = 5);
  }
  // Platine mit ESP32-S2 mini, Vorverstärker, Summer
  translate([0, 6, 62]) {
    color(C_PCB) translate([-17, 0, 0]) cube([34, 1.6, 62]);
    color("#2b3a8f") translate([-13, -3, 8]) cube([26, 3, 34]);       // ESP32-S2 mini
    color("#111") translate([-6, -4, 46]) cube([10, 4, 7]);            // OPV
  }
  color("#111") translate([0, -8, 120]) rotate([90, 0, 0]) cylinder(h = 4, d = 12);   // Summer
  color("#1b3f8f") translate([-15, -11, 99]) cube([30, 1.6, 24]);                      // OLED-Modul
}

module sensor_markierung(a = 1) {
  color(C_RED, a) {
    translate([0, 0, HEAD_Z + HEAD_H - 0.3]) slab(58, 11, 3, 0.6);
    translate([0, 0, HEAD_Z + HEAD_H - 11]) slab(HEAD_W + 0.6, HEAD_D + 0.6, 7, 3);
  }
}

module hand_roentgen() {
  innen();
  hand_form(0.18);
  sensor_markierung(0.5);
}

// ------------------------------------------------------------------ Sender (einfach)
SX = 105; SY = 62; SZ = 32;
module klemme(c) {
  color(c) hull() { cylinder(h = 26, d = 9); translate([0, 0, 26]) cylinder(h = 6, d = 11); }
  color(C_METAL) translate([0, 0, -12]) hull() {
    translate([0, 2, 12]) cube([5, 1.5, 1], center = true); translate([0, 1, 0]) cube([3, 1, 1], center = true);
    translate([0, -2, 12]) cube([5, 1.5, 1], center = true); translate([0, -1, 0]) cube([3, 1, 1], center = true);
  }
}
module sender() {
  color(C_BODY) rbox(SX, SY, SZ, 7);
  color(C_RED) translate([0, 0, SZ - 7]) difference() { slab(SX + 0.6, SY + 0.6, 7, 2); translate([0, 0, -1]) slab(SX - 2, SY - 2, 6, 4); }
  // LEDs mit Beschriftung
  leds = [[-30, "#3fd16a", "EIN"], [-12, "#f2c230", "STROM"], [6, C_RED, "BATT"]];
  for (l = leds) {
    color(l[1]) translate([l[0], 8, SZ - 0.5]) sphere(d = 5);
    color(C_TEXT) ttext(l[2], 2.4, l[0], 1, SZ);
  }
  // Taste Pegel
  color(C_RED) translate([32, 6, SZ - 0.5]) cylinder(h = 3, d = 11);
  color(C_TEXT) ttext("PEGEL", 2.4, 32, -3.5, SZ);
  color(C_RED) ttext("NUR AN SPANNUNGSFREIEN LEITUNGEN", 2.6, 0, -20, SZ);
  // Schiebeschalter vorn
  color("#24282d") translate([SX / 2 - 22, -SY / 2 - 1.5, 14]) cube([10, 2, 5]);
  // zwei Messleitungen links
  for (l = [[-12, C_RED], [12, "#7fb321"]]) {
    y = l[0];
    color("#24282d") translate([-SX / 2, y, 16]) rotate([0, -90, 0]) cylinder(h = 12, d1 = 10, d2 = 7);
    color(l[1]) hull() { translate([-SX / 2 - 10, y, 16]) sphere(d = 3.6); translate([-SX / 2 - 60, y * 2, 6]) sphere(d = 3.6); }
    translate([-SX / 2 - 60, y * 2, 6]) rotate([0, -90, 0]) klemme(l[1]);
  }
}

module aa() { rotate([0, 90, 0]) { color("#c9a227") cylinder(h = 47, d = 14.3); color(C_METAL) translate([0, 0, 47]) cylinder(h = 3, d = 5); } }

if (teil == "hand") hand();
else if (teil == "roentgen") hand_roentgen();
else if (teil == "sender") sender();
else {
  translate([-30, 40, 0]) hand();
  translate([110, -15, 0]) sender();
  translate([25, -80, 7.2]) aa();
}
