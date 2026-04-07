# Taster — Firmware / Display Sketch (v0.1.1)

Diese Version (v0.1.1) enthält ein Display‑Sketch für ein Adafruit ST7789 Display mit angepasstem Layout:
- Hauptzeitblock nach oben verschoben
- Top-/Bottom‑Leisten farbig
- Trennstrich & Unteres Panel feinjustiert
- Texte: alle grauen Texte → weiß, Vitaldaten → grau

Inhalt dieses Releases:
- firmware/display_views_test_adafruit_rounded_fonts_layout_edgefill_v3_raise_one_line_text_colors.ino
- CHANGELOG.md (v0.1.1)
- LICENSE (MIT)
- .gitignore

Verkabelung (Beispiel: XIAO_ESP32S3)
- MOSI -> GPIO5
- SCLK -> GPIO4
- CS   -> GPIO3
- DC   -> GPIO43
- RST  -> GPIO44
- Backlight: 3.3V / PWM je nach Hardware

Benötigte Arduino Libraries
- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library
- (ESP32 core für XIAO_ESP32S3)

Kompilieren & Upload
1. Bibliotheken über den Library Manager installieren.
2. Board auf `XIAO_ESP32S3` stellen (ESP32 core).
3. Sketch öffnen: `firmware/display_views_test_adafruit_rounded_fonts_layout_edgefill_v3_raise_one_line_text_colors.ino`
4. Kompilieren & Hochladen.

Anpassungen
- Die Layout‑Konstanten (SEPARATOR_Y, MAIN_DOWN_OFFSET, NAME_OFFSET, ...) sind im Sketch oben definiert und können zur Feinjustierung geändert werden.
- Farben: `MODE_*_BG` und `TEXT_COLOR` / `VITAL_COLOR` direkt anpassen.

Kontakt / Entwicklung
- Repo: https://github.com/andreasschoeler93/Taster
- Issues & Feature Requests bitte im Repo als Issues anlegen.
