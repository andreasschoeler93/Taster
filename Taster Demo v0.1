/*
  ST7789 Bahn-Ansicht — ESP32-S3 XIAO SEEED (kompletter Sketch)

  Layout-Update:
  - Vitals: kompakter, zentriert zwischen linken Rand und Symbolen,
            Text 5px nach unten (mehr Luft links/rechts)
  - Separator/Unterblock: 10px nach unten
  - Oberer Bereich (Timer/Name/Disziplin): 10px nach unten
  - Trennstrich fest bei SEPARATOR_Y (malt nichts unten kaputt)
  - UTF-8/Umlaute: U8g2_for_Adafruit_GFX
  - Piezo: GPIO8, kurzer Beep
  - Akku links neben WLAN (Dummy-Level)

  Libraries:
  - Adafruit GFX Library
  - Adafruit ST7789 and ST7735 Library
  - Adafruit AHTX0
  - U8g2 by olikraus
*/

#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_AHTX0.h>

#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans24pt7b.h>

#include <U8g2_for_Adafruit_GFX.h>

// ---------- Pins (ggf. anpassen) ----------
const int PIN_SCLK     = 4;
const int PIN_MOSI     = 5;
const int PIN_CS       = 3;
const int PIN_DC       = 43;
const int PIN_RST      = 44;

const int PIN_BUTTON   = 9;   // Button to GND (INPUT_PULLUP)

const int PIN_I2C_SDA  = 6;
const int PIN_I2C_SCL  = 7;

const int PIN_PIEZO    = 8;   // Piezo beeper

// ---------- Layout shifts ----------
static const int SHIFT_TOP_PX   = 10;  // Timer/Name/Disziplin 10px nach unten
static const int SHIFT_UNDER_PX = 10;  // Block ab Trennstrich 10px nach unten
static const int VITAL_TEXT_DOWN_PX = 25; // Vitals-Text 5px nach unten

// ---------- Beeper (tone/noTone, non-blocking) ----------
static const uint16_t BEEP_FREQ = 2500; // Hz
static const uint16_t BEEP_MS   = 12;   // sehr kurz
bool beepActive = false;
uint32_t beepStopAt = 0;

void beepStart() {
  tone(PIN_PIEZO, BEEP_FREQ);
  beepActive = true;
  beepStopAt = millis() + BEEP_MS;
}
void beepTick() {
  if (beepActive && (int32_t)(millis() - beepStopAt) >= 0) {
    noTone(PIN_PIEZO);
    beepActive = false;
  }
}

// ---------- Timer state ----------
enum TimerState { T_READY, T_RUNNING, T_STOPPED };
TimerState timerState = T_READY;

// ---------- RGB->565 helper ----------
static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (b >> 3);
}

// ---------- Geometry ----------
const int SCREEN_W = 240;
const int SCREEN_H = 280;
const int TOP_H = 28;
const int SAFE_MARGIN = 14;

const int WIFI_BARS_Y = SCREEN_H - 4;
const int WIFI_LEFT_X = SCREEN_W - SAFE_MARGIN - 36;

// Akku links neben WLAN
const int BATTERY_W = 20;
const int BATTERY_H = 10;
const int BATTERY_GAP_TO_WIFI = 8;
const int BATTERY_X = WIFI_LEFT_X - BATTERY_GAP_TO_WIFI - BATTERY_W;
const int BATTERY_Y = WIFI_BARS_Y - 2;

// ---------- Layout ----------
int SEPARATOR_Y = 154 + SHIFT_UNDER_PX; // <<< 10px nach unten

// Timer + Name moved 5 px up ursprünglich, jetzt zusätzlich SHIFT_TOP
const int TIMER_DST_Y_BASE = SAFE_MARGIN + TOP_H + 4 - 5;
int TIMER_DST_Y = TIMER_DST_Y_BASE + SHIFT_TOP_PX - 15;

int NAME_CENTER_Y = (120 - 5) + SHIFT_TOP_PX - 15;

// Disziplin (Marquee)
static const int MARQ_H = 40;
static const int MARQ_W = SCREEN_W - 2 * SAFE_MARGIN;

int MARQUEE_CENTER_Y = 150 + SHIFT_TOP_PX + 10;
const int MARQUEE_RAISE_Y = 10; // Disziplin 10px höher relativ zu Center

// Disziplin-Canvas NUR Text (Trennstrich separat!)
static const int MARQ_TOTAL_H = MARQ_H;

// Name marquee sizes
static const int NAME_CANVAS_H = 28;
static const int NAME_CANVAS_W = MARQ_W;

// gaps
const int MARQUEE_GAP = 24;
static const int NAME_GAP = 16;

// Unterblock
const int UNDERLINE_RAISE = 8;
const int LASTBLOCK_DOWN  = 20;
const int LAST_TIME_OFFSET = 41;
const int META_OFFSET = 61;

// Labels
const int LABEL_LASTRUN_RAISE = 28;  // "Letzter Lauf" eine Zeile höher
const int LASTTIME_RAISE      = 10;  // Zeit höher

// Bottom layout
const int BOTTOM_H = 20;
const int BOTTOM_LINE_SHIFT = 10;
const int VITALS_DOWN = 28;

// ---------- Colors ----------
const uint16_t MODE_BLUE_BG  = rgb565(20, 60, 180);
const uint16_t MODE_GREEN_BG = rgb565(0, 160, 0);

#define GREY 0xBDF7
const uint16_t TEXT_COLOR  = ST77XX_WHITE;
const uint16_t UNDER_COLOR = 0xDEF7;
const uint16_t VITAL_COLOR = GREY;

uint16_t currentBg() { return (timerState == T_RUNNING) ? MODE_GREEN_BG : MODE_BLUE_BG; }

// ---------- Peripherals ----------
SPIClass spi = SPIClass();
Adafruit_ST7789 tft = Adafruit_ST7789(&spi, PIN_CS, PIN_DC, PIN_RST);
Adafruit_AHTX0 aht;

bool aht_ok = false;
float humidity = NAN;
float temperature = NAN;
int rssi = -99;

// ---------- Example run data ----------
struct RunEntry { const char *name; const char *discipline; };
const RunEntry runList[] = {
  {"Lukas Schneider",            "50 m Hindernisschwimmen"},
  {"Marie-Luise Hoffmann",       "50 m Retten einer Puppe"},
  {"Jannik Müller",              "100 m Lifesaver"},
  {"Sophie Kühn",                "100 m Kombinierte Rettungsübung"},
  {"Maximilian von Hohenberg",   "200 m Super Lifesaver"},
  {"Annika Schröder",            "4 × 25 m Puppenstaffel"},
  {"Florian Weißkirchen",        "4 × 50 m Hindernisstaffel"},
  {"Leonhard Böhm-Friedrichsen", "4 × 50 m Gurtretterstaffel"},
  {"Katharina Jäger-Müller",     "4 × 50 m Rettungsstaffel"},
  {"Johann-Sebastian Überacker", "50 m Retten mit Flossen"}
};
const int RUN_COUNT = sizeof(runList)/sizeof(runList[0]);

// ---------- State ----------
int bahn = 1;
int current_lauf = 1; // 1-based
String swimmerName = "";
String discipline = "";
String lastRunName = "";
char letter = 'A';

// ---------- Timer ----------
uint64_t start_ts_ms = 0;
uint64_t elapsed_ms = 0;
uint64_t last_stop_time_ms = 0;

// Timer fixed X
int16_t timerFixedX = 0;
bool timerFixedXInit = false;

// ---------- Debounce ----------
static const uint32_t DEBOUNCE_MS = 40;
bool lastBtnStable = true;
bool lastBtnRead = true;
uint32_t lastChangeMs = 0;

// ---------- Canvases ----------
static const int TIMER_CANVAS_W = 240;
static const int TIMER_CANVAS_H = 58;
GFXcanvas16 timerCanvas(TIMER_CANVAS_W, TIMER_CANVAS_H);

GFXcanvas16 marqueeCanvas(MARQ_W, MARQ_TOTAL_H);
GFXcanvas16 nameCanvasTop(NAME_CANVAS_W, NAME_CANVAS_H);
GFXcanvas16 nameCanvasBottom(NAME_CANVAS_W, NAME_CANVAS_H);

// ---------- U8g2 ----------
U8G2_FOR_ADAFRUIT_GFX u8_marq;
U8G2_FOR_ADAFRUIT_GFX u8_nameTop;
U8G2_FOR_ADAFRUIT_GFX u8_nameBottom;

static const uint8_t* FONT_DISC    = u8g2_font_helvR10_tf;
static const uint8_t* FONT_NAMETOP = u8g2_font_helvB12_tf;
static const uint8_t* FONT_NAMEBOT = u8g2_font_helvR10_tf;

// ---------- Scroll state ----------
int16_t marqueeX = 0;
int marqueeTextW = 0;
uint32_t lastMarqueeMs = 0;
static const uint16_t MARQUEE_SPEED_PX_PER_TICK = 1;
static const uint32_t MARQUEE_TICK_MS = 35;

int nameTopW = 0;
int16_t nameTopX = 0;
uint32_t lastNameTopMs = 0;

int nameBottomW = 0;
int16_t nameBottomX = 0;
uint32_t lastNameBottomMs = 0;

static const uint32_t NAME_TICK_MS = 50;

// ---------- Helpers ----------
String fmtTime(uint64_t ms) {
  uint64_t totalSec = ms / 1000ULL;
  uint16_t millisPart = ms % 1000ULL;
  uint16_t minutes = totalSec / 60ULL;
  uint16_t seconds = totalSec % 60ULL;
  char buf[32];
  sprintf(buf, "%02u:%02u,%03u", (unsigned)minutes, (unsigned)seconds, (unsigned)millisPart);
  return String(buf);
}

int disciplineCenterY() { return (MARQUEE_CENTER_Y - MARQUEE_RAISE_Y); }
int nameCenterY() { return NAME_CENTER_Y; }

int u8_width(U8G2_FOR_ADAFRUIT_GFX &u8, const uint8_t *font, const String &s) {
  u8.setFont(font);
  return u8.getUTF8Width(s.c_str());
}
int u8_centerBaseline(U8G2_FOR_ADAFRUIT_GFX &u8, const uint8_t *font, int canvasH) {
  u8.setFont(font);
  int a = u8.getFontAscent();
  int d = u8.getFontDescent();
  int textH = a - d;
  return (canvasH - textH) / 2 + a;
}

// ---------- Battery icon ----------
uint8_t getBatteryLevel0to4() { return 3; } // Dummy: 0..4

void drawBatteryIcon(int16_t x, int16_t y, uint8_t level0to4, uint16_t fg, uint16_t bg) {
  if (level0to4 > 4) level0to4 = 4;

  tft.drawRect(x, y - BATTERY_H, BATTERY_W, BATTERY_H, fg);
  tft.fillRect(x + BATTERY_W, y - BATTERY_H + 3, 2, BATTERY_H - 6, fg);

  tft.fillRect(x + 1, y - BATTERY_H + 1, BATTERY_W - 2, BATTERY_H - 2, bg);

  const int innerW = BATTERY_W - 2;
  const int segGap = 1;
  const int segW = (innerW - 3 * segGap) / 4;
  int sx = x + 1;
  int sy = y - BATTERY_H + 1;
  int sh = BATTERY_H - 2;

  for (int i = 0; i < 4; i++) {
    if (i < level0to4) tft.fillRect(sx, sy, segW, sh, fg);
    sx += segW + segGap;
  }
}

// ---------- WiFi bars ----------
void drawWifiBarsLeftToRight(int16_t x, int16_t y, int rssiVal) {
  const int barW = 5, spacing = 3, baseH = 6, step = 6;
  int bars = 0;
  if (rssiVal >= -60) bars = 4;
  else if (rssiVal >= -70) bars = 3;
  else if (rssiVal >= -80) bars = 2;
  else if (rssiVal > -99)  bars = 1;

  for (int i = 0; i < 4; ++i) {
    int h = baseH + i * step;
    int x0 = x + i * (barW + spacing);
    int y0 = y - h;
    uint16_t c = (i < bars) ? rgb565(0,200,0) : GREY;
    tft.fillRect(x0, y0, barW, h, c);
  }
}

// ---------- Separator line (fixed position) ----------
void drawSeparatorLine() {
  int x = SAFE_MARGIN;
  int w = SCREEN_W - 2 * SAFE_MARGIN;
  tft.drawFastHLine(x, SEPARATOR_Y, w, TEXT_COLOR);
}

// ---------- Top bar ----------
void renderTopBar(uint16_t accent) {
  tft.fillRect(SAFE_MARGIN, SAFE_MARGIN, SCREEN_W - 2*SAFE_MARGIN, TOP_H, accent);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(VITAL_COLOR);

  String left = "Bahn " + String(bahn);
  int16_t xb,yb; uint16_t w,h;
  tft.getTextBounds(left,0,0,&xb,&yb,&w,&h);
  int leftBaseline = SAFE_MARGIN + (TOP_H + (int)h)/2 - 2;
  tft.setCursor(SAFE_MARGIN + 6, leftBaseline); tft.print(left);

  String right = "Lauf " + String(current_lauf);
  tft.getTextBounds(right,0,0,&xb,&yb,&w,&h);
  int rightX = SCREEN_W - SAFE_MARGIN - (int)w - 6;
  if (rightX < SAFE_MARGIN) rightX = SAFE_MARGIN;
  int rightBaseline = SAFE_MARGIN + (TOP_H + (int)h)/2 - 2;
  tft.setCursor(rightX, rightBaseline); tft.print(right);
}

// ---------- Bottom bar ----------
int bottomBarY() { return SCREEN_H - SAFE_MARGIN - BOTTOM_H - 2; }

// kompakter Text (zusammenrücken)
String buildBottomTextCompact() {
  char buf[64];
  if (!aht_ok || isnan(temperature) || isnan(humidity)) {
    // kompakt
    sprintf(buf, "--.-C --%% %c", letter);
  } else {
    // kompakt: weniger Leerzeichen
    sprintf(buf, "%.1fC %.0f%% %c", temperature, humidity, letter);
  }
  return String(buf);
}

void renderBottomBar(uint16_t accent) {
  int bottomY = bottomBarY();
  tft.fillRect(SAFE_MARGIN, bottomY - 4, SCREEN_W - 2*SAFE_MARGIN, BOTTOM_H + 12, accent);

  // Symbole
  drawBatteryIcon(BATTERY_X, BATTERY_Y, getBatteryLevel0to4(), VITAL_COLOR, accent);
  drawWifiBarsLeftToRight(WIFI_LEFT_X, WIFI_BARS_Y, rssi);

  // Text: zentriert im freien Bereich zwischen linkem Rand und Akku
  String txt = buildBottomTextCompact();
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(VITAL_COLOR);

  int16_t xb, yb; uint16_t tw, th;
  tft.getTextBounds(txt, 0, 0, &xb, &yb, &tw, &th);

  int leftLimit  = SAFE_MARGIN + 4;
  int rightLimit = BATTERY_X - 6; // Luft vor Akku
  int availW = rightLimit - leftLimit;
  if (availW < 10) availW = 10;

  int x = leftLimit + (availW - (int)tw) / 2;
  if (x < leftLimit) x = leftLimit;

  int vitY = bottomY + 2 - BOTTOM_LINE_SHIFT + VITALS_DOWN + VITAL_TEXT_DOWN_PX;
  if (vitY > SCREEN_H - SAFE_MARGIN - 1) vitY = SCREEN_H - SAFE_MARGIN - 1;

  tft.setCursor(x, vitY - 2);
  tft.print(txt);
}

void updateBottomBar() { renderBottomBar(currentBg()); }

// ---------- Timer canvas positioning ----------
int timerDstY() {
  int dstY = TIMER_DST_Y;
  int maxY = SEPARATOR_Y - TIMER_CANVAS_H - 2;
  if (dstY > maxY) dstY = maxY;
  return dstY;
}

// ---------- initTimerFixedX (only once) ----------
void initTimerFixedX() {
  timerCanvas.setFont(&FreeSans24pt7b);
  int16_t x1,y1; uint16_t w,h;
  timerCanvas.getTextBounds("88:88,888", 0, 0, &x1, &y1, &w, &h);
  timerFixedX = (TIMER_CANVAS_W - (int16_t)w) / 2;
  if (timerFixedX < SAFE_MARGIN) timerFixedX = SAFE_MARGIN;
  timerFixedXInit = true;
}

// ---------- Timer drawing ----------
void updateMainTime_canvas(bool force) {
  String timeStr;
  if (timerState == T_RUNNING) timeStr = fmtTime(elapsed_ms + (millis() - start_ts_ms));
  else if (timerState == T_STOPPED) timeStr = fmtTime(elapsed_ms);
  else timeStr = fmtTime(0);

  static String lastDrawn="";
  if (!force && timeStr == lastDrawn) return;
  lastDrawn = timeStr;

  uint16_t bg = currentBg();
  timerCanvas.fillScreen(bg);
  timerCanvas.setFont(&FreeSans24pt7b);
  timerCanvas.setTextColor(TEXT_COLOR);

  int16_t x1,y1; uint16_t w,h;
  timerCanvas.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);

  if (!timerFixedXInit) initTimerFixedX();
  int16_t x = timerFixedX;
  int16_t baselineY = (TIMER_CANVAS_H + (int16_t)h) / 2;

  timerCanvas.setCursor(x, baselineY);
  timerCanvas.print(timeStr);

  tft.drawRGBBitmap(0, timerDstY(), timerCanvas.getBuffer(), TIMER_CANVAS_W, TIMER_CANVAS_H);
}

// ---------- Underline region (clean + labels) ----------
void clearUnderlineRegion() {
  uint16_t bg = currentBg();
  int x = SAFE_MARGIN;
  int y = SEPARATOR_Y + 2;
  int w = MARQ_W;
  int h = (bottomBarY() - 2) - y;  // bis kurz vor Bottom-Bar
  if (h < 10) h = 10;
  tft.fillRect(x, y, w, h, bg);
}

void drawUnderlineLabel_LastRun() {
  int lineY = SEPARATOR_Y;
  int raise = UNDERLINE_RAISE;

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(UNDER_COLOR);

  const String label = "Letzter Lauf";
  int16_t x1,y1; uint16_t w,h;
  tft.getTextBounds(label, 0, 0, &x1, &y1, &w, &h);

  int x = (SCREEN_W - (int)w) / 2;
  if (x < SAFE_MARGIN) x = SAFE_MARGIN;

  int nameYCenter2 = lineY + META_OFFSET + LASTBLOCK_DOWN - raise - 2;
  int labelYCenter = (nameYCenter2 - (NAME_CANVAS_H / 2) - 10) - LABEL_LASTRUN_RAISE;

  int baseline = labelYCenter + (int)h / 2;
  tft.setCursor(x, baseline);
  tft.print(label);
}

// ---------- Name init ----------
void initNameTop() {
  nameTopW = u8_width(u8_nameTop, FONT_NAMETOP, swimmerName);
  nameTopX = 0;
  lastNameTopMs = millis();
}

void initNameBottom() {
  nameBottomW = u8_width(u8_nameBottom, FONT_NAMEBOT, lastRunName);
  nameBottomX = 0;
  lastNameBottomMs = millis();
}

// ---------- Name draw (top) ----------
void drawNameLineTop(bool force) {
  (void)force;
  uint16_t bg = currentBg();
  int yCenter = nameCenterY();

  nameCanvasTop.fillScreen(bg);

  u8_nameTop.setFontMode(1);
  u8_nameTop.setForegroundColor(TEXT_COLOR);
  u8_nameTop.setBackgroundColor(bg);

  int w = u8_width(u8_nameTop, FONT_NAMETOP, swimmerName);
  int baseline = u8_centerBaseline(u8_nameTop, FONT_NAMETOP, NAME_CANVAS_H);

  if (w <= NAME_CANVAS_W) {
    int x = (NAME_CANVAS_W - w) / 2;
    if (x < 0) x = 0;
    u8_nameTop.setFont(FONT_NAMETOP);
    u8_nameTop.drawUTF8(x, baseline, swimmerName.c_str());
    nameTopW = w;
  } else {
    const int period = nameTopW + NAME_GAP;
    u8_nameTop.setFont(FONT_NAMETOP);
    u8_nameTop.drawUTF8(nameTopX, baseline, swimmerName.c_str());
    u8_nameTop.drawUTF8(nameTopX + period, baseline, swimmerName.c_str());
  }

  int boxX = SAFE_MARGIN;
  int boxY = yCenter - (NAME_CANVAS_H/2);
  tft.drawRGBBitmap(boxX, boxY, nameCanvasTop.getBuffer(), NAME_CANVAS_W, NAME_CANVAS_H);
}

// ---------- Name draw (bottom: last run) ----------
void drawLastRunName(bool force) {
  (void)force;
  uint16_t bg = currentBg();
  int lineY = SEPARATOR_Y;
  int raise = UNDERLINE_RAISE;
  int nameYCenter2 = lineY + META_OFFSET + LASTBLOCK_DOWN - raise - 2;

  nameCanvasBottom.fillScreen(bg);

  u8_nameBottom.setFontMode(1);
  u8_nameBottom.setForegroundColor(UNDER_COLOR);
  u8_nameBottom.setBackgroundColor(bg);

  int w = u8_width(u8_nameBottom, FONT_NAMEBOT, lastRunName);
  int baseline = u8_centerBaseline(u8_nameBottom, FONT_NAMEBOT, NAME_CANVAS_H);

  if (w <= NAME_CANVAS_W) {
    int x = (NAME_CANVAS_W - w) / 2;
    if (x < 0) x = 0;
    u8_nameBottom.setFont(FONT_NAMEBOT);
    u8_nameBottom.drawUTF8(x, baseline, lastRunName.c_str());
    nameBottomW = w;
  } else {
    const int period = nameBottomW + NAME_GAP;
    u8_nameBottom.setFont(FONT_NAMEBOT);
    u8_nameBottom.drawUTF8(nameBottomX, baseline, lastRunName.c_str());
    u8_nameBottom.drawUTF8(nameBottomX + period, baseline, lastRunName.c_str());
  }

  int boxX = SAFE_MARGIN;
  int boxY = nameYCenter2 - (NAME_CANVAS_H / 2);
  tft.drawRGBBitmap(boxX, boxY, nameCanvasBottom.getBuffer(), NAME_CANVAS_W, NAME_CANVAS_H);
}

// ---------- Discipline marquee init/draw ----------
void initMarquee() {
  marqueeTextW = u8_width(u8_marq, FONT_DISC, discipline);
  marqueeX = 0;
  lastMarqueeMs = millis();
}

void drawDisciplineMarquee(bool force) {
  (void)force;
  uint16_t bg = currentBg();

  marqueeCanvas.fillScreen(bg);

  u8_marq.setFontMode(1);
  u8_marq.setForegroundColor(TEXT_COLOR);
  u8_marq.setBackgroundColor(bg);

  int baseline = u8_centerBaseline(u8_marq, FONT_DISC, MARQ_H);
  u8_marq.setFont(FONT_DISC);

  const int period = marqueeTextW + MARQUEE_GAP;
  u8_marq.drawUTF8(marqueeX, baseline, discipline.c_str());
  u8_marq.drawUTF8(marqueeX + period, baseline, discipline.c_str());

  int boxX = SAFE_MARGIN;

  // Wunschposition
  int boxYTop = disciplineCenterY() - (MARQ_H / 2);

  // Grenzen: nicht in den Namen UND nicht in den Unterblock
  int nameBoxTop    = nameCenterY() - (NAME_CANVAS_H / 2);
  int nameBoxBottom = nameBoxTop + NAME_CANVAS_H;

  int minYTop = nameBoxBottom + 6;
  int maxBottom = SEPARATOR_Y - 4;
  int maxYTop   = maxBottom - MARQ_TOTAL_H;

  if (boxYTop < minYTop) boxYTop = minYTop;
  if (boxYTop > maxYTop) boxYTop = maxYTop;

  tft.drawRGBBitmap(boxX, boxYTop, marqueeCanvas.getBuffer(), MARQ_W, MARQ_TOTAL_H);
}

void tickMarquee() {
  if (marqueeTextW <= 0) return;
  if (millis() - lastMarqueeMs < MARQUEE_TICK_MS) return;
  lastMarqueeMs = millis();

  const int period = marqueeTextW + MARQUEE_GAP;
  marqueeX -= (int16_t)MARQUEE_SPEED_PX_PER_TICK;
  if (marqueeX <= -period) marqueeX += period;

  drawDisciplineMarquee(false);
}

// ---------- Name ticks ----------
void tickNames() {
  uint32_t now = millis();

  if (nameTopW > NAME_CANVAS_W) {
    if (now - lastNameTopMs >= NAME_TICK_MS) {
      lastNameTopMs = now;
      const int period = nameTopW + NAME_GAP;
      nameTopX -= 2;
      if (nameTopX <= -period) nameTopX += period;
      drawNameLineTop(true);
    }
  }

  if (nameBottomW > NAME_CANVAS_W) {
    if (now - lastNameBottomMs >= NAME_TICK_MS) {
      lastNameBottomMs = now;
      const int period = nameBottomW + NAME_GAP;
      nameBottomX -= 2;
      if (nameBottomX <= -period) nameBottomX += period;
      drawLastRunName(true);
    }
  }
}

// ---------- Underline values ----------
void updateUnderlineValues(bool force) {
  (void)force;

  clearUnderlineRegion();

  // Zeit (10px höher)
  int lineY = SEPARATOR_Y;
  int raise = UNDERLINE_RAISE;
  int valueYCenter = (lineY + LAST_TIME_OFFSET + LASTBLOCK_DOWN - raise) - LASTTIME_RAISE;

  String lastTimeStr = fmtTime(last_stop_time_ms);

  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(UNDER_COLOR);

  int16_t x1,y1; uint16_t w,h;
  tft.getTextBounds(lastTimeStr, 0, 0, &x1, &y1, &w, &h);
  int x = (SCREEN_W - (int)w) / 2;
  if (x < SAFE_MARGIN) x = SAFE_MARGIN;

  tft.setCursor(x, valueYCenter + (int)h/2);
  tft.print(lastTimeStr);

  // Label + Name (untere Zeile)
  drawUnderlineLabel_LastRun();
  drawLastRunName(true);
}

// ---------- Sensors ----------
void pollAHT20() {
  if (!aht_ok) return;
  sensors_event_t hum,temp;
  if (aht.getEvent(&hum,&temp)) {
    humidity = hum.relative_humidity;
    temperature = temp.temperature;
  }
}
void pollRSSI() {
  int v = WiFi.RSSI();
  if (v==0) rssi = -99;
  else rssi = v;
}

// ---------- Run management ----------
void loadRun(int idx) {
  if (idx < 1) idx = 1;
  if (idx > RUN_COUNT) idx = 1;

  current_lauf = idx;
  swimmerName = String(runList[idx-1].name);
  discipline  = String(runList[idx-1].discipline);

  initNameTop();
  initMarquee();

  drawNameLineTop(true);
  drawDisciplineMarquee(true);
}

void advanceRun() {
  int next = current_lauf + 1;
  if (next > RUN_COUNT) next = 1;
  loadRun(next);
}

// ---------- Full redraw ----------
void drawFullFrame(bool soft) {
  (void)soft;
  uint16_t bg = currentBg();
  tft.fillScreen(bg);

  renderTopBar(bg);
  updateMainTime_canvas(true);

  // 1) Disziplin
  drawDisciplineMarquee(true);

  // 2) Separator fix
  drawSeparatorLine();

  // 3) Name oben NACH Disziplin
  drawNameLineTop(true);

  // 4) Unterblock
  updateUnderlineValues(true);

  // 5) Bottom bar
  renderBottomBar(bg);
}

// ---------- Button logic ----------
void onButtonPressed() {
  beepStart();

  if (timerState == T_READY) {
    elapsed_ms = 0;
    start_ts_ms = millis();
    timerState = T_RUNNING;

  } else if (timerState == T_RUNNING) {
    elapsed_ms += (millis() - start_ts_ms);
    last_stop_time_ms = elapsed_ms;
    timerState = T_STOPPED;

    // current swimmer becomes last run
    lastRunName = swimmerName;
    initNameBottom();

    // next run
    advanceRun();

  } else {
    elapsed_ms = 0;
    start_ts_ms = millis();
    timerState = T_RUNNING;
  }

  drawFullFrame(false);
}

void handleButton() {
  bool readNow = digitalRead(PIN_BUTTON);
  if (readNow != lastBtnRead) { lastBtnRead = readNow; lastChangeMs = millis(); }
  if ((millis() - lastChangeMs) >= DEBOUNCE_MS) {
    if (readNow != lastBtnStable) {
      lastBtnStable = readNow;
      if (lastBtnStable == false) onButtonPressed();
    }
  }
}

// ---------- Setup/Loop ----------
void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_PIEZO, OUTPUT);
  noTone(PIN_PIEZO);

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.setClock(400000);
  delay(50);

  aht_ok = aht.begin(&Wire);

  spi.begin(PIN_SCLK, -1, PIN_MOSI, PIN_CS);
  delay(50);

  tft.init(240, 280);
  tft.setRotation(0);
  tft.setTextWrap(false);

  // Attach U8g2 helpers to canvases
  u8_marq.begin(marqueeCanvas);
  u8_nameTop.begin(nameCanvasTop);
  u8_nameBottom.begin(nameCanvasBottom);

  WiFi.mode(WIFI_STA);

  timerState = T_READY;
  elapsed_ms = 0;
  last_stop_time_ms = 0;

  pollAHT20();
  pollRSSI();

  initTimerFixedX();

  lastRunName = "";
  initNameBottom();

  loadRun(1);
  drawFullFrame(false);
}

void loop() {
  beepTick();
  handleButton();

  uint32_t now = millis();

  static uint32_t lastSensorMs = 0;
  if (now - lastSensorMs >= 500) {
    lastSensorMs = now;
    pollAHT20();
    pollRSSI();
    updateBottomBar();
  }

  static uint32_t lastUiFastMs = 0;
  if (now - lastUiFastMs >= 50) {
    lastUiFastMs = now;
    if (timerState == T_RUNNING) updateMainTime_canvas(false);
  }

  tickMarquee();
  tickNames();

  delay(5);
}
