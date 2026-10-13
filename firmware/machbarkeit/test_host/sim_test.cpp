// Simulationstest für lockin.h: erzeugt Spulensignale wie vom PCM1808 und prüft,
// ob Frequenzsuche, Pegel, Rauschschätzung, Links/Rechts und Stromrichtung stimmen.
// Aufruf: ./build.sh  (oder g++ -O2 -std=c++17 sim_test.cpp -o sim_test && ./sim_test)
#include "../empfaenger_test/lockin.h"
#include <stdio.h>
#include <random>

using namespace lockin;

static const double FS = 96000.0;
static const int BLOCK = 1920;
static int fails = 0;

#define CHECK(cond, ...) do { bool ok_ = (cond); printf("  [%s] ", ok_ ? " OK " : "FAIL"); printf(__VA_ARGS__); printf("\n"); if (!ok_) fails++; } while (0)

struct Sig {
  double fL = 8333.333333 + 4.7;   // tatsächliche Sendefrequenz (DAC-Takt leicht daneben)
  double aH = 0.01;                // Ortungston an der H-Spule (ADC-Einheiten, Spitze)
  double aC = 0.004;               // Richtungston
  double phL = 0.7, phC = -1.2;    // Phasen
  double lr = 0.35;                // V/H-Verhältnis (seitlicher Versatz)
  double sign = +1;                // Stromrichtung
  double noise = 0.001;            // weißes Rauschen (Effektivwert) je Kanal
  double hum = 0.0;                // Netzoberwellen 8300 und 8350 Hz
  double drift = 0.0;              // Frequenzdrift Hz/s
  double t = 0;
  std::mt19937 rng{42};
  std::normal_distribution<double> nd{0.0, 1.0};
  void next(float& l, float& r) {
    double th = 2 * M_PI * (fL * t + 0.5 * drift * t * t);   // Momentanfrequenz fL + drift * t
    double h = sign * aH * cos(th + phL);
    double c = sign * aC * cos(th / 2 + phC);
    double hm = hum * (cos(2 * M_PI * 8300 * t) + cos(2 * M_PI * 8350 * t + 1.0));
    l = (float)(h + c + hm + noise * nd(rng));
    r = (float)(lr * h + noise * nd(rng));
    t += 1.0 / FS;
  }
};

// n Blöcke durch Empfänger (und optional Scanner) schicken
static void run(Sig& s, Receiver& rx, Scanner* sc, int blocks) {
  const float* T = sinTab();
  for (int b = 0; b < blocks; b++) {
    for (int i = 0; i < BLOCK; i++) {
      float l, r;
      s.next(l, r);
      rx.sample(l, r, T);
      if (sc) sc->sample(l, T);
    }
    rx.endBlock((float)BLOCK / (float)FS);
    if (sc) sc->endBlock();
  }
}

int main() {
  const double fNom = 200000.0 / 24.0;
  const ScanStage grob{60.0f, 3.0f, 8}, fein{4.0f, 0.4f, 60};

  printf("1) Frequenzsuche (Signal 4,7 Hz neben Nennwert, Netzoberwellen 30 %% des Signals)\n");
  {
    Sig s; s.hum = 0.003;
    Receiver rx; rx.begin(FS, fNom, true); rx.setTau(0.06f);
    Scanner sc; sc.start(fNom, grob, fein, FS);
    int n = 0;
    while (sc.active() && n < 200) { run(s, rx, &sc, 1); n++; }
    double err = sc.result() - s.fL;
    CHECK(sc.done() && fabs(err) < 0.3, "gefunden %.3f Hz, Soll %.3f Hz, Fehler %.3f Hz, Dauer %.2f s, Kontrast %.0f",
          sc.result(), s.fL, err, n * BLOCK / FS, sc.contrast());
  }

  printf("2) Pegel, Rauschen, Links/Rechts nach Frequenzsuche\n");
  Sig s;
  Receiver rx; rx.begin(FS, fNom, true); rx.setTau(0.06f);
  {
    Scanner sc; sc.start(fNom, grob, fein, FS);
    while (sc.active()) run(s, rx, &sc, 1);
    rx.setLocate(sc.result());
    run(s, rx, nullptr, 100);
    const Result& r = rx.result();
    CHECK(fabs(r.hAmp / s.aH - 1) < 0.02, "H-Pegel %.5f, Soll %.5f (%.2f %%)", r.hAmp, s.aH, 100 * (r.hAmp / s.aH - 1));
    CHECK(fabs(r.vAmp / (s.lr * s.aH) - 1) < 0.05, "V-Pegel %.5f, Soll %.5f", r.vAmp, s.lr * s.aH);
    CHECK(fabs(r.lr - s.lr) < 0.03, "Links/Rechts-Verhältnis %.3f, Soll %.3f", r.lr, s.lr);
    CHECK(fabs(r.fL - s.fL) < 0.1, "AFC hält Frequenz: %.3f Hz, Soll %.3f Hz", r.fL, s.fL);
    // Theoretisches Rauschen in der Glättungsbandbreite: Blockzeiger-Leistung 4 sigma^2 / N
    double alpha = (BLOCK / FS) / 0.06;
    double theo = sqrt(4 * s.noise * s.noise / BLOCK * smoothVarFactor((float)alpha));
    CHECK(fabs(20 * log10(r.noise / theo)) < 2.0, "Rauschschätzung %.2e, Theorie %.2e (%.1f dB)", r.noise, theo, 20 * log10(r.noise / theo));
    printf("     SNR %.1f dB\n", 20 * log10(r.hAmp / r.noise));
  }

  printf("3) Stromrichtung: Referenz setzen, dann Strom umdrehen\n");
  {
    const Result& r1 = rx.result();
    float ref = r1.delta;
    int d1 = direction(r1.delta, ref, r1.cdValid);
    CHECK(d1 == 1 && r1.cdValid, "an der Referenz: Richtung %+d (Delta %.0f Grad, CD gültig %d)", d1, r1.delta * 57.3, r1.cdValid);
    s.sign = -1;
    run(s, rx, nullptr, 100);
    const Result& r2 = rx.result();
    int d2 = direction(r2.delta, ref, r2.cdValid);
    CHECK(d2 == -1, "umgedrehter Strom: Richtung %+d (Delta %.0f Grad, Differenz %.0f Grad)", d2, r2.delta * 57.3,
          wrapPi(r2.delta - ref) * 57.3);
    s.sign = +1;
    s.lr = -0.5;
    run(s, rx, nullptr, 100);
    const Result& r3 = rx.result();
    int d3 = direction(r3.delta, ref, r3.cdValid);
    CHECK(d3 == 1, "Strom wieder hin: Richtung %+d", d3);
    CHECK(r3.lr < -0.45 && r3.lr > -0.55, "andere Seite: Links/Rechts %.3f (Soll -0,5)", r3.lr);
  }

  printf("4) Frequenzdrift 0,3 Hz/s über 10 s (AFC muss folgen)\n");
  {
    Sig d; d.drift = 0.3;
    Receiver r; r.begin(FS, d.fL, true); r.setTau(0.06f);
    run(d, r, nullptr, 500);
    double fTrue = d.fL + d.drift * d.t;
    CHECK(fabs(r.result().fL - fTrue) < 0.2 && fabs(r.result().hAmp / d.aH - 1) < 0.05,
          "nach 10 s: fL %.3f Hz, Soll %.3f Hz, Pegel %.1f %%", r.result().fL, fTrue, 100 * r.result().hAmp / d.aH);
  }

  printf("5) Netzoberwelle 8350 Hz gleich stark wie das Signal (8333,3 Hz), ruhige Glättung\n");
  {
    Sig h; h.fL = fNom; h.hum = h.aH; h.noise = 0.0002;
    Receiver r; r.begin(FS, fNom, true); r.setTau(0.3f); r.setAfc(false);
    float mn = 1e9, mx = 0;
    run(h, r, nullptr, 150);
    for (int i = 0; i < 100; i++) { run(h, r, nullptr, 1); float a = r.result().hAmp; if (a < mn) mn = a; if (a > mx) mx = a; }
    CHECK(mx / h.aH < 1.03 && mn / h.aH > 0.97, "Pegel schwankt zwischen %.1f %% und %.1f %%", 100 * mn / h.aH, 100 * mx / h.aH);
  }

  printf("6) Hoch-Satz 33,3 kHz ohne Richtungston\n");
  {
    Sig q; q.fL = 400000.0 / 12.0 + 12.0; q.aC = 0; q.phC = 0;
    Receiver r; r.begin(FS, 400000.0 / 12.0, false); r.setTau(0.06f);
    Scanner sc; sc.start(400000.0 / 12.0, ScanStage{250.0f, 10.0f, 2}, ScanStage{15.0f, 1.0f, 25}, FS);
    // Richtungston im Signal abschalten: q.next erzeugt cos(th/2) mit aC = 0
    while (sc.active()) run(q, r, &sc, 1);
    r.setLocate(sc.result());
    run(q, r, nullptr, 100);
    CHECK(fabs(sc.result() - q.fL) < 0.5 && fabs(r.result().hAmp / q.aH - 1) < 0.03,
          "gefunden %.2f Hz (Soll %.2f), Pegel %.1f %%", sc.result(), q.fL, 100 * r.result().hAmp / q.aH);
  }

  printf("\n%s (%d Fehler)\n", fails ? "NICHT BESTANDEN" : "ALLE TESTS BESTANDEN", fails);
  return fails ? 1 : 0;
}
