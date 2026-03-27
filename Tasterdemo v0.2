/*
  ESP32-S3 XIAO SEEED — ST7789

  Flow:
  1) Splash (kurz automatisch)
  2) Assign/Vergabe: Akku% + Buchstabe, Hintergrund abhängig vom Akku (live gemessen)
     -> Tastendruck (heller Ton): weiter
  3) Starter Ansicht + simulierter Lauf:
     - Starterliste (5 Bahnen + Disziplin)
     - Tastendruck (heller Ton): Simulation startet (Timer + 5 Bahnen stoppen zufällig 20..40s)
       -> bei jedem Lane-Stop (EXTERNES Event / Simulation): dumpfer Ton
       -> wenn alle gestoppt: 2s Hold (rot) -> weiter
  4) RunUI Bahntaster: 3 Bahnen (Bahn 1..3)
     - Taste (heller Ton): Start -> Stop -> nächster Lauf
     - KEIN dumpfer Ton in RunUI (weil Stop per Taste)
  5) Danach: KOMPLETT AUS (HOLD LOW), kein DeepSleep
*/

#include <Wire.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_AHTX0.h>

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans24pt7b.h>

#include <U8g2_for_Adafruit_GFX.h>

// ---------------- Debug ----------------
#define DBG 1
#if DBG
  #define DPRINTLN(x) Serial.println(x)
  #define DPRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DPRINTLN(x)
  #define DPRINTF(...)
#endif

// ---------------- Config ----------------
namespace cfg {

struct Pins {
  static constexpr int SCLK     = 4;
  static constexpr int MOSI     = 5;
  static constexpr int CS       = 3;
  static constexpr int DC       = 43;
  static constexpr int RST      = -1;

  static constexpr int BUTTON   = 9;
  static constexpr int PIEZO    = 8;

  static constexpr int I2C_SDA  = 6;
  static constexpr int I2C_SCL  = 7;

  static constexpr int VBAT_ADC = 1;
  static constexpr int MEAS_EN  = 2;

  static constexpr int HOLD     = 44;
};

struct Screen {
  static constexpr int W = 240;
  static constexpr int H = 280;
  static constexpr int SAFE = 14;
  static constexpr int TOP_H = 28;
  static constexpr int BOTTOM_H = 20;
};

static constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (uint16_t)(b >> 3);
}

struct Colors {
  static constexpr uint16_t PREP_BG  = rgb565(20, 60, 180);   // Blau
  static constexpr uint16_t RUN_BG   = rgb565(0, 160, 0);     // Grün
  static constexpr uint16_t STOP_BG  = rgb565(180, 0, 0);     // Rot

  static constexpr uint16_t SPLASH   = rgb565(12, 18, 40);
  static constexpr uint16_t SUB      = rgb565(180, 180, 180);

  static constexpr uint16_t GREY  = 0xBDF7;
  static constexpr uint16_t WHITE = ST77XX_WHITE;
  static constexpr uint16_t BLACK = ST77XX_BLACK;
};

struct Layout {
  static constexpr int TIMER_W = 240;
  static constexpr int TIMER_H = 58;

  static constexpr int TIMER_DST_Y = (Screen::SAFE + Screen::TOP_H + 4 - 5) + 10 - 15;

  // Starterliste / Simulation
  static constexpr int DISC_GAP = 4;
  static constexpr int DISC_H   = 18;
  static constexpr int LANE_H   = 22;

  static constexpr int VITAL_SHIFT_DOWN = 6;

  static constexpr int WIFI_BARS_Y = Screen::H - 4;
  static constexpr int WIFI_LEFT_X = Screen::W - Screen::SAFE - 36;

  static constexpr int BAT_W = 20;
  static constexpr int BAT_H = 10;
  static constexpr int BAT_GAP_WIFI = 8;
  static constexpr int BAT_X = WIFI_LEFT_X - BAT_GAP_WIFI - BAT_W;
  static constexpr int BAT_Y = WIFI_BARS_Y - 2;

  // Assign/Vergabe
  static constexpr int ASSIGN_PCT_BASELINE_Y    = 105;
  static constexpr int ASSIGN_LETTER_BASELINE_Y = 175;

  // RunUI
  static constexpr int RUNUI_NAME_H = 24;
  static constexpr int RUNUI_DISC_H = 18;
  static constexpr int RUNUI_SEP_Y  = 168;
};

struct Timing {
  static constexpr uint32_t DEBOUNCE_MS = 40;

  static constexpr uint32_t SPLASH_MS = 2500;

  static constexpr uint32_t AHT_POLL_MS = 10000;
  static constexpr uint8_t  AHT_FAIL_REINIT_AFTER = 3;

  static constexpr uint32_t VBAT_POLL_MS = 10000;
  static constexpr uint32_t VBAT_SETTLE_MS = 3;

  static constexpr uint32_t BOTTOM_MS = 500;

  // Simulation
  static constexpr uint32_t LANE_MIN_MS = 20000;
  static constexpr uint32_t LANE_MAX_MS = 40000;
  static constexpr uint32_t SIM_UI_MS   = 80;
  static constexpr uint32_t SIM_HOLD_MS = 2000;

  // RunUI
  static constexpr uint32_t RUNUI_FAST_MS = 50;
};

} // namespace cfg

// ---------------- Data ----------------
struct RunEntry { const char* name; const char* discipline; };

static const RunEntry RUNS[] = {
  {"Lukas Schneider",            "50 m Hindernisschwimmen"},
  {"Marie-Luise Hoffmann",       "50 m Retten einer Puppe"},
  {"Jannik Mueller",             "100 m Lifesaver"},
  {"Sophie Kuehn",               "100 m Kombinierte Rettungsuebung"},
  {"Maximilian von Hohenberg",   "200 m Super Lifesaver"},
  {"Annika Schroeder",           "4 x 25 m Puppenstaffel"},
  {"Florian Weisskirchen",       "4 x 50 m Hindernisstaffel"},
  {"Leonhard Boehm-Friedrichsen","4 x 50 m Gurtretterstaffel"},
  {"Katharina Jaeger-Mueller",   "4 x 50 m Rettungsstaffel"},
  {"Johann-Sebastian Ueberacker","50 m Retten mit Flossen"}
};
static const int RUN_COUNT = (int)(sizeof(RUNS)/sizeof(RUNS[0]));
static constexpr int LANES = 5;

// ---------------- State ----------------
enum class UiMode : uint8_t {
  SPLASH,
  ASSIGN,
  STARTERS,
  SIM_RUNNING,
  SIM_HOLD,
  RUNUI
};

enum class RunTimerState : uint8_t { READY, RUNNING, STOPPED };

struct AppState {
  UiMode uiMode = UiMode::SPLASH;
  uint32_t modeStartedMs = 0;

  // Assign
  char assignLetter = 'A';
  uint16_t assignBg = cfg::Colors::PREP_BG;

  // Sensor / Battery
  bool aht_ok = false;
  float humidity = NAN;
  float temperature = NAN;
  uint8_t aht_fail = 0;
  uint32_t lastAhtMs = 0;

  float vbat = NAN;
  uint8_t vbatLevel = 0;
  uint32_t lastVbatMs = 0;
  uint32_t lastBottomMs = 0;

  // Button debounce
  bool lastRead = true;
  bool stable = true;
  uint32_t lastChangeMs = 0;

  // Starterliste Content
  int runIdx = 1; // 1..RUN_COUNT
  String discipline = "";
  String laneNames[LANES] = {"Max Mustermann","Sophie Kuehn","Jannik Mueller","Marie-Luise Hoffmann","Lukas Schneider"};

  // Simulation
  uint32_t simStartMs = 0;
  uint32_t laneTargetMs[LANES] = {0};
  bool laneStopped[LANES] = {false,false,false,false,false};
  uint32_t simHoldStartMs = 0;
  uint32_t lastSimUiMs = 0;
  bool simFrameDrawn = false;

  // Cache (Simulation lane times)
  String lastTopTime = "";
  String lastLaneTime[LANES] = {"","","","",""};
  bool   lastLaneStopped[LANES] = {false,false,false,false,false};
  int16_t laneTimeFixedX = 0;
  bool laneTimeFixedXInit = false;

  // Timer draw fixed
  int16_t timerFixedX = 0;
  bool timerFixedXInit = false;

  // RunUI
  RunTimerState runState = RunTimerState::READY;
  uint64_t runStartMs = 0;
  uint64_t runElapsedMs = 0;
  uint64_t runLastStopMs = 0;

  int bahn = 1;           // 1..3
  int currentLauf = 1;    // 1..RUN_COUNT (rotiert)
  String swimmerName = "";
  String runDiscipline = "";
  String lastRunName = "";
  uint32_t lastRunUiFastMs = 0;
};

static AppState S;

// ---------------- HW ----------------
static SPIClass spi = SPIClass();
static Adafruit_ST7789 tft(&spi, cfg::Pins::CS, cfg::Pins::DC, cfg::Pins::RST);
static Adafruit_AHTX0 aht;

static GFXcanvas16 timerCanvas(cfg::Layout::TIMER_W, cfg::Layout::TIMER_H);

static U8G2_FOR_ADAFRUIT_GFX u8;
static const uint8_t* FONT_DISC = u8g2_font_helvR10_tf;
static const uint8_t* FONT_NAME = u8g2_font_helvR10_tf;

// ============================================================
// Helpers
// ============================================================
static int bottomBarY() {
  return cfg::Screen::H - cfg::Screen::SAFE - cfg::Screen::BOTTOM_H - 2;
}

static int u8_centerBaseline(const uint8_t* font, int h) {
  u8.setFont(font);
  int a = u8.getFontAscent();
  int d = u8.getFontDescent();
  int th = a - d;
  return (h - th)/2 + a;
}

static String fmtTime(uint64_t ms) {
  uint64_t totalCs = ms / 10ULL;
  uint16_t cs = totalCs % 100ULL;
  uint64_t totalSec = totalCs / 100ULL;
  uint16_t minutes = totalSec / 60ULL;
  uint16_t seconds = totalSec % 60ULL;
  char buf[24];
  sprintf(buf, "%02u:%02u,%02u", (unsigned)minutes, (unsigned)seconds, (unsigned)cs);
  return String(buf);
}

static uint8_t vbatToPercent(float v) {
  if (!isfinite(v)) return 0;
  if (v <= 3.30f) return 0;
  if (v >= 4.20f) return 100;
  float p = (v - 3.30f) / (4.20f - 3.30f) * 100.0f;
  if (p < 0) p = 0;
  if (p > 100) p = 100;
  return (uint8_t)(p + 0.5f);
}

static uint16_t bgFromPercent(uint8_t pct) {
  if (pct >= 60) return cfg::rgb565(0, 160, 0);
  if (pct >= 25) return cfg::rgb565(200, 140, 0);
  return cfg::rgb565(180, 0, 0);
}

// ============================================================
// Power Off (nur Selbsthaltung lösen, kein DeepSleep)
// ============================================================
static void powerOffNow() {
#if DBG
  Serial.println("[PWR] powerOffNow(): HOLD -> LOW (no deep sleep)");
  Serial.flush();
#endif
  digitalWrite(cfg::Pins::HOLD, LOW);
  delay(50);
  while (true) { delay(1000); }
}

// ============================================================
// Beeper: Click (Taste) vs Event (Extern/Simulation)
// ============================================================
namespace beep {
  static constexpr uint8_t  RES = 10;

  // heller Click
  static constexpr uint16_t FREQ_CLICK = 4200;
  static constexpr uint16_t DUR_CLICK_MS = 55;

  // dumpf (extern)
  static constexpr uint16_t FREQ_EVENT = 1200;
  static constexpr uint16_t DUR_EVENT_MS = 120;

  static bool ok = false;
  static uint32_t offAt = 0;

  static void attach(uint16_t freq) {
    ledcDetach(cfg::Pins::PIEZO);
    ok = ledcAttach(cfg::Pins::PIEZO, freq, RES);
  }

  static void init() {
    pinMode(cfg::Pins::PIEZO, OUTPUT);
    digitalWrite(cfg::Pins::PIEZO, LOW);
    ok = ledcAttach(cfg::Pins::PIEZO, FREQ_CLICK, RES);
    if (ok) ledcWrite(cfg::Pins::PIEZO, 0);
#if DBG
    Serial.printf("[BEEP] ledcAttach=%s\n", ok ? "OK" : "FAIL");
#endif
  }

  static void play(uint16_t freq, uint16_t durMs, float dutyFrac) {
    if (!ok) return;
    attach(freq);
    if (!ok) return;
    uint16_t duty = (uint16_t)((1U << RES) * dutyFrac);
    ledcWrite(cfg::Pins::PIEZO, duty);
    offAt = millis() + durMs;
  }

  static void click() { play(FREQ_CLICK, DUR_CLICK_MS, 0.50f); }
  static void event() { play(FREQ_EVENT, DUR_EVENT_MS, 0.30f); }

  static void tick() {
    if (!ok) return;
    if (offAt && (int32_t)(millis() - offAt) >= 0) {
      ledcWrite(cfg::Pins::PIEZO, 0);
      offAt = 0;
    }
  }
}

// ============================================================
// Sensors / Battery
// ============================================================
static constexpr float VBAT_DIV_RATIO = 2.0f;

static float readVbat_once() {
  digitalWrite(cfg::Pins::MEAS_EN, HIGH);
  delay(cfg::Timing::VBAT_SETTLE_MS);
  uint32_t mv = analogReadMilliVolts(cfg::Pins::VBAT_ADC);
  float v = (mv/1000.0f) * VBAT_DIV_RATIO;
  digitalWrite(cfg::Pins::MEAS_EN, LOW);
  return v;
}

static uint8_t vbatToLevel0to4(float v) {
  if (!isfinite(v)) return 0;
  if (v >= 4.10f) return 4;
  if (v >= 3.90f) return 3;
  if (v >= 3.75f) return 2;
  if (v >= 3.60f) return 1;
  return 0;
}

static void pollVBAT_10s() {
  uint32_t now = millis();
  if (now - S.lastVbatMs < cfg::Timing::VBAT_POLL_MS) return;
  S.lastVbatMs = now;

  float v1 = readVbat_once();
  delay(2);
  float v2 = readVbat_once();

  S.vbat = (v1 + v2) * 0.5f;
  S.vbatLevel = vbatToLevel0to4(S.vbat);

#if DBG
  Serial.printf("[VBAT] %.3fV lvl=%u pct=%u\n", S.vbat, S.vbatLevel, vbatToPercent(S.vbat));
#endif
}

static void ahtReinit() {
#if DBG
  Serial.println("[AHT] reinit...");
#endif
  Wire.end();
  delay(5);
  Wire.begin(cfg::Pins::I2C_SDA, cfg::Pins::I2C_SCL);
  Wire.setClock(100000);
  delay(10);

  S.aht_ok = aht.begin(&Wire);
  S.aht_fail = 0;

#if DBG
  Serial.printf("[AHT] begin=%s\n", S.aht_ok ? "OK" : "FAIL");
#endif
}

static void pollAHT_robust() {
  uint32_t now = millis();
  if (now - S.lastAhtMs < cfg::Timing::AHT_POLL_MS) return;
  S.lastAhtMs = now;

  if (!S.aht_ok) { ahtReinit(); return; }

  sensors_event_t hum, temp;
  bool ok = aht.getEvent(&hum, &temp);

  if (ok) {
    S.humidity = hum.relative_humidity;
    S.temperature = temp.temperature;
    S.aht_fail = 0;
  } else {
    S.aht_fail++;
    if (S.aht_fail >= cfg::Timing::AHT_FAIL_REINIT_AFTER) ahtReinit();
  }
}

// ============================================================
// Bottom Bar
// ============================================================
static void drawBatteryIcon(int16_t x, int16_t y, uint8_t level0to4, uint16_t fg, uint16_t bg) {
  if (level0to4 > 4) level0to4 = 4;

  tft.drawRect(x, y - cfg::Layout::BAT_H, cfg::Layout::BAT_W, cfg::Layout::BAT_H, fg);
  tft.fillRect(x + cfg::Layout::BAT_W, y - cfg::Layout::BAT_H + 3, 2, cfg::Layout::BAT_H - 6, fg);
  tft.fillRect(x + 1, y - cfg::Layout::BAT_H + 1, cfg::Layout::BAT_W - 2, cfg::Layout::BAT_H - 2, bg);

  const int innerW = cfg::Layout::BAT_W - 2;
  const int segGap = 1;
  const int segW = (innerW - 3 * segGap) / 4;

  int sx = x + 1;
  int sy = y - cfg::Layout::BAT_H + 1;
  int sh = cfg::Layout::BAT_H - 2;

  for (int i = 0; i < 4; i++) {
    if (i < level0to4) tft.fillRect(sx, sy, segW, sh, fg);
    sx += segW + segGap;
  }
}

static void drawWifiBarsDummy(int16_t x, int16_t y) {
  const int barW = 5, spacing = 3, baseH = 6, step = 6;
  for (int i = 0; i < 4; ++i) {
    int h = baseH + i * step;
    int x0 = x + i * (barW + spacing);
    int y0 = y - h;
    tft.fillRect(x0, y0, barW, h, cfg::Colors::GREY);
  }
}

static String bottomText() {
  char buf[64];
  if (!S.aht_ok || isnan(S.temperature) || isnan(S.humidity)) {
    sprintf(buf, "--.-C --%% %c", S.assignLetter);
  } else {
    sprintf(buf, "%.1fC %.0f%% %c", S.temperature, S.humidity, S.assignLetter);
  }
  return String(buf);
}

static void renderBottomBar(uint16_t accent) {
  int rectTop = bottomBarY() - 4;
  int rectH   = cfg::Screen::BOTTOM_H + 12;

  int neededBottom = cfg::Layout::BAT_Y + 2;
  int currentBottom = rectTop + rectH - 1;
  if (neededBottom > currentBottom) rectH = (neededBottom - rectTop) + 1;

  tft.fillRect(cfg::Screen::SAFE, rectTop,
               cfg::Screen::W - 2*cfg::Screen::SAFE, rectH, accent);

  drawBatteryIcon(cfg::Layout::BAT_X, cfg::Layout::BAT_Y, S.vbatLevel, cfg::Colors::GREY, accent);
  drawWifiBarsDummy(cfg::Layout::WIFI_LEFT_X, cfg::Layout::WIFI_BARS_Y);

  String txt = bottomText();
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(cfg::Colors::GREY);

  int16_t xb,yb; uint16_t tw,th;
  tft.getTextBounds(txt, 0, 0, &xb,&yb,&tw,&th);

  int leftLimit  = cfg::Screen::SAFE + 4;
  int rightLimit = cfg::Layout::BAT_X - 6;
  int availW = rightLimit - leftLimit;
  if (availW < 10) availW = 10;

  int x = leftLimit + (availW - (int)tw)/2;
  if (x < leftLimit) x = leftLimit;

  int vitBaseline = cfg::Screen::H - cfg::Screen::SAFE - 1 + cfg::Layout::VITAL_SHIFT_DOWN;
  int bottomLimit = rectTop + rectH - 1;
  if (vitBaseline > bottomLimit) vitBaseline = bottomLimit;

  tft.setCursor(x, vitBaseline);
  tft.print(txt);
}

// ============================================================
// Top bar
// ============================================================
static void renderTopBar(uint16_t accent, const String& left, const String& right) {
  tft.fillRect(cfg::Screen::SAFE, cfg::Screen::SAFE,
               cfg::Screen::W - 2*cfg::Screen::SAFE, cfg::Screen::TOP_H, accent);

  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(cfg::Colors::GREY);

  int16_t xb,yb; uint16_t w,h;
  tft.getTextBounds(left,0,0,&xb,&yb,&w,&h);
  int base = cfg::Screen::SAFE + (cfg::Screen::TOP_H + (int)h)/2 - 2;
  tft.setCursor(cfg::Screen::SAFE + 6, base);
  tft.print(left);

  tft.getTextBounds(right,0,0,&xb,&yb,&w,&h);
  int xR = cfg::Screen::W - cfg::Screen::SAFE - (int)w - 6;
  if (xR < cfg::Screen::SAFE) xR = cfg::Screen::SAFE;
  tft.setCursor(xR, base);
  tft.print(right);
}

// ============================================================
// Timer canvas (oben)
// ============================================================
static void initTimerFixedX() {
  timerCanvas.setFont(&FreeSans24pt7b);
  int16_t x1,y1; uint16_t w,h;
  timerCanvas.getTextBounds("88:88,88", 0, 0, &x1,&y1,&w,&h);
  S.timerFixedX = (cfg::Layout::TIMER_W - (int)w)/2;
  if (S.timerFixedX < cfg::Screen::SAFE) S.timerFixedX = cfg::Screen::SAFE;
  S.timerFixedXInit = true;
}

static void drawTimerIfChanged(const String& timeStr, uint16_t bg) {
  if (timeStr == S.lastTopTime) return;
  S.lastTopTime = timeStr;

  timerCanvas.fillScreen(bg);
  timerCanvas.setFont(&FreeSans24pt7b);
  timerCanvas.setTextColor(cfg::Colors::WHITE);

  int16_t x1,y1; uint16_t w,h;
  timerCanvas.getTextBounds(timeStr, 0, 0, &x1,&y1,&w,&h);

  if (!S.timerFixedXInit) initTimerFixedX();
  int baseline = (cfg::Layout::TIMER_H + (int)h)/2;

  timerCanvas.setCursor(S.timerFixedX, baseline);
  timerCanvas.print(timeStr);

  tft.drawRGBBitmap(0, cfg::Layout::TIMER_DST_Y, timerCanvas.getBuffer(), cfg::Layout::TIMER_W, cfg::Layout::TIMER_H);
}

// ============================================================
// UTF-8 centered line helper
// ============================================================
static void drawCenteredUTF8Line(int x0, int y0, int w, int h,
                                 const String& s, const uint8_t* font,
                                 uint16_t fg, uint16_t bg) {
  tft.fillRect(x0, y0, w, h, bg);

  u8.setFontMode(1);
  u8.setForegroundColor(fg);
  u8.setBackgroundColor(bg);
  u8.setFont(font);

  int tw = u8.getUTF8Width(s.c_str());
  int x = x0 + (w - tw)/2;
  if (x < x0) x = x0;
  int baseline = y0 + u8_centerBaseline(font, h);

  u8.drawUTF8(x, baseline, s.c_str());
}

// ============================================================
// Starterliste Content laden
// ============================================================
static int discY() { return cfg::Layout::TIMER_DST_Y + cfg::Layout::TIMER_H + cfg::Layout::DISC_GAP; }
static int laneListY() { return discY() + cfg::Layout::DISC_H + 4; }

static void loadStartersBlock() {
  if (S.runIdx < 1) S.runIdx = 1;
  if (S.runIdx > RUN_COUNT) S.runIdx = 1;

  S.discipline = String(RUNS[S.runIdx - 1].discipline);

  int base = (S.runIdx - 1);
  for (int i=0;i<LANES;i++) {
    int idx = (base + i) % RUN_COUNT;
    S.laneNames[i] = String(RUNS[idx].name);
  }
}

// ============================================================
// Screens
// ============================================================
static void drawSplash() {
  tft.fillScreen(cfg::Colors::SPLASH);

  tft.setTextColor(cfg::Colors::WHITE);
  tft.setFont(&FreeSans12pt7b);

  const String t1 = "Stoppuhr 2.0";
  int16_t xb,yb; uint16_t w,h;
  tft.getTextBounds(t1,0,0,&xb,&yb,&w,&h);
  tft.setCursor((cfg::Screen::W - (int)w)/2, 120);
  tft.print(t1);

  tft.setTextColor(cfg::Colors::SUB);
  tft.setFont(&FreeSans9pt7b);
  const String t2 = "Demo Flow";
  tft.getTextBounds(t2,0,0,&xb,&yb,&w,&h);
  tft.setCursor((cfg::Screen::W - (int)w)/2, 160);
  tft.print(t2);

  renderBottomBar(cfg::Colors::SPLASH);
}

static void drawAssign() {
  // sofort (live) messen: damit oben nicht 0% steht
  float v1 = readVbat_once();
  delay(2);
  float v2 = readVbat_once();
  S.vbat = (v1 + v2) * 0.5f;
  S.vbatLevel = vbatToLevel0to4(S.vbat);

  uint8_t pct = vbatToPercent(S.vbat);
  uint16_t bg = bgFromPercent(pct);
  S.assignBg = bg;

  tft.fillScreen(bg);
  tft.setTextColor(cfg::Colors::WHITE);

  String pctTxt = String(pct) + "%";
  tft.setFont(&FreeSans12pt7b);
  int16_t xb,yb; uint16_t w,h;
  tft.getTextBounds(pctTxt,0,0,&xb,&yb,&w,&h);
  tft.setCursor((cfg::Screen::W - (int)w)/2, cfg::Layout::ASSIGN_PCT_BASELINE_Y);
  tft.print(pctTxt);

  char buf[2] = {S.assignLetter, 0};
  tft.setFont(&FreeSans24pt7b);
  tft.getTextBounds(buf,0,0,&xb,&yb,&w,&h);
  tft.setCursor((cfg::Screen::W - (int)w)/2, cfg::Layout::ASSIGN_LETTER_BASELINE_Y);
  tft.print(buf);

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(cfg::Colors::GREY);
  const String hint = "Taste: Starter";
  tft.getTextBounds(hint,0,0,&xb,&yb,&w,&h);
  tft.setCursor((cfg::Screen::W - (int)w)/2, 230);
  tft.print(hint);

  renderBottomBar(bg);
}

static void drawStarters() {
  uint16_t bg = cfg::Colors::PREP_BG;
  tft.fillScreen(bg);

  renderTopBar(bg, "Start", "Starter");
  S.lastTopTime = "";
  drawTimerIfChanged(fmtTime(0), bg);

  // Disziplin
  {
    int x0 = cfg::Screen::SAFE;
    int w  = cfg::Screen::W - 2*cfg::Screen::SAFE;
    int y0 = discY();
    int h  = cfg::Layout::DISC_H;
    String disc = S.discipline;
    if (disc.length() == 0) disc = "Disziplin";
    drawCenteredUTF8Line(x0, y0, w, h, disc, FONT_DISC, cfg::Colors::WHITE, bg);
  }

  // Liste
  int x0 = cfg::Screen::SAFE;
  int w  = cfg::Screen::W - 2*cfg::Screen::SAFE;
  int y0 = laneListY();
  int h0 = cfg::Layout::LANE_H * LANES;

  tft.fillRect(x0, y0, w, h0, bg);

  u8.setFontMode(1);
  u8.setForegroundColor(cfg::Colors::WHITE);
  u8.setBackgroundColor(bg);
  u8.setFont(FONT_NAME);

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(cfg::Colors::WHITE);

  for (int i=0;i<LANES;i++) {
    int rowY = y0 + i*cfg::Layout::LANE_H;

    tft.setCursor(x0 + 6, rowY + 16);
    tft.print(String(i+1) + ":");

    int baseline = rowY + u8_centerBaseline(FONT_NAME, cfg::Layout::LANE_H);
    String name = S.laneNames[i];

    int nameLeft = x0 + 28;
    int nameRight = x0 + w - 6;
    int maxW = nameRight - nameLeft;

    String shown = name;
    while (shown.length() > 3 && u8.getUTF8Width(shown.c_str()) > maxW) {
      shown.remove(shown.length()-1);
      String tmp = shown + "...";
      if (u8.getUTF8Width(tmp.c_str()) <= maxW) { shown = tmp; break; }
    }
    u8.drawUTF8(nameLeft, baseline, shown.c_str());
  }

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(cfg::Colors::GREY);
  int16_t xb,yb; uint16_t tw,th;
  const String hint = "Taste: Simulation Start";
  tft.getTextBounds(hint,0,0,&xb,&yb,&tw,&th);
  tft.setCursor((cfg::Screen::W - (int)tw)/2, 230);
  tft.print(hint);

  renderBottomBar(bg);

  // Reset Simulation caches
  for (int i=0;i<LANES;i++) {
    S.lastLaneTime[i] = "";
    S.lastLaneStopped[i] = false;
  }
  S.simFrameDrawn = false;
}

// ============================================================
// Simulation UI (ähnlich Racing-Ansicht)
// ============================================================
static void drawSimStaticFrame(uint16_t bg) {
  tft.fillScreen(bg);
  renderTopBar(bg, "Start", "Sim");

  S.lastTopTime = "";
  drawTimerIfChanged(fmtTime(0), bg);

  // Disziplin
  int x0 = cfg::Screen::SAFE;
  int w  = cfg::Screen::W - 2*cfg::Screen::SAFE;
  drawCenteredUTF8Line(x0, discY(), w, cfg::Layout::DISC_H, S.discipline, FONT_DISC, cfg::Colors::WHITE, bg);

  // Lane rows (nur Nummern links)
  int y0 = laneListY();
  int h0 = cfg::Layout::LANE_H * LANES;
  tft.fillRect(x0, y0, w, h0, bg);

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(cfg::Colors::WHITE);
  for (int i=0;i<LANES;i++) {
    int rowY = y0 + i*cfg::Layout::LANE_H;
    tft.setCursor(x0 + 6, rowY + 16);
    tft.print(String(i+1) + ":");
  }

  renderBottomBar(bg);
  S.simFrameDrawn = true;
}

static void initLaneTimeFixedX() {
  tft.setFont(&FreeSans9pt7b);
  int16_t x1,y1; uint16_t w,h;
  tft.getTextBounds("88:88,88", 0, 0, &x1,&y1,&w,&h);

  int x0 = cfg::Screen::SAFE;
  int wArea = cfg::Screen::W - 2*cfg::Screen::SAFE;

  int timeAreaLeft  = x0 + 34;
  int timeAreaRight = x0 + wArea - 6;
  int timeAreaW = timeAreaRight - timeAreaLeft;

  S.laneTimeFixedX = timeAreaLeft + (timeAreaW - (int)w)/2;
  if (S.laneTimeFixedX < timeAreaLeft) S.laneTimeFixedX = timeAreaLeft;
  S.laneTimeFixedXInit = true;
}

static void updateSimLaneTime(int laneIdx, const String& ts, bool stopped, uint16_t bg) {
  if (ts == S.lastLaneTime[laneIdx] && stopped == S.lastLaneStopped[laneIdx]) return;

  S.lastLaneTime[laneIdx] = ts;
  S.lastLaneStopped[laneIdx] = stopped;

  int x0 = cfg::Screen::SAFE;
  int w  = cfg::Screen::W - 2*cfg::Screen::SAFE;
  int y0 = laneListY();
  int rowY = y0 + laneIdx*cfg::Layout::LANE_H;

  int timeAreaLeft  = x0 + 34;
  int timeAreaRight = x0 + w - 6;
  int timeAreaW = timeAreaRight - timeAreaLeft;
  int timeAreaH = cfg::Layout::LANE_H;

  tft.fillRect(timeAreaLeft, rowY, timeAreaW, timeAreaH, bg);

  uint16_t col = cfg::Colors::WHITE;
  if (bg == cfg::Colors::RUN_BG) {
    col = stopped ? cfg::Colors::BLACK : cfg::Colors::WHITE;
  } else if (bg == cfg::Colors::STOP_BG) {
    col = cfg::Colors::WHITE;
  }

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(col);

  if (!S.laneTimeFixedXInit) initLaneTimeFixedX();

  int16_t xb,yb; uint16_t tw,th;
  tft.getTextBounds(ts,0,0,&xb,&yb,&tw,&th);
  int baseline = rowY + (timeAreaH/2) + (int)th/2 - 2;

  tft.setCursor(S.laneTimeFixedX, baseline);
  tft.print(ts);
}

static bool simAllStopped() {
  for (int i=0;i<LANES;i++) if (!S.laneStopped[i]) return false;
  return true;
}

static void startSimulation() {
  S.simStartMs = millis();
  for (int i=0;i<LANES;i++) {
    S.laneStopped[i] = false;
    S.laneTargetMs[i] = (uint32_t)random(cfg::Timing::LANE_MIN_MS, cfg::Timing::LANE_MAX_MS + 1);
    S.lastLaneTime[i] = "";
    S.lastLaneStopped[i] = false;
  }
  S.lastSimUiMs = 0;
  S.simFrameDrawn = false;

  S.uiMode = UiMode::SIM_RUNNING;
}

// ============================================================
// RunUI
// ============================================================
static uint16_t runui_bg() {
  if (S.runState == RunTimerState::RUNNING) return cfg::Colors::RUN_BG;
  if (S.runState == RunTimerState::STOPPED) return cfg::Colors::STOP_BG;
  return cfg::Colors::PREP_BG;
}

static void runui_loadRun(int idx) {
  if (idx < 1) idx = 1;
  if (idx > RUN_COUNT) idx = 1;
  S.currentLauf = idx;
  S.swimmerName = String(RUNS[idx-1].name);
  S.runDiscipline = String(RUNS[idx-1].discipline);
}

static void runui_drawStatic(uint16_t bg) {
  tft.fillScreen(bg);

  renderTopBar(bg, "Bahn " + String(S.bahn), "Lauf " + String(S.currentLauf));
  S.lastTopTime = "";
  drawTimerIfChanged(fmtTime(0), bg);

  int x0 = cfg::Screen::SAFE;
  int w  = cfg::Screen::W - 2*cfg::Screen::SAFE;

  int nameY = cfg::Layout::TIMER_DST_Y + cfg::Layout::TIMER_H + 6;
  drawCenteredUTF8Line(x0, nameY, w, cfg::Layout::RUNUI_NAME_H, S.swimmerName, FONT_NAME, cfg::Colors::WHITE, bg);

  int discY0 = nameY + cfg::Layout::RUNUI_NAME_H + 2;
  drawCenteredUTF8Line(x0, discY0, w, cfg::Layout::RUNUI_DISC_H, S.runDiscipline, FONT_DISC, cfg::Colors::WHITE, bg);

  tft.drawFastHLine(x0, cfg::Layout::RUNUI_SEP_Y, w, cfg::Colors::WHITE);

  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(cfg::Colors::WHITE);

  String lt = fmtTime(S.runLastStopMs);
  int16_t xb,yb; uint16_t tw,th;
  tft.getTextBounds(lt,0,0,&xb,&yb,&tw,&th);

  int ltX = (cfg::Screen::W - (int)tw)/2;
  int ltY = cfg::Layout::RUNUI_SEP_Y + 34;
  tft.setCursor(ltX, ltY);
  tft.print(lt);

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(cfg::Colors::GREY);
  String ln = S.lastRunName;
  if (ln.length() == 0) ln = " ";
  int lny = ltY + 28;
  drawCenteredUTF8Line(x0, lny - 14, w, 20, ln, FONT_NAME, cfg::Colors::GREY, bg);

  renderBottomBar(bg);
}

static void runui_updateTimerFast() {
  if (S.runState != RunTimerState::RUNNING) return;
  uint64_t now = millis();
  uint64_t shown = S.runElapsedMs + (now - S.runStartMs);
  drawTimerIfChanged(fmtTime(shown), runui_bg());
}

static void runui_onButton() {
  if (S.runState == RunTimerState::READY) {
    S.runElapsedMs = 0;
    S.runStartMs = millis();
    S.runState = RunTimerState::RUNNING;
    runui_drawStatic(runui_bg());
    return;
  }

  if (S.runState == RunTimerState::RUNNING) {
    S.runElapsedMs += (millis() - S.runStartMs);
    S.runLastStopMs = S.runElapsedMs;
    S.runState = RunTimerState::STOPPED;

    // KEIN dumpfer Ton hier! (Stop per Taste, Click kommt aus onButtonPressed)

    S.lastRunName = S.swimmerName;

    int next = S.currentLauf + 1;
    if (next > RUN_COUNT) next = 1;
    runui_loadRun(next);

    S.bahn++;
    runui_drawStatic(runui_bg());

    if (S.bahn > 3) {
#if DBG
      Serial.println("[FLOW] 3 Bahnen fertig -> PowerOff");
      Serial.flush();
#endif
      delay(400);
      powerOffNow();
    }
    return;
  }

  // STOPPED -> neuer Start
  S.runElapsedMs = 0;
  S.runStartMs = millis();
  S.runState = RunTimerState::RUNNING;
  runui_drawStatic(runui_bg());
}

// ============================================================
// Button Debounce + dispatch
// ============================================================
static void onButtonPressed() {
  beep::click(); // immer heller Ton für Taster

  if (S.uiMode == UiMode::ASSIGN) {
    S.uiMode = UiMode::STARTERS;
    loadStartersBlock();
    drawStarters();
    return;
  }

  if (S.uiMode == UiMode::STARTERS) {
    startSimulation();
    return;
  }

  if (S.uiMode == UiMode::RUNUI) {
    runui_onButton();
    return;
  }
}

static void handleButton() {
  bool readNow = digitalRead(cfg::Pins::BUTTON);
  if (readNow != S.lastRead) { S.lastRead = readNow; S.lastChangeMs = millis(); }

  if ((millis() - S.lastChangeMs) >= cfg::Timing::DEBOUNCE_MS) {
    if (readNow != S.stable) {
      S.stable = readNow;
      if (S.stable == false) onButtonPressed();
    }
  }
}

// ============================================================
// Setup / Loop
// ============================================================
void setup() {
  Serial.begin(115200);
  delay(50);

  pinMode(cfg::Pins::HOLD, OUTPUT);
  digitalWrite(cfg::Pins::HOLD, HIGH);
  delay(5);

  pinMode(cfg::Pins::BUTTON, INPUT_PULLUP);

  beep::init();

  pinMode(cfg::Pins::MEAS_EN, OUTPUT);
  digitalWrite(cfg::Pins::MEAS_EN, LOW);

  analogReadResolution(12);
  analogSetPinAttenuation(cfg::Pins::VBAT_ADC, ADC_11db);

  Wire.begin(cfg::Pins::I2C_SDA, cfg::Pins::I2C_SCL);
  Wire.setClock(100000);
  delay(50);

  S.aht_ok = aht.begin(&Wire);

  spi.begin(cfg::Pins::SCLK, -1, cfg::Pins::MOSI, cfg::Pins::CS);
  delay(50);

  tft.init(cfg::Screen::W, cfg::Screen::H);
  tft.setRotation(0);
  tft.setTextWrap(false);

  u8.begin(tft);

  randomSeed((uint32_t)esp_random());

  initTimerFixedX();

  // init lane time fixed X for sim
  initLaneTimeFixedX();

  // initial content
  S.runIdx = 1;
  loadStartersBlock();

  // start
  S.uiMode = UiMode::SPLASH;
  S.modeStartedMs = millis();
  drawSplash();

  S.lastVbatMs = 0;
  S.lastAhtMs = 0;
  S.lastBottomMs = 0;
}

void loop() {
  beep::tick();
  handleButton();

  uint32_t now = millis();

  pollAHT_robust();
  pollVBAT_10s();

  // Splash -> Assign (auto)
  if (S.uiMode == UiMode::SPLASH) {
    if (now - S.modeStartedMs >= cfg::Timing::SPLASH_MS) {
      S.uiMode = UiMode::ASSIGN;
      drawAssign();
    }
    if (now - S.lastBottomMs >= cfg::Timing::BOTTOM_MS) {
      S.lastBottomMs = now;
      renderBottomBar(cfg::Colors::SPLASH);
    }
    delay(5);
    return;
  }

  // Simulation running
  if (S.uiMode == UiMode::SIM_RUNNING) {
    uint16_t bg = cfg::Colors::RUN_BG;

    uint32_t elapsed = now - S.simStartMs;
    for (int i=0;i<LANES;i++) {
      if (!S.laneStopped[i] && elapsed >= S.laneTargetMs[i]) {
        // externes Event (Simulation) -> dumpfer Ton
        S.laneStopped[i] = true;
        beep::event();
      }
    }

    if (!S.simFrameDrawn) {
      drawSimStaticFrame(bg);
      S.lastSimUiMs = 0;
    }

    if (now - S.lastSimUiMs >= cfg::Timing::SIM_UI_MS) {
      S.lastSimUiMs = now;

      uint32_t shown = elapsed;
      if (simAllStopped()) {
        uint32_t maxT = 0;
        for (int i=0;i<LANES;i++) if (S.laneTargetMs[i] > maxT) maxT = S.laneTargetMs[i];
        shown = maxT;
      }

      drawTimerIfChanged(fmtTime(shown), bg);

      for (int i=0;i<LANES;i++) {
        bool stopped = S.laneStopped[i];
        uint32_t laneShown = stopped ? S.laneTargetMs[i] : min(shown, S.laneTargetMs[i]);
        updateSimLaneTime(i, fmtTime(laneShown), stopped, bg);
      }

      if (simAllStopped()) {
        // Hold rot
        S.uiMode = UiMode::SIM_HOLD;
        S.simHoldStartMs = now;

        drawSimStaticFrame(cfg::Colors::STOP_BG);
        drawTimerIfChanged(fmtTime(shown), cfg::Colors::STOP_BG);
        for (int i=0;i<LANES;i++) {
          updateSimLaneTime(i, fmtTime(S.laneTargetMs[i]), true, cfg::Colors::STOP_BG);
        }
      }
    }

    if (now - S.lastBottomMs >= cfg::Timing::BOTTOM_MS) {
      S.lastBottomMs = now;
      renderBottomBar(bg);
    }

    delay(5);
    return;
  }

  // Simulation Hold -> RunUI
  if (S.uiMode == UiMode::SIM_HOLD) {
    uint16_t bg = cfg::Colors::STOP_BG;

    if (now - S.lastBottomMs >= cfg::Timing::BOTTOM_MS) {
      S.lastBottomMs = now;
      renderBottomBar(bg);
    }

    if (now - S.simHoldStartMs >= cfg::Timing::SIM_HOLD_MS) {
      // Wechsel in RunUI (Bahn 1..3)
      S.uiMode = UiMode::RUNUI;

      S.bahn = 1;
      S.runState = RunTimerState::READY;
      S.runElapsedMs = 0;
      S.runLastStopMs = 0;
      S.lastRunName = "";

      runui_loadRun(1);
      runui_drawStatic(runui_bg());
      delay(5);
      return;
    }

    delay(5);
    return;
  }

  // RunUI
  if (S.uiMode == UiMode::RUNUI) {
    uint16_t bg = runui_bg();

    if (now - S.lastBottomMs >= cfg::Timing::BOTTOM_MS) {
      S.lastBottomMs = now;
      renderBottomBar(bg);
    }

    if (now - S.lastRunUiFastMs >= cfg::Timing::RUNUI_FAST_MS) {
      S.lastRunUiFastMs = now;
      runui_updateTimerFast();
    }

    delay(5);
    return;
  }

  // Assign / Starters idle: BottomBar nachführen und in Assign Akku-Hintergrund aktualisieren
  if (S.uiMode == UiMode::ASSIGN) {
    // optional: alle 10s neu messen und BG aktualisieren
    if (now - S.lastVbatMs >= cfg::Timing::VBAT_POLL_MS) {
      // pollVBAT_10s() läuft schon, aber Assign-Ansicht soll auch BG/Prozent refreshen
      drawAssign();
    }
    if (now - S.lastBottomMs >= cfg::Timing::BOTTOM_MS) {
      S.lastBottomMs = now;
      renderBottomBar(S.assignBg);
    }
    delay(5);
    return;
  }

  if (S.uiMode == UiMode::STARTERS) {
    if (now - S.lastBottomMs >= cfg::Timing::BOTTOM_MS) {
      S.lastBottomMs = now;
      renderBottomBar(cfg::Colors::PREP_BG);
    }
    delay(5);
    return;
  }

  delay(5);
}
