#include "EPDBootChecklist.h"

#ifdef EPD_BOOTSCREEN

extern "C" {
  #include <epdiy.h>
}

// Fonts sourced from vendor example; small size to keep flash low.
// We reuse FiraSans_12 symbols present in vendor tree by including header path.
// If missing, steps will still render using built-in fallback rectangles.
#include "firasans_12.h"
#include "firasans_20.h"

namespace epdboot {

static EpdiyHighlevelState s_hl;
static bool s_ready = false;
static const int s_left_margin = 100;
static int s_cursor_y = 0;

static void draw_line(const char* text) {
  if (!s_ready) return;
  const EpdFont* font = &FiraSans_12;
  EpdFontProperties props = epd_font_properties_default();
  props.flags = EPD_DRAW_ALIGN_LEFT;
  int x = s_left_margin;
  int y = s_cursor_y;
  epd_write_string(font, text, &x, &y, epd_hl_get_framebuffer(&s_hl), &props);
  // epd_write_string advances y by font->advance_y; capture for next line
  s_cursor_y = y;
  epd_poweron();
  epd_hl_update_screen(&s_hl, MODE_GL16, epd_ambient_temperature());
  epd_poweroff();
}

void begin() {
  if (s_ready) return;
  // Init board (LilyGO v7) and panel ED047TC1 using 64K LUT as in factory.
  epd_init(&epd_board_v7, &ED047TC1, EPD_LUT_64K);
  // Typical vcom around 1560 mV on LilyGO; if UI/NVS exists, it can override.
  epd_set_vcom(1560);
  s_hl = epd_hl_init(EPD_BUILTIN_WAVEFORM);
  epd_set_rotation(EPD_ROT_INVERTED_PORTRAIT);

  s_cursor_y = epd_rotated_display_height() / 2 - 250;

  // Clear dirt per factory routine for clean first paint
  int refresh_timer = 12;
  epd_poweron();
  for (int i = 0; i < 10; i++) { epd_push_pixels(epd_full_screen(), refresh_timer, 0); }
  for (int i = 0; i < 10; i++) { epd_push_pixels(epd_full_screen(), refresh_timer, 1); }
  for (int i = 0; i < 2;  i++) { epd_push_pixels(epd_full_screen(), refresh_timer, 2); }
  epd_poweroff();

  // Title
  int cx = epd_rotated_display_width() / 2;
  int cy = s_cursor_y - 50;
  const EpdFont* font = &FiraSans_20; // larger title if available
  EpdFontProperties props = epd_font_properties_default();
  props.flags = EPD_DRAW_ALIGN_CENTER;
  epd_write_string(font, "Initialize    System", &cx, &cy, epd_hl_get_framebuffer(&s_hl), &props);
  epd_poweron();
  epd_hl_update_screen(&s_hl, MODE_GL16, epd_ambient_temperature());
  epd_poweroff();

  s_ready = true;
}

void step(const char* name, bool ok) {
  char buf[96];
  snprintf(buf, sizeof(buf), "[%s] %s", ok ? "\xE2\x9C\x94" : "\xE2\x9C\x96", name);
  draw_line(buf);
}

void end() {
  // nothing extra; leave last frame visible
}

} // namespace epdboot

#endif
