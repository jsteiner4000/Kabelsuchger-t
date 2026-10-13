// =============================================================================
// Spulenkörper für die Ferritstab-Spulen des Kabelsuchgeräts (parametrisch)
// -----------------------------------------------------------------------------
// Druck: PETG, Düse 0,4 mm, Schichthöhe 0,2 mm, 3 Wandlinien, stehend drucken
// (unterer Flansch auf dem Druckbett). Kein Stützmaterial nötig: der obere
// Flansch hat unten eine 45°-Schräge.
//
// STL erzeugen (Beispiel):
//   openscad -o spulenkoerper_H.stl -D 'wickel_l=50' -D 'flansch_d=22' spulenkoerper.scad
//   openscad -o spulenkoerper_V.stl -D 'wickel_l=36' -D 'flansch_d=15.5' spulenkoerper.scad
// =============================================================================

stab_d    = 10.0;   // Durchmesser des Ferritstabs (mm)
spiel     = 0.4;    // Spiel im Durchmesser, damit der Stab ohne Gewalt hineingeht
wand      = 0.9;    // Wandstärke des Rohrs
wickel_l  = 50;     // Länge der Wicklung zwischen den Flanschen
flansch_d = 22;     // Flanschdurchmesser (V-Spule: 15,5, passt in M20-Isolierrohr)
flansch_t = 1.6;    // Flanschdicke
schlitz_b = 1.0;    // Breite des Drahtschlitzes im Flansch

$fn = 96;

innen_d = stab_d + spiel;
aussen_d = innen_d + 2 * wand;
konus_h = (flansch_d - aussen_d) / 2;          // 45°-Schräge unter dem oberen Flansch
gesamt_h = flansch_t + wickel_l + konus_h + flansch_t;

module koerper() {
  // Rohr
  cylinder(h = gesamt_h, d = aussen_d);
  // unterer Flansch (liegt auf dem Druckbett)
  cylinder(h = flansch_t, d = flansch_d);
  // oberer Flansch mit 45°-Schräge darunter
  translate([0, 0, flansch_t + wickel_l]) {
    cylinder(h = konus_h, d1 = aussen_d, d2 = flansch_d);
    translate([0, 0, konus_h]) cylinder(h = flansch_t, d = flansch_d);
  }
}

difference() {
  koerper();
  // Bohrung für den Ferritstab
  translate([0, 0, -1]) cylinder(h = gesamt_h + 2, d = innen_d);
  // Drahtschlitz unten (Wicklungsanfang)
  translate([aussen_d / 2 - 0.2, -schlitz_b / 2, -1]) cube([flansch_d, schlitz_b, flansch_t + 1.01]);
  // Drahtschlitz oben (Wicklungsende), gegenüber
  translate([-flansch_d - aussen_d / 2 + 0.2, -schlitz_b / 2, flansch_t + wickel_l - 0.01])
    cube([flansch_d, schlitz_b, konus_h + flansch_t + 1.02]);
}
