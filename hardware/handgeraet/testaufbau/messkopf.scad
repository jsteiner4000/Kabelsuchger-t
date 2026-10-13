// =============================================================================
// Messkopf für den Machbarkeitstest (Phase 3)
// -----------------------------------------------------------------------------
// Hält die H-Spule waagerecht unter einer Platte (Kabelbinder) und nimmt oben
// ein Elektro-Isolierrohr M20 als Stiel auf. Die V-Spule (Flansch 15,5 mm)
// steckt senkrecht im unteren Ende des Rohrs; ihr Ferritstab steht auf der
// Platte. Die Drähte der H-Spule laufen durch das Loch neben der Hülse nach
// oben und werden außen am Rohr festgeklebt.
// Druck: PETG, Platte flach auf dem Druckbett, kein Stützmaterial nötig.
// =============================================================================

platte_l = 110;
platte_b = 36;
platte_h = 4;
rohr_d   = 20.4;   // Außendurchmesser M20-Rohr + Spiel
huelse_h = 22;
huelse_wand = 3;
loch_d   = 5;      // Durchführung für die Drähte der H-Spule
loch_x   = 20;     // neben der Hülse
binder_b = 4;      // Kabelbinder-Schlitz
binder_l = 2.2;

$fn = 96;

difference() {
  union() {
    translate([-platte_l / 2, -platte_b / 2, 0]) cube([platte_l, platte_b, platte_h]);
    cylinder(h = platte_h + huelse_h, d = rohr_d + 2 * huelse_wand);
  }
  // Aufnahme für das Rohr
  translate([0, 0, platte_h]) cylinder(h = huelse_h + 1, d = rohr_d);
  // Drahtdurchführung neben der Hülse
  translate([loch_x, 0, -1]) cylinder(h = platte_h + 2, d = loch_d);
  // Kabelbinder-Schlitze für die H-Spule (je zwei links und rechts)
  for (x = [-45, -30, 30, 45])
    for (y = [-1, 1])
      translate([x - binder_l / 2, y * 10 - binder_b / 2, -1]) cube([binder_l, binder_b, platte_h + 2]);
}
