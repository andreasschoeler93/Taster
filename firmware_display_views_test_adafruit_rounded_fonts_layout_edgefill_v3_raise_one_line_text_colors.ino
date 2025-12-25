/*
 Display Views Test - Farben angepasst
 - Alle Texte, die vorher grau waren, sind jetzt weiß.
 - Vitaldaten (untere Zeile) sind grau.
 - Basierend auf der Variante: "raise one line" (Hauptblock eine Zeile nach oben).
 - FreeSans Fonts, edge-fill, wifi bars L->R kept.
*/

#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans24pt7b.h>

// RGB->565 helper
static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (b >> 3);
}

// Pins
const int PIN_SCLK = 4;
const int PIN_MOSI = 5;
const int PIN_CS   = 3;
const int PIN_DC   = 43;
const int PIN_RST  = 44;

SPIClass spi = SPIClass();
Adafruit_ST7789 tft = Adafruit_ST7789(&spi, PIN_CS, PIN_DC, PIN_RST);

// Display geometry
const int SCREEN_W = 240;
const int SCREEN_H = 280;
const int TOP_H = 28;
const int BOTTOM_H = 20;

// Safe margin
const int SAFE_MARGIN = 14;

// Bottom layout
const int BOTTOM_LINE_SHIFT = 12;
const int WIFI_BARS_Y = SCREEN_H - 4; // Y for wifi bars

// Colors
#define BG      ST77XX_BLACK
#define FG      ST77XX_WHITE
#define ACCENT  ST77XX_CYAN
#define GREY    0x7BEF

// Text color mapping
const uint16_t TEXT_COLOR = FG;     // previously grey text -> now white
const uint16_t VITAL_COLOR = GREY;  // vitals should be gray

// Mode colors (full fill)
const uint16_t MODE_GREEN_BG = rgb565(0,120,0);
const uint16_t MODE_RED_BG   = rgb565(180,30,30);
const uint16_t MODE_BLUE_BG  = rgb565(0,80,160);

// Simulated telemetry & race info
float humidity = 47.0f;
float temperature = 23.5f;
int rssi = -63;
int battery_percent = 87;
char letter = 'B';
int bahn = 3;
int current_lauf = 5;
bool isRunning = false;
uint64_t start_ts_ms = 0;
uint64_t last_stop_time_ms = 4056;

String swimmerName = "Max Mustermann";
String discipline = "50m Freistil";

// Views
enum View { V_NONE, V_BAHN, V_START, V_FINISH, V_PAIRING, V_WAIT };
View views[] = { V_NONE, V_BAHN, V_START, V_FINISH, V_PAIRING, V_WAIT };
const int VIEW_COUNT = sizeof(views) / sizeof(views[0]);
const unsigned long VIEW_INTERVAL_MS = 3000;
unsigned long lastSwitch = 0;
int currentViewIndex = 0;

// Layout anchors
const int SEPARATOR_Y = 142;          // "eine Zeile" nach oben gegenüber vorher
const int MAIN_TO_LINE = 76;
const int MAIN_DOWN_OFFSET = 36;

// Offsets (wie zuvor)
const int NAME_OFFSET = 23;
const int DISCIPLINE_OFFSET = 40;
const int LAST_TIME_OFFSET = 41;
const int META_OFFSET = 61;

// Vitals and adjustments
const int VITALS_DOWN = 20;    // vitals minimal larger / slightly shifted
const int LASTBLOCK_DOWN = 20; // existing offset for "Letzte Zeit" block
const int UNDERLINE_RAISE = 8; // previous half-line raise

// WIFI bars X
const int WIFI_LEFT_X = SCREEN_W - SAFE_MARGIN - 36;

// Forward helpers
void drawStringCenteredFont(const String &s, int16_t cx, int16_t cy, uint16_t color);
void drawModeBackgroundAndBars(uint16_t accent);
void drawWifiBarsLeftToRight(int16_t x, int16_t y, int rssiVal); // left-to-right bars
void renderTopBar(uint16_t accent, const String& left, const String& right);

// Text helper
void drawStringCenteredFont(const String &s, int16_t cx, int16_t cy, uint16_t color) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);
  int16_t x = cx - w / 2;
  int16_t y = cy - h / 2;
  if (x < SAFE_MARGIN) x = SAFE_MARGIN;
  if (x + w > SCREEN_W - SAFE_MARGIN) x = SCREEN_W - SAFE_MARGIN - w;
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.print(s);
}

String fmtTime(uint64_t ms) {
  uint64_t totalSec = ms / 1000ULL;
  uint16_t millisPart = ms % 1000ULL;
  uint16_t minutes = totalSec / 60ULL;
  uint16_t seconds = totalSec % 60ULL;
  char buf[32];
  sprintf(buf, "%02u:%02u,%03u", minutes, seconds, millisPart);
  return String(buf);
}

// draw wifi bars left->right starting at left x
void drawWifiBarsLeftToRight(int16_t x, int16_t y, int rssiVal) {
  const int barW = 5;
  const int spacing = 3;
  const int baseH = 6;
  const int step = 6;
  int bars = 0;
  if (rssiVal >= -60) bars = 4;
  else if (rssiVal >= -70) bars = 3;
  else if (rssiVal >= -80) bars = 2;
  else bars = 1;
  for (int i = 0; i < 4; ++i) {
    int h = baseH + i * step;
    int x0 = x + i * (barW + spacing);
    int y0 = y - h;
    uint16_t c = (i < bars) ? rgb565(0,200,0) : GREY;
    tft.fillRect(x0, y0, barW, h, c);
  }
}

// Top bar: left large, right 'Lauf' large
void renderTopBar(uint16_t accent, const String& left, const String& right) {
  tft.fillRect(SAFE_MARGIN, SAFE_MARGIN, SCREEN_W - 2*SAFE_MARGIN, TOP_H, accent);

  // left always large
  tft.setFont(&FreeSans12pt7b);
  int16_t lx,ly; uint16_t lw,lh;
  tft.getTextBounds(left, 0,0,&lx,&ly,&lw,&lh);
  int16_t leftY = SAFE_MARGIN + (TOP_H + lh)/2 - 2;
  tft.setCursor(SAFE_MARGIN + 6, leftY);
  tft.setTextColor(TEXT_COLOR);
  tft.print(left);

  // right: larger if "Lauf"
  if (right.indexOf("Lauf") >= 0) {
    tft.setFont(&FreeSans12pt7b);
    int16_t rx,ry; uint16_t rw,rh;
    tft.getTextBounds(right, 0,0,&rx,&ry,&rw,&rh);
    int16_t rightY = SAFE_MARGIN + (TOP_H + rh)/2 - 2;
    int16_t rightX = SCREEN_W - SAFE_MARGIN - rw - 6;
    if (rightX < SAFE_MARGIN) rightX = SAFE_MARGIN;
    tft.setCursor(rightX, rightY);
    tft.setTextColor(TEXT_COLOR);
    tft.print(right);
  } else {
    tft.setFont(); tft.setTextSize(1);
    tft.setTextColor(TEXT_COLOR);
    int16_t rx,ry; uint16_t rw,rh;
    tft.getTextBounds(right, 0,0,&rx,&ry,&rw,&rh);
    int16_t rightX = SCREEN_W - SAFE_MARGIN - rw - 6;
    tft.setCursor(rightX, SAFE_MARGIN + 6);
    tft.print(right);
  }
}

// Draw bottom bar: vitals (minimal larger, grey) and wifi bars left->right
void renderBottomBar(uint16_t accent) {
  int bottomY = SCREEN_H - SAFE_MARGIN - BOTTOM_H - 2;
  tft.fillRect(SAFE_MARGIN, bottomY - 4, SCREEN_W - 2*SAFE_MARGIN, BOTTOM_H + 12, accent);

  // Vitals: minimal larger using FreeSans9pt7b, color = VITAL_COLOR (grey)
  char buf[128];
  sprintf(buf, "H %.0f%% T %.1fC  %d%% %c", humidity, temperature, battery_percent, letter);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(VITAL_COLOR);
  int vitY = bottomY + 2 - BOTTOM_LINE_SHIFT + VITALS_DOWN;
  if (vitY > SCREEN_H - SAFE_MARGIN - 2) vitY = SCREEN_H - SAFE_MARGIN - 2;
  tft.setCursor(SAFE_MARGIN + 6, vitY - 2);
  tft.print(buf);

  // wifi bars
  drawWifiBarsLeftToRight(WIFI_LEFT_X, WIFI_BARS_Y, rssi);
}

void drawModeBackgroundAndBars(uint16_t accent) {
  if (accent == BG) tft.fillScreen(BG);
  else tft.fillScreen(accent);
  renderTopBar(accent, "", "");
  renderBottomBar(accent);
}

// Views
void renderNoneView() {
  drawModeBackgroundAndBars(BG);
  renderTopBar(BG, "Status", "Lauf " + String(current_lauf));
  tft.setFont(&FreeSans12pt7b);
  drawStringCenteredFont("Nicht zugeordnet", SCREEN_W/2, 100, TEXT_COLOR);
  tft.setFont();
  drawStringCenteredFont("WLAN: Verbinde...", SCREEN_W/2, 140, ACCENT);
}

void renderBahnView() {
  drawModeBackgroundAndBars(MODE_GREEN_BG);
  renderTopBar(MODE_GREEN_BG, "Bahn " + String(bahn), "Lauf " + String(current_lauf));

  int lineY = SEPARATOR_Y;
  int mainY = lineY - MAIN_TO_LINE + MAIN_DOWN_OFFSET;

  // main time (moved up globally)
  tft.setFont(&FreeSans24pt7b);
  String timeStr = isRunning ? fmtTime(millis() - start_ts_ms) : fmtTime(last_stop_time_ms);
  int16_t bx, by; uint16_t bw, bh;
  tft.getTextBounds(timeStr, 0, 0, &bx, &by, &bw, &bh);
  int16_t mainX = SCREEN_W/2 - bw/2;
  if (mainX < SAFE_MARGIN) mainX = SAFE_MARGIN;
  tft.setCursor(mainX, mainY - bh/2);
  tft.setTextColor(TEXT_COLOR);
  tft.print(timeStr);

  // name
  tft.setFont(&FreeSans12pt7b);
  drawStringCenteredFont(swimmerName, SCREEN_W/2, mainY + NAME_OFFSET, TEXT_COLOR);

  // discipline
  tft.setFont(&FreeSans9pt7b);
  drawStringCenteredFont(discipline, SCREEN_W/2, mainY + DISCIPLINE_OFFSET, TEXT_COLOR);

  // separator line
  tft.drawFastHLine(SAFE_MARGIN + 8, lineY, SCREEN_W - 2*(SAFE_MARGIN + 8), TEXT_COLOR);

  // Move "unter Strich" elements UP by UNDERLINE_RAISE (8px) relative to previous offsets
  int raise = UNDERLINE_RAISE;

  // last time block (apply existing down offset then raise)
  tft.setFont(&FreeSans9pt7b);
  drawStringCenteredFont("Letzte Zeit", SCREEN_W/2, lineY + 14 + LASTBLOCK_DOWN - raise, TEXT_COLOR);

  tft.setFont(&FreeSans12pt7b);
  String lastTimeStr = fmtTime(last_stop_time_ms);
  drawStringCenteredFont(lastTimeStr, SCREEN_W/2, lineY + LAST_TIME_OFFSET + LASTBLOCK_DOWN - raise, TEXT_COLOR);

  tft.setFont(&FreeSans9pt7b);
  String meta = swimmerName + " / Lauf " + String(current_lauf);
  drawStringCenteredFont(meta, SCREEN_W/2, lineY + META_OFFSET + LASTBLOCK_DOWN - raise, TEXT_COLOR);
}

void renderStartView() {
  drawModeBackgroundAndBars(MODE_RED_BG);
  renderTopBar(MODE_RED_BG, "Start", "Lauf " + String(current_lauf));

  int lineY = SEPARATOR_Y;
  int mainY = lineY - MAIN_TO_LINE + MAIN_DOWN_OFFSET;

  tft.setFont(&FreeSans24pt7b);
  String timeStr = isRunning ? fmtTime(millis() - start_ts_ms) : String("00:00,000");
  int16_t bx, by; uint16_t bw, bh;
  tft.getTextBounds(timeStr, 0, 0, &bx, &by, &bw, &bh);
  int16_t mainX = SCREEN_W/2 - bw/2;
  if (mainX < SAFE_MARGIN) mainX = SAFE_MARGIN;
  tft.setCursor(mainX, mainY - bh/2);
  tft.setTextColor(TEXT_COLOR);
  tft.print(timeStr);

  tft.setFont(&FreeSans12pt7b);
  drawStringCenteredFont(swimmerName, SCREEN_W/2, mainY + NAME_OFFSET, TEXT_COLOR);
  tft.setFont(&FreeSans9pt7b);
  drawStringCenteredFont(discipline, SCREEN_W/2, mainY + DISCIPLINE_OFFSET, TEXT_COLOR);

  tft.drawFastHLine(SAFE_MARGIN + 8, lineY, SCREEN_W - 2*(SAFE_MARGIN + 8), TEXT_COLOR);

  int raise = UNDERLINE_RAISE;
  tft.setFont(&FreeSans9pt7b);
  drawStringCenteredFont("Massenstart", SCREEN_W/2, lineY + 14 + LASTBLOCK_DOWN - raise, TEXT_COLOR);
}

void renderFinishView() {
  drawModeBackgroundAndBars(MODE_BLUE_BG);
  renderTopBar(MODE_BLUE_BG, "Ziel", "Lauf " + String(current_lauf));

  int lineY = SEPARATOR_Y;
  int mainY = lineY - MAIN_TO_LINE + MAIN_DOWN_OFFSET;

  tft.setFont(&FreeSans24pt7b);
  String timeStr = isRunning ? fmtTime(millis() - start_ts_ms) : fmtTime(last_stop_time_ms);
  int16_t bx, by; uint16_t bw, bh;
  tft.getTextBounds(timeStr, 0, 0, &bx, &by, &bw, &bh);
  int16_t mainX = SCREEN_W/2 - bw/2;
  if (mainX < SAFE_MARGIN) mainX = SAFE_MARGIN;
  tft.setCursor(mainX, mainY - bh/2);
  tft.setTextColor(TEXT_COLOR);
  tft.print(timeStr);

  tft.setFont(&FreeSans12pt7b);
  drawStringCenteredFont(swimmerName, SCREEN_W/2, mainY + NAME_OFFSET, TEXT_COLOR);
  tft.setFont(&FreeSans9pt7b);
  drawStringCenteredFont(discipline, SCREEN_W/2, mainY + DISCIPLINE_OFFSET, TEXT_COLOR);

  tft.drawFastHLine(SAFE_MARGIN + 8, lineY, SCREEN_W - 2*(SAFE_MARGIN + 8), TEXT_COLOR);

  int raise = UNDERLINE_RAISE;
  tft.setFont(&FreeSans9pt7b);
  drawStringCenteredFont("Zieleinlauf-Backup", SCREEN_W/2, lineY + 14 + LASTBLOCK_DOWN - raise, TEXT_COLOR);
}

void renderPairingView() {
  drawModeBackgroundAndBars(BG);
  renderTopBar(BG, "Pairing-Modus", "");
  tft.setFont(&FreeSans12pt7b);
  drawStringCenteredFont("Verbinde STPW-SETUP...", SCREEN_W/2, 120, TEXT_COLOR);
}

void renderWaitingView() {
  drawModeBackgroundAndBars(BG);
  renderTopBar(BG, "Warten", "");
  tft.setFont(&FreeSans12pt7b);
  drawStringCenteredFont("Suche Stoppuhr-WLAN...", SCREEN_W/2, 120, ACCENT);
}

void renderView() {
  switch (views[currentViewIndex]) {
    case V_NONE:    renderNoneView(); break;
    case V_BAHN:    renderBahnView(); break;
    case V_START:   renderStartView(); break;
    case V_FINISH:  renderFinishView(); break;
    case V_PAIRING: renderPairingView(); break;
    case V_WAIT:    renderWaitingView(); break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Display Views Test - v3 raise one line, text colors adjusted");

  spi.begin(PIN_SCLK, -1, PIN_MOSI, PIN_CS);
  delay(50);
  tft.init(240, 280);
  tft.setRotation(0);
  tft.setTextWrap(false);

  drawModeBackgroundAndBars(BG);

  isRunning = false;
  last_stop_time_ms = 4056;
  start_ts_ms = millis();

  lastSwitch = millis() - VIEW_INTERVAL_MS;
}

void loop() {
  unsigned long now = millis();
  if (now - lastSwitch >= VIEW_INTERVAL_MS) {
    lastSwitch = now;
    currentViewIndex = (currentViewIndex + 1) % VIEW_COUNT;
    renderView();
  }

  static unsigned long lastToggle = 0;
  if (now - lastToggle > VIEW_INTERVAL_MS * VIEW_COUNT * 2) {
    lastToggle = now;
    isRunning = !isRunning;
    if (isRunning) start_ts_ms = now;
    else last_stop_time_ms = now - start_ts_ms;
  }

  delay(20);
}