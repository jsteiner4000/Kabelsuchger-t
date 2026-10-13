// =============================================================================
// Kabelsuchgerät – Sender-Testfirmware für den Mini-Test (Phase 3, Variante "Einfach")
// -----------------------------------------------------------------------------
// Hardware:  ESP32-S2 mini (LOLIN S2 Mini). Der eingebaute D/A-Wandler an GPIO18
//            gibt die Tonsumme aus; danach Tiefpass und Brücken-Endstufe an 24 V
//            (Schaltplan: hardware/sender/testaufbau/). OLED 1,3" (SH1106, I²C),
//            Strommessung über einen 10-Ω-Messwiderstand, eine Taste PEGEL.
// Anleitung: docs/machbarkeitstest.md
//
// Signal:    Summe aus Ortungston fL und Richtungston fL/2 mit fester Phasenlage
//            (Konzept Kap. 4.2), exakt aus dem DAC-Takt und der Tabellenlänge:
//              Standard:  200 kHz / 24 = 8333,3 Hz  +  200 kHz / 48 = 4166,7 Hz
//              Prüfton:    50 kHz / 48 = 1041,7 Hz, reiner Sinus (nur zum Nachmessen
//                          mit dem Multimeter, das bei 8 kHz meist falsch anzeigt)
//
// Strom:     Durch den Messwiderstand fließt der Betrag des Ausgangsstroms (er liegt
//            im Rückweg der beiden PNP-Transistoren). Ein RC-Glied glättet; die
//            Firmware rechnet den Mittelwert über die Kurvenform in den
//            Effektivwert um. Genauigkeit ca. ±10 %.
//
// Arduino IDE:  Boardverwalter "esp32" von Espressif, Version 3.x
//               Board "LOLIN S2 Mini" (oder "ESP32S2 Dev Module"),
//               "USB CDC On Boot: Enabled", Bibliothek "U8g2" (von olikraus).
//               Erstes Hochladen: Taste "0" halten, "RST" kurz drücken, "0" loslassen.
//               Serieller Monitor: 115200 Baud.
//
// Taste PEGEL (oder Taste "0" auf dem Board):
//   kurz = Pegel FEIN -> NORMAL -> MAX     lang = Ausgang aus / ein
// Serielle Befehle (Zeichen + Enter):
//   1 / 2 / 3   Pegel FEIN / NORMAL / MAX     o  Ausgang aus/ein
//   c           nur Ortungston (ohne Richtungston), zum Vergleich
//   p           Prüfton 1 kHz an/aus          ?  Status
// =============================================================================

#include <Arduino.h>
#include <math.h>
#include "driver/dac_continuous.h"
#include "esp_err.h"

#define USE_OLED 1          // 0 = ohne Display
#if USE_OLED
#include <U8g2lib.h>
#endif

// ------------------------------------------------------------- Pins (LOLIN S2 mini)
// GPIO18 = D/A-Wandler Kanal 2 (fest). Er liegt auf der äußeren Stiftreihe; damit reichen
// zwei Stiftleisten am Modul (Kanal 1 an GPIO17 läge auf der inneren Reihe).
constexpr int PIN_STROM = 3;               // Spannung am Messwiderstand (über RC-Glied)
constexpr int PIN_BAT   = 5;               // Batterie über 100 kΩ / 47 kΩ
constexpr int PIN_TASTE = 7;               // Taste PEGEL gegen GND
constexpr int PIN_BOOT  = 0;               // Taste "0" auf dem Board (wie PEGEL)
constexpr int PIN_LED   = 15;              // blaue LED auf dem Board
constexpr int PIN_SDA   = 33;              // OLED
constexpr int PIN_SCL   = 35;              // OLED

// ------------------------------------------------------------- Einstellungen
constexpr float R_MESS = 10.0f;            // Messwiderstand im PNP-Rückweg (Ω)
constexpr float BAT_TEILER = (100.0f + 47.0f) / 47.0f;
constexpr float I_MAX_MA = 60.0f;          // darüber: Pegel automatisch zurücknehmen
constexpr float I_WENIG_MA = 0.5f;         // darunter: Hinweis "kaum Strom"
constexpr float AMP_CD = 0.5f;             // Richtungston relativ zum Ortungston (-6 dB)

struct Satz {
  const char* name;
  uint32_t fsDac;        // DAC-Takt (Hz)
  uint16_t perLen;       // Tabellenlänge einer Periode des tiefsten Tons
  bool cd;               // mit Richtungston?
};
// Standard: die Tabelle enthält 1 Periode Richtungston und 2 Perioden Ortungston.
// Prüfton:  die Tabelle enthält 1 Periode Sinus.
const Satz SAETZE[] = {
  {"Standard", 200000, 48, true},
  {"Pruefton",  50000, 48, false},
};

const char* const PEGEL_NAME[] = {"FEIN", "NORMAL", "MAX"};
const float PEGEL[] = {0.25f, 0.50f, 0.90f};   // Anteil am DAC-Vollausschlag
// Spitzenspannung an A-E ca. 5 V / 9 V / 17 V (D/A-Wandler 3,3 V, Tiefpass, Brücke 2 x 6,45)

constexpr size_t TAB_MAX = 1920;           // Bytes; Vielfaches der Periodenlängen
uint8_t tab[TAB_MAX];
size_t tabLen = 0;

dac_continuous_handle_t dac = nullptr;
#if USE_OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE, PIN_SCL, PIN_SDA);
#endif

int satz = 0;
int pegel = 1;
bool nurOrtung = false;
bool aus = false;
float formFaktor = 0.9f;                   // Mittelwert |i| / Effektivwert, aus der Tabelle
float nullMv = 0;                          // Messwert bei Ausgang aus (Nullpunkt)
float stromMa = 0, batV = 0;
const char* warnung = nullptr;
uint32_t warnBis = 0;

// ------------------------------------------------------------- Signal
// Tabelle berechnen: Spitzenwert = Pegel * 127 um die Mitte 128
void baueTabelle() {
  const Satz& s = SAETZE[satz];
  float v[96];
  float vmax = 0, sAbs = 0, sQ = 0;
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
    sAbs += fabsf(y);
    sQ += y * y;
  }
  formFaktor = (sAbs / s.perLen) / sqrtf(sQ / s.perLen);
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

void status();

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
  cfg.chan_mask = DAC_CHANNEL_MASK_CH1;     // GPIO18
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

double fOrtung() {
  const Satz& s = SAETZE[satz];
  return s.cd ? 2.0 * s.fsDac / s.perLen : (double)s.fsDac / s.perLen;
}

void status() {
  const Satz& s = SAETZE[satz];
  Serial.println();
  if (s.cd) {
    Serial.printf("Satz %s: Ortung %.2f Hz, Richtung %.2f Hz%s\n", s.name, fOrtung(), fOrtung() / 2,
                  nurOrtung ? " (aus, nur Ortungston)" : "");
  } else {
    Serial.printf("Satz %s: %.2f Hz, reiner Sinus\n", s.name, fOrtung());
  }
  Serial.printf("Pegel %s (%.0f %%)%s, Formfaktor %.3f\n", PEGEL_NAME[pegel], PEGEL[pegel] * 100.0f,
                aus ? " - AUSGANG AUS" : "", formFaktor);
  Serial.println("Befehle: 1/2/3 Pegel, o aus/ein, c nur Ortungston, p Pruefton, ? Status");
}

// ------------------------------------------------------------- Messen
float mittelMv(int pin, int n) {
  uint32_t s = 0;
  for (int i = 0; i < n; i++) s += analogReadMilliVolts(pin);
  return (float)s / (float)n;
}

void messe() {
  float mv = mittelMv(PIN_STROM, 32) - nullMv;
  if (mv < 0) mv = 0;
  float iMittel = mv / R_MESS;                         // mA, Mittelwert des Betrags
  float i = aus ? 0.0f : iMittel / formFaktor;         // mA eff
  stromMa += 0.3f * (i - stromMa);
  batV = mittelMv(PIN_BAT, 8) * BAT_TEILER / 1000.0f;
}

// Zu viel Strom: Pegel zurücknehmen, auf der kleinsten Stufe abschalten
void schuetze() {
  static uint32_t tUeber = 0;
  uint32_t now = millis();
  if (aus || stromMa < I_MAX_MA) { tUeber = now; return; }
  if (now - tUeber < 300) return;
  if (pegel > 0) { pegel--; warnung = "STROMBEGRENZUNG"; }
  else { aus = true; warnung = "KURZSCHLUSS?"; }
  warnBis = now + 4000;
  Serial.printf("# %s: %.1f mA\n", warnung, stromMa);
  starteDac();
  stromMa = 0;
  tUeber = now;
}

// ------------------------------------------------------------- Anzeige
#if USE_OLED
void zeichneBatterie(float v) {
  // 4 x AA: voll ab 5,8 V, leer bei 4,4 V
  int seg = (int)((v - 4.4f) / (5.8f - 4.4f) * 4.0f + 0.5f);
  if (seg < 0) seg = 0;
  if (seg > 4) seg = 4;
  oled.drawFrame(110, 1, 16, 8);
  oled.drawBox(126, 3, 2, 4);
  for (int i = 0; i < seg; i++) oled.drawBox(112 + 3 * i, 3, 2, 4);
}

void zeichne() {
  char t[40];
  uint32_t now = millis();
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);
  snprintf(t, sizeof(t), "%.0f Hz", fOrtung());
  oled.drawStr(0, 9, t);
  oled.drawStr(56, 9, SAETZE[satz].cd ? (nurOrtung ? "1-TON" : "ZWEITON") : "PRUEF");
  zeichneBatterie(batV);
  oled.drawHLine(0, 12, 128);

  // großer Stromwert
  oled.setFont(u8g2_font_logisoso24_tr);
  if (aus) snprintf(t, sizeof(t), "AUS");
  else if (stromMa < 10) snprintf(t, sizeof(t), "%.1f", stromMa);
  else snprintf(t, sizeof(t), "%.0f", stromMa);
  oled.drawStr(0, 41, t);
  int w = oled.getStrWidth(t);
  oled.setFont(u8g2_font_6x10_tf);
  if (!aus) oled.drawStr(w + 3, 41, "mA");

  // kleine laufende Sinuswelle als Lebenszeichen (rechts oben)
  if (!aus) {
    float ph = (float)(now % 1000) / 1000.0f * 2.0f * (float)M_PI;
    int yAlt = 26;
    for (int x = 0; x < 34; x++) {
      int y = 26 - (int)(8.0f * sinf(ph + x * 0.37f) + (SAETZE[satz].cd && !nurOrtung ? 4.0f * sinf(ph * 0.5f + x * 0.185f) : 0));
      if (x > 0) oled.drawLine(93 + x, yAlt, 94 + x, y);
      yAlt = y;
    }
  }

  // Balken: Strom logarithmisch 0,1 ... 100 mA
  int bw = 0;
  if (!aus && stromMa > 0.1f) bw = (int)(log10f(stromMa / 0.1f) / 3.0f * 126.0f);
  if (bw > 126) bw = 126;
  oled.drawFrame(0, 46, 128, 8);
  oled.drawBox(1, 47, bw, 6);

  if (warnung && now < warnBis) oled.drawStr(0, 63, warnung);
  else if (aus) oled.drawStr(0, 63, "Taste lang = EIN");
  else if (stromMa < I_WENIG_MA) oled.drawStr(0, 63, "kaum Strom: Erde?");
  else if (pegel == 2) oled.drawStr(0, 63, "MAX: nur gegen Erde!");
  else { snprintf(t, sizeof(t), "PEGEL %s", PEGEL_NAME[pegel]); oled.drawStr(0, 63, t); }
  oled.sendBuffer();
}
#endif

// ------------------------------------------------------------- setup / loop
void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TASTE, INPUT_PULLUP);
  pinMode(PIN_BOOT, INPUT_PULLUP);
  analogSetPinAttenuation(PIN_STROM, ADC_0db);      // 0 ... ca. 0,75 V
  analogSetPinAttenuation(PIN_BAT, ADC_11db);       // 0 ... ca. 2,5 V
  delay(500);
  Serial.println("Kabelsuchgeraet Sender-Test (Variante Einfach)");

#if USE_OLED
  oled.setBusClock(400000);
  oled.begin();
#endif

  // Nullpunkt der Strommessung bei stillem Ausgang
  aus = true;
  starteDac();
  delay(400);
  nullMv = mittelMv(PIN_STROM, 64);
  Serial.printf("Nullpunkt Strommessung: %.1f mV\n", nullMv);
  aus = false;
  starteDac();
}

void loop() {
  static uint32_t tLed = 0, tMess = 0, tOled = 0, tTaste = 0, tAusgabe = 0;
  static bool unten = false;
  uint32_t now = millis();

  while (Serial.available()) {
    char c = (char)Serial.read();
    bool neu = true;
    switch (c) {
      case '1': pegel = 0; aus = false; break;
      case '2': pegel = 1; aus = false; break;
      case '3': pegel = 2; aus = false; break;
      case 'o': aus = !aus; break;
      case 'c': nurOrtung = !nurOrtung; break;
      case 'p': satz = (satz == 0) ? 1 : 0; break;
      case '?': status(); neu = false; break;
      default: neu = false; break;
    }
    if (neu) starteDac();
  }

  // Taste PEGEL: kurz = nächster Pegel, lang = Ausgang aus/ein
  bool d = digitalRead(PIN_TASTE) == LOW || digitalRead(PIN_BOOT) == LOW;
  if (d && !unten) tTaste = now;
  if (!d && unten) {
    uint32_t dauer = now - tTaste;
    if (dauer > 1000) { aus = !aus; starteDac(); }
    else if (dauer > 30) { pegel = (pegel + 1) % 3; aus = false; starteDac(); }
  }
  unten = d;

  if (now - tMess >= 50) { tMess = now; messe(); schuetze(); }

#if USE_OLED
  if (now - tOled >= 80) { tOled = now; zeichne(); }
#endif

  if (now - tAusgabe >= 1000) {
    tAusgabe = now;
    Serial.printf("# Strom %.2f mA eff, Batterie %.2f V, Pegel %s%s\n", stromMa, batV, PEGEL_NAME[pegel],
                  aus ? " (AUS)" : "");
  }

  // LED: blinkt langsam = läuft, schnell = Ausgang aus
  if (now - tLed > (aus ? 100u : 500u)) {
    tLed = now;
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
  }
  delay(5);
}
