// =============================================================================
// lockin.h – Digitale Lock-in-Auswertung (IQ-Demodulation) für das Kabelsuchgerät
// -----------------------------------------------------------------------------
// Reines C++ ohne Arduino-Abhängigkeiten, damit es auch auf dem PC mit
// simulierten Signalen getestet werden kann (siehe ../test_host/).
//
// Prinzip (Konzept Kap. 5.3):
//   Das Eingangssignal x(t) wird mit cos und sin der Sendefrequenz multipliziert
//   und über einen Block gemittelt. Übrig bleibt ein komplexer Zeiger z:
//     |z|    = Amplitude des Tons (Spitzenwert, in Einheiten des ADC-Vollausschlags)
//     arg(z) = Phase des Tons relativ zum internen Oszillator
//   Alles, was nicht genau auf der Frequenz liegt (50 Hz, Schaltnetzteile ...),
//   mittelt sich weg. Die Bandbreite stellt man über die Glättung ein.
// =============================================================================
#pragma once
#include <stdint.h>
#include <math.h>

namespace lockin {

// ----------------------------------------------------------------- Sinustabelle
constexpr int SIN_BITS = 12;                 // 4096 Stützstellen
constexpr int SIN_N = 1 << SIN_BITS;
constexpr int SIN_MASK = SIN_N - 1;
constexpr int COS_OFS = SIN_N / 4;           // cos(x) = sin(x + 90°)

struct SinTab {
  float v[SIN_N];
  SinTab() {
    for (int i = 0; i < SIN_N; i++) v[i] = sinf(2.0f * (float)M_PI * (float)i / (float)SIN_N);
  }
};
inline const float* sinTab() {
  static const SinTab t;                     // wird beim ersten Aufruf einmal gefüllt
  return t.v;
}

// ----------------------------------------------------------------- komplexe Zahl
struct Cpx {
  float re = 0, im = 0;
  float mag() const { return sqrtf(re * re + im * im); }
  float arg() const { return atan2f(im, re); }
};
inline Cpx mulConj(const Cpx& a, const Cpx& b) {   // a * konj(b)
  return {a.re * b.re + a.im * b.im, a.im * b.re - a.re * b.im};
}
inline float wrapPi(float p) {                       // Winkel auf -pi..pi
  while (p > (float)M_PI) p -= 2.0f * (float)M_PI;
  while (p < -(float)M_PI) p += 2.0f * (float)M_PI;
  return p;
}
inline float dB(float x) { return 20.0f * log10f(x > 1e-12f ? x : 1e-12f); }

// ----------------------------------------------------------------- Oszillator + Mischer
// Phase als 32-bit-Zähler: 2^32 entspricht einer vollen Schwingung.
// Dadurch läuft die Frequenz ohne Rundungsdrift beliebig lange.
struct Tone {
  uint32_t ph = 0, inc = 0;
  float accI = 0, accQ = 0;
  int n = 0;

  static uint32_t incFor(double f, double fs) {
    return (uint32_t)llround(f / fs * 4294967296.0);
  }
  void setInc(uint32_t i) { inc = i; }
  double freq(double fs) const { return (double)inc * fs / 4294967296.0; }

  inline void step(float x, const float* T) {
    uint32_t idx = ph >> (32 - SIN_BITS);
    accI += x * T[(idx + COS_OFS) & SIN_MASK];   // x * cos
    accQ += x * T[idx];                          // x * sin
    ph += inc;
    n++;
  }
  // Blockende: Zeiger z = A * e^{j phi} für x = A*cos(wt + phi), Akku leeren.
  Cpx take() {
    Cpx z;
    if (n > 0) {
      z.re = 2.0f * accI / (float)n;
      z.im = -2.0f * accQ / (float)n;
    }
    accI = accQ = 0;
    n = 0;
    return z;
  }
};

// ----------------------------------------------------------------- Glättung
// Zwei hintereinandergeschaltete IIR-Tiefpässe erster Ordnung auf dem komplexen
// Zeiger (zusammen 2. Ordnung). alpha = Blockdauer / Zeitkonstante.
// Zwei Stufen trennen benachbarte Störtöne (z. B. eine Netzoberwelle 16,7 Hz
// neben der Sendefrequenz) doppelt so stark (in dB) wie eine Stufe.
struct Smooth {
  Cpx a, z;
  bool init = false;
  void add(const Cpx& x, float alpha) {
    if (!init) { a = z = x; init = true; return; }
    a.re += alpha * (x.re - a.re);
    a.im += alpha * (x.im - a.im);
    z.re += alpha * (a.re - z.re);
    z.im += alpha * (a.im - z.im);
  }
  void reset() { init = false; a = z = Cpx{}; }
};

// Varianzreduktion der zweistufigen Glättung für weißes Rauschen:
// Impulsantwort h[n] = alpha^2 (n+1) r^n mit r = 1 - alpha
// -> Summe h^2 = alpha^4 (1 + r^2) / (1 - r^2)^3
inline float smoothVarFactor(float alpha) {
  float r2 = (1.0f - alpha) * (1.0f - alpha);
  float d = 1.0f - r2;
  return alpha * alpha * alpha * alpha * (1.0f + r2) / (d * d * d);
}

// Rauschschätzung: mittlere Leistung der Blockzeiger auf einer leeren Frequenz.
// Daraus wird das Rauschen in der geglätteten Bandbreite berechnet.
struct NoiseEst {
  float pow = 0;
  bool init = false;
  void add(const Cpx& x, float beta = 0.05f) {
    float p = x.re * x.re + x.im * x.im;
    if (!init) { pow = p; init = true; return; }
    pow += beta * (p - pow);
  }
  // Effektivwert des Rauschens nach der Glättung mit alpha (weißes Rauschen angenommen)
  float afterSmooth(float alpha) const { return sqrtf(pow * smoothVarFactor(alpha)); }
};

// ----------------------------------------------------------------- Frequenzsuche
// Sucht den stärksten Ton in einem Fenster um f0. Zwei Stufen:
// grob (breites Fenster, kurze Messzeit) und fein (schmales Fenster, lange Messzeit).
// Schrittweite <= halbe Breite des Hauptmaximums, damit kein Ton durchrutscht.
struct ScanStage {
  float span;      // +/- Hz um die Mitte
  float step;      // Hz zwischen zwei Prüffrequenzen
  int blocks;      // Messzeit in Blöcken
};

class Scanner {
 public:
  static constexpr int MAXT = 64;

  void start(double fCenter, const ScanStage& coarse, const ScanStage& fine, double fs) {
    fs_ = fs; coarse_ = coarse; fine_ = fine;
    stage_ = 0;
    setup(fCenter, coarse_);
    active_ = true; done_ = false;
  }
  bool active() const { return active_; }
  bool done() const { return done_; }
  void reset() { active_ = false; done_ = false; }
  double result() const { return fBest_; }
  float peakAmp() const { return aBest_; }
  float contrast() const { return contrast_; }   // Spitze / Median aller Prüffrequenzen

  // Ein Abtastwert des Kanals, auf dem gesucht wird
  inline void sample(float x, const float* T) {
    if (!active_) return;
    for (int k = 0; k < nt_; k++) tones_[k].step(x, T);
  }
  // Blockende
  void endBlock() {
    if (!active_) return;
    for (int k = 0; k < nt_; k++) {
      Cpx z = tones_[k].take();
      sumI_[k] += z.re; sumQ_[k] += z.im;
    }
    if (++blk_ < cur_.blocks) return;

    // Auswertung der Stufe
    if (nt_ <= 0) { active_ = false; return; }
    int kb = 0;
    float amp[MAXT] = {};
    for (int k = 0; k < nt_; k++) {
      amp[k] = sqrtf(sumI_[k] * sumI_[k] + sumQ_[k] * sumQ_[k]) / (float)blk_;
      if (amp[k] > amp[kb]) kb = k;
    }
    double f = f0_ + kb * cur_.step;
    if (kb > 0 && kb < nt_ - 1) {             // Parabel durch die drei höchsten Punkte
      float a = amp[kb - 1], b = amp[kb], c = amp[kb + 1];
      float den = a - 2 * b + c;
      if (fabsf(den) > 1e-20f) {
        float d = 0.5f * (a - c) / den;
        if (d > -1 && d < 1) f += d * cur_.step;
      }
    }
    // Kontrast: Spitze gegen Median (Maß für "das ist wirklich ein Ton")
    float tmp[MAXT];
    for (int k = 0; k < nt_; k++) tmp[k] = amp[k];
    for (int i = 1; i < nt_; i++) {           // kleines Insertion-Sort
      float v = tmp[i]; int j = i - 1;
      while (j >= 0 && tmp[j] > v) { tmp[j + 1] = tmp[j]; j--; }
      tmp[j + 1] = v;
    }
    float med = tmp[nt_ / 2];
    contrast_ = med > 0 ? amp[kb] / med : 0;
    aBest_ = amp[kb];
    fBest_ = f;

    if (stage_ == 0) {
      stage_ = 1;
      setup(f, fine_);
    } else {
      active_ = false;
      done_ = true;
    }
  }

 private:
  void setup(double fc, const ScanStage& s) {
    cur_ = s;
    nt_ = (int)(2 * s.span / s.step) + 1;
    if (nt_ > MAXT) nt_ = MAXT;
    f0_ = fc - s.step * (nt_ - 1) / 2.0;
    for (int k = 0; k < nt_; k++) {
      tones_[k] = Tone();
      tones_[k].setInc(Tone::incFor(f0_ + k * s.step, fs_));
      sumI_[k] = sumQ_[k] = 0;
    }
    blk_ = 0;
  }

  Tone tones_[MAXT];
  float sumI_[MAXT], sumQ_[MAXT];
  int nt_ = 0, blk_ = 0, stage_ = 0;
  double f0_ = 0, fs_ = 96000, fBest_ = 0;
  float aBest_ = 0, contrast_ = 0;
  ScanStage coarse_{}, fine_{}, cur_{};
  bool active_ = false, done_ = false;
};

// ----------------------------------------------------------------- Empfänger-Kern
// Zwei Kanäle (L = H-Spule, R = V-Spule oder E-Spitze), Ortungston fL,
// Richtungston fL/2 (nur L), ein Rauschfühler neben fL (nur L).
struct Result {
  float hAmp = 0, vAmp = 0, cdAmp = 0;   // Spitzenwerte in ADC-Vollausschlag-Einheiten
  float noise = 0;                        // Rauscheffektivwert in der Glättungsbandbreite
  float delta = 0;                        // Richtungswinkel phi_L - 2*phi_CD (rad)
  float lr = 0;                           // vorzeichenbehaftetes V/H-Verhältnis (in Phase)
  double fL = 0;                          // aktuelle Ortungsfrequenz (Hz)
  bool cdValid = false;
  uint32_t blocks = 0;
};

class Receiver {
 public:
  void begin(double fs, double fLocate, bool withCD) {
    fs_ = fs;
    withCD_ = withCD;
    setLocate(fLocate);
    sH_.reset(); sV_.reset(); sCD_.reset();
    lastH_ = Cpx{};
    haveLast_ = false;
  }
  // Ortungsfrequenz setzen; der Richtungston liegt exakt bei der Hälfte.
  void setLocate(double f) {
    uint32_t inc = Tone::incFor(f, fs_) & ~1u;   // gerade -> Hälfte ist exakt
    tH_.setInc(inc); tV_.setInc(inc);
    tCD_.setInc(inc >> 1);
    tN_.setInc(Tone::incFor(f + NOISE_OFS_HZ, fs_));
    fL_ = tH_.freq(fs_);
  }
  double locate() const { return fL_; }
  void setTau(float tauSec) { tau_ = tauSec; }
  void setAfc(bool on) { afc_ = on; }

  // Ein Abtastpaar (Werte im Bereich -1..+1)
  inline void sample(float l, float r, const float* T) {
    tH_.step(l, T);
    tN_.step(l, T);
    if (withCD_) tCD_.step(l, T);
    tV_.step(r, T);
  }

  // Blockende, blockSec = Dauer eines Blocks in Sekunden
  void endBlock(float blockSec) {
    Cpx zH = tH_.take(), zV = tV_.take(), zN = tN_.take();
    Cpx zC = withCD_ ? tCD_.take() : Cpx{};
    float alpha = blockSec / tau_;
    if (alpha > 1) alpha = 1;

    noise_.add(zN);
    float nBlock = sqrtf(noise_.pow);

    // Nachführung der Frequenz (AFC): Phasenänderung des H-Zeigers pro Block
    if (afc_ && haveLast_ && zH.mag() > 10.0f * nBlock) {
      float dphi = wrapPi(mulConj(zH, lastH_).arg());
      double fErr = dphi / (2.0 * M_PI * blockSec);
      if (fErr > 3.0) fErr = 3.0;
      if (fErr < -3.0) fErr = -3.0;
      setLocate(fL_ + 0.2 * fErr);
    }
    lastH_ = zH;
    haveLast_ = true;

    sH_.add(zH, alpha);
    sV_.add(zV, alpha);
    if (withCD_) sCD_.add(zC, alpha);

    res_.hAmp = sH_.z.mag();
    res_.vAmp = sV_.z.mag();
    res_.noise = noise_.afterSmooth(alpha);
    float h2 = res_.hAmp * res_.hAmp;
    res_.lr = h2 > 0 ? mulConj(sV_.z, sH_.z).re / h2 : 0;
    if (withCD_) {
      res_.cdAmp = sCD_.z.mag();
      res_.delta = wrapPi(sH_.z.arg() - 2.0f * sCD_.z.arg());
      res_.cdValid = res_.cdAmp > 3.0f * res_.noise;   // ca. 10 dB Abstand
    } else {
      res_.cdAmp = 0;
      res_.cdValid = false;
    }
    res_.fL = fL_;
    res_.blocks++;
  }
  const Result& result() const { return res_; }

  // Leere Frequenz neben fL für die Rauschmessung. 100 Hz liegt auf einer Nullstelle
  // des 20-ms-Blockfilters (Vielfache von 50 Hz), dort sickert das eigene Signal nicht ein.
  static constexpr double NOISE_OFS_HZ = 100.0;

 private:
  Tone tH_, tV_, tCD_, tN_;
  Smooth sH_, sV_, sCD_;
  NoiseEst noise_;
  Cpx lastH_;
  bool haveLast_ = false, withCD_ = true, afc_ = true;
  double fs_ = 96000, fL_ = 0;
  float tau_ = 0.15f;
  Result res_;
};

// Richtungsentscheidung aus delta und Referenz: +1 HIN, -1 ZURÜCK, 0 unklar
inline int direction(float delta, float ref, bool valid) {
  if (!valid) return 0;
  float c = cosf(wrapPi(delta - ref));
  if (c > 0.5f) return 1;      // innerhalb +/-60° der Referenz
  if (c < -0.5f) return -1;    // innerhalb +/-60° der Gegenrichtung
  return 0;
}

}  // namespace lockin
