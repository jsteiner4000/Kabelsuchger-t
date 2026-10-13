// =============================================================================
// Kabelsuchgerät – Sender-Testfirmware für den Machbarkeitstest (Phase 3)
// -----------------------------------------------------------------------------
// Hardware:  ESP32-S2 mini (LOLIN S2 Mini). Der eingebaute D/A-Wandler an GPIO17
//            gibt die Tonsumme aus; danach Tiefpass und Endstufe (siehe
//            docs/machbarkeitstest.md, Abschnitt "Sender-Steckbrett").
//
// Signal:    Summe aus Ortungston fL und Richtungston fL/2 mit fester Phasenlage
//            (Konzept Kap. 4.2). Die Frequenzen ergeben sich exakt aus dem
//            DAC-Takt und der Tabellenlänge:
//              Standard: 200 kHz / 24 = 8333,3 Hz  +  200 kHz / 48 = 4166,7 Hz
//              Tief:      50 kHz / 48 = 1041,7 Hz  +   50 kHz / 96 =  520,8 Hz
//              Hoch:     400 kHz / 12 = 33333 Hz   (ohne Richtungston)
//
// Arduino IDE:  Board "LOLIN S2 Mini" (oder "ESP32S2 Dev Module"),
//               "USB CDC On Boot: Enabled". Erstes Hochladen: Taste "0" halten,
//               "RST" kurz drücken, "0" loslassen, dann hochladen.
//               Serieller Monitor: 115200 Baud.
//
// Bedienung (seriell, Zeichen + Enter) bzw. Taste "0" (GPIO0):
//   1 / 2 / 3   Pegel 25 % / 50 % / 90 %          | Taste "0": Pegel weiterschalten
//   s / t / h   Frequenzsatz Standard / Tief / Hoch
//   c           nur Ortungston (ohne Richtungston), zum Vergleich
//   o           Ausgang aus (Ruhepegel)        ?  Status
// =============================================================================

#include <Arduino.h>
#include <math.h>
#include "driver/dac_continuous.h"
#include "esp_err.h"

constexpr int PIN_LED = 15;                 // blaue LED auf dem S2 mini
constexpr int PIN_BTN = 0;                  // Taste "0"
constexpr float AMP_CD = 0.5f;              // Richtungston relativ zum Ortungston (-6 dB)

struct Satz {
  const char* name;
  uint32_t fsDac;        // DAC-Takt (Hz)
  uint16_t perLen;       // Tabellenlänge einer Periode des tiefsten Tons
  bool cd;               // mit Richtungston?
};
// Standard und Tief: die Tabelle enthält 1 Periode Richtungston und 2 Perioden Ortungston.
// Hoch: die Tabelle enthält 1 Periode des Ortungstons.
const Satz SAETZE[] = {
  {"Standard", 200000, 48, true},
  {"Tief",      50000, 96, true},
  {"Hoch",     400000, 12, false},
};

constexpr size_t TAB_MAX = 1920;            // Bytes; Vielfaches aller Periodenlängen
uint8_t tab[TAB_MAX];
size_t tabLen = 0;

dac_continuous_handle_t dac = nullptr;
void status();
int satz = 0;
int pegel = 1;                              // 0 = 25 %, 1 = 50 %, 2 = 90 %
bool nurOrtung = false;
bool aus = false;
const float PEGEL[] = {0.25f, 0.50f, 0.90f};

// Tabelle berechnen: Spitzenwert = Pegel * 127 um die Mitte 128
void baueTabelle() {
  const Satz& s = SAETZE[satz];
  float v[96];
  float vmax = 0;
  for (int n = 0; n < s.perLen; n++) {
    float x = 2.0f * (float)M_PI * (float)n / (float)s.perLen;
    float y;
    if (s.cd) {
      y = cosf(2.0f * x);                               // Ortungston (2 Perioden)
      if (!nurOrtung) y += AMP_CD * sinf(x);            // Richtungston (1 Periode)
    } else {
      y = cosf(x);
    }
    v[n] = y;
    if (fabsf(y) > vmax) vmax = fabsf(y);
  }
  float k = aus ? 0.0f : PEGEL[pegel] * 127.0f / vmax;
  int reps = TAB_MAX / s.perLen;
  tabLen = (size_t)reps * s.perLen;
  for (size_t i = 0; i < tabLen; i++) {
    int q = (int)lroundf(128.0f + k * v[i % s.perLen]);
    tab[i] = (uint8_t)(q < 0 ? 0 : (q > 255 ? 255 : q));
  }
}

bool check(esp_err_t e, const char* was) {
  if (e != ESP_OK) {
    Serial.printf("FEHLER bei %s: %s\n", was, esp_err_to_name(e));
    return false;
  }
  return true;
}

// DAC (neu) starten. Bei jeder Änderung wird der Kanal komplett neu angelegt.
void starteDac() {
  if (dac) {
    dac_continuous_disable(dac);
    dac_continuous_del_channels(dac);
    dac = nullptr;
  }
  baueTabelle();
  const Satz& s = SAETZE[satz];

  dac_continuous_config_t cfg = {};
  cfg.chan_mask = DAC_CHANNEL_MASK_CH0;     // GPIO17
  cfg.desc_num = 4;
  cfg.buf_size = 2048;                      // 4 x 2048 >= Tabellenlänge
  cfg.freq_hz = s.fsDac;
  cfg.offset = 0;
  cfg.clk_src = DAC_DIGI_CLK_SRC_DEFAULT;
  cfg.chan_mode = DAC_CHANNEL_MODE_SIMUL;

  if (!check(dac_continuous_new_channels(&cfg, &dac), "dac_continuous_new_channels")) return;
  if (!check(dac_continuous_enable(dac), "dac_continuous_enable")) return;
  if (!check(dac_continuous_write_cyclically(dac, tab, tabLen, nullptr), "write_cyclically")) return;
  status();
}

void status() {
  const Satz& s = SAETZE[satz];
  Serial.println();
  if (s.cd) {
    Serial.printf("Satz %s: Ortung %.2f Hz, Richtung %.2f Hz%s\n", s.name,
                  2.0 * s.fsDac / s.perLen, (double)s.fsDac / s.perLen,
                  nurOrtung ? " (aus, nur Ortungston)" : "");
  } else {
    Serial.printf("Satz %s: Ortung %.2f Hz (ohne Richtungston)\n", s.name, (double)s.fsDac / s.perLen);
  }
  Serial.printf("Pegel %.0f %%%s, DAC-Takt %u Hz, Tabelle %u Werte\n",
                PEGEL[pegel] * 100.0f, aus ? " (AUSGANG AUS)" : "", (unsigned)s.fsDac, (unsigned)tabLen);
  Serial.println("Befehle: 1/2/3 Pegel, s/t/h Satz, c nur Ortungston, o aus, ? Status");
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);
  delay(500);
  Serial.println("Kabelsuchgeraet Sender-Test (Phase 3)");
  starteDac();
}

void loop() {
  static uint32_t tLed = 0;
  static bool btnAlt = false;

  while (Serial.available()) {
    char c = (char)Serial.read();
    bool neu = true;
    switch (c) {
      case '1': pegel = 0; aus = false; break;
      case '2': pegel = 1; aus = false; break;
      case '3': pegel = 2; aus = false; break;
      case 's': satz = 0; break;
      case 't': satz = 1; break;
      case 'h': satz = 2; break;
      case 'c': nurOrtung = !nurOrtung; break;
      case 'o': aus = !aus; break;
      case '?': status(); neu = false; break;
      default: neu = false; break;
    }
    if (neu) starteDac();
  }

  bool btn = digitalRead(PIN_BTN) == LOW;
  if (btn && !btnAlt) {
    pegel = (pegel + 1) % 3;
    aus = false;
    starteDac();
  }
  btnAlt = btn;

  // LED: blinkt langsam = läuft, schnell = Ausgang aus
  uint32_t now = millis();
  if (now - tLed > (aus ? 100u : 500u)) {
    tLed = now;
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
  }
  delay(20);
}
