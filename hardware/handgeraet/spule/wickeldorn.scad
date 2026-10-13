// =============================================================================
// Wickeldorn: hält den Spulenkörper im Akkuschrauber
// -----------------------------------------------------------------------------
// Unten ein 1/4"-Sechskantschaft für das Bohrfutter, darüber eine Schulter als
// Anschlag, oben der Dorn, auf den der Spulenkörper stramm aufgeschoben wird.
// Druck: PETG, stehend (Sechskant unten), Brim an, 100 % Füllung für den Schaft.
// =============================================================================

dorn_d    = 10.25;  // etwas kleiner als die Bohrung des Spulenkörpers (10,4)
dorn_l    = 60;     // länger als der längste Spulenkörper
schulter_d = 16;
schulter_h = 4;
sechskant_sw = 6.35; // Schlüsselweite 1/4"
schaft_l  = 25;

$fn = 96;

// Sechskantschaft
cylinder(h = schaft_l, d = sechskant_sw / cos(30), $fn = 6);
// Schulter
translate([0, 0, schaft_l]) cylinder(h = schulter_h, d = schulter_d);
// Dorn mit leichter Fase oben
translate([0, 0, schaft_l + schulter_h]) {
  cylinder(h = dorn_l - 1, d = dorn_d);
  translate([0, 0, dorn_l - 1]) cylinder(h = 1, d1 = dorn_d, d2 = dorn_d - 1);
}
