// =============================================================================
// Kabelsuchgerät – Handgerät-Testfirmware für den Mini-Test (Phase 3, Variante "Einfach")
// -----------------------------------------------------------------------------
// Hardware:  ESP32-S3 SuperMini + MCP6002 (2 Verstärkerstufen) + 1 Spule
//            + OLED 1,3" (SH1106, I²C) + Piezo + 2 Taster + 4 x AA
// Anleitung: docs/machbarkeitstest.md, Schaltplan: hardware/handgeraet/testaufbau/
//
// Was die Firmware macht:
//   - tastet mit dem eingebauten Wandler (ADC1) drei Eingänge reihum ab:
//       NIEDRIG = Ausgang Stufe 1 (ca. 5,7-fach), HOCH = Ausgang Stufe 2
//       (ca. 230-fach), BATT = Batteriespannung über Teiler
//   - misst beim Start die tatsächliche Abtastrate
//   - wertet Ortungston, Richtungston und eine leere Nachbarfrequenz per
//     Lock-in aus (lockin.h), in beiden Kanälen gleichzeitig
//   - nimmt automatisch den Kanal, der nicht übersteuert (Bereichswahl)
//   - zeigt Pegel (dB), Balken mit Spitzenwert, Stromrichtung und Batterie
//   - Piezo-Ton: Tonhöhe steigt mit dem Signal
//   - gibt Werte für den seriellen Plotter der Arduino IDE aus
//
// Arduino IDE:  Boardverwalter "esp32" von Espressif, Version 3.x
//               Board "ESP32S3 Dev Module", "USB CDC On Boot: Enabled",
//               Bibliothek "U8g2" (von olikraus) über den Bibliotheksverwalter.
//               Serieller Monitor/Plotter: 115200 Baud.
//
// Tasten:   REF   kurz = Referenz für die Stromrichtung setzen (nahe am Sender)
//                 lang = Frequenzsuche (Gerät dabei nah an die Leitung halten)
//           MODUS kurz = Empfindlichkeit: AUTO, 1 (empfindlich) ... 5 (grob)
//                 lang = Ton: laut, leise, aus
//           Die BOOT-Taste auf dem Board wirkt wie REF.
//
// Serielle Befehle (Zeichen + Enter):
//   s  Frequenzsuche          r  Referenz setzen       e  Empfindlichkeit weiter
//   m  Ton weiter             t  Glättung schnell/ruhig
//   b  Bereich: AUTO / nur NIEDRIG / nur HOCH (für Messung M6)
//   a  Frequenznachführung an/aus      v  ausführliche Ausgabe an/aus
//   c<I_mA> <r_cm>   Kalibrieren an bekanntem Strom, z. B. "c10 10"
//   k<Wert>          Kalibrierfaktor setzen (µV pro nT und kHz)      ?  Hilfe
// =============================================================================

#include <Arduino.h>
#include "esp_adc/adc_continuous.h"
#include "soc/soc_caps.h"
#include "esp_timer.h"
#include "lockin.h"

#define USE_OLED 1          // 0 = ohne Display
#if USE_OLED
#include <U8g2lib.h>
#endif

// ------------------------------------------------------------- Pins (ESP32-S3 SuperMini)
constexpr int PIN_LO    = 1;               // ADC1: Ausgang Stufe 1 (NIEDRIG)
constexpr int PIN_HI    = 2;               // ADC1: Ausgang Stufe 2 (HOCH)
constexpr int PIN_BAT   = 4;               // ADC1: Batterie über 100 kΩ / 47 kΩ
// GPIO5 und GPIO6 sind für eine zweite Spule reserviert.
constexpr int PIN_SDA   = 8;               // OLED
constexpr int PIN_SCL   = 9;               // OLED
constexpr int PIN_REF   = 10;              // Taste REF gegen GND
constexpr int PIN_MODUS = 11;              // Taste MODUS gegen GND
constexpr int PIN_PIEZO = 12;              // Piezo über 100 Ω gegen GND
constexpr int PIN_BOOT  = 0;               // BOOT-Taste auf dem Board (wie REF)

// ------------------------------------------------------------- Analogkette
constexpr float GAIN_LO = 1.0f + 47.0f / 10.0f;    // Stufe 1: 1 + R13/R14 = 5,7
constexpr float GAIN_2  = 1.0f + 390.0f / 10.0f;   // Stufe 2: 1 + R16/R17 = 40
constexpr float ADC_VFS = 3.1f;            // Messbereich des Wandlers bei 12 dB Dämpfung (V)
constexpr float BAT_TEILER = (100.0f + 47.0f) / 47.0f;

// Kalibrierfaktor: µV (eff) an der Spule pro nT und pro kHz.
// Theorie für 1000 Wdg. auf 10 x 80 mm Ferrit: ca. 17,3. Nach Messung M4 ersetzen.
float calK = 17.3f;

// ------------------------------------------------------------- Abtastung
constexpr uint32_t FS_GESAMT = 81000;      // Wandlungen pro Sekunde, reihum NIEDRIG/HOCH/BATT
constexpr int KANAELE = 3;
constexpr uint32_t FRAME_WERTE = 135 * KANAELE;   // 5 ms pro DMA-Rahmen
constexpr uint32_t FRAME_BYTES = FRAME_WERTE * SOC_ADC_DIGI_RESULT_BYTES;

// Frequenz (muss zu sender_test.ino passen): 200 kHz / 24 = 8333,3 Hz, Richtungston die Hälfte
constexpr double F_ORTUNG = 200000.0 / 24.0;
const lockin::ScanStage SUCHE_GROB{60.0f, 3.0f, 8}, SUCHE_FEIN{4.0f, 0.4f, 60};

// ------------------------------------------------------------- Objekte
adc_continuous_handle_t adc = nullptr;
uint8_t chLo = 0, chHi = 1, chBat = 3;     // ADC-Kanalnummern, werden aus den Pins ermittelt
lockin::DualRange kette;                   // NIEDRIG + HOCH, Bereichswahl (lockin.h)
lockin::Scanner scanner;
#if USE_OLED
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE, PIN_SCL, PIN_SDA);
#endif

// Zähler aus der Wandler-Unterbrechung (für die Messung der Abtastrate)
volatile uint32_t isrFrames = 0;
volatile int64_t isrZeit = 0;
volatile uint32_t isrUeberlauf = 0;

// Austausch zwischen Rechen-Task (Kern 0) und Anzeige (Kern 1)
struct Shared {
  lockin::Result r;          // Ergebnis des aktiven Kanals
  float pegel = 0;           // Ortungston in HOCH-Einheiten
  float rauschen = 0;        // Rauschen in HOCH-Einheiten
  float cd = 0;              // Richtungston in HOCH-Einheiten
  bool niedrig = false;      // NIEDRIG aktiv
  float verh = GAIN_2;       // gelerntes Verhältnis HOCH/NIEDRIG
  float spitzeHi = 0, spitzeLo = 0;
  float batV = 0;
  bool scanning = false;
  bool scanNew = false;
  double scanF = 0;
  float scanContrast = 0;
  bool keineDaten = false;
};
Shared shared;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// Befehle an den Rechen-Task
volatile bool cmdInit = false;
volatile double cmdFs = 27000.0;
volatile bool cmdScan = false;
volatile bool cmdTauSlow = false;
volatile bool cmdTauChanged = false;
volatile bool cmdAfc = true;
volatile int cmdBereich = 0;               // 0 AUTO, 1 nur NIEDRIG, 2 nur HOCH

// Zustand der Anzeige
double fsKanal = 27000.0;
int block = 540;                           // Abtastwerte je Kanal in 20 ms
bool refSet = false;
float refDelta = 0;
bool verbose = false;
bool tauSlow = false;
int empf = 0;                              // 0 = AUTO, 1 ... 5 manuell
int tonModus = 0;                          // 0 laut, 1 leise, 2 aus
float spitzeDb = 0;                        // Spitzenwert-Halten für den Balken
float autoOben = 60;                       // obere Balkengrenze im AUTO-Modus (dBµV)

// ------------------------------------------------------------- Umrechnungen
// Spitzenwert in HOCH-Einheiten (1 = halber Messbereich) -> µV eff an der Spule
float zuUv(float amp) { return amp * (ADC_VFS * 0.5f) / (GAIN_LO * GAIN_2) * 0.70710678f * 1e6f; }
float dBuV(float uv) { return lockin::dB(uv); }
float zuNT(float uv, double f) { return uv / (calK * (float)(f / 1000.0)); }

// ------------------------------------------------------------- Wandler
static bool IRAM_ATTR beiRahmen(adc_continuous_handle_t, const adc_continuous_evt_data_t*, void*) {
  isrFrames = isrFrames + 1;
  isrZeit = esp_timer_get_time();
  return false;
}
static bool IRAM_ATTR beiUeberlauf(adc_continuous_handle_t, const adc_continuous_evt_data_t*, void*) {
  isrUeberlauf = isrUeberlauf + 1;
  return false;
}

bool pruefe(esp_err_t e, const char* was) {
  if (e != ESP_OK) Serial.printf("FEHLER bei %s: %s\n", was, esp_err_to_name(e));
  return e == ESP_OK;
}

uint8_t kanalVon(int pin) {
  adc_unit_t u;
  adc_channel_t c;
  if (adc_continuous_io_to_channel(pin, &u, &c) != ESP_OK || u != ADC_UNIT_1) {
    Serial.printf("FEHLER: GPIO%d ist kein ADC1-Eingang\n", pin);
    return 0;
  }
  return (uint8_t)c;
}

bool starteWandler() {
  chLo = kanalVon(PIN_LO);
  chHi = kanalVon(PIN_HI);
  chBat = kanalVon(PIN_BAT);

  adc_continuous_handle_cfg_t hc = {};
  hc.max_store_buf_size = FRAME_BYTES * 8;
  hc.conv_frame_size = FRAME_BYTES;
  if (!pruefe(adc_continuous_new_handle(&hc, &adc), "adc_continuous_new_handle")) return false;

  adc_digi_pattern_config_t muster[KANAELE] = {};
  const uint8_t kan[KANAELE] = {chLo, chHi, chBat};
  for (int i = 0; i < KANAELE; i++) {
    muster[i].atten = 3;                   // 12 dB (ADC_ATTEN_DB_12, früher DB_11): 0 ... ca. 3,1 V
    muster[i].channel = kan[i];
    muster[i].unit = ADC_UNIT_1;
    muster[i].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;
  }
  adc_continuous_config_t cfg = {};
  cfg.pattern_num = KANAELE;
  cfg.adc_pattern = muster;
  cfg.sample_freq_hz = FS_GESAMT;
  cfg.conv_mode = ADC_CONV_SINGLE_UNIT_1;
  cfg.format = ADC_DIGI_OUTPUT_FORMAT_TYPE2;
  if (!pruefe(adc_continuous_config(adc, &cfg), "adc_continuous_config")) return false;

  adc_continuous_evt_cbs_t cbs = {};
  cbs.on_conv_done = beiRahmen;
  cbs.on_pool_ovf = beiUeberlauf;
  if (!pruefe(adc_continuous_register_event_callbacks(adc, &cbs, nullptr), "register_event_callbacks")) return false;
  return pruefe(adc_continuous_start(adc), "adc_continuous_start");
}

// ------------------------------------------------------------- Rechen-Task
void dspTask(void*) {
  static uint8_t buf[FRAME_BYTES];
  const float* T = lockin::sinTab();
  bool bereit = false;
  int leer = 0;
  double sumBat = 0;
  int nBat = 0;
  int lo = -1;                             // letzter NIEDRIG-Wert, wartet auf seinen HOCH-Partner

  for (;;) {
    uint32_t got = 0;
    esp_err_t e = adc_continuous_read(adc, buf, FRAME_BYTES, &got, 100);
    if (e != ESP_OK) {
      if (++leer > 20) { portENTER_CRITICAL(&mux); shared.keineDaten = true; portEXIT_CRITICAL(&mux); }
      continue;
    }
    leer = 0;

    // Befehle übernehmen
    if (cmdInit) {
      cmdInit = false;
      double fs = cmdFs;
      kette.begin(fs, (int)lround(fs * 0.02), F_ORTUNG, GAIN_2);   // 20 ms pro Auswerteblock
      kette.setTau(cmdTauSlow ? 0.3f : 0.06f);
      scanner.reset();
      lo = -1;
      bereit = true;
    }
    if (!bereit) continue;                 // während der Abtastratenmessung nur lesen
    if (cmdTauChanged) { kette.setTau(cmdTauSlow ? 0.3f : 0.06f); cmdTauChanged = false; }
    kette.setAfc(cmdAfc);
    kette.force(cmdBereich);
    if (cmdScan) {
      cmdScan = false;
      scanner.start(kette.locate(), SUCHE_GROB, SUCHE_FEIN, cmdFs);
    }

    // Werte nach Kanal sortieren (jeder Wert trägt seine Kanalnummer)
    for (uint32_t i = 0; i + SOC_ADC_DIGI_RESULT_BYTES <= got; i += SOC_ADC_DIGI_RESULT_BYTES) {
      const adc_digi_output_data_t* p = (const adc_digi_output_data_t*)&buf[i];
      uint32_t ch = p->type2.channel;
      int raw = (int)p->type2.data;
      if (ch == chBat) { sumBat += raw; nBat++; continue; }
      if (ch == chLo) { lo = raw; continue; }
      if (ch != chHi || lo < 0) continue;  // HOCH ohne vorherigen NIEDRIG-Wert: verwerfen
      bool blockEnde = kette.sample(lo, raw, T, &scanner);
      lo = -1;
      if (!blockEnde) continue;

      // ---------------- Blockende: Ergebnisse veröffentlichen
      bool fertig = false;
      double fNeu = 0;
      float kontrast = 0;
      if (scanner.done()) {
        fertig = true;
        fNeu = scanner.result();
        kontrast = scanner.contrast();
        kette.setLocate(fNeu);
        scanner.reset();
      }
      portENTER_CRITICAL(&mux);
      shared.r = kette.result();
      shared.pegel = kette.level();
      shared.rauschen = kette.noise();
      shared.cd = kette.cd();
      shared.niedrig = kette.low();
      shared.verh = kette.ratio();
      shared.spitzeHi = kette.peakHi();
      shared.spitzeLo = kette.peakLo();
      if (nBat > 0) shared.batV = (float)(sumBat / nBat) * ADC_VFS / 4095.0f * BAT_TEILER;
      shared.scanning = scanner.active();
      if (fertig) { shared.scanNew = true; shared.scanF = fNeu; shared.scanContrast = kontrast; }
      shared.keineDaten = false;
      portEXIT_CRITICAL(&mux);
      sumBat = 0; nBat = 0;
    }
  }
}

// ------------------------------------------------------------- Balken und Ton
// Obere Grenze des Balkens in dBµV; der Balken zeigt 40 dB darunter bis zur Grenze.
float balkenOben(float db) {
  static const float STUFE[] = {0, 40, 55, 70, 85, 100};   // 1 = empfindlich ... 5 = grob
  if (empf > 0) return STUFE[empf];
  // AUTO: Grenze folgt dem Maximum nach oben sofort, nach unten langsam (ca. 3 dB/s)
  if (db + 3 > autoOben) autoOben = db + 3;
  else autoOben -= 3.0f * 0.06f;
  if (autoOben < 40) autoOben = 40;
  return autoOben;
}

uint32_t tonAlt = 0;
void setzeTon(float anteil, bool signal) {
  uint32_t f = 0;
  if (signal && tonModus < 2 && anteil > 0) {
    // 300 ... 3000 Hz, auf Vielfache von 50 Hz gerundet: deren Oberwellen liegen wie
    // die Netzoberwellen neben der Sendefrequenz und stören die Auswertung nicht.
    f = (uint32_t)(300.0f + 2700.0f * fminf(anteil, 1.0f));
    f = (f + 25) / 50 * 50;
  }
  if (f == tonAlt) return;
  tonAlt = f;
  if (f == 0) { ledcWrite(PIN_PIEZO, 0); return; }
  ledcChangeFrequency(PIN_PIEZO, f, 10);
  ledcWrite(PIN_PIEZO, tonModus == 0 ? 512 : 40);   // 50 % = laut, 4 % = leise
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

void zeichne(const Shared& s, int dir, bool signal, float db, float oben) {
  char t[40];
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);
  snprintf(t, sizeof(t), "%.0fHz", s.r.fL);
  oled.drawStr(0, 9, t);
  if (empf == 0) oled.drawStr(48, 9, "AUTO");
  else { snprintf(t, sizeof(t), "E%d", empf); oled.drawStr(52, 9, t); }
  oled.drawStr(80, 9, tonModus == 0 ? "))" : tonModus == 1 ? ")" : "x");
  zeichneBatterie(s.batV);
  oled.drawHLine(0, 12, 128);

  if (s.scanning) {
    oled.setFont(u8g2_font_logisoso16_tr);
    oled.drawStr(0, 38, "Suche...");
    oled.setFont(u8g2_font_6x10_tf);
    oled.drawStr(0, 56, "nah an die Leitung");
    oled.sendBuffer();
    return;
  }

  // großer Pegel in dB (bezogen auf 1 µV an der Spule)
  oled.setFont(u8g2_font_logisoso24_tr);
  if (signal) snprintf(t, sizeof(t), "%.0f", db);
  else snprintf(t, sizeof(t), "--");
  oled.drawStr(0, 41, t);
  int w = oled.getStrWidth(t);
  oled.setFont(u8g2_font_6x10_tf);
  oled.drawStr(w + 3, 41, "dB");

  // Stromrichtung rechts
  if (!refSet) oled.drawStr(84, 30, "REF?");
  else if (dir > 0) { oled.drawTriangle(124, 26, 104, 16, 104, 36); oled.drawStr(92, 44, "HIN"); }
  else if (dir < 0) { oled.drawTriangle(84, 26, 104, 16, 104, 36); oled.drawUTF8(80, 44, "ZURÜCK"); }
  else { oled.setFont(u8g2_font_logisoso16_tr); oled.drawStr(100, 36, "?"); oled.setFont(u8g2_font_6x10_tf); }

  // Balken: 40 dB Fenster unterhalb der oberen Grenze, Strich = Spitzenwert
  float unten = oben - 40.0f;
  int bw = signal ? (int)((db - unten) / 40.0f * 126.0f) : 0;
  int pw = (int)((spitzeDb - unten) / 40.0f * 126.0f);
  if (bw < 0) bw = 0;
  if (bw > 126) bw = 126;
  oled.drawFrame(0, 46, 128, 8);
  oled.drawBox(1, 47, bw, 6);
  if (pw > 0 && pw < 127) oled.drawVLine(1 + pw, 45, 10);

  if (s.spitzeHi > 0.95f && s.spitzeLo > 0.95f) oled.drawStr(0, 63, "UEBERSTEUERT");
  else if (!signal) oled.drawStr(0, 63, "kein Signal");
  else {
    float snr = lockin::dB(s.pegel) - lockin::dB(s.rauschen);
    snprintf(t, sizeof(t), "SNR %.0fdB  %s", snr, s.niedrig ? "NIEDRIG" : "HOCH");
    oled.drawStr(0, 63, t);
  }
  oled.sendBuffer();
}
#endif

// ------------------------------------------------------------- Bedienung
void printHelp() {
  Serial.println();
  Serial.println("Kabelsuchgeraet Handgeraet-Test (Variante Einfach)");
  Serial.println(" s  Frequenzsuche (nah an die Leitung)   r  Referenz Stromrichtung");
  Serial.println(" e  Empfindlichkeit weiter               m  Ton weiter");
  Serial.println(" t  Glaettung schnell/ruhig              b  Bereich AUTO/NIEDRIG/HOCH");
  Serial.println(" a  AFC an/aus   v  ausfuehrlich an/aus   ? Hilfe");
  Serial.println(" c<I_mA> <r_cm>  kalibrieren, z. B. c10 10");
  Serial.println(" k<Wert>         Kalibrierfaktor setzen (uV pro nT und kHz)");
  Serial.printf(" Abtastrate je Kanal %.1f Hz, Block %d Werte, calK = %.2f\n\n", fsKanal, block, calK);
}

void handleCommand(const String& line, const Shared& s) {
  if (line.length() == 0) return;
  const lockin::Result& r = s.r;
  switch (line[0]) {
    case 's': cmdScan = true; Serial.println("# Frequenzsuche gestartet"); break;
    case 'r':
      if (!r.cdValid) { Serial.println("# Kein Richtungston erkennbar - naeher an die Leitung"); break; }
      refDelta = r.delta; refSet = true;
      Serial.printf("# Referenz gesetzt (Delta = %.0f Grad)\n", r.delta * 57.2958f);
      break;
    case 'e': empf = (empf + 1) % 6; Serial.printf("# Empfindlichkeit %s%d\n", empf ? "E" : "AUTO ", empf); break;
    case 'm': tonModus = (tonModus + 1) % 3; Serial.printf("# Ton %s\n", tonModus == 0 ? "laut" : tonModus == 1 ? "leise" : "aus"); break;
    case 't':
      tauSlow = !tauSlow; cmdTauSlow = tauSlow; cmdTauChanged = true;
      Serial.printf("# Glaettung %s\n", tauSlow ? "ruhig (2 x 0,3 s)" : "schnell (2 x 0,06 s)");
      break;
    case 'b': {
      int b = (cmdBereich + 1) % 3;
      cmdBereich = b;
      Serial.printf("# Bereich %s\n", b == 0 ? "AUTO" : b == 1 ? "nur NIEDRIG" : "nur HOCH");
      break;
    }
    case 'a': cmdAfc = !cmdAfc; Serial.printf("# AFC %s\n", cmdAfc ? "an" : "aus"); break;
    case 'v': verbose = !verbose; break;
    case 'k': {
      float k = line.substring(1).toFloat();
      if (k > 0) { calK = k; Serial.printf("# calK = %.3f\n", calK); }
      break;
    }
    case 'c': {
      // c<I_mA> <r_cm>: bekannter Strom in einem geraden Draht, Abstand zur Spulenmitte
      int sp = line.indexOf(' ');
      float iMA = line.substring(1, sp > 0 ? sp : line.length()).toFloat();
      float rCM = sp > 0 ? line.substring(sp + 1).toFloat() : 0;
      if (iMA > 0 && rCM > 0) {
        float bNT = 0.2f * iMA / (rCM / 100.0f);
        float uv = zuUv(s.pegel);
        calK = uv / (bNT * (float)(r.fL / 1000.0));
        Serial.printf("# Kalibriert: B = %.3f nT, U = %.2f uV -> calK = %.3f uV/(nT*kHz)\n", bNT, uv, calK);
        Serial.println("#   Wert notieren und oben im Sketch bei calK eintragen.");
      } else {
        Serial.println("# Format: c<I_mA> <r_cm>, z. B. c10 10");
      }
      break;
    }
    case '?': printHelp(); break;
    default: break;
  }
}

// Taste mit kurz/lang: liefert 1 = kurz, 2 = lang (beim Loslassen), sonst 0
struct Taste {
  int pin;
  bool unten = false;
  uint32_t t0 = 0;
  int lies(uint32_t now) {
    bool d = digitalRead(pin) == LOW;
    int ev = 0;
    if (d && !unten) t0 = now;
    if (!d && unten) ev = (now - t0 > 1000) ? 2 : (now - t0 > 30 ? 1 : 0);
    unten = d;
    return ev;
  }
};
Taste tRef{PIN_REF}, tModus{PIN_MODUS}, tBoot{PIN_BOOT};

// ------------------------------------------------------------- setup / loop
void setup() {
  Serial.begin(115200);
  pinMode(PIN_REF, INPUT_PULLUP);
  pinMode(PIN_MODUS, INPUT_PULLUP);
  pinMode(PIN_BOOT, INPUT_PULLUP);
  ledcAttach(PIN_PIEZO, 1000, 10);
  ledcWrite(PIN_PIEZO, 0);
  delay(300);

#if USE_OLED
  oled.setBusClock(400000);
  oled.begin();
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);
  oled.drawStr(0, 12, "Handgeraet-Test");
  oled.drawStr(0, 28, "messe Abtastrate ...");
  oled.sendBuffer();
#endif

  if (!starteWandler()) {
    Serial.println("FEHLER: Wandler laesst sich nicht starten (Boardverwalter esp32 3.x?)");
    for (;;) delay(1000);
  }
  xTaskCreatePinnedToCore(dspTask, "dsp", 8192, nullptr, 5, nullptr, 0);

  // Tatsächliche Abtastrate über 2 s messen (Zeitstempel der DMA-Rahmen)
  delay(300);
  uint32_t n0 = isrFrames;
  int64_t t0 = isrZeit;
  delay(2000);
  uint32_t n1 = isrFrames;
  int64_t t1 = isrZeit;
  if (n1 > n0 + 10 && t1 > t0) {
    double fsGesamt = (double)(n1 - n0) * FRAME_WERTE / ((double)(t1 - t0) * 1e-6);
    fsKanal = fsGesamt / KANAELE;
  } else {
    Serial.println("WARNUNG: keine Rahmen vom Wandler - nehme Sollwert");
    fsKanal = (double)FS_GESAMT / KANAELE;
  }
  block = (int)lround(fsKanal * 0.02);
  Serial.printf("# Abtastrate gesamt %.0f Hz (Soll %u), je Kanal %.1f Hz\n",
                fsKanal * KANAELE, (unsigned)FS_GESAMT, fsKanal);
  cmdFs = fsKanal;
  cmdInit = true;
  printHelp();
}

void loop() {
  static uint32_t tPlot = 0, tOled = 0, tVerb = 0, tWarn = 0;
  static uint32_t ueberlaufAlt = 0;
  static String line;

  Shared s;
  portENTER_CRITICAL(&mux);
  s = shared;
  shared.scanNew = false;
  portEXIT_CRITICAL(&mux);
  const lockin::Result& r = s.r;
  uint32_t now = millis();

  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') { handleCommand(line, s); line = ""; }
    else line += c;
  }

  int eRef = tRef.lies(now), eBoot = tBoot.lies(now), eMod = tModus.lies(now);
  if (eRef == 1 || eBoot == 1) handleCommand("r", s);
  if (eRef == 2 || eBoot == 2) handleCommand("s", s);
  if (eMod == 1) handleCommand("e", s);
  if (eMod == 2) handleCommand("m", s);

  if (s.scanNew) Serial.printf("# Frequenz gefunden: %.2f Hz (Kontrast %.0f)\n", s.scanF, s.scanContrast);
  if (s.keineDaten && now - tWarn > 2000) {
    Serial.println("# WARNUNG: keine Daten vom Wandler");
    tWarn = now;
  }
  uint32_t ue = isrUeberlauf;
  if (ue != ueberlaufAlt && now - tWarn > 2000) {
    Serial.printf("# WARNUNG: Wandler-Puffer uebergelaufen (%u x) - Daten verloren\n", (unsigned)ue);
    ueberlaufAlt = ue;
    tWarn = now;
  }

  float uv = zuUv(s.pegel), uvN = zuUv(s.rauschen), uvC = zuUv(s.cd);
  float db = dBuV(uv);
  bool signal = s.pegel > 2.0f * s.rauschen;         // mind. 6 dB über dem Rauschen
  int dir = lockin::direction(r.delta, refDelta, refSet && r.cdValid);

  // Spitzenwert-Halten (fällt nach 2 s langsam ab)
  static uint32_t tSpitze = 0;
  if (signal && db >= spitzeDb) { spitzeDb = db; tSpitze = now; }
  else if (now - tSpitze > 2000) spitzeDb -= 0.3f;

  if (now - tOled >= 60) {
    tOled = now;
    float oben = balkenOben(signal ? db : 0);
    setzeTon((db - (oben - 40.0f)) / 40.0f, signal && !s.scanning);
#if USE_OLED
    zeichne(s, dir, signal, db, oben);
#endif
  }

  // serieller Plotter: dB bezogen auf 1 µV an der Spule
  if (now - tPlot >= 60) {
    tPlot = now;
    Serial.printf("Pegel:%.1f Rauschen:%.1f CD:%.1f Richtung:%d Bereich:%d\n",
                  db, dBuV(uvN), dBuV(uvC), dir * 20, s.niedrig ? 10 : 30);
  }
  if (verbose && now - tVerb >= 1000) {
    tVerb = now;
    Serial.printf("# fL=%.2fHz Pegel=%.1fdBuV (%.3f nT) SNR=%.1fdB CD=%.1fdBuV Delta=%.0fdeg "
                  "Richtung=%s Bereich=%s Verh=%.1f Spitze HOCH=%.2f NIEDRIG=%.2f Batt=%.2fV\n",
                  r.fL, db, zuNT(uv, r.fL), db - dBuV(uvN), dBuV(uvC), r.delta * 57.2958f,
                  dir > 0 ? "HIN" : dir < 0 ? "ZURUECK" : "?", s.niedrig ? "NIEDRIG" : "HOCH",
                  s.verh, s.spitzeHi, s.spitzeLo, s.batV);
  }
  delay(5);
}
