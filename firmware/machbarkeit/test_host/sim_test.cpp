// Simulationstest für lockin.h (Variante "Einfach"): bildet die Kette des Handgeräts nach
//   Spule -> Stufe 1 (NIEDRIG) -> Stufe 2 (HOCH) -> 12-bit-Wandler des ESP32-S3
// mit Verstärkerrauschen, Wandlerrauschen, Anschlag an den Rändern und dem Zeitversatz
// zwischen den reihum abgetasteten Kanälen. Die Rohwerte laufen durch denselben Code
// wie im Gerät (lockin::DualRange) und prüfen Frequenzsuche, Pegel, Rauschschätzung,
// Stromrichtung, Bereichswahl und die Empfindlichkeit bei 0,1 nT.
// Aufruf: ./build.sh  (oder g++ -O2 -std=c++17 sim_test.cpp -o sim_test && ./sim_test)
#include "../empfaenger_test/lockin.h"
#include <stdio.h>
#include <random>

using namespace lockin;

// Abtastung: 3 Kanäle reihum. Absichtlich nicht der Sollwert 81 kHz, sondern 83,3 kHz,
// wie er sich bei ungünstiger Teilerrundung im Wandler ergeben kann. Die Firmware misst
// die Rate und rechnet damit; das prüft der Test mit.
static const double FS_GESAMT = 250000.0 * 1.0 / 3.0;   // 83 333 Hz
static const double FS = FS_GESAMT / 3.0;               // je Kanal 27 778 Hz
static const int BLOCK = (int)lround(FS * 0.02);        // 556 Werte = 20 ms
static const double F_NOM = 200000.0 / 24.0;            // 8333,3 Hz

// Analogkette wie im Schaltplan
static const double G1 = 5.7;          // Stufe 1 (NIEDRIG)
static const double G2_NENN = 40.0;    // Stufe 2, Nennwert (Firmware)
static const double G2_ECHT = 38.5;    // Stufe 2, tatsächlich (Toleranz, Verstärkungsabfall)
static const double VFS = 3.1;         // Messbereich des Wandlers (V)
static const double VREF = 1.65;       // Ruhepegel (V)
static const double VDD = 3.3;         // Versorgung der Verstärker (Anschlag)
static const double UV_PRO_NT = 17.3 * F_NOM / 1000.0;   // Spule: ca. 144 µV eff pro nT

static int fails = 0;
#define CHECK(cond, ...) do { bool ok_ = (cond); printf("  [%s] ", ok_ ? " OK " : "FAIL"); printf(__VA_ARGS__); printf("\n"); if (!ok_) fails++; } while (0)

struct Kette {
  double fL = F_NOM + 4.7;     // tatsächliche Sendefrequenz (DAC-Takt leicht daneben)
  double bNT = 10.0;           // Feld des Ortungstons an der Spule (nT eff)
  double cdAnteil = 0.5;       // Richtungston relativ zum Ortungston (-6 dB)
  double phL = 0.7, phC = -1.2;
  double sign = +1;            // Stromrichtung
  double humNT = 0.0;          // Netzoberwellen 8300 Hz und 8350 Hz (nT eff, je Ton)
  double drift = 0.0;          // Frequenzdrift Hz/s
  double enIn = 30e-9;         // Eingangsrauschen Verstärker (V/Wurzel Hz)
  double adcLsb = 3.0;         // Wandlerrauschen (LSB eff)
  double t = 0;
  std::mt19937 rng{42};
  std::normal_distribution<double> nd{0.0, 1.0};

  // Spannung an der Spule (V) zum Zeitpunkt tt
  double spule(double tt) {
    double th = 2 * M_PI * (fL * tt + 0.5 * drift * tt * tt);
    double a = bNT * UV_PRO_NT * 1e-6 * M_SQRT2;          // Spitzenwert
    double v = sign * a * (cos(th + phL) + cdAnteil * cos(th / 2 + phC));
    double h = humNT * UV_PRO_NT * 1e-6 * M_SQRT2;
    v += h * (cos(2 * M_PI * 8300 * tt) + cos(2 * M_PI * 8350 * tt + 1.0));
    return v;
  }
  int wandle(double v) {
    if (v < 0) v = 0;
    if (v > VDD) v = VDD;                                 // Verstärker am Anschlag
    double r = v / VFS * 4095.0 + adcLsb * nd(rng);
    if (r < 0) r = 0;
    if (r > 4095) r = 4095;                               // Wandler am Anschlag
    return (int)lround(r);
  }
  void next(int& lo, int& hi) {
    double sigIn = enIn * sqrt(FS_GESAMT / 2.0) * nd(rng); // Verstärkerrauschen in der Bandbreite
    double vLo = VREF + G1 * (spule(t) + sigIn);
    double vHi = VREF + G1 * G2_ECHT * (spule(t + 1.0 / FS_GESAMT) + sigIn);   // eine Wandlung später
    lo = wandle(vLo);
    hi = wandle(vHi);
    t += 1.0 / FS;
  }
  // Erwarteter Betrag des Ortungstons in HOCH-Einheiten (1 = halber Messbereich)
  double sollHoch() const { return bNT * UV_PRO_NT * 1e-6 * M_SQRT2 * G1 * G2_ECHT / VFS * 4095.0 / 2048.0; }
};

static void run(Kette& k, DualRange& d, Scanner* sc, int blocks) {
  const float* T = sinTab();
  int b = 0;
  while (b < blocks) {
    int lo, hi;
    k.next(lo, hi);
    if (d.sample(lo, hi, T, sc)) b++;
  }
}

static DualRange neu(double f0, float tau) {
  DualRange d;
  d.begin(FS, BLOCK, f0, (float)G2_NENN);
  d.setTau(tau);
  return d;
}

int main() {
  const ScanStage grob{60.0f, 3.0f, 8}, fein{4.0f, 0.4f, 60};
  printf("Abtastrate je Kanal %.1f Hz, Block %d Werte, Spule %.0f uV/nT\n\n", FS, BLOCK, UV_PRO_NT);

  printf("1) Frequenzsuche (Signal 4,7 Hz neben Nennwert, Netzoberwellen 30 %% des Signals)\n");
  {
    Kette k; k.bNT = 5; k.humNT = 1.5;
    DualRange d = neu(F_NOM, 0.06f);
    Scanner sc; sc.start(F_NOM, grob, fein, FS);
    int n = 0;
    while (sc.active() && n < 200) { run(k, d, &sc, 1); n++; }
    double err = sc.result() - k.fL;
    CHECK(sc.done() && fabs(err) < 0.3, "gefunden %.3f Hz, Soll %.3f Hz, Fehler %.3f Hz, Dauer %.2f s, Kontrast %.0f",
          sc.result(), k.fL, err, n * BLOCK / FS, sc.contrast());
  }

  printf("2) Pegel und Rauschen nach der Frequenzsuche (10 nT)\n");
  Kette k;
  DualRange d = neu(F_NOM, 0.06f);
  {
    Scanner sc; sc.start(F_NOM, grob, fein, FS);
    while (sc.active()) run(k, d, &sc, 1);
    d.setLocate(sc.result());
    run(k, d, nullptr, 100);
    const Result& r = d.result();
    CHECK(!d.low(), "Bereich HOCH aktiv (Aussteuerung %.2f)", d.peakHi());
    CHECK(fabs(d.level() / k.sollHoch() - 1) < 0.03, "Pegel %.4f, Soll %.4f (%+.1f %%)", d.level(), k.sollHoch(),
          100 * (d.level() / k.sollHoch() - 1));
    CHECK(fabs(r.fL - k.fL) < 0.1, "AFC hält Frequenz: %.3f Hz, Soll %.3f Hz", r.fL, k.fL);
    printf("     SNR %.1f dB bei 10 nT (Anzeige sättigt bei ca. 60 dB, siehe NoiseEst)\n", 20 * log10(d.level() / d.noise()));
  }
  {
    // Rauschschätzung bei schwachem Signal (0,5 nT) gegen die Theorie:
    // Rauschen in HOCH-Einheiten je Abtastwert = Wandler + Verstärker
    Kette w; w.fL = F_NOM; w.bNT = 0.5;
    DualRange r = neu(F_NOM, 0.06f);
    run(w, r, nullptr, 200);
    double sAdc = w.adcLsb / 2048.0;
    double sAmp = w.enIn * sqrt(FS_GESAMT / 2.0) * G1 * G2_ECHT / VFS * 4095.0 / 2048.0;
    double sig = sqrt(sAdc * sAdc + sAmp * sAmp);
    double alpha = (BLOCK / FS) / 0.06;
    double theo = sqrt(4 * sig * sig / BLOCK * smoothVarFactor((float)alpha));
    CHECK(fabs(20 * log10(r.noise() / theo)) < 2.0, "Rauschschätzung bei 0,5 nT %.2e, Theorie %.2e (%.1f dB)", r.noise(),
          theo, 20 * log10(r.noise() / theo));
  }

  printf("3) Stromrichtung: Referenz setzen, dann Strom umdrehen\n");
  float ref = 0;
  {
    const Result& r1 = d.result();
    ref = r1.delta;
    int d1 = direction(r1.delta, ref, r1.cdValid);
    CHECK(d1 == 1 && r1.cdValid, "an der Referenz: Richtung %+d (Delta %.0f Grad)", d1, r1.delta * 57.3);
    k.sign = -1;
    run(k, d, nullptr, 100);
    const Result& r2 = d.result();
    int d2 = direction(r2.delta, ref, r2.cdValid);
    CHECK(d2 == -1, "umgedrehter Strom: Richtung %+d (Differenz %.0f Grad)", d2, wrapPi(r2.delta - ref) * 57.3);
    k.sign = +1;
    run(k, d, nullptr, 100);
    int d3 = direction(d.result().delta, ref, d.result().cdValid);
    CHECK(d3 == 1, "Strom wieder hin: Richtung %+d", d3);
  }

  printf("4) Bereichswahl: Feld 10 nT -> 300 nT -> 10 nT (HOCH übersteuert bei 300 nT)\n");
  {
    // Verhältnis HOCH/NIEDRIG lernt sich bei sauberem Signal nach (Nennwert 40, echt 38,5)
    run(k, d, nullptr, 250);
    CHECK(fabs(d.ratio() / G2_ECHT - 1) < 0.01, "gelerntes Verhältnis %.2f, echt %.2f", d.ratio(), G2_ECHT);
    k.bNT = 300;
    run(k, d, nullptr, 50);
    CHECK(d.low(), "bei 300 nT auf NIEDRIG umgeschaltet (Aussteuerung HOCH %.2f, NIEDRIG %.2f)", d.peakHi(), d.peakLo());
    CHECK(fabs(d.level() / k.sollHoch() - 1) < 0.03, "Pegel %.3f, Soll %.3f (%+.1f %%)", d.level(), k.sollHoch(),
          100 * (d.level() / k.sollHoch() - 1));
    int dd = direction(d.result().delta, ref, d.result().cdValid);
    CHECK(dd == 1, "Stromrichtung auch im Bereich NIEDRIG richtig: %+d", dd);
    k.bNT = 10;
    run(k, d, nullptr, 60);
    CHECK(!d.low(), "zurück auf HOCH nach Abklingen");
    CHECK(fabs(d.level() / k.sollHoch() - 1) < 0.03, "Pegel %.4f, Soll %.4f", d.level(), k.sollHoch());
  }

  printf("5) Frequenzdrift 0,3 Hz/s über 10 s (AFC muss folgen)\n");
  {
    Kette q; q.drift = 0.3;
    DualRange r = neu(q.fL, 0.06f);
    run(q, r, nullptr, 500);
    double fTrue = q.fL + q.drift * q.t;
    CHECK(fabs(r.result().fL - fTrue) < 0.2 && fabs(r.level() / q.sollHoch() - 1) < 0.05,
          "nach 10 s: fL %.3f Hz, Soll %.3f Hz, Pegel %.1f %%", r.result().fL, fTrue, 100 * r.level() / q.sollHoch());
  }

  printf("6) Netzoberwellen 8300/8350 Hz gleich stark wie das Signal, ruhige Glättung\n");
  {
    Kette h; h.fL = F_NOM; h.bNT = 5; h.humNT = 5;
    DualRange r = neu(F_NOM, 0.3f);
    r.setAfc(false);
    float mn = 1e9, mx = 0;
    run(h, r, nullptr, 150);
    for (int i = 0; i < 100; i++) {
      run(h, r, nullptr, 1);
      float a = r.level();
      if (a < mn) mn = a;
      if (a > mx) mx = a;
    }
    CHECK(mx / h.sollHoch() < 1.03 && mn / h.sollHoch() > 0.97, "Pegel schwankt zwischen %.1f %% und %.1f %%",
          100 * mn / h.sollHoch(), 100 * mx / h.sollHoch());
  }

  printf("7) Empfindlichkeit: 0,1 nT mit Wandler- und Verstärkerrauschen, ruhige Glättung\n");
  {
    Kette e; e.fL = F_NOM; e.bNT = 10;
    DualRange r = neu(F_NOM, 0.3f);
    run(e, r, nullptr, 100);
    float ref7 = r.result().delta;
    e.bNT = 0.1;
    run(e, r, nullptr, 200);
    double snr = 20 * log10(r.level() / r.noise());
    CHECK(snr > 20 && fabs(r.level() / e.sollHoch() - 1) < 0.1, "SNR %.1f dB, Pegel %.1f %% vom Sollwert", snr,
          100 * r.level() / e.sollHoch());
    int dd = direction(r.result().delta, ref7, r.result().cdValid);
    CHECK(r.result().cdValid && dd == 1, "Richtungston gültig und Richtung %+d", dd);
    printf("     Rauschgrenze (SNR 0 dB): ca. %.4f nT\n", 0.1 * r.noise() / r.level());
  }

  printf("\n%s (%d Fehler)\n", fails ? "NICHT BESTANDEN" : "ALLE TESTS BESTANDEN", fails);
  return fails ? 1 : 0;
}
