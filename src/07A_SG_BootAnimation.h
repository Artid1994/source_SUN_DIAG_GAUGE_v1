#pragma once
/*
  07A_SG_BootAnimation.h
  SUN DIAG GAUGE v1.1.1 BETA11R2 PAGE1 FIRST FRAME HANDOFF

  Base:
  - SUN_DIAG_GAUGE_v1_1_1_BETA10R3_SMOOTH_PAGE1_HANDOFF

  Architecture:
  - LVGL 8.3 native animations (lv_anim_t).
  - Non-blocking: no while-loop, delay(1), micros frame scheduler,
    manual lv_tick_inc, or manual lv_timer_handler in this file.
  - Wordmark uses two labels instead of hundreds of rectangle objects.
  - Objects are created once and removed once at the handoff.
  - Main loop continues to service LVGL; ECU/K-Line startup remains deferred
    until the Boot animation reports completion.
*/

static bool sgBoot11Active = false;
static bool sgBoot11FinishPending = false;
static bool sgBoot11Page1FirstFramePending = false;

static lv_obj_t *sgBoot11Screen = nullptr;
static lv_obj_t *sgBoot11Content = nullptr;
static lv_obj_t *sgBoot11RingBg = nullptr;
static lv_obj_t *sgBoot11RingScan = nullptr;
static lv_obj_t *sgBoot11RingInner = nullptr;
static lv_obj_t *sgBoot11EnergyL = nullptr;
static lv_obj_t *sgBoot11EnergyR = nullptr;
static lv_obj_t *sgBoot11LogoSmart = nullptr;
static lv_obj_t *sgBoot11LogoGauge = nullptr;
static lv_obj_t *sgBoot11UnderlineTrack = nullptr;
static lv_obj_t *sgBoot11Underline = nullptr;
static lv_obj_t *sgBoot11Runner = nullptr;
static lv_obj_t *sgBoot11Subtitle = nullptr;
static lv_obj_t *sgBoot11Credit = nullptr;
static lv_obj_t *sgBoot11ScanTop = nullptr;
static lv_obj_t *sgBoot11ScanBottom = nullptr;
static lv_obj_t *sgBoot11ReadyBorder = nullptr;
static lv_obj_t *sgBoot11CycleDot[3] = {nullptr, nullptr, nullptr};

static lv_obj_t *sgBoot11Rect(lv_obj_t *parent, int x, int y, int w, int h,
                              lv_color_t color, lv_opa_t opa = LV_OPA_COVER,
                              int radius = 0) {
  lv_obj_t *obj = lv_obj_create(parent);
  lv_obj_remove_style_all(obj);
  lv_obj_set_pos(obj, x, y);
  lv_obj_set_size(obj, w, h);
  lv_obj_set_style_bg_color(obj, color, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(obj, opa, LV_PART_MAIN);
  lv_obj_set_style_radius(obj, radius, LV_PART_MAIN);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  return obj;
}

static lv_obj_t *sgBoot11Label(lv_obj_t *parent, const char *text,
                               const lv_font_t *font, lv_color_t color,
                               int x, int y, int w) {
  lv_obj_t *label = lv_label_create(parent);
  lv_label_set_text(label, text);
  lv_obj_set_pos(label, x, y);
  lv_obj_set_width(label, w);
  lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
  lv_obj_set_style_text_color(label, color, LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_clear_flag(label, LV_OBJ_FLAG_SCROLLABLE);
  return label;
}

static lv_obj_t *sgBoot11Arc(lv_obj_t *parent, int size, int width,
                             lv_color_t color, int startAngle, int endAngle) {
  lv_obj_t *arc = lv_arc_create(parent);
  lv_obj_set_size(arc, size, size);
  lv_obj_center(arc);
  lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_clear_flag(arc, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_arc_opa(arc, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_arc_width(arc, width, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(arc, color, LV_PART_INDICATOR);
  lv_obj_set_style_arc_rounded(arc, true, LV_PART_INDICATOR);
  lv_obj_set_style_bg_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);
  lv_obj_set_style_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);
  lv_arc_set_angles(arc, startAngle, endAngle);
  return arc;
}

/* ---------- Native animation callbacks ---------- */

static void sgBoot11AnimOpa(void *var, int32_t value) {
  lv_obj_set_style_opa((lv_obj_t *)var, (lv_opa_t)value, LV_PART_MAIN);
}

static void sgBoot11AnimX(void *var, int32_t value) {
  lv_obj_set_x((lv_obj_t *)var, (lv_coord_t)value);
}

static void sgBoot11AnimY(void *var, int32_t value) {
  lv_obj_set_y((lv_obj_t *)var, (lv_coord_t)value);
}

static void sgBoot11AnimWidth(void *var, int32_t value) {
  lv_obj_set_width((lv_obj_t *)var, (lv_coord_t)value);
}

static void sgBoot11AnimRingRotation(void *var, int32_t value) {
  lv_arc_set_rotation((lv_obj_t *)var, (uint16_t)value);
}

static void sgBoot11AnimEnergyLeft(void *var, int32_t value) {
  lv_obj_t *obj = (lv_obj_t *)var;
  lv_obj_set_x(obj, 160 - value);
  lv_obj_set_width(obj, value);
}

static void sgBoot11AnimEnergyRight(void *var, int32_t value) {
  lv_obj_t *obj = (lv_obj_t *)var;
  lv_obj_set_x(obj, 160);
  lv_obj_set_width(obj, value);
}

static void sgBoot11AnimLoadWidth(void *var, int32_t value) {
  lv_obj_t *obj = (lv_obj_t *)var;
  lv_obj_set_x(obj, 35);
  lv_obj_set_width(obj, value);
}

static void sgBoot11AnimCycleDots(void *, int32_t value) {
  const uint8_t completed =
      (value >= 1400) ? 3U : ((value >= 700) ? 2U : ((value > 0) ? 1U : 0U));

  for (uint8_t i = 0; i < 3U; ++i) {
    const lv_opa_t opa = (i < completed) ? LV_OPA_COVER : LV_OPA_30;
    lv_obj_set_style_opa(sgBoot11CycleDot[i], opa, LV_PART_MAIN);
  }
}

static void sgBoot11AnimFinished(lv_anim_t *) {
  sgBoot11FinishPending = true;
}

static void sgBoot11StartAnim(lv_obj_t *obj,
                              lv_anim_exec_xcb_t exec,
                              int32_t from,
                              int32_t to,
                              uint32_t timeMs,
                              uint32_t delayMs,
                              lv_anim_path_cb_t path = lv_anim_path_ease_in_out,
                              uint16_t repeatCount = 0,
                              uint32_t repeatDelayMs = 0,
                              lv_anim_ready_cb_t ready = nullptr) {
  lv_anim_t anim;
  lv_anim_init(&anim);
  lv_anim_set_var(&anim, obj);
  lv_anim_set_exec_cb(&anim, exec);
  lv_anim_set_values(&anim, from, to);
  lv_anim_set_time(&anim, timeMs);
  lv_anim_set_delay(&anim, delayMs);
  lv_anim_set_path_cb(&anim, path);
  lv_anim_set_repeat_count(&anim, repeatCount);
  lv_anim_set_repeat_delay(&anim, repeatDelayMs);
  if (ready) lv_anim_set_ready_cb(&anim, ready);
  lv_anim_start(&anim);
}

static void sgBoot11DeleteAnimations() {
  lv_anim_del(sgBoot11RingScan, nullptr);
  lv_anim_del(sgBoot11EnergyL, nullptr);
  lv_anim_del(sgBoot11EnergyR, nullptr);
  lv_anim_del(sgBoot11LogoSmart, nullptr);
  lv_anim_del(sgBoot11LogoGauge, nullptr);
  lv_anim_del(sgBoot11Underline, nullptr);
  lv_anim_del(sgBoot11Runner, nullptr);
  lv_anim_del(sgBoot11Subtitle, nullptr);
  lv_anim_del(sgBoot11Credit, nullptr);
  lv_anim_del(sgBoot11ScanTop, nullptr);
  lv_anim_del(sgBoot11ScanBottom, nullptr);
  lv_anim_del(sgBoot11ReadyBorder, nullptr);
  lv_anim_del(sgBoot11Content, nullptr);
}

/* ---------- Public Boot API ---------- */

static bool sgUiBootAnimationActive() {
  return sgBoot11Active;
}

static void sgUiPlayBootAnimation() {
  if (!sgLvReady || sgBoot11Active) return;

  const lv_color_t black = lv_color_hex(0x000000);
  const lv_color_t red = lv_color_hex(0xFF1638);
  const lv_color_t redBright = lv_color_hex(0xFF0033);
  const lv_color_t redDark = lv_color_hex(0x62000D);
  const lv_color_t gray = lv_color_hex(0x2A2A2A);
  const lv_color_t white = lv_color_hex(0xF4F4F4);
  const lv_color_t sub = lv_color_hex(0xA9ADB0);

  sgBoot11Active = true;
  sgBoot11FinishPending = false;

  sgBoot11Screen = lv_scr_act();
  lv_obj_clean(sgBoot11Screen);
  lv_obj_clear_flag(sgBoot11Screen, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_bg_color(sgBoot11Screen, black, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(sgBoot11Screen, LV_OPA_COVER, LV_PART_MAIN);

  sgBoot11Content = lv_obj_create(sgBoot11Screen);
  lv_obj_remove_style_all(sgBoot11Content);
  lv_obj_set_size(sgBoot11Content, 320, 240);
  lv_obj_set_pos(sgBoot11Content, 0, 0);
  lv_obj_clear_flag(sgBoot11Content, LV_OBJ_FLAG_SCROLLABLE);

  /* Static decorations: never animated, therefore no repeated invalidation. */
  sgBoot11Rect(sgBoot11Content, 20, 34, 280, 1, gray, LV_OPA_40);
  sgBoot11Rect(sgBoot11Content, 20, 205, 280, 1, gray, LV_OPA_40);
  sgBoot11Rect(sgBoot11Content, 26, 48, 1, 144, gray, LV_OPA_30);
  sgBoot11Rect(sgBoot11Content, 293, 48, 1, 144, gray, LV_OPA_30);

  sgBoot11Rect(sgBoot11Content, 8, 8, 30, 2, redDark);
  sgBoot11Rect(sgBoot11Content, 8, 8, 2, 20, redDark);
  sgBoot11Rect(sgBoot11Content, 282, 8, 30, 2, redDark);
  sgBoot11Rect(sgBoot11Content, 310, 8, 2, 20, redDark);
  sgBoot11Rect(sgBoot11Content, 8, 230, 30, 2, redDark);
  sgBoot11Rect(sgBoot11Content, 8, 212, 2, 20, redDark);
  sgBoot11Rect(sgBoot11Content, 282, 230, 30, 2, redDark);
  sgBoot11Rect(sgBoot11Content, 310, 212, 2, 20, redDark);

  lv_obj_t *halo = lv_obj_create(sgBoot11Content);
  lv_obj_remove_style_all(halo);
  lv_obj_set_pos(halo, 38, 58);
  lv_obj_set_size(halo, 244, 124);
  lv_obj_set_style_bg_color(halo, lv_color_hex(0x120004), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(halo, LV_OPA_70, LV_PART_MAIN);
  lv_obj_set_style_border_width(halo, 1, LV_PART_MAIN);
  lv_obj_set_style_border_color(halo, redDark, LV_PART_MAIN);
  lv_obj_set_style_border_opa(halo, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_radius(halo, 18, LV_PART_MAIN);
  lv_obj_clear_flag(halo, LV_OBJ_FLAG_SCROLLABLE);

  sgBoot11RingBg = sgBoot11Arc(sgBoot11Content, 184, 2, redDark, 0, 300);
  sgBoot11RingScan = sgBoot11Arc(sgBoot11Content, 184, 5, redBright, 0, 52);
  sgBoot11RingInner = sgBoot11Arc(sgBoot11Content, 154, 3,
                                  lv_color_hex(0x7A0A1B), 0, 235);
  lv_arc_set_rotation(sgBoot11RingBg, 195);
  lv_arc_set_rotation(sgBoot11RingScan, 270);
  lv_arc_set_rotation(sgBoot11RingInner, 25);

  sgBoot11EnergyL = sgBoot11Rect(sgBoot11Content, 160, 119, 1, 3,
                                  redBright, LV_OPA_COVER, 1);
  sgBoot11EnergyR = sgBoot11Rect(sgBoot11Content, 160, 119, 1, 3,
                                  white, LV_OPA_COVER, 1);

  /*
    Lightweight wordmark:
    2 labels replace the old 5x7 bitmap made from hundreds of LVGL objects.
    Both fonts are already enabled in the confirmed project lv_conf.h.
  */
  sgBoot11LogoSmart = sgBoot11Label(sgBoot11Content, "SMART",
                                     &lv_font_montserrat_36, red,
                                     24, 92, 134);
  sgBoot11LogoGauge = sgBoot11Label(sgBoot11Content, "GAUGE",
                                     &lv_font_montserrat_36, white,
                                     150, 92, 146);
  lv_obj_set_style_text_letter_space(sgBoot11LogoSmart, 1, LV_PART_MAIN);
  lv_obj_set_style_text_letter_space(sgBoot11LogoGauge, 1, LV_PART_MAIN);

  sgBoot11UnderlineTrack = sgBoot11Rect(sgBoot11Content, 35, 142, 250, 1,
                                         redDark, LV_OPA_COVER, 1);
  sgBoot11Underline = sgBoot11Rect(sgBoot11Content, 35, 141, 1, 3,
                                    redBright, LV_OPA_COVER, 1);
  sgBoot11Runner = sgBoot11Rect(sgBoot11Content, 34, 140, 20, 4,
                                 white, LV_OPA_COVER, 2);

  sgBoot11Subtitle = sgBoot11Label(sgBoot11Content,
                                    "RESEARCH AND DEVELOPMENT",
                                    &lv_font_montserrat_14, sub,
                                    10, 159, 300);
  sgBoot11Credit = sgBoot11Label(sgBoot11Content,
                                  "GITHUB.COM/ARTID1994",
                                  &lv_font_montserrat_14, red,
                                  10, 181, 300);
  lv_obj_set_style_text_letter_space(sgBoot11Subtitle, 1, LV_PART_MAIN);
  lv_obj_set_style_text_letter_space(sgBoot11Credit, 2, LV_PART_MAIN);

  sgBoot11ScanTop = sgBoot11Rect(sgBoot11Content, -50, 54, 50, 1, redBright);
  sgBoot11ScanBottom = sgBoot11Rect(sgBoot11Content, 320, 188, 38, 1, redDark);

  sgBoot11CycleDot[0] = sgBoot11Rect(sgBoot11Content, 145, 207, 8, 3,
                                      redDark, LV_OPA_COVER, 1);
  sgBoot11CycleDot[1] = sgBoot11Rect(sgBoot11Content, 156, 207, 8, 3,
                                      redDark, LV_OPA_COVER, 1);
  sgBoot11CycleDot[2] = sgBoot11Rect(sgBoot11Content, 167, 207, 8, 3,
                                      redDark, LV_OPA_COVER, 1);

  sgBoot11ReadyBorder = lv_obj_create(sgBoot11Content);
  lv_obj_remove_style_all(sgBoot11ReadyBorder);
  lv_obj_set_pos(sgBoot11ReadyBorder, 4, 4);
  lv_obj_set_size(sgBoot11ReadyBorder, 312, 232);
  lv_obj_set_style_bg_opa(sgBoot11ReadyBorder, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(sgBoot11ReadyBorder, 2, LV_PART_MAIN);
  lv_obj_set_style_border_color(sgBoot11ReadyBorder, redBright, LV_PART_MAIN);
  lv_obj_set_style_border_opa(sgBoot11ReadyBorder, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_radius(sgBoot11ReadyBorder, 7, LV_PART_MAIN);
  lv_obj_clear_flag(sgBoot11ReadyBorder, LV_OBJ_FLAG_SCROLLABLE);

  /* Initial visibility. */
  lv_obj_set_style_opa(sgBoot11RingBg, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11RingScan, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11RingInner, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11EnergyL, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11EnergyR, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11LogoSmart, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11LogoGauge, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11UnderlineTrack, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11Underline, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11Runner, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11Subtitle, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11Credit, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11ScanTop, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11ScanBottom, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_opa(sgBoot11ReadyBorder, LV_OPA_TRANSP, LV_PART_MAIN);
  for (uint8_t i = 0; i < 3U; ++i) {
    lv_obj_set_style_opa(sgBoot11CycleDot[i], LV_OPA_TRANSP, LV_PART_MAIN);
  }

  /*
    Timeline, total ~5.05 seconds:
    0-0.95s    energy + ring scan
    0.45-1.15s wordmark
    0.90-1.60s subtitle/credit
    1.75-3.85s three loading passes
    3.85-4.25s ready pulse
    4.55-5.05s content fade
  */

  sgBoot11StartAnim(sgBoot11EnergyL, sgBoot11AnimEnergyLeft,
                    1, 145, 500, 0, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11EnergyR, sgBoot11AnimEnergyRight,
                    1, 145, 500, 0, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11EnergyL, sgBoot11AnimOpa,
                    0, 255, 250, 0, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11EnergyR, sgBoot11AnimOpa,
                    0, 255, 250, 0, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11EnergyL, sgBoot11AnimOpa,
                    255, 0, 200, 560);
  sgBoot11StartAnim(sgBoot11EnergyR, sgBoot11AnimOpa,
                    255, 0, 200, 560);

  sgBoot11StartAnim(sgBoot11RingBg, sgBoot11AnimOpa,
                    0, 120, 420, 100, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11RingInner, sgBoot11AnimOpa,
                    0, 150, 420, 100, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11RingScan, sgBoot11AnimOpa,
                    0, 255, 420, 100, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11RingScan, sgBoot11AnimRingRotation,
                    270, 490, 870, 80);

  sgBoot11StartAnim(sgBoot11LogoSmart, sgBoot11AnimOpa,
                    0, 255, 700, 450, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11LogoGauge, sgBoot11AnimOpa,
                    0, 255, 700, 450, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11LogoSmart, sgBoot11AnimY,
                    98, 92, 700, 450, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11LogoGauge, sgBoot11AnimY,
                    98, 92, 700, 450, lv_anim_path_ease_out);

  sgBoot11StartAnim(sgBoot11UnderlineTrack, sgBoot11AnimOpa,
                    0, 130, 500, 850, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11Subtitle, sgBoot11AnimOpa,
                    0, 225, 500, 900, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11Subtitle, sgBoot11AnimY,
                    164, 159, 500, 900, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11Credit, sgBoot11AnimOpa,
                    0, 220, 450, 1150, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11Credit, sgBoot11AnimY,
                    186, 181, 450, 1150, lv_anim_path_ease_out);

  sgBoot11StartAnim(sgBoot11ScanTop, sgBoot11AnimOpa,
                    0, 145, 250, 700, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11ScanTop, sgBoot11AnimX,
                    -50, 370, 900, 700);
  sgBoot11StartAnim(sgBoot11ScanTop, sgBoot11AnimOpa,
                    145, 0, 250, 1350);
  sgBoot11StartAnim(sgBoot11ScanBottom, sgBoot11AnimOpa,
                    0, 95, 250, 900, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11ScanBottom, sgBoot11AnimX,
                    320, -80, 900, 900);
  sgBoot11StartAnim(sgBoot11ScanBottom, sgBoot11AnimOpa,
                    95, 0, 250, 1550);

  /* Three native loading cycles; repeat_count=2 means 3 total passes. */
  sgBoot11StartAnim(sgBoot11Runner, sgBoot11AnimOpa,
                    0, 240, 200, 1650, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11Runner, sgBoot11AnimX,
                    34, 266, 700, 1750, lv_anim_path_ease_in_out, 2, 0);
  sgBoot11StartAnim(sgBoot11Underline, sgBoot11AnimOpa,
                    0, 255, 150, 1700, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11Underline, sgBoot11AnimLoadWidth,
                    1, 250, 700, 1750, lv_anim_path_ease_in_out, 2, 0);

  lv_anim_t dotAnim;
  lv_anim_init(&dotAnim);
  lv_anim_set_var(&dotAnim, nullptr);
  lv_anim_set_exec_cb(&dotAnim, sgBoot11AnimCycleDots);
  lv_anim_set_values(&dotAnim, 0, 2100);
  lv_anim_set_time(&dotAnim, 2100);
  lv_anim_set_delay(&dotAnim, 1750);
  lv_anim_set_path_cb(&dotAnim, lv_anim_path_linear);
  lv_anim_start(&dotAnim);

  sgBoot11StartAnim(sgBoot11ReadyBorder, sgBoot11AnimOpa,
                    0, 190, 200, 3850, lv_anim_path_ease_out);
  sgBoot11StartAnim(sgBoot11ReadyBorder, sgBoot11AnimOpa,
                    190, 0, 200, 4050, lv_anim_path_ease_in);

  /*
    Fade only the content container. The screen background stays black.
    This avoids cross-fading Page 1 and Boot at the same time.
  */
  sgBoot11StartAnim(sgBoot11Content, sgBoot11AnimOpa,
                    255, 0, 240, 4550, lv_anim_path_ease_in_out,
                    0, 0, sgBoot11AnimFinished);
}

/*
  Call once per main loop before sgUiTask().
  The actual Page 1 rebuild occurs outside the LVGL animation callback.
*/
static void sgUiBootAnimationTask() {
  if (!sgBoot11Active || !sgBoot11FinishPending) return;

  sgBoot11FinishPending = false;
  sgBoot11DeleteAnimations();

  sgPage = SG_PAGE_GAUGE;
  sgLvNeedBuild = true;
  sgLvBuildPage();
  sgLvUpdateCommon();
  sgLvUpdateGauge(millis());

  sgBoot11Active = false;
  sgBoot11Page1FirstFramePending = true;
  sgBoot11Screen = nullptr;
  sgBoot11Content = nullptr;
}
