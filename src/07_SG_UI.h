#pragma once
#include "assets/07B_SG_RacingPage1Asset.h"
#include "assets/07C_SG_RacingPage1DayAsset.h"
#include "assets/07D_SG_BluetoothIconAsset.h"
/*
  07_SG_UI.h — SUN DIAG GAUGE v1.1.0 BETA47 — LVGL Opacity Compile Fix

  เป้าหมาย:
  - ใช้ระบบ ECU/K-Line/Decode/DTC/Bluetooth/Input จากฐานที่ผ่านการทดสอบ แล้วเริ่มชื่อใหม่ v1.0.0.1
  - เปลี่ยนเฉพาะชั้นแสดงผลจากการวาด TFT_eSPI โดยตรงเป็น LVGL 8.3.x
  - ใช้ Partial Draw Buffer เพื่อลด RAM และลดการกระพริบ
  - Touch ถูกปิด ใช้ TTP223 ปุ่มเดียวตามแนวทาง SUN DIAG GAUGE

  หมายเหตุ:
  - Target: LVGL 8.3.x เท่านั้นใน Stable Candidate นี้
  - TFT_eSPI ยังทำหน้าที่เป็น Display Driver ให้ LVGL
*/

#include <Arduino.h>
#ifndef LV_CONF_INCLUDE_SIMPLE
#define LV_CONF_INCLUDE_SIMPLE
#endif
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <string.h>

LV_FONT_DECLARE(sg_font_thai_14);

#if LVGL_VERSION_MAJOR != 8
#error "SUN DIAG GAUGE v1.0.0.1 LVGL UI requires LVGL 8.3.x"
#endif

// -------------------- Display driver --------------------
// LVGL display driver configuration:
//  1. sgLvDrawBuf: draw buffer descriptor (2 segments, 320x16 each)
//  2. sgLvBuf1: main color frame buffer for ILI9341 display
//     - Size: 320 x 16 = 5,120 bytes (5 KB) per segment
//     - Total: 2 x 5 KB = 10 KB in BSS (no heap allocation)
//     - Rationale: 16 scanlines per segment reduces SPI flush rounds,
//       improving rendering speed on 320x240 ILI9341.
//     - RAM budget: 10 KB of 327,680 B dynamic memory (~3%).
static lv_disp_draw_buf_t sgLvDrawBuf;
static lv_color_t sgLvBuf1[320 * 16]; // SC: เพิ่มเป็น 16 scanlines ลดจำนวนรอบ flush บน SPI
static lv_disp_drv_t sgLvDispDrv;
static bool sgLvReady = false;
static uint32_t sgLvLastTickMs = 0;
static volatile uint32_t sgScreenCaptureFrameId = 0;

static void sgLvFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *colorP) {
  const uint32_t w = (uint32_t)(area->x2 - area->x1 + 1);
  const uint32_t h = (uint32_t)(area->y2 - area->y1 + 1);
  sgScreenCaptureFrameId++;
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&colorP->full, w * h, true);
  tft.endWrite();
  lv_disp_flush_ready(disp);
}

// -------------------- Theme colors --------------------
static lv_color_t SG_LV_BG;
static lv_color_t SG_LV_PANEL;
static lv_color_t SG_LV_BORDER;
static lv_color_t SG_LV_YELLOW;
static lv_color_t SG_LV_RED;
static lv_color_t SG_LV_GREEN;
static lv_color_t SG_LV_CYAN;
static lv_color_t SG_LV_ORANGE;
static lv_color_t SG_LV_WHITE;
static lv_color_t SG_LV_DIM;

// BETA80: ตารางสี Accent กลาง 8 สี ใช้ลำดับเดียวกับ SgAccentTheme
// ป้องกันค่า Runtime/NVS หลุดช่วงโดยย้อนกลับเป็น Racing Red โดยไม่รีสตาร์ต
static const uint32_t SG_LV_ACCENT_RGB[SG_ACCENT_COUNT] = {
  0xE00018, // RED
  0x168BFF, // BLUE
  0x25D366, // GREEN
  0xFF7A00, // ORANGE
  0xA855F7, // PURPLE
  0x7A828A, // STEALTH BLACK: เทาเงินเพื่อให้เห็นบนพื้นดำ
  0xD5D9DC, // PEARL SILVER
  0xFFD21F  // CHEEKY YELLOW
};

static lv_color_t sgLvAccentColor() {
  const uint8_t safeIndex = (sgAccentTheme < SG_ACCENT_COUNT) ? sgAccentTheme : SG_ACCENT_RED;
  return lv_color_hex(SG_LV_ACCENT_RGB[safeIndex]);
}

static uint8_t sgLvSafeAccentIndex() {
  return (sgAccentTheme < SG_ACCENT_COUNT) ? sgAccentTheme : SG_ACCENT_RED;
}

static lv_color_t sgLvThemeNightBg() {
  static const uint32_t rgb[SG_ACCENT_COUNT] = {
    0x120307, 0x03101D, 0x03160C, 0x1A0B02,
    0x10051A, 0x050607, 0x101316, 0x171300
  };
  return lv_color_hex(rgb[sgLvSafeAccentIndex()]);
}

static lv_color_t sgLvThemeNightPanel() {
  static const uint32_t rgb[SG_ACCENT_COUNT] = {
    0x260A10, 0x0A263A, 0x092B19, 0x311706,
    0x281034, 0x111416, 0x252B30, 0x302805
  };
  return lv_color_hex(rgb[sgLvSafeAccentIndex()]);
}

static lv_color_t sgLvThemeDayBg() {
  static const uint32_t rgb[SG_ACCENT_COUNT] = {
    0xF3EDEF, 0xEAF2F8, 0xEAF5EE, 0xF8F0E8,
    0xF2ECF7, 0xECEEEF, 0xF1F3F4, 0xF7F4E5
  };
  return lv_color_hex(rgb[sgLvSafeAccentIndex()]);
}

static lv_color_t sgLvThemeDayPanel() {
  static const uint32_t rgb[SG_ACCENT_COUNT] = {
    0xFFF8FA, 0xF7FBFF, 0xF7FFF9, 0xFFF9F3,
    0xFCF8FF, 0xF7F8F8, 0xFFFFFF, 0xFFFDF1
  };
  return lv_color_hex(rgb[sgLvSafeAccentIndex()]);
}

static lv_color_t sgLvThemeRootColor() {
  return sgDayThemeActive ? sgLvThemeDayBg() : sgLvThemeNightBg();
}

static lv_color_t sgLvThemePanelColor() {
  return sgDayThemeActive ? sgLvThemeDayPanel() : sgLvThemeNightPanel();
}

static void sgLvApplyThemePalette() {
  if (sgDayThemeActive) {
    // Day mode: พื้นขาว/เทาอ่อน อ่านตัวเลขดำกลางแจ้งได้ง่าย
    SG_LV_BG = sgLvThemeDayBg();
    SG_LV_PANEL = sgLvThemeDayPanel();
    SG_LV_BORDER = sgLvAccentColor();
    SG_LV_YELLOW = lv_color_hex(0xB06A00);
    SG_LV_RED = lv_color_hex(0xC51616);
    SG_LV_GREEN = lv_color_hex(0x087A2D);
    SG_LV_CYAN = lv_color_hex(0x006B8F);
    SG_LV_ORANGE = lv_color_hex(0xB84A00);
    SG_LV_WHITE = lv_color_hex(0x111111);
    SG_LV_DIM = lv_color_hex(0x4F5960);
  } else {
    // Night/Auto-night แบบ cockpit สีน้ำเงินเข้ม ไม่ดำทึบเกินไป
    SG_LV_BG = sgLvThemeNightBg();
    SG_LV_PANEL = sgLvThemeNightPanel();
    SG_LV_BORDER = sgLvAccentColor();
    SG_LV_YELLOW = lv_color_hex(0xFFD84A);
    SG_LV_RED = lv_color_hex(0xFF4B3E);
    SG_LV_GREEN = lv_color_hex(0x57F075);
    SG_LV_CYAN = lv_color_hex(0x45D2FF);
    SG_LV_ORANGE = lv_color_hex(0xFFAA35);
    SG_LV_WHITE = lv_color_hex(0xF4F7FA);
    SG_LV_DIM = lv_color_hex(0xC0CAD0);
  }
}

// -------------------- UI state --------------------
static bool sgLvNeedBuild = true;
static int8_t sgLvLastPage = -1;
static uint32_t sgLvLastDynamicMs = 0;
static uint32_t sgLvLastFastMs = 0;
static uint32_t sgLvLastMediumMs = 0;
static uint32_t sgLvLastSlowMs = 0;
static int8_t sgLvLastRpmColorZone = -1;
static int8_t sgLvLastAfrColorZone = -1;
static int8_t sgLvLastBatColorZone = -1;
static int8_t sgLvLastTempColorZone = -1;
// BETA74: cache the exact quantized values shown on Page 1 so snprintf/text work
// only runs when the displayed value would actually change.
static int32_t sgLvLastPage1Tps0 = INT32_MIN;
static int32_t sgLvLastPage1Ign10 = INT32_MIN;
static int32_t sgLvLastPage1Bat10 = INT32_MIN;
static int32_t sgLvLastPage1Temp = INT32_MIN;
// BETA75: cache the exact quantized values shown on Page 2. Indexes match
// sgLvDataLines[] and are reset whenever the Live Data page is rebuilt.
static int32_t sgLvLastPage2Value[18] = {
  INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN,
  INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN,
  INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN, INT32_MIN
};
// BETA76: cache hold-feedback formatting state. The visible countdown still updates
// at the same 0.1 s / 1 s resolution, but snprintf is skipped between display changes.
static uint32_t sgLvLastHoldFormatKey = UINT32_MAX;
static char sgLvCachedHoldText[64] = {0};
// BETA77: cache System Info values after formatting. This page is diagnostic-only,
// so the visible values and update cadence stay unchanged while repeated snprintf
// work is skipped when counters/quality/health have not changed.
static int16_t sgLvLastSystemHealth = INT16_MIN;
static int16_t sgLvLastSystemQuality = INT16_MIN;
static uint32_t sgLvLastSystemRxOk = UINT32_MAX;
static uint32_t sgLvLastSystemTimeout = UINT32_MAX;
static uint32_t sgLvLastSystemChecksum = UINT32_MAX;
static uint32_t sgLvLastSystemReconnect = UINT32_MAX;

static int32_t sgLvQuantizeSigned(float value, float scale) {
  const float scaled = value * scale;
  return (int32_t)(scaled >= 0.0f ? scaled + 0.5f : scaled - 0.5f);
}

static lv_obj_t *sgLvTitle = nullptr;
static lv_obj_t *sgLvModel = nullptr;
static lv_obj_t *sgLvStatus = nullptr;
static lv_obj_t *sgLvPage = nullptr;
static lv_obj_t *sgLvNotice = nullptr;
static lv_obj_t *sgLvHold = nullptr;
static lv_obj_t *sgLvHoldText = nullptr;
static lv_obj_t *sgLvFooterHint = nullptr;
static lv_obj_t *sgLvBt = nullptr;
static lv_obj_t *sgLvVersion = nullptr;

// Page 1
static lv_obj_t *sgLvRpm = nullptr;
static lv_obj_t *sgLvRpmArc = nullptr;
static lv_obj_t *sgLvAfr = nullptr;
static lv_obj_t *sgLvAfrBold = nullptr;   // duplicate label 1px offset: ทำให้ตัวเลข AFR ดูหนาขึ้นโดยไม่เพิ่มฟอนต์
static lv_obj_t *sgLvStftBold = nullptr;  // duplicate label 1px offset: ทำให้ตัวเลข STFT ดูหนาขึ้นโดยไม่เพิ่มฟอนต์
static lv_obj_t *sgLvBat = nullptr;
static lv_obj_t *sgLvTps = nullptr;
static lv_obj_t *sgLvTemp = nullptr;
static lv_obj_t *sgLvInj = nullptr;
static lv_obj_t *sgLvIgn = nullptr;
static lv_obj_t *sgLvStft = nullptr;
static lv_obj_t *sgLvLtft = nullptr;
static lv_obj_t *sgLvPanelBat = nullptr;
static lv_obj_t *sgLvPanelTps = nullptr;
static lv_obj_t *sgLvPanelTemp = nullptr;
static lv_obj_t *sgLvPanelAfr = nullptr;
static lv_obj_t *sgLvPanelInj = nullptr;
static lv_obj_t *sgLvPanelIgn = nullptr;
static lv_obj_t *sgLvPanelStft = nullptr;
static lv_obj_t *sgLvPanelLtft = nullptr;
static lv_obj_t *sgLvAdaptiveBox = nullptr;
static lv_obj_t *sgLvAdaptiveText = nullptr;
static SgSmartWarningType sgLvLastAdaptiveType = SG_WARN_NONE;
static SgSmartSeverity sgLvLastAdaptiveSeverity = SG_SMART_OK;
static bool sgLvLastAdaptiveConnected = false;
static bool sgLvLastAdaptiveThai = true;
static bool sgLvLastAdaptiveDayTheme = false;
static bool sgLvAdaptiveRenderValid = false;
static int8_t sgLvLastDtcAdviceSlot = -1;


// Page 1 vector digits: RPM และ AFR วาดด้วย LVGL objects ไม่ใช้ฟอนต์
struct SgLvSegDigit {
  lv_obj_t *root;
  lv_obj_t *piece[7][4];
  uint8_t pieceCount[7];
  lv_obj_t *dot;
  int16_t baseX;
  int16_t baseY;
  int16_t width;
  int16_t height;
  int8_t lastValue;
};
static SgLvSegDigit sgLvRpmDigits[5] = {};
static SgLvSegDigit sgLvAfrDigits[3] = {};
static lv_obj_t *sgLvRpmBars[12] = {nullptr};
static const int SG_LV_RPM_DIGIT_Y = 2;
static const int SG_LV_RPM_PANEL_W = 304;
static const int SG_LV_RPM_SLOT_RIGHT_X = 296;
static const int SG_LV_AFR_PANEL_W = 304;
static const int SG_LV_DTC_MAIN_THAI_ZOOM = 260;
static int32_t sgLvLastVectorRpm = -1;
static int32_t sgLvLastVectorAfr10 = -1;
static lv_color_t sgLvLastVectorRpmColor = lv_color_hex(0xFFFFFF);
static lv_color_t sgLvLastVectorAfrColor = lv_color_hex(0xFFFFFF);

// Page 2 / 3 / 4
static lv_obj_t *sgLvDataPanels[18] = {nullptr};
static lv_obj_t *sgLvDataNames[18] = {nullptr};
static lv_obj_t *sgLvDataLines[18] = {nullptr};
static lv_obj_t *sgLvDataEmpty = nullptr;
// ลำดับหน้า Live Data: ค่าหลักสำหรับการขับขี่ก่อน แล้วจึงตามด้วยค่าช่าง/แรงดันเซ็นเซอร์
// index อ้างอิง backing arrays เดิม เพื่อไม่เปลี่ยน decode หรือ vehicle capability logic
static const uint8_t SG_LV_DATA_DISPLAY_ORDER[18] = {
  0, 1, 3, 10, 13, 15, 16, 11, 12,
  9, 7, 5, 2, 4, 6, 8, 14, 17
};
static lv_obj_t *sgLvDtcLines[12] = {nullptr};
static lv_obj_t *sgLvDtcSpinner = nullptr;
static lv_obj_t *sgLvDtcMainShadow = nullptr;
static lv_obj_t *sgLvSettingLines[10] = {nullptr};
static lv_obj_t *sgLvSettingPanels[10] = {nullptr};
static lv_obj_t *sgLvSystemLines[8] = {nullptr};
static lv_obj_t *sgLvSystemHealth = nullptr;

// BETA78: cache state that affects Settings text/highlight styles.
// sgLvUpdateSetting() is called on the normal UI cadence, but the screen only
// needs writes when one of these settings/selection values actually changes.
static uint32_t sgLvSettingVisualKey = 0xFFFFFFFFUL;

static lv_obj_t* sgLvLabel(lv_obj_t *parent, const char *txt, int x, int y,
                           lv_color_t color, const lv_font_t *font = LV_FONT_DEFAULT) {
  lv_obj_t *o = lv_label_create(parent);
  lv_label_set_text(o, txt ? txt : "");
  lv_obj_set_style_text_color(o, color, LV_PART_MAIN);
  lv_obj_set_style_text_font(o, font, LV_PART_MAIN);
  lv_obj_set_pos(o, x, y);
  lv_label_set_long_mode(o, LV_LABEL_LONG_CLIP);  // ตัดข้อความที่ยาวเกินกรอบ ไม่ให้ไหลไปทับส่วนอื่น
  return o;
}

static void sgLvSetText(lv_obj_t *obj, const char *txt);

static const lv_font_t* sgLvUiFont() {
  return sgLanguageThai() ? &sg_font_thai_14 : LV_FONT_DEFAULT;
}

static lv_obj_t* sgLvLocalizedLabel(lv_obj_t *parent, SgTextId id, int x, int y,
                                    lv_color_t color) {
  return sgLvLabel(parent, sgTextLocalized(id), x, y, color, sgLvUiFont());
}

static void sgLvSetLocalizedText(lv_obj_t *obj, SgTextId id) {
  if (!obj) return;
  lv_obj_set_style_text_font(obj, sgLvUiFont(), LV_PART_MAIN);
  sgLvSetText(obj, sgTextLocalized(id));
}

static lv_obj_t* sgLvPanel(lv_obj_t *parent, int x, int y, int w, int h) {
  lv_obj_t *o = lv_obj_create(parent);
  lv_obj_set_pos(o, x, y);
  lv_obj_set_size(o, w, h);
  lv_obj_set_style_bg_color(o, SG_LV_PANEL, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(o, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_color(o, SG_LV_BORDER, LV_PART_MAIN);
  lv_obj_set_style_border_width(o, 1, LV_PART_MAIN);
  lv_obj_set_style_border_opa(o, LV_OPA_60, LV_PART_MAIN);
  lv_obj_set_style_radius(o, 4, LV_PART_MAIN);
  lv_obj_set_style_pad_all(o, 2, LV_PART_MAIN);
  lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
  return o;
}

static void sgLvSetText(lv_obj_t *obj, const char *txt) {
  if (!obj) return;
  const char *old = lv_label_get_text(obj);
  if (!old || strcmp(old, txt ? txt : "") != 0) lv_label_set_text(obj, txt ? txt : "");
}

// BETA72: ลด invalidation/redraw ที่ไม่จำเป็นใน hot update paths
// ใช้เฉพาะตอน runtime update; object creation ยังคงกำหนด style โดยตรงตามเดิม
static void sgLvSetTextColorIfChanged(lv_obj_t *obj, lv_color_t color) {
  if (!obj) return;
  const lv_color_t old = lv_obj_get_style_text_color(obj, LV_PART_MAIN);
  if (old.full != color.full) lv_obj_set_style_text_color(obj, color, LV_PART_MAIN);
}

static void sgLvSetTextFontIfChanged(lv_obj_t *obj, const lv_font_t *font) {
  if (!obj || !font) return;
  if (lv_obj_get_style_text_font(obj, LV_PART_MAIN) != font) {
    lv_obj_set_style_text_font(obj, font, LV_PART_MAIN);
  }
}

static void sgLvSetTextOpaIfChanged(lv_obj_t *obj, lv_opa_t opa) {
  if (!obj) return;
  if (lv_obj_get_style_text_opa(obj, LV_PART_MAIN) != opa) {
    lv_obj_set_style_text_opa(obj, opa, LV_PART_MAIN);
  }
}

static void sgLvSetImageOpaIfChanged(lv_obj_t *obj, lv_opa_t opa) {
  if (!obj) return;
  if (lv_obj_get_style_img_opa(obj, LV_PART_MAIN) != opa) {
    lv_obj_set_style_img_opa(obj, opa, LV_PART_MAIN);
  }
}

static void sgLvSetBorderColorIfChanged(lv_obj_t *obj, lv_color_t color) {
  if (!obj) return;
  const lv_color_t old = lv_obj_get_style_border_color(obj, LV_PART_MAIN);
  if (old.full != color.full) lv_obj_set_style_border_color(obj, color, LV_PART_MAIN);
}

static void sgLvSetBgColorIfChanged(lv_obj_t *obj, lv_color_t color) {
  if (!obj) return;
  const lv_color_t old = lv_obj_get_style_bg_color(obj, LV_PART_MAIN);
  if (old.full != color.full) lv_obj_set_style_bg_color(obj, color, LV_PART_MAIN);
}

static void sgLvSetBorderOpaIfChanged(lv_obj_t *obj, lv_opa_t opa) {
  if (!obj) return;
  if (lv_obj_get_style_border_opa(obj, LV_PART_MAIN) != opa) {
    lv_obj_set_style_border_opa(obj, opa, LV_PART_MAIN);
  }
}

static void sgLvSetBorderWidthIfChanged(lv_obj_t *obj, lv_coord_t width) {
  if (!obj) return;
  if (lv_obj_get_style_border_width(obj, LV_PART_MAIN) != width) {
    lv_obj_set_style_border_width(obj, width, LV_PART_MAIN);
  }
}

static void sgLvSetHiddenIfChanged(lv_obj_t *obj, bool hidden) {
  if (!obj) return;
  const bool isHidden = lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN);
  if (hidden == isHidden) return;
  if (hidden) lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
  else lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

static void sgLvSetXIfChanged(lv_obj_t *obj, lv_coord_t x) {
  if (obj && lv_obj_get_x(obj) != x) lv_obj_set_x(obj, x);
}

static void sgLvSetYIfChanged(lv_obj_t *obj, lv_coord_t y) {
  if (obj && lv_obj_get_y(obj) != y) lv_obj_set_y(obj, y);
}

static void sgLvSetPosIfChanged(lv_obj_t *obj, lv_coord_t x, lv_coord_t y) {
  if (!obj) return;
  if (lv_obj_get_x(obj) != x || lv_obj_get_y(obj) != y) lv_obj_set_pos(obj, x, y);
}

static void sgLvSetSizeIfChanged(lv_obj_t *obj, lv_coord_t w, lv_coord_t h) {
  if (!obj) return;
  if (lv_obj_get_width(obj) != w || lv_obj_get_height(obj) != h) lv_obj_set_size(obj, w, h);
}

static void sgLvSetBarValueIfChanged(lv_obj_t *obj, int32_t value) {
  if (obj && lv_bar_get_value(obj) != value) lv_bar_set_value(obj, value, LV_ANIM_OFF);
}

static const char* sgLvSmartFooterText() {
  if ((sgPage == SG_PAGE_GAUGE || sgPage == SG_PAGE_DATA) &&
      sgGauge.ecuConnected && sgActiveWarning != SG_WARN_NONE) {
    return sgSmartWarningText(sgLanguageThai());
  }
  return nullptr;
}

static lv_color_t sgLvSmartFooterColor() {
  if (sgActiveWarningSeverity >= SG_SMART_DANGER) return SG_LV_RED;
  if (sgActiveWarningSeverity >= SG_SMART_WARNING) return SG_LV_ORANGE;
  return SG_LV_CYAN;
}

enum SgUiEcuState : uint8_t {
  SG_UI_ECU_REALTIME = 0,
  SG_UI_ECU_PAUSED,
  SG_UI_ECU_DIAGNOSTIC,
  SG_UI_ECU_CONNECTION_FAILED,
  SG_UI_ECU_RESTART_REQUIRED
};

static SgUiEcuState sgLvGetEcuState() {
  if (sgEcuRestartRequired) return SG_UI_ECU_RESTART_REQUIRED;
  if (sgPage == SG_PAGE_DTC) return SG_UI_ECU_DIAGNOSTIC;
  if (sgPage == SG_PAGE_SETTING) return SG_UI_ECU_PAUSED;
  if (!sgGauge.ecuConnected || sgConnectionHealth == SG_HEALTH_LOST) {
    return SG_UI_ECU_CONNECTION_FAILED;
  }
  return SG_UI_ECU_REALTIME;
}

static const char* sgLvHealthText() {
  switch (sgLvGetEcuState()) {
    case SG_UI_ECU_PAUSED:
      return sgLanguageThai() ? "พักการเชื่อมต่อ" : "CONNECTION PAUSED";
    case SG_UI_ECU_DIAGNOSTIC:
      return sgLanguageThai() ? "โหมดวินิจฉัย" : "DIAGNOSTIC";
    case SG_UI_ECU_RESTART_REQUIRED:
      return sgUxRestartRequiredText();
    case SG_UI_ECU_CONNECTION_FAILED:
      return sgLanguageThai() ? "ขาดการเชื่อมต่อ" : "CONNECTION LOST";
    case SG_UI_ECU_REALTIME:
    default:
      if (sgConnectionHealth == SG_HEALTH_UNSTABLE) {
        return sgLanguageThai() ? "การเชื่อมต่อไม่นิ่ง" : "CONNECTION UNSTABLE";
      }
      return sgLanguageThai() ? "กำลังอ่านข้อมูล" : "READING DATA";
  }
}

static const char* sgLvHeaderModelText() {
  static char headerBuf[96];
  const SgUiEcuState state = sgLvGetEcuState();

  if (state == SG_UI_ECU_PAUSED) return sgLanguageThai() ? "พัก ECU" : "CONNECTION PAUSED";
  if (state == SG_UI_ECU_DIAGNOSTIC) return sgLanguageThai() ? "วินิจฉัย ECU" : "DIAGNOSTIC";
  if (state == SG_UI_ECU_RESTART_REQUIRED) return sgUxRestartRequiredText();
  if (state == SG_UI_ECU_CONNECTION_FAILED) return sgLanguageThai() ? "ไม่ต่อ ECU" : "CONNECTION LOST";

  if (sgConnectionHealth == SG_HEALTH_UNSTABLE) {
    return sgLanguageThai() ? "ECU ไม่นิ่ง" : "CONNECTION UNSTABLE";
  }

  snprintf(headerBuf, sizeof(headerBuf), sgLanguageThai() ? "ต่อสำเร็จ" : "LIVE");
  return headerBuf;
}

static lv_color_t sgLvHealthColor() {
  const SgUiEcuState state = sgLvGetEcuState();
  if (state == SG_UI_ECU_RESTART_REQUIRED || state == SG_UI_ECU_CONNECTION_FAILED) return SG_LV_RED;
  if (state == SG_UI_ECU_PAUSED || state == SG_UI_ECU_DIAGNOSTIC) return SG_LV_ORANGE;
  if (sgConnectionHealth == SG_HEALTH_UNSTABLE) return SG_LV_ORANGE;
  return SG_LV_GREEN;
}

bool sgUiDtcReadAllowed() { return sgGauge.rpm == 0; }

void sgUiShowNotice(const char *text, uint16_t durationMs) {
  sgUiNoticeTextId = SG_TEXT_NONE;
  snprintf(sgUiNotice, sizeof(sgUiNotice), "%s", text ? text : "");
  sgUiNoticeUntilMs = millis() + durationMs;
}

void sgUiShowNoticeText(SgTextId id, uint16_t durationMs) {
  sgUiNotice[0] = '\0';
  sgUiNoticeTextId = (uint8_t)id;
  sgUiNoticeUntilMs = millis() + durationMs;
}

static lv_obj_t* sgLvHudRect(lv_obj_t *parent, int x, int y, int w, int h,
                               lv_color_t color, uint8_t opa, int radius);

static void sgLvBuildHeader(lv_obj_t *root) {
  const lv_color_t red = sgLvAccentColor();
  // BETA1R1: Header ต้องใช้ Day palette เช่นเดียวกับหน้าหลัก
  // Night mode คงสีเดิมทุกค่า
  const lv_color_t headerBg = sgDayThemeActive ? SG_LV_PANEL : lv_color_hex(0x090A0C);
  const lv_color_t edge = sgDayThemeActive ? lv_color_hex(0xB8C0C6) : lv_color_hex(0x3A3D42);

  lv_obj_t *bar = sgLvPanel(root, 2, 1, 316, 22);
  lv_obj_set_style_bg_color(bar, headerBg, LV_PART_MAIN);
  lv_obj_set_style_border_color(bar, red, LV_PART_MAIN);
  lv_obj_set_style_border_width(bar, 1, LV_PART_MAIN);
  lv_obj_set_style_border_opa(bar, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_radius(bar, 1, LV_PART_MAIN);
  sgLvHudRect(bar, 1, 19, 314, 2, red, LV_OPA_80, 0);
  sgLvHudRect(bar, 2, 2, 312, 1, edge, LV_OPA_60, 0);

  sgLvTitle = sgLvLabel(bar, "SDG", 7, 1, SG_LV_WHITE, &lv_font_montserrat_14);
  lv_obj_set_width(sgLvTitle, 42);
  sgLvPage = sgLvLabel(bar, "GAUGE", 51, 2, red, &lv_font_montserrat_14);
  lv_obj_set_width(sgLvPage, 82);

  sgLvModel = sgLvLabel(bar, sgLvHeaderModelText(), 137, 3, SG_LV_GREEN, sgLvUiFont());
  lv_obj_set_width(sgLvModel, 124);
  lv_obj_set_style_text_align(sgLvModel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

  sgLvBt = lv_img_create(bar);
  lv_img_set_src(sgLvBt, &sg_bt_icon_blue);
  lv_obj_set_pos(sgLvBt, 287, 6);
  lv_obj_clear_flag(sgLvBt, LV_OBJ_FLAG_SCROLLABLE);
}

enum SgLvFeedbackLayout : uint8_t {
  SG_LV_FEEDBACK_FOOTER = 0,
  SG_LV_FEEDBACK_GAUGE_STRIP,
  SG_LV_FEEDBACK_CLEAN,
  SG_LV_FEEDBACK_NOTICE_ONLY
};

// Central feedback/overlay controller.
// Creates the only Hold Feedback and Notice object set for the active page.
static void sgLvBuildFeedbackLayer(lv_obj_t *root, SgLvFeedbackLayout layout) {
  sgLvStatus = nullptr;
  sgLvFooterHint = nullptr;
  sgLvHold = nullptr;
  sgLvHoldText = nullptr;
  sgLvNotice = nullptr;

  if (layout == SG_LV_FEEDBACK_FOOTER) {
    lv_obj_t *footer = sgLvPanel(root, 4, 216, 312, 20);
    sgLvStatus = sgLvLabel(footer, "", 4, 2, SG_LV_RED, sgLvUiFont());
    lv_obj_set_width(sgLvStatus, 150);
    sgLvFooterHint = sgLvLabel(footer, sgTextLocalized(SG_TEXT_BUTTON_HINT), 154, 2, SG_LV_DIM, sgLvUiFont());
    lv_obj_set_width(sgLvFooterHint, 152);
    lv_obj_set_style_text_align(sgLvFooterHint, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  } else if (layout == SG_LV_FEEDBACK_GAUGE_STRIP) {
    lv_obj_t *statusStrip = sgLvPanel(root, 4, 221, 312, 15);
    lv_obj_set_style_bg_color(statusStrip, sgDayThemeActive ? SG_LV_PANEL : lv_color_hex(0x08141D), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(statusStrip, LV_OPA_80, LV_PART_MAIN);
    lv_obj_set_style_border_color(statusStrip, SG_LV_BORDER, LV_PART_MAIN);
    lv_obj_set_style_border_opa(statusStrip, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_border_width(statusStrip, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(statusStrip, 4, LV_PART_MAIN);
    sgLvStatus = sgLvLabel(statusStrip, "", 0, -1, SG_LV_DIM, sgLvUiFont());
    lv_obj_set_width(sgLvStatus, 308);
    lv_obj_set_style_text_align(sgLvStatus, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  }

  if (layout != SG_LV_FEEDBACK_NOTICE_ONLY) {
    sgLvHold = lv_bar_create(root);
    lv_bar_set_range(sgLvHold, 0, 1000);
    sgLvSetBarValueIfChanged(sgLvHold, 0);

    if (layout == SG_LV_FEEDBACK_FOOTER) {
      lv_obj_set_pos(sgLvHold, 4, 207); lv_obj_set_size(sgLvHold, 312, 7);
      lv_obj_set_style_bg_opa(sgLvHold, LV_OPA_COVER, LV_PART_MAIN);
      lv_obj_set_style_bg_color(sgLvHold, SG_LV_GREEN, LV_PART_INDICATOR);
      lv_obj_set_style_radius(sgLvHold, 3, LV_PART_MAIN);
      lv_obj_set_style_radius(sgLvHold, 3, LV_PART_INDICATOR);
      sgLvHoldText = sgLvLabel(root, "", 0, 218, SG_LV_CYAN, sgLvUiFont());
    } else if (layout == SG_LV_FEEDBACK_GAUGE_STRIP) {
      lv_obj_set_pos(sgLvHold, 58, 217); lv_obj_set_size(sgLvHold, 204, 3);
      lv_obj_set_style_bg_opa(sgLvHold, LV_OPA_60, LV_PART_MAIN);
      lv_obj_set_style_bg_color(sgLvHold, SG_LV_GREEN, LV_PART_INDICATOR);
      lv_obj_set_style_radius(sgLvHold, 2, LV_PART_MAIN);
      lv_obj_set_style_radius(sgLvHold, 2, LV_PART_INDICATOR);
      sgLvHoldText = sgLvLabel(root, "", 0, 221, SG_LV_CYAN, sgLvUiFont());
    } else {
      lv_obj_set_pos(sgLvHold, 70, 212); lv_obj_set_size(sgLvHold, 180, 5);
      lv_obj_set_style_bg_opa(sgLvHold, LV_OPA_50, LV_PART_MAIN);
      lv_obj_set_style_bg_color(sgLvHold, SG_LV_CYAN, LV_PART_INDICATOR);
      lv_obj_set_style_radius(sgLvHold, 2, LV_PART_MAIN);
      lv_obj_set_style_radius(sgLvHold, 2, LV_PART_INDICATOR);
      sgLvHoldText = sgLvLabel(root, "", 0, 219, SG_LV_CYAN, sgLvUiFont());
    }
    lv_obj_set_style_bg_color(sgLvHold, SG_LV_PANEL, LV_PART_MAIN);
    lv_obj_add_flag(sgLvHold, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_width(sgLvHoldText, 320);
    lv_obj_set_style_text_align(sgLvHoldText, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_add_flag(sgLvHoldText, LV_OBJ_FLAG_HIDDEN);
  }

  const int noticeY = (layout == SG_LV_FEEDBACK_NOTICE_ONLY) ? 105 : 102;
  sgLvNotice = sgLvLabel(root, "", 0, noticeY, SG_LV_WHITE, sgLvUiFont());
  lv_obj_set_width(sgLvNotice, 320);
  lv_obj_set_style_text_align(sgLvNotice, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_add_flag(sgLvNotice, LV_OBJ_FLAG_HIDDEN);
}

static void sgLvBuildFooter(lv_obj_t *root) {
  sgLvBuildFeedbackLayer(root, SG_LV_FEEDBACK_FOOTER);
}


static void sgLvBuildGaugeOverlayNoFooter(lv_obj_t *root) {
  sgLvBuildFeedbackLayer(root, SG_LV_FEEDBACK_GAUGE_STRIP);
}


static void sgLvBuildCleanOverlayNoFooter(lv_obj_t *root) {
  sgLvBuildFeedbackLayer(root, SG_LV_FEEDBACK_CLEAN);
}

static lv_obj_t* sgLvValueBox(lv_obj_t *root, int x, int y, int w, int h,
                              const char *name, lv_obj_t **outValue, lv_color_t color) {
  // กล่องค่าหน้าแรก: ชื่ออยู่กึ่งกลางด้านบน และตัวเลขใหญ่กว่าชื่อชัดเจน
  lv_obj_t *p = sgLvPanel(root, x, y, w, h);
  lv_obj_set_style_bg_color(p, SG_LV_PANEL, LV_PART_MAIN);
  lv_obj_set_style_border_color(p, SG_LV_BORDER, LV_PART_MAIN);
  lv_obj_set_style_border_opa(p, LV_OPA_40, LV_PART_MAIN);
  lv_obj_set_style_border_width(p, 1, LV_PART_MAIN);

  lv_obj_t *n = sgLvLabel(p, name, 0, 3, SG_LV_DIM,
                          sgLanguageThai() ? &sg_font_thai_14 : &lv_font_montserrat_12);
  lv_obj_set_width(n, w);
  lv_obj_set_style_text_align(n, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_set_style_text_letter_space(n, sgLanguageThai() ? -1 : 0, LV_PART_MAIN);

  lv_obj_t *valueLabel = sgLvLabel(p, "--", 0, 18, color, &lv_font_montserrat_20);
  lv_obj_set_width(valueLabel, w);
  lv_obj_set_style_text_align(valueLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  if (outValue) *outValue = valueLabel;
  return p;
}

static lv_obj_t* sgLvValueBoxLarge(lv_obj_t *root, int x, int y, int w, int h,
                                   const char *name, lv_obj_t **outValue, lv_color_t color) {
  lv_obj_t *p = sgLvPanel(root, x, y, w, h);
  lv_obj_set_style_bg_color(p, SG_LV_PANEL, LV_PART_MAIN);
  lv_obj_set_style_border_color(p, SG_LV_BORDER, LV_PART_MAIN);
  lv_obj_set_style_border_opa(p, LV_OPA_90, LV_PART_MAIN);
  lv_obj_set_style_border_width(p, 1, LV_PART_MAIN);
  lv_obj_t *n = sgLvLabel(p, name, 4, 0, SG_LV_DIM,
                          sgLanguageThai() ? &sg_font_thai_14 : &lv_font_montserrat_12);
  lv_obj_set_width(n, w - 8);
  lv_obj_set_style_text_letter_space(n, sgLanguageThai() ? -1 : 0, LV_PART_MAIN);
  lv_obj_t *valueLabel = sgLvLabel(p, "--", 2, 26, color, &lv_font_montserrat_30);
  lv_obj_set_width(valueLabel, w - 8);
  lv_obj_set_style_text_align(valueLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
  if (outValue) *outValue = valueLabel;
  return p;
}

static lv_obj_t* sgLvAddBoldValueOverlay(lv_obj_t *panel, lv_obj_t *base, int w, lv_color_t color) {
  // วางตัวเลขซ้ำเยื้อง 1px เพื่อให้ AFR/STFT หนาขึ้นโดยไม่ต้องใช้ฟอนต์ bold เพิ่ม
  lv_obj_t *dup = sgLvLabel(panel, "--", 1, 26, color, &lv_font_montserrat_30);
  lv_obj_set_width(dup, w - 8);
  lv_obj_set_style_text_align(dup, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
  if (base) lv_obj_move_foreground(base);
  return dup;
}

static void sgLvTickLabel(lv_obj_t *root, const char *txt, int x, int y, lv_color_t c) {
  lv_obj_t *o = sgLvLabel(root, txt, x, y, c, &lv_font_montserrat_14);
  lv_obj_set_width(o, 22);
  lv_obj_set_style_text_align(o, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
}


static const uint8_t sgLvSegMask[10] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static lv_obj_t* sgLvSegPiece(lv_obj_t *parent, int x, int y, int w, int h, int radius) {
  lv_obj_t *o = lv_obj_create(parent);
  lv_obj_remove_style_all(o);
  lv_obj_set_pos(o, x, y);
  lv_obj_set_size(o, w, h);
  lv_obj_set_style_bg_opa(o, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_radius(o, radius, LV_PART_MAIN);
  lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
  return o;
}

static lv_obj_t* sgLvSegDot(lv_obj_t *parent, int x, int y, int size) {
  lv_obj_t *o = sgLvSegPiece(parent, x, y, size, size, size / 2);
  return o;
}

static void sgLvBuildHorizontalStroke(SgLvSegDigit &d, uint8_t segIndex,
                                      int x, int y, int length, int thick) {
  d.pieceCount[segIndex] = 1;
  d.piece[segIndex][0] = sgLvSegPiece(d.root, x, y, length, thick, thick / 2);
}

static void sgLvBuildVerticalStroke(SgLvSegDigit &d, uint8_t segIndex,
                                    int xTop, int y, int thick, int length,
                                    int slantPx) {
  (void)slantPx;
  d.pieceCount[segIndex] = 1;
  d.piece[segIndex][0] = sgLvSegPiece(d.root, xTop, y, thick, length, thick / 2);
}

static void sgLvCreateSegDigit(lv_obj_t *parent, SgLvSegDigit &d,
                               int x, int y, int w, int h, int thick,
                               bool decimalPoint) {
  memset(&d, 0, sizeof(d));

  d.root = lv_obj_create(parent);
  lv_obj_remove_style_all(d.root);
  lv_obj_set_pos(d.root, x, y);
  lv_obj_set_size(d.root, w + thick + 6, h + 3);
  lv_obj_clear_flag(d.root, LV_OBJ_FLAG_SCROLLABLE);

  d.baseX = x;
  d.baseY = y;
  d.width = w;
  d.height = h;

  const int half = h / 2;
  const int horizontalX = thick / 2;
  const int horizontalLen = w - thick;
  const int upperLen = half - thick;
  const int lowerLen = half - thick;

  sgLvBuildHorizontalStroke(
      d, 0, horizontalX, 0,
      horizontalLen, thick);

  sgLvBuildVerticalStroke(
      d, 1, w - thick,
      thick / 2, thick, upperLen, 0);

  sgLvBuildVerticalStroke(
      d, 2, w - thick,
      half + thick / 2, thick, lowerLen, 0);

  sgLvBuildHorizontalStroke(
      d, 3, horizontalX, h - thick,
      horizontalLen, thick);

  sgLvBuildVerticalStroke(
      d, 4, 0,
      half + thick / 2, thick, lowerLen, 0);

  sgLvBuildVerticalStroke(
      d, 5, 0,
      thick / 2, thick, upperLen, 0);

  sgLvBuildHorizontalStroke(
      d, 6, horizontalX,
      half - (thick / 2),
      horizontalLen, thick);

  d.dot = decimalPoint
      ? sgLvSegDot(d.root,
                   w + 2,
                   h - thick,
                   thick + 2)
      : nullptr;

  d.lastValue = -2;
}

static void sgLvSetDigitPosition(SgLvSegDigit &d, int x, int y) {
  if (!d.root) return;
  lv_obj_set_pos(d.root, x, y);
}

static void sgLvSetDigitVisible(SgLvSegDigit &d, bool visible) {
  if (!d.root) return;
  if (visible) lv_obj_clear_flag(d.root, LV_OBJ_FLAG_HIDDEN);
  else lv_obj_add_flag(d.root, LV_OBJ_FLAG_HIDDEN);
}

static int sgLvDigitCellWidth(const SgLvSegDigit &d) {
  // ใช้ความกว้าง object จริง ไม่ใช้เฉพาะความกว้าง segment
  // เพื่อให้การจัดกึ่งกลางตรงทั้ง 1–5 หลัก
  return d.root ? lv_obj_get_width(d.root) : d.width;
}

static void sgLvSetSegDigitMask(SgLvSegDigit &d, uint8_t mask, lv_color_t color,
                                lv_color_t offColor, bool showDot) {
  for (int seg = 0; seg < 7; ++seg) {
    const bool on = (mask & (1U << seg)) != 0;
    for (uint8_t p = 0; p < d.pieceCount[seg]; ++p) {
      lv_obj_t *piece = d.piece[seg][p];
      if (!piece) continue;
      lv_obj_set_style_bg_color(piece, on ? color : offColor, LV_PART_MAIN);
      lv_obj_set_style_bg_opa(piece, on ? LV_OPA_COVER : LV_OPA_TRANSP, LV_PART_MAIN);
    }
  }
  if (d.dot) {
    lv_obj_set_style_bg_color(d.dot, color, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(d.dot, showDot ? LV_OPA_COVER : LV_OPA_TRANSP, LV_PART_MAIN);
  }
}

static void sgLvSetSegDigit(SgLvSegDigit &d, int value, lv_color_t color,
                            lv_color_t offColor, bool showDot) {
  uint8_t mask = 0;
  if (value >= 0 && value <= 9) mask = sgLvSegMask[value];
  sgLvSetSegDigitMask(d, mask, color, offColor, showDot);
  d.lastValue = value;
}

static uint8_t sgLvRpmDigitCount(uint16_t rpm) {
  if (rpm >= 10000U) return 5;
  if (rpm >= 1000U) return 4;
  if (rpm >= 100U) return 3;
  if (rpm >= 10U) return 2;
  return 1;
}

static lv_color_t sgLvRpmGradientColor(uint8_t digitIndex, uint8_t digitCount) {
  // ไล่สีตามตำแหน่งตัวเลขจากซ้ายไปขวาตามภาพต้นแบบ Page 1:
  // เขียว -> เหลือง -> ส้ม -> แดง โดยไม่ผูกกับโซน RPM
  if (digitCount <= 1U) return lv_color_hex(0x9BE000);

  const uint16_t step = (uint16_t)digitIndex * 100U / (uint16_t)(digitCount - 1U);
  if (step <= 20U) return lv_color_hex(0x9BE000);  // เขียว
  if (step <= 45U) return lv_color_hex(0xD8F000);  // เขียวอมเหลือง
  if (step <= 65U) return lv_color_hex(0xFFD400);  // เหลือง
  if (step <= 85U) return lv_color_hex(0xFF9200);  // ส้ม
  return lv_color_hex(0xF01800);                   // แดง
}

static void sgLvUpdateVectorRpm(uint16_t rpm, lv_color_t color) {
  (void)color;  // คง signature เดิมไว้เพื่อลดขอบเขตการแก้ไข
  const uint16_t value = rpm > 12500U ? 12500U : rpm;
  const uint8_t digits = sgLvRpmDigitCount(value);
  const int cellW = sgLvDigitCellWidth(sgLvRpmDigits[0]);
  const int gap = 1;
  const int totalW = digits * cellW + (digits - 1) * gap;
  const int rpmSlotRightX = SG_LV_RPM_SLOT_RIGHT_X;
  const int startX = value == 0U ? ((SG_LV_RPM_PANEL_W - totalW) / 2) : (rpmSlotRightX - totalW);
  const lv_color_t offColor = sgDayThemeActive ? lv_color_hex(0x2B3238) : lv_color_hex(0x08141D);

  int divisor = 1;
  for (uint8_t i = 1; i < digits; ++i) divisor *= 10;

  for (uint8_t i = 0; i < 5; ++i) {
    if (i < digits) {
      const int digit = (value / divisor) % 10;
      sgLvSetDigitPosition(sgLvRpmDigits[i], startX + i * (cellW + gap), SG_LV_RPM_DIGIT_Y);
      sgLvSetDigitVisible(sgLvRpmDigits[i], true);
      // ใช้สีไล่ตามตำแหน่งหลักซ้าย -> ขวา ตามภาพต้นแบบ ไม่เปลี่ยนทั้งค่าเป็นสีเขียว
      const lv_color_t digitColor = sgLvRpmGradientColor(i, digits);
      sgLvSetSegDigit(sgLvRpmDigits[i], digit, digitColor, offColor, false);
      if (divisor > 1) divisor /= 10;
    } else {
      sgLvSetDigitVisible(sgLvRpmDigits[i], false);
    }
  }
}

static void sgLvUpdateVectorAfr(float afr, lv_color_t color) {
  const lv_color_t offColor = sgDayThemeActive ? lv_color_hex(0x2B3238) : lv_color_hex(0x08141D);
  if (afr <= 0.1f || afr > 99.9f) {
    const int cellW = sgLvDigitCellWidth(sgLvAfrDigits[0]);
    const int gap = 3;
    const int totalW = cellW * 2 + gap;
    const int startX = (SG_LV_AFR_PANEL_W - totalW) / 2;
    sgLvSetDigitVisible(sgLvAfrDigits[0], false);
    sgLvSetDigitPosition(sgLvAfrDigits[1], startX, 8);
    sgLvSetDigitPosition(sgLvAfrDigits[2], startX + cellW + gap, 8);
    sgLvSetDigitVisible(sgLvAfrDigits[1], true);
    sgLvSetDigitVisible(sgLvAfrDigits[2], true);
    sgLvSetSegDigitMask(sgLvAfrDigits[1], (1U << 6), color, offColor, false);
    sgLvSetSegDigitMask(sgLvAfrDigits[2], (1U << 6), color, offColor, false);
    return;
  }

  int value10 = (int)(afr * 10.0f + 0.5f);
  if (value10 > 999) value10 = 999;
  const int tens = (value10 / 100) % 10;
  const int ones = (value10 / 10) % 10;
  const int tenths = value10 % 10;
  const int cellW = sgLvDigitCellWidth(sgLvAfrDigits[0]);
  const int gap = 3;

  // ล็อกตำแหน่งหลักหน่วย จุดทศนิยม และหลักทศนิยมให้คงที่ทั้งค่า 9.5 และ 14.4
  const int totalW = cellW * 3 + gap * 2;
  const int startX = (SG_LV_AFR_PANEL_W - totalW) / 2;
  for (int i = 0; i < 3; ++i) {
    sgLvSetDigitPosition(sgLvAfrDigits[i], startX + i * (cellW + gap), 8);
  }
  sgLvSetDigitVisible(sgLvAfrDigits[0], tens != 0);
  sgLvSetDigitVisible(sgLvAfrDigits[1], true);
  sgLvSetDigitVisible(sgLvAfrDigits[2], true);
  if (tens != 0) sgLvSetSegDigit(sgLvAfrDigits[0], tens, color, offColor, false);
  sgLvSetSegDigit(sgLvAfrDigits[1], ones, color, offColor, true);
  sgLvSetSegDigit(sgLvAfrDigits[2], tenths, color, offColor, false);
}

static lv_obj_t* sgLvHudRect(lv_obj_t *parent, int x, int y, int w, int h,
                               lv_color_t color, uint8_t opa, int radius = 0) {
  lv_obj_t *o = lv_obj_create(parent);
  lv_obj_remove_style_all(o);
  lv_obj_set_pos(o, x, y);
  lv_obj_set_size(o, w, h);
  lv_obj_set_style_bg_color(o, color, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(o, opa, LV_PART_MAIN);
  lv_obj_set_style_radius(o, radius, LV_PART_MAIN);
  lv_obj_clear_flag(o, LV_OBJ_FLAG_SCROLLABLE);
  return o;
}

static void sgLvHudCorners(lv_obj_t *parent, int w, int h, lv_color_t color, uint8_t opa) {
  const int len = 15;
  const int thick = 1;
  sgLvHudRect(parent, 3, 3, len, thick, color, opa, 0);
  sgLvHudRect(parent, 3, 3, thick, len, color, opa, 0);
  sgLvHudRect(parent, w - len - 3, 3, len, thick, color, opa, 0);
  sgLvHudRect(parent, w - 4, 3, thick, len, color, opa, 0);
  sgLvHudRect(parent, 3, h - 4, len, thick, color, opa, 0);
  sgLvHudRect(parent, 3, h - len - 3, thick, len, color, opa, 0);
  sgLvHudRect(parent, w - len - 3, h - 4, len, thick, color, opa, 0);
  sgLvHudRect(parent, w - 4, h - len - 3, thick, len, color, opa, 0);
}

static lv_color_t sgLvRpmBarColor(uint8_t index) {
  if (index >= 10) return SG_LV_RED;
  if (index >= 8) return SG_LV_ORANGE;
  if (index >= 6) return SG_LV_YELLOW;
  return SG_LV_GREEN;
}

static void sgLvUpdateRpmBars(uint16_t rpm) {
  const uint16_t value = rpm > 12500U ? 12500U : rpm;
  uint8_t active = (uint8_t)(((uint32_t)value * 12U + 12499U) / 12500U);
  if (active > 12) active = 12;
  for (uint8_t i = 0; i < 12; ++i) {
    if (!sgLvRpmBars[i]) continue;
    const bool on = i < active;
    lv_obj_set_style_bg_color(sgLvRpmBars[i], on ? sgLvRpmBarColor(i) : SG_LV_CYAN, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(sgLvRpmBars[i], on ? LV_OPA_COVER : LV_OPA_10, LV_PART_MAIN);
  }
}

static lv_color_t sgLvRpmColorForZone(int8_t zone) {
  if (sgDayThemeActive) {
    if (zone == 2) return lv_color_hex(0xFF4A3D);
    if (zone == 1) return lv_color_hex(0xFFE66D);
    return lv_color_hex(0x6DFF7A);
  }
  return zone == 2 ? lv_color_hex(0xFF2418)
                   : (zone == 1 ? lv_color_hex(0xFFE04A)
                                : lv_color_hex(0x39FF57));
}

static void sgLvBuildNeoBackdrop(lv_obj_t *root) {
  // ลายพื้นหลังนิ่ง สร้างครั้งเดียว ไม่อัปเดตทุกเฟรม
  // ลดความสว่างลงเพื่อให้ RPM/AFR เด่นกว่ากรอบ HUD
  const lv_color_t rail = sgDayThemeActive ? lv_color_hex(0x536878) : lv_color_hex(0x123B55);
  const lv_color_t glow = sgDayThemeActive ? lv_color_hex(0x008DB5) : lv_color_hex(0x0E8BB8);
  const lv_color_t shadow = sgDayThemeActive ? lv_color_hex(0x9FB3BD) : lv_color_hex(0x061723);
  sgLvHudRect(root, 0, 24, 320, 1, rail, LV_OPA_20);
  sgLvHudRect(root, 0, 185, 320, 1, rail, LV_OPA_20);
  sgLvHudRect(root, 0, 25, 72, 1, shadow, LV_OPA_30);
  sgLvHudRect(root, 248, 25, 72, 1, shadow, LV_OPA_30);
  sgLvHudRect(root, 3, 236, 36, 2, glow, LV_OPA_50, 1);
  sgLvHudRect(root, 281, 236, 36, 2, glow, LV_OPA_50, 1);
}

// LVGL-only renderer: ห้ามกลับไปใช้ TFT_eSPI วาด UI โดยตรง ยกเว้น flush driver
static void sgLvBuildGauge(lv_obj_t *root) {
  // BETA5: ปรับความอ่านง่าย Page 1 จากฐานภาพ BETA4R1 โดยไม่แตะระบบอื่น
  // ภาพพื้นหลังสร้างครั้งเดียว; runtime เปลี่ยนเฉพาะ object ของตัวเลข/สถานะเดิม
  lv_obj_set_style_bg_color(root, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, LV_PART_MAIN);

  lv_obj_t *bg = lv_img_create(root);
  lv_img_set_src(bg, sgDayThemeActive ? &sg_racing_page1_day_bg : &sg_racing_page1_bg);
  lv_obj_set_pos(bg, 0, 0);
  lv_obj_clear_flag(bg, LV_OBJ_FLAG_SCROLLABLE);

  // Header overlay: รักษา pointer/state update เดิม แต่ไม่สร้างแถบใหม่ทับ artwork
  sgLvTitle = nullptr; // SDG อยู่ใน artwork แล้ว
  sgLvPage = sgLvLabel(root, "GAUGE", 52, 5, sgLvAccentColor(), &lv_font_montserrat_14);
  lv_obj_set_width(sgLvPage, 108);
  sgLvModel = sgLvLabel(root, sgLvHeaderModelText(), 137, 5, SG_LV_GREEN, sgLvUiFont());
  lv_obj_set_width(sgLvModel, 124);
  lv_obj_set_style_text_align(sgLvModel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  sgLvBt = lv_img_create(root);
  lv_img_set_src(sgLvBt, &sg_bt_icon_blue);
  lv_obj_set_pos(sgLvBt, 288, 7);
  lv_obj_clear_flag(sgLvBt, LV_OBJ_FLAG_SCROLLABLE);
  sgLvBuildFeedbackLayer(root, SG_LV_FEEDBACK_NOTICE_ONLY);

  // Transparent RPM container aligned to the artwork opening.
  lv_obj_t *rpmArea = lv_obj_create(root);
  lv_obj_set_pos(rpmArea, 5, 26);
  lv_obj_set_size(rpmArea, 310, 86);
  lv_obj_set_style_bg_opa(rpmArea, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(rpmArea, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(rpmArea, 0, LV_PART_MAIN);
  lv_obj_clear_flag(rpmArea, LV_OBJ_FLAG_SCROLLABLE);
  for (int i = 0; i < 12; ++i) sgLvRpmBars[i] = nullptr; // artwork มี tach arc อยู่แล้ว
  const int rpmX = 11, rpmY = 1, rpmW = 43, rpmH = 66, rpmThick = 8, rpmGap = 1;
  for (int i = 0; i < 5; ++i) {
    sgLvCreateSegDigit(rpmArea, sgLvRpmDigits[i], rpmX + i * (rpmW + rpmGap), rpmY,
                       rpmW, rpmH, rpmThick, false);
  }
  sgLvRpm = nullptr;
  sgLvRpmArc = nullptr;
  sgLvLastVectorRpm = -1;

  // Transparent AFR container; label/bar/frame อยู่ใน artwork แล้ว.
  sgLvPanelAfr = lv_obj_create(root);
  lv_obj_set_pos(sgLvPanelAfr, 5, 107);
  lv_obj_set_size(sgLvPanelAfr, 310, 65);
  lv_obj_set_style_bg_opa(sgLvPanelAfr, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(sgLvPanelAfr, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(sgLvPanelAfr, 0, LV_PART_MAIN);
  lv_obj_clear_flag(sgLvPanelAfr, LV_OBJ_FLAG_SCROLLABLE);
  const int afrX = 72, afrY = 5, afrW = 51, afrH = 48, afrThick = 8, afrGap = 3;
  for (int i = 0; i < 3; ++i) {
    sgLvCreateSegDigit(sgLvPanelAfr, sgLvAfrDigits[i], afrX + i * (afrW + afrGap), afrY,
                       afrW, afrH, afrThick, i == 1);
  }
  sgLvAfr = nullptr;
  sgLvAfrBold = nullptr;
  sgLvLastVectorAfr10 = -1;

  // Bottom live values. Frame and labels come from artwork; only values are LVGL labels.
  const int y = 184, h = 52, w = 75;
  sgLvPanelTps = lv_obj_create(root);
  sgLvPanelTemp = lv_obj_create(root);
  sgLvPanelIgn = lv_obj_create(root);
  sgLvPanelBat = lv_obj_create(root);
  lv_obj_t *pods[4] = {sgLvPanelTps, sgLvPanelTemp, sgLvPanelIgn, sgLvPanelBat};
  const int xs[4] = {5, 82, 159, 236};
  const int ws[4] = {75, 75, 75, 79};
  for (int i = 0; i < 4; ++i) {
    lv_obj_set_pos(pods[i], xs[i], y);
    lv_obj_set_size(pods[i], ws[i], h);
    lv_obj_set_style_bg_opa(pods[i], LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(pods[i], 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(pods[i], 0, LV_PART_MAIN);
    lv_obj_clear_flag(pods[i], LV_OBJ_FLAG_SCROLLABLE);
  }
  const lv_color_t page1ValueColor = sgDayThemeActive ? lv_color_hex(0x202428) : SG_LV_WHITE;
  sgLvTps = sgLvLabel(sgLvPanelTps, "--", 0, 17, page1ValueColor, &lv_font_montserrat_20);
  sgLvTemp = sgLvLabel(sgLvPanelTemp, "--", 0, 17, page1ValueColor, &lv_font_montserrat_20);
  sgLvIgn = sgLvLabel(sgLvPanelIgn, "--", 0, 17, page1ValueColor, &lv_font_montserrat_20);
  sgLvBat = sgLvLabel(sgLvPanelBat, "--", 0, 17, page1ValueColor, &lv_font_montserrat_20);
  lv_obj_t *values[4] = {sgLvTps, sgLvTemp, sgLvIgn, sgLvBat};
  for (int i = 0; i < 4; ++i) {
    lv_obj_set_width(values[i], ws[i]);
    lv_obj_set_style_text_align(values[i], LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  }

  sgLvPanelInj = sgLvPanelStft = sgLvPanelLtft = nullptr;
  sgLvInj = sgLvStft = sgLvStftBold = sgLvLtft = nullptr;
  sgLvAdaptiveBox = nullptr;
  sgLvAdaptiveText = nullptr;
}

static void sgLvBuildData(lv_obj_t *root) {
  for (uint8_t i = 0; i < 18; ++i) sgLvLastPage2Value[i] = INT32_MIN;
  const lv_color_t accent=sgLvAccentColor();
  const lv_color_t panel = sgLvThemePanelColor();
  lv_obj_set_style_bg_color(root, sgLvThemeRootColor(), LV_PART_MAIN);
  sgLvBuildHeader(root);
  sgLvBuildCleanOverlayNoFooter(root);

  sgLvDataEmpty=sgLvLabel(root,sgTextLocalized(SG_TEXT_WAITING_ECU_DATA),0,112,SG_LV_DIM,sgLvUiFont());
  lv_obj_set_width(sgLvDataEmpty,320);
  lv_obj_set_style_text_align(sgLvDataEmpty,LV_TEXT_ALIGN_CENTER,LV_PART_MAIN);

  static const char *names[18]={
    "RPM","TPS","TPS V","TEMP","ECT V","IAT","IAT V","MAP","MAP V",
    "SPEED","BAT","INJ","IGN","AFR","O2","STFT","LTFT","IACV"
  };
  for(int i=0;i<18;++i){
    lv_obj_t *card=sgLvPanel(root,5,28,152,27);
    sgLvDataPanels[i]=card;
    lv_obj_set_style_bg_color(card,panel,LV_PART_MAIN);
    lv_obj_set_style_border_color(card,accent,LV_PART_MAIN);
    lv_obj_set_style_border_width(card,1,LV_PART_MAIN);
    lv_obj_set_style_border_opa(card,LV_OPA_70,LV_PART_MAIN);
    lv_obj_set_style_radius(card,2,LV_PART_MAIN);

    sgLvDataNames[i]=sgLvLabel(card,names[i],6,4,SG_LV_DIM,&lv_font_montserrat_12);
    lv_obj_set_width(sgLvDataNames[i],58);
    sgLvDataLines[i]=sgLvLabel(card,"--",64,3,SG_LV_WHITE,&lv_font_montserrat_18);
    lv_obj_set_width(sgLvDataLines[i],82);
    lv_obj_set_style_text_align(sgLvDataLines[i],LV_TEXT_ALIGN_RIGHT,LV_PART_MAIN);
    lv_obj_add_flag(card,LV_OBJ_FLAG_HIDDEN);
  }
}

static void sgLvBuildDtc(lv_obj_t *root) {
  const lv_color_t red = sgLvAccentColor();
  const lv_color_t panel = sgLvThemePanelColor();
  lv_obj_set_style_bg_color(root, sgLvThemeRootColor(), LV_PART_MAIN);
  sgLvBuildHeader(root); sgLvBuildCleanOverlayNoFooter(root);
  lv_obj_t *statusCard=sgLvPanel(root,8,31,304,174);
  lv_obj_set_style_bg_color(statusCard,panel,LV_PART_MAIN); lv_obj_set_style_border_color(statusCard,red,LV_PART_MAIN);
  lv_obj_set_style_border_opa(statusCard,LV_OPA_COVER,LV_PART_MAIN); lv_obj_set_style_radius(statusCard,3,LV_PART_MAIN);
  sgLvHudCorners(statusCard,304,174,red,LV_OPA_COVER);
  sgLvLabel(statusCard,"DTC STATUS",8,5,red,&lv_font_montserrat_14);
  sgLvDtcLines[0]=sgLvLabel(root,"",10,67,SG_LV_YELLOW,sgLanguageThai()?sgLvUiFont():&lv_font_montserrat_28);
  lv_obj_set_width(sgLvDtcLines[0],300); lv_obj_set_style_text_align(sgLvDtcLines[0],LV_TEXT_ALIGN_CENTER,LV_PART_MAIN);
  if(sgLanguageThai()) lv_obj_set_style_transform_zoom(sgLvDtcLines[0],SG_LV_DTC_MAIN_THAI_ZOOM,LV_PART_MAIN);
  sgLvDtcMainShadow=sgLvLabel(root,"",11,68,SG_LV_YELLOW,sgLanguageThai()?sgLvUiFont():&lv_font_montserrat_28);
  lv_obj_set_width(sgLvDtcMainShadow,300); lv_obj_set_style_text_align(sgLvDtcMainShadow,LV_TEXT_ALIGN_CENTER,LV_PART_MAIN);
  if(sgLanguageThai()) lv_obj_set_style_transform_zoom(sgLvDtcMainShadow,SG_LV_DTC_MAIN_THAI_ZOOM,LV_PART_MAIN);
  lv_obj_move_foreground(sgLvDtcLines[0]);
  sgLvDtcSpinner=lv_spinner_create(root,900,70); lv_obj_set_size(sgLvDtcSpinner,42,42); lv_obj_align(sgLvDtcSpinner,LV_ALIGN_CENTER,0,4);
  lv_obj_set_style_arc_width(sgLvDtcSpinner,4,LV_PART_MAIN); lv_obj_set_style_arc_width(sgLvDtcSpinner,4,LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(sgLvDtcSpinner,SG_LV_BORDER,LV_PART_MAIN); lv_obj_set_style_arc_color(sgLvDtcSpinner,SG_LV_CYAN,LV_PART_INDICATOR);
  lv_obj_add_flag(sgLvDtcSpinner,LV_OBJ_FLAG_HIDDEN);
  sgLvDtcLines[1]=sgLvLabel(root,"",10,126,SG_LV_GREEN,sgLvUiFont()); lv_obj_set_width(sgLvDtcLines[1],300); lv_obj_set_style_text_align(sgLvDtcLines[1],LV_TEXT_ALIGN_CENTER,LV_PART_MAIN);
  for(int i=2;i<5;++i){sgLvDtcLines[i]=sgLvLabel(root,"",12,146+(i-2)*18,SG_LV_WHITE,sgLvUiFont());lv_obj_set_width(sgLvDtcLines[i],296);lv_obj_set_style_text_align(sgLvDtcLines[i],LV_TEXT_ALIGN_CENTER,LV_PART_MAIN);}
  sgLvDtcLines[5]=sgLvLabel(root,"",8,211,SG_LV_CYAN,sgLvUiFont()); lv_obj_set_width(sgLvDtcLines[5],304); lv_obj_set_style_text_align(sgLvDtcLines[5],LV_TEXT_ALIGN_CENTER,LV_PART_MAIN);
}


static void sgLvBuildSystem(lv_obj_t *root) {
  sgLvLastSystemHealth = INT16_MIN;
  sgLvLastSystemQuality = INT16_MIN;
  sgLvLastSystemRxOk = UINT32_MAX;
  sgLvLastSystemTimeout = UINT32_MAX;
  sgLvLastSystemChecksum = UINT32_MAX;
  sgLvLastSystemReconnect = UINT32_MAX;
  sgLvBuildHeader(root);
  sgLvBuildFooter(root);
  lv_obj_t *title = sgLvLabel(root, "SYSTEM INFO", 8, 24, SG_LV_YELLOW, &lv_font_montserrat_14);
  lv_obj_set_width(title, 160);
  sgLvSystemHealth = sgLvLabel(root, "HEALTH --%", 164, 24, SG_LV_GREEN, &lv_font_montserrat_14);
  lv_obj_set_width(sgLvSystemHealth, 148);
  lv_obj_set_style_text_align(sgLvSystemHealth, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);

  static const char *names[8] = {
    "FIRMWARE", "ECU ID", "MODEL", "K-LINE QUALITY",
    "RX OK", "TIMEOUT", "CHECKSUM ERR", "RECONNECT"
  };
  for (int i = 0; i < 8; ++i) {
    const int y = 45 + i * 20;
    lv_obj_t *p = sgLvPanel(root, 8, y, 304, 18);
    lv_obj_set_style_bg_opa(p, LV_OPA_20, LV_PART_MAIN);
    lv_obj_set_style_border_opa(p, LV_OPA_30, LV_PART_MAIN);
    lv_obj_t *n = sgLvLabel(p, names[i], 5, 1, SG_LV_DIM, &lv_font_montserrat_14);
    lv_obj_set_width(n, 120);
    sgLvSystemLines[i] = sgLvLabel(p, "--", 126, 1, (i == 3 ? SG_LV_GREEN : SG_LV_WHITE), &lv_font_montserrat_14);
    lv_obj_set_width(sgLvSystemLines[i], 170);
    lv_obj_set_style_text_align(sgLvSystemLines[i], LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
  }
}

static void sgLvUpdateSystem() {
  char b[48];
  const int16_t health = (int16_t)sgVehicleHealthScore;
  if (health != sgLvLastSystemHealth) {
    sgLvLastSystemHealth = health;
    snprintf(b, sizeof(b), "HEALTH %u%%", (unsigned)sgVehicleHealthScore);
    sgLvSetText(sgLvSystemHealth, b);
    sgLvSetTextColorIfChanged(sgLvSystemHealth,
      sgVehicleHealthScore < 60 ? SG_LV_RED :
      (sgVehicleHealthScore < 85 ? SG_LV_ORANGE : SG_LV_GREEN));
  }

  // These strings already use sgLvSetText(), which suppresses duplicate LVGL writes.
  sgLvSetText(sgLvSystemLines[0], SG_FIRMWARE_VERSION);
  sgLvSetText(sgLvSystemLines[1], sgEcuIdHex[0] ? sgEcuIdHex : "--");
  sgLvSetText(sgLvSystemLines[2], sgModelName[0] ? sgModelName : "--");

  const int16_t quality = (int16_t)sgV330QualityPercent();
  if (quality != sgLvLastSystemQuality) {
    sgLvLastSystemQuality = quality;
    snprintf(b, sizeof(b), "%u%%", (unsigned)quality);
    sgLvSetText(sgLvSystemLines[3], b);
  }
  if (sgRxOkCount != sgLvLastSystemRxOk) {
    sgLvLastSystemRxOk = sgRxOkCount;
    snprintf(b, sizeof(b), "%lu", (unsigned long)sgRxOkCount);
    sgLvSetText(sgLvSystemLines[4], b);
  }
  if (sgRxTimeoutCount != sgLvLastSystemTimeout) {
    sgLvLastSystemTimeout = sgRxTimeoutCount;
    snprintf(b, sizeof(b), "%lu", (unsigned long)sgRxTimeoutCount);
    sgLvSetText(sgLvSystemLines[5], b);
  }
  if (sgChecksumErrorCount != sgLvLastSystemChecksum) {
    sgLvLastSystemChecksum = sgChecksumErrorCount;
    snprintf(b, sizeof(b), "%lu", (unsigned long)sgChecksumErrorCount);
    sgLvSetText(sgLvSystemLines[6], b);
  }
  if (sgReconnectCount != sgLvLastSystemReconnect) {
    sgLvLastSystemReconnect = sgReconnectCount;
    snprintf(b, sizeof(b), "%lu", (unsigned long)sgReconnectCount);
    sgLvSetText(sgLvSystemLines[7], b);
  }
}

static void sgLvBuildSetting(lv_obj_t *root) {
  sgLvSettingVisualKey = 0xFFFFFFFFUL;
  const lv_color_t red = sgLvAccentColor();
  const lv_color_t panel = sgLvThemePanelColor();
  lv_obj_set_style_bg_color(root, sgLvThemeRootColor(), LV_PART_MAIN);
  sgLvBuildHeader(root); sgLvBuildCleanOverlayNoFooter(root);
  // [BETA STEP 1] Firmware version label - top-right header area
  sgLvVersion = sgLvLabel(root, SG_FIRMWARE_VERSION, 265, 1, SG_LV_WHITE, &lv_font_montserrat_14);
  lv_obj_set_width(sgLvVersion, 60);
  lv_obj_set_style_text_align(sgLvVersion, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
  if(sgFactoryResetConfirmActive){
    lv_obj_t *card=sgLvPanel(root,22,55,276,120); lv_obj_set_style_bg_color(card,panel,LV_PART_MAIN); lv_obj_set_style_border_color(card,red,LV_PART_MAIN); lv_obj_set_style_border_opa(card,LV_OPA_COVER,LV_PART_MAIN);
    lv_obj_t *q=sgLvLabel(root,sgLanguageThai()?"ยืนยันคืนค่าโรงงาน?":"CONFIRM FACTORY RESET?",0,70,SG_LV_ORANGE,sgLvUiFont()); lv_obj_set_width(q,320); lv_obj_set_style_text_align(q,LV_TEXT_ALIGN_CENTER,LV_PART_MAIN);
    lv_obj_t *h=sgLvLabel(root,sgLanguageThai()?"ค่าตั้งทั้งหมดจะถูกล้าง":"ALL SETTINGS WILL BE CLEARED",0,96,SG_LV_DIM,sgLvUiFont()); lv_obj_set_width(h,320); lv_obj_set_style_text_align(h,LV_TEXT_ALIGN_CENTER,LV_PART_MAIN);
    lv_obj_t *c=sgLvPanel(root,38,128,105,38); sgLvSettingLines[0]=sgLvLabel(c,sgLanguageThai()?"ยกเลิก":"CANCEL",0,10,SG_LV_WHITE,sgLvUiFont()); lv_obj_set_width(sgLvSettingLines[0],105); lv_obj_set_style_text_align(sgLvSettingLines[0],LV_TEXT_ALIGN_CENTER,LV_PART_MAIN);
    lv_obj_t *o=sgLvPanel(root,177,128,105,38); sgLvSettingLines[1]=sgLvLabel(o,sgLanguageThai()?"ตกลง":"OK",0,10,SG_LV_WHITE,sgLvUiFont()); lv_obj_set_width(sgLvSettingLines[1],105); lv_obj_set_style_text_align(sgLvSettingLines[1],LV_TEXT_ALIGN_CENTER,LV_PART_MAIN); return;
  }
  const char *names[SG_SETTING_ITEM_COUNT]={sgLanguageThai()?"ตั้งค่าจอ":"DISPLAY",sgTextLocalized(SG_TEXT_LANGUAGE),sgLanguageThai()?"อ่านโค้ดอัตโนมัติ":"AUTO DTC",sgLanguageThai()?"ความสมูทค่า":"SMOOTH LEVEL",sgLanguageThai()?"สีธีม":"THEME COLOR",sgLanguageThai()?"ประเภทน้ำมัน":"FUEL TYPE",sgUxFactoryResetText(),sgLanguageThai()?"ECT Alert 105C":"ECT Alert",sgLanguageThai()?"AFR Lean 15.5 / Rich 11.5":"AFR Alert"};
  constexpr int rowX = 6;
  constexpr int rowY = 31;
  constexpr int rowW = 308;
  constexpr int rowH = 27;
  constexpr int rowGap = 1;
  for(int i=0;i<SG_SETTING_ITEM_COUNT;++i){
    const int y = rowY + i * (rowH + rowGap);
    lv_obj_t *p=sgLvPanel(root,rowX,y,rowW,rowH); sgLvSettingPanels[i]=p;
    lv_obj_set_style_bg_color(p,panel,LV_PART_MAIN);
    lv_obj_set_style_bg_opa(p,LV_OPA_70,LV_PART_MAIN);
    lv_obj_set_style_border_color(p,red,LV_PART_MAIN);
    lv_obj_set_style_border_opa(p,LV_OPA_20,LV_PART_MAIN);
    lv_obj_set_style_border_width(p,1,LV_PART_MAIN);
    lv_obj_set_style_radius(p,1,LV_PART_MAIN);
    lv_obj_set_style_pad_all(p,0,LV_PART_MAIN);

    lv_obj_t *name=sgLvLabel(p,names[i],9,7,SG_LV_WHITE,sgLvUiFont());
    lv_obj_set_width(name,190);

    sgLvSettingLines[i]=sgLvLabel(p,"--",198,7,SG_LV_GREEN,sgLvUiFont());
    lv_obj_set_width(sgLvSettingLines[i],100);
    lv_obj_set_style_text_align(sgLvSettingLines[i],LV_TEXT_ALIGN_RIGHT,LV_PART_MAIN);

    if(i == SG_SETTING_FACTORY_RESET){
      lv_obj_set_style_border_color(p,SG_LV_ORANGE,LV_PART_MAIN);
      lv_obj_set_style_border_opa(p,LV_OPA_40,LV_PART_MAIN);
      lv_obj_set_style_text_color(name,SG_LV_ORANGE,LV_PART_MAIN);
      // ปุ่ม Factory Reset: ซ่อนชื่อด้านซ้าย แล้วใช้ label เดียว กึ่งกลางเต็มแถว
      lv_obj_add_flag(name, LV_OBJ_FLAG_HIDDEN);
      lv_obj_set_pos(sgLvSettingLines[i], 9, 7);
      lv_obj_set_width(sgLvSettingLines[i], rowW - 18);
      lv_obj_set_style_text_align(sgLvSettingLines[i], LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    }
    if(i == SG_SETTING_ECT_ALERT){
      sgLvSettingLines[i]=sgLvLabel(p,sgLanguageThai()?"รักษาอุณหภูมิ 105C":"ECT Alert 105C",198,7,SG_LV_WHITE,sgLvUiFont());
      lv_obj_set_width(sgLvSettingLines[i],100);
      lv_obj_set_style_text_align(sgLvSettingLines[i],LV_TEXT_ALIGN_RIGHT,LV_PART_MAIN);
    }
    if(i == SG_SETTING_AFR_ALERT){
      sgLvSettingLines[i]=sgLvLabel(p,sgLanguageThai()?"LEAN 15.5 RICH 11.5":"AFR Alert",198,7,SG_LV_WHITE,sgLvUiFont());
      lv_obj_set_width(sgLvSettingLines[i],100);
      lv_obj_set_style_text_align(sgLvSettingLines[i],LV_TEXT_ALIGN_RIGHT,LV_PART_MAIN);
    }
  }
}
static void sgLvBuildPage() {
  sgLvApplyThemePalette();
  lv_obj_t *root = lv_scr_act();
  lv_obj_clean(root);
  lv_obj_set_style_bg_color(root, SG_LV_BG, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(root, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);

  memset(sgLvDataPanels, 0, sizeof(sgLvDataPanels));
  memset(sgLvDataNames, 0, sizeof(sgLvDataNames));
  memset(sgLvDataLines, 0, sizeof(sgLvDataLines));
  sgLvDataEmpty = nullptr;
  memset(sgLvDtcLines, 0, sizeof(sgLvDtcLines));
  sgLvDtcSpinner = nullptr;
  sgLvDtcMainShadow = nullptr;
  memset(sgLvSettingLines, 0, sizeof(sgLvSettingLines));
  memset(sgLvSettingPanels, 0, sizeof(sgLvSettingPanels));
  memset(sgLvRpmBars, 0, sizeof(sgLvRpmBars));
  memset(sgLvSystemLines, 0, sizeof(sgLvSystemLines));
  sgLvSystemHealth = nullptr;
  sgLvPanelBat = sgLvPanelTps = sgLvPanelTemp = sgLvPanelAfr = nullptr;
  sgLvPanelInj = sgLvPanelIgn = sgLvPanelStft = sgLvPanelLtft = nullptr;
  sgLvAfrBold = nullptr;
  sgLvStftBold = nullptr;
  sgLvAdaptiveBox = nullptr;
  sgLvAdaptiveText = nullptr;
  sgLvLastAdaptiveType = SG_WARN_NONE;
  sgLvLastAdaptiveSeverity = SG_SMART_OK;
  sgLvLastAdaptiveConnected = false;
  sgLvLastAdaptiveThai = sgLanguageThai();
  sgLvLastAdaptiveDayTheme = sgDayThemeActive;
  sgLvAdaptiveRenderValid = false;
  sgLvLastDtcAdviceSlot = -1;

  if (sgPage == SG_PAGE_GAUGE) sgLvBuildGauge(root);
  else if (sgPage == SG_PAGE_DATA) sgLvBuildData(root);
  else if (sgPage == SG_PAGE_DTC) sgLvBuildDtc(root);
  else if (sgPage == SG_PAGE_SETTING) sgLvBuildSetting(root);
  else { sgPage = SG_PAGE_GAUGE; sgLvBuildGauge(root); }

  sgLvLastPage = (int8_t)sgPage;
  sgLvLastFastMs = sgLvLastMediumMs = sgLvLastSlowMs = 0;
  sgLvLastRpmColorZone = sgLvLastAfrColorZone = sgLvLastBatColorZone = sgLvLastTempColorZone = -1;
  sgLvLastPage1Tps0 = INT32_MIN;
  sgLvLastPage1Ign10 = INT32_MIN;
  sgLvLastPage1Bat10 = INT32_MIN;
  sgLvLastPage1Temp = INT32_MIN;
  sgLvLastHoldFormatKey = UINT32_MAX;
  sgLvCachedHoldText[0] = '\0';
  sgLvNeedBuild = false;
}

static void sgLvUpdateCommon() {
  const uint32_t animNow = millis();
  const bool pausedConn = (sgPage == SG_PAGE_DTC || sgPage == SG_PAGE_SETTING);
  const bool lostConn = !sgGauge.ecuConnected || sgConnectionHealth == SG_HEALTH_LOST;

  sgLvSetText(sgLvModel, sgLvHeaderModelText());
  if (sgLvModel) {
    sgLvSetTextFontIfChanged(sgLvModel, sgLvUiFont());
    if (pausedConn) {
      sgLvSetXIfChanged(sgLvModel, 137);
      sgLvSetTextOpaIfChanged(sgLvModel, LV_OPA_COVER);
      sgLvSetTextColorIfChanged(sgLvModel, SG_LV_ORANGE);
    } else if (lostConn) {
      sgLvSetXIfChanged(sgLvModel, 137);
      sgLvSetTextOpaIfChanged(sgLvModel, LV_OPA_COVER);
      sgLvSetTextColorIfChanged(sgLvModel, SG_LV_RED);
    } else {
      sgLvSetXIfChanged(sgLvModel, 137);
      sgLvSetTextOpaIfChanged(sgLvModel, LV_OPA_COVER);
      sgLvSetTextColorIfChanged(sgLvModel, SG_LV_GREEN);
    }
  }
  const char *pageText = sgPage == SG_PAGE_GAUGE ? (sgModelName[0] ? sgModelName : "GAUGE") :
                         (sgPage == SG_PAGE_DATA ? "LIVE DATA" :
                         (sgPage == SG_PAGE_DTC ? "DIAGNOSTIC" : "SETTINGS"));
  sgLvSetText(sgLvPage, pageText);
  if (sgLvStatus) {
    sgLvSetTextFontIfChanged(sgLvStatus, sgLvUiFont());
    const char *smartText = sgLvSmartFooterText();
    sgLvSetText(sgLvStatus, smartText ? smartText : sgLvHealthText());
    sgLvSetTextColorIfChanged(sgLvStatus, smartText ? sgLvSmartFooterColor() : sgLvHealthColor());
  }

  if (sgLvBt) {
    const lv_img_dsc_t *btSrc = &sg_bt_icon_red;
    lv_opa_t btOpa = LV_OPA_COVER;

    if (sgBtReady && sgBtClientConnected) {
      // Connected: ไอคอนสีน้ำเงินค้าง ไม่กระพริบ
      btSrc = &sg_bt_icon_blue;
      btOpa = LV_OPA_COVER;
    } else if (sgBtReady && sgBtConnectWindowActive) {
      // Pairing / Advertising: ติด 250 ms / ดับ 250 ms
      const bool btBlink = ((animNow / 250UL) & 1U) == 0U;
      btSrc = &sg_bt_icon_blue;
      btOpa = btBlink ? LV_OPA_COVER : LV_OPA_TRANSP;
    } else if (sgBtReady && sgBtIdleOffDueMs != 0) {
      // Reconnecting: ติด 1000 ms / ดับ 1000 ms
      const bool btBlink = ((animNow / 1000UL) & 1U) == 0U;
      btSrc = &sg_bt_icon_blue;
      btOpa = btBlink ? LV_OPA_COVER : LV_OPA_TRANSP;
    } else {
      // Bluetooth Off / Not Ready: ไอคอนสีแดงค้าง
      btSrc = &sg_bt_icon_red;
      btOpa = LV_OPA_COVER;
    }

    if (lv_img_get_src(sgLvBt) != btSrc) {
      lv_img_set_src(sgLvBt, btSrc);
    }
    sgLvSetImageOpaIfChanged(sgLvBt, btOpa);
  }

  if (sgButtonHoldVisualActive && sgButtonHoldVisualTargetMs > 0) {
    // ซ่อนคำใบ้แตะเปลี่ยนหน้าชั่วคราว เพื่อไม่ให้ซ้อนกับข้อความกดค้าง
    if (sgLvFooterHint) lv_obj_add_flag(sgLvFooterHint, LV_OBJ_FLAG_HIDDEN);
    const uint32_t held = sgButtonHoldVisualElapsedMs;
    // เวอร์ชันนี้ใช้ข้อความนับถอยหลังด้านล่างแทนแถบ/Notice กลางจอ
    if (sgLvHold) {
      sgLvSetBarValueIfChanged(sgLvHold, 0);
      lv_obj_add_flag(sgLvHold, LV_OBJ_FLAG_HIDDEN);
    }

    lv_color_t c = SG_LV_GREEN;
    if (sgPage == SG_PAGE_SETTING) {
      if (held >= sgButtonHoldVisualTargetMs) c = (sgSettingItem == SG_SETTING_FACTORY_RESET) ? SG_LV_ORANGE : SG_LV_CYAN;
    } else if (sgPage == SG_PAGE_DTC) {
      if (held >= SG_BUTTON_HOLD5_MIN_MS) c = SG_LV_CYAN;
    } else if (sgPage == SG_PAGE_GAUGE) {
      if (held >= SG_BUTTON_HOLD5_MIN_MS) c = SG_LV_ORANGE;
    } else {
      if (held >= SG_BUTTON_HOLD5_MIN_MS) c = SG_LV_ORANGE;
    }
    const uint32_t target = sgButtonHoldVisualTargetMs;
    const uint32_t remainMs = (held >= target) ? 0UL : (target - held);
    const uint32_t remainTenth = (remainMs + 99UL) / 100UL;
    const char *actionTh = "";
    const char *actionEn = "";
    if (sgPage == SG_PAGE_SETTING) {
      if (target == SG_BUTTON_HOLD3_MIN_MS) {
        actionTh = "บันทึกและออก"; actionEn = "SAVE AND EXIT";
      } else if (sgFactoryResetConfirmActive || sgSettingUiMode == SG_SETTING_EDIT) {
        actionTh = "ตกลง"; actionEn = "OK";
      } else if (sgSettingUiMode == SG_SETTING_SELECT) {
        actionTh = "เลือก"; actionEn = "SELECT";
      } else {
        actionTh = "ตั้งค่า"; actionEn = "SETTINGS";
      }
    }
    const bool thai = sgLanguageThai();
    const uint32_t displayRemain = (sgPage == SG_PAGE_SETTING) ? remainTenth : ((remainMs + 999UL) / 1000UL);
    uint32_t holdFormatKey = displayRemain;
    holdFormatKey = holdFormatKey * 5U + (uint32_t)sgPage;
    holdFormatKey = holdFormatKey * 2U + (thai ? 1U : 0U);
    holdFormatKey = holdFormatKey * 4U + (uint32_t)sgSettingUiMode;
    holdFormatKey = holdFormatKey * 2U + (sgFactoryResetConfirmActive ? 1U : 0U);
    holdFormatKey = holdFormatKey * 8U + (uint32_t)sgSettingItem;
    holdFormatKey = holdFormatKey * 2U + (remainTenth == 0 ? 1U : 0U);
    if (holdFormatKey != sgLvLastHoldFormatKey) {
      if (remainTenth == 0) {
        snprintf(sgLvCachedHoldText, sizeof(sgLvCachedHoldText), thai ? "ปล่อยเพื่อยืนยัน" : "RELEASE TO CONFIRM");
      } else if (sgPage == SG_PAGE_SETTING) {
        snprintf(sgLvCachedHoldText, sizeof(sgLvCachedHoldText), thai ? "%s %lu.%lu วิ" : "%s %lu.%lus",
                 thai ? actionTh : actionEn,
                 (unsigned long)(remainTenth / 10UL), (unsigned long)(remainTenth % 10UL));
      } else {
        snprintf(sgLvCachedHoldText, sizeof(sgLvCachedHoldText), thai ? "กดค้างต่ออีก %lu วิ" : "HOLD %lu MORE SEC",
                 (unsigned long)displayRemain);
      }
      sgLvLastHoldFormatKey = holdFormatKey;
    }
    if (sgLvHoldText) {
      sgLvSetText(sgLvHoldText, "");
      lv_obj_add_flag(sgLvHoldText, LV_OBJ_FLAG_HIDDEN);
    }
    if (sgLvModel) {
      lv_obj_set_width(sgLvModel, 224);
      sgLvSetTextFontIfChanged(sgLvModel, sgLvUiFont());
      sgLvSetTextColorIfChanged(sgLvModel, c);
      sgLvSetTextOpaIfChanged(sgLvModel, LV_OPA_COVER);
      sgLvSetText(sgLvModel, sgLvCachedHoldText);
      if (sgLvBt) lv_obj_add_flag(sgLvBt, LV_OBJ_FLAG_HIDDEN);
      if (sgLvPage) lv_obj_add_flag(sgLvPage, LV_OBJ_FLAG_HIDDEN);
    }
  } else {
    sgLvLastHoldFormatKey = UINT32_MAX;
    sgLvCachedHoldText[0] = '\0';
    if (sgLvHold) {
      sgLvSetBarValueIfChanged(sgLvHold, 0);
      lv_obj_add_flag(sgLvHold, LV_OBJ_FLAG_HIDDEN);
    }
    if (sgLvHoldText) {
      sgLvSetText(sgLvHoldText, "");
      lv_obj_add_flag(sgLvHoldText, LV_OBJ_FLAG_HIDDEN);
    }
    if (sgLvFooterHint) {
      lv_obj_clear_flag(sgLvFooterHint, LV_OBJ_FLAG_HIDDEN);
      lv_obj_set_style_text_font(sgLvFooterHint, sgLvUiFont(), LV_PART_MAIN);
      sgLvSetTextColorIfChanged(sgLvFooterHint, SG_LV_DIM);
      sgLvSetText(sgLvFooterHint, sgTextLocalized(SG_TEXT_BUTTON_HINT));
    }
    if (sgLvModel) lv_obj_set_width(sgLvModel, 124);
    if (sgLvBt) lv_obj_clear_flag(sgLvBt, LV_OBJ_FLAG_HIDDEN);
    if (sgLvPage) lv_obj_clear_flag(sgLvPage, LV_OBJ_FLAG_HIDDEN);
  }

  const bool noticeActive = (sgUiNotice[0] || sgUiNoticeTextId != SG_TEXT_NONE) &&
                            (int32_t)(sgUiNoticeUntilMs - millis()) > 0;
  if (sgLvNotice) {
    if (noticeActive) {
      const bool localizedNotice = !sgUiNotice[0] && sgUiNoticeTextId != SG_TEXT_NONE;
      const char *txt = sgUiNotice[0] ? sgUiNotice : sgTextLocalized((SgTextId)sgUiNoticeTextId);
      lv_obj_set_style_text_font(sgLvNotice, (localizedNotice || sgLanguageThai()) ? sgLvUiFont() : LV_FONT_DEFAULT, LV_PART_MAIN);
      sgLvSetText(sgLvNotice, txt);
      lv_obj_clear_flag(sgLvNotice, LV_OBJ_FLAG_HIDDEN);
    } else {
      lv_obj_add_flag(sgLvNotice, LV_OBJ_FLAG_HIDDEN);
      if ((int32_t)(sgUiNoticeUntilMs - millis()) <= 0) {
        sgUiNotice[0] = '\0'; sgUiNoticeTextId = SG_TEXT_NONE;
      }
    }
  }
}

static lv_color_t sgLvAfrColor(float afr) {
  if (afr <= 0.1f) return SG_LV_DIM;
  if (sgDayThemeActive) {
    if (afr > 15.0f) return lv_color_hex(0xFF4A3D);
    if (afr < 10.0f) return lv_color_hex(0xFFE66D);
    return lv_color_hex(0xFFB347);
  }
  if (afr > 15.0f) return lv_color_hex(0xFF2418);
  if (afr < 10.0f) return lv_color_hex(0xFFE04A);
  return lv_color_hex(0xFF9F1A);
}

static lv_color_t sgLvBatteryColor(float v) {
  if (v < 10.0f || v > 15.0f) return SG_LV_RED;
  if (v < 11.5f) return SG_LV_YELLOW;
  return sgDayThemeActive ? lv_color_hex(0x202428) : SG_LV_WHITE;
}

static lv_color_t sgLvTempColor(int c) {
  if (c >= SG_ECT_ALERT_TEMP_C) return lv_color_hex(SG_ECT_ALERT_RED_HEX);
  if (c >= 100) return SG_LV_ORANGE;
  return sgDayThemeActive ? lv_color_hex(0x202428) : SG_LV_WHITE;
}


static const char* sgLvAdaptiveAdviceText(bool thai) {
  switch (sgActiveWarning) {
    case SG_WARN_TEMP_DANGER:
      return thai ? "TEMP สูงมาก หยุดพัก" : "TEMP DANGER STOP";
    case SG_WARN_TEMP_HIGH:
      return thai ? "TEMP สูง ลดรอบ" : "TEMP HIGH EASE";
    case SG_WARN_BAT_LOW:
      return thai ? "แบตต่ำ ตรวจชาร์จ" : "LOW BAT CHECK";
    case SG_WARN_BAT_HIGH:
      return thai ? "ไฟชาร์จสูง" : "CHARGE HIGH";
    case SG_WARN_AFR_LEAN:
      return thai ? "AFR บาง ตรวจอากาศ" : "AFR LEAN CHECK";
    case SG_WARN_AFR_RICH:
      return thai ? "AFR หนา ตรวจหัวฉีด" : "AFR RICH CHECK";
    case SG_WARN_KLINE:
      return thai ? "K-LINE ไม่นิ่ง" : "K-LINE UNSTABLE";
    default:
      return "";
  }
}

static lv_obj_t* sgLvAdaptivePanelForWarning() {
  switch (sgActiveWarning) {
    case SG_WARN_TEMP_HIGH:
    case SG_WARN_TEMP_DANGER: return sgLvPanelTemp;
    case SG_WARN_BAT_LOW:
    case SG_WARN_BAT_HIGH:    return sgLvPanelBat;
    case SG_WARN_AFR_LEAN:
    case SG_WARN_AFR_RICH:    return sgLvPanelAfr;
    // ไม่เน้นกรอบ RPM ตอน K-LINE ไม่นิ่ง เพราะตอนเปิดกุญแจ/ยังไม่สตาร์ต RPM=0 ทำให้เข้าใจผิด
    case SG_WARN_KLINE:       return nullptr;
    default:                  return nullptr;
  }
}

static lv_obj_t* sgLvAdaptiveValueForWarning() {
  switch (sgActiveWarning) {
    case SG_WARN_TEMP_HIGH:
    case SG_WARN_TEMP_DANGER: return sgLvTemp;
    case SG_WARN_BAT_LOW:
    case SG_WARN_BAT_HIGH:    return sgLvBat;
    case SG_WARN_AFR_LEAN:
    case SG_WARN_AFR_RICH:    return sgLvAfr;
    default:                  return nullptr;
  }
}

static void sgLvPanelAlertStyle(lv_obj_t *panel, bool active, lv_color_t c) {
  if (!panel) return;
  sgLvSetBorderWidthIfChanged(panel, active ? 2 : 1);
  sgLvSetBorderOpaIfChanged(panel, active ? LV_OPA_COVER : LV_OPA_40);
  sgLvSetBorderColorIfChanged(panel, active ? c : SG_LV_BORDER);
  sgLvSetBgColorIfChanged(panel, active ? (sgDayThemeActive ? lv_color_hex(0x3C3030) : lv_color_hex(0x21151A)) : (sgDayThemeActive ? SG_LV_PANEL : lv_color_hex(0x08141D)));
}

static void sgLvUpdateAdaptiveDashboard() {
  const bool connected = sgGauge.ecuConnected;
  const bool active = connected && sgActiveWarning != SG_WARN_NONE;
  const bool thai = sgLanguageThai();

  // BETA73: งาน adaptive warning ไม่ต้องเขียน LVGL ซ้ำเมื่อ state ที่มีผลต่อภาพยังเหมือนเดิม
  // ไม่เปลี่ยน threshold, confirm timing, blink cadence หรือรูปลักษณ์เดิม
  const bool renderChanged =
      !sgLvAdaptiveRenderValid ||
      sgLvLastAdaptiveConnected != connected ||
      sgLvLastAdaptiveType != sgActiveWarning ||
      sgLvLastAdaptiveSeverity != sgActiveWarningSeverity ||
      sgLvLastAdaptiveThai != thai ||
      sgLvLastAdaptiveDayTheme != sgDayThemeActive;

  if (!renderChanged) return;

  if (!active) {
    if (!sgLvAdaptiveRenderValid || sgLvLastAdaptiveType != SG_WARN_NONE || sgLvLastAdaptiveConnected) {
      sgLvPanelAlertStyle(sgLvPanelTemp, false, SG_LV_BORDER);
      sgLvPanelAlertStyle(sgLvPanelBat, false, SG_LV_BORDER);
      sgLvPanelAlertStyle(sgLvPanelAfr, false, SG_LV_BORDER);
      if (sgLvTemp) sgLvSetTextFontIfChanged(sgLvTemp, &lv_font_montserrat_20);
      if (sgLvBat) sgLvSetTextFontIfChanged(sgLvBat, &lv_font_montserrat_20);
      if (sgLvAfr) sgLvSetTextFontIfChanged(sgLvAfr, &lv_font_montserrat_32);
      if (sgLvTemp) sgLvSetTextColorIfChanged(sgLvTemp, sgLvTempColor((int)sgGauge.tempC));
      if (sgLvBat) sgLvSetTextColorIfChanged(sgLvBat, sgLvBatteryColor(sgGauge.batteryV));
    }
    sgLvSetHiddenIfChanged(sgLvAdaptiveBox, true);
  } else {
    const lv_color_t c = sgActiveWarningSeverity >= SG_SMART_DANGER ? SG_LV_RED : SG_LV_ORANGE;

    // คืน style ของทุก panel ก่อนใช้ warning ใหม่ เพื่อให้การสลับชนิดเตือนเหมือนพฤติกรรมเดิม
    if (!sgLvAdaptiveRenderValid || sgActiveWarning != sgLvLastAdaptiveType) {
      sgLvPanelAlertStyle(sgLvPanelTemp, false, SG_LV_BORDER);
      sgLvPanelAlertStyle(sgLvPanelBat, false, SG_LV_BORDER);
      sgLvPanelAlertStyle(sgLvPanelAfr, false, SG_LV_BORDER);
      if (sgLvTemp) sgLvSetTextFontIfChanged(sgLvTemp, &lv_font_montserrat_20);
      if (sgLvBat) sgLvSetTextFontIfChanged(sgLvBat, &lv_font_montserrat_20);
      if (sgLvAfr) sgLvSetTextFontIfChanged(sgLvAfr, &lv_font_montserrat_32);
      if (sgLvTemp) sgLvSetTextColorIfChanged(sgLvTemp, sgLvTempColor((int)sgGauge.tempC));
      if (sgLvBat) sgLvSetTextColorIfChanged(sgLvBat, sgLvBatteryColor(sgGauge.batteryV));
    }

    sgLvPanelAlertStyle(sgLvAdaptivePanelForWarning(), true, c);
    lv_obj_t *adaptiveValue = sgLvAdaptiveValueForWarning();
    if (adaptiveValue) {
      sgLvSetTextFontIfChanged(adaptiveValue,
        (sgActiveWarning == SG_WARN_AFR_LEAN || sgActiveWarning == SG_WARN_AFR_RICH)
          ? &lv_font_montserrat_32 : &lv_font_montserrat_20);
      sgLvSetTextColorIfChanged(adaptiveValue, c);
    }
    if (sgLvAdaptiveBox && sgLvAdaptiveText) {
      sgLvSetBorderColorIfChanged(sgLvAdaptiveBox, c);
      sgLvSetTextColorIfChanged(sgLvAdaptiveText, c);
      sgLvSetTextFontIfChanged(sgLvAdaptiveText, sgLvUiFont());
      sgLvSetText(sgLvAdaptiveText, sgLvAdaptiveAdviceText(thai));
      sgLvSetHiddenIfChanged(sgLvAdaptiveBox, false);
      lv_obj_move_foreground(sgLvAdaptiveBox);
    }
  }

  sgLvLastAdaptiveConnected = connected;
  sgLvLastAdaptiveType = sgActiveWarning;
  sgLvLastAdaptiveSeverity = sgActiveWarningSeverity;
  sgLvLastAdaptiveThai = thai;
  sgLvLastAdaptiveDayTheme = sgDayThemeActive;
  sgLvAdaptiveRenderValid = true;
}

static const char* sgLvDtcAdviceText() {
  if (!sgDtc.readOk || sgDtc.count == 0) return nullptr;
  const char *line = sgDtc.lines[0];
  const bool th = sgLanguageThai();
  if (strstr(line, "TPS") || strstr(line, "TP")) return th ? "แนะนำ: ตรวจปลั๊ก TPS/สายไฟ" : "TIP: CHECK TPS PLUG/WIRE";
  if (strstr(line, "ECT")) return th ? "แนะนำ: ตรวจเซ็นเซอร์อุณหภูมิ" : "TIP: CHECK ECT SENSOR";
  if (strstr(line, "MAP")) return th ? "แนะนำ: ตรวจท่อ/ปลั๊ก MAP" : "TIP: CHECK MAP HOSE/PLUG";
  if (strstr(line, "INJECTOR") || strstr(line, "หัวฉีด")) return th ? "แนะนำ: ตรวจหัวฉีด/ปลั๊ก" : "TIP: CHECK INJECTOR";
  if (strstr(line, "O2") || strstr(line, "A/F")) return th ? "แนะนำ: ตรวจเซ็นเซอร์ไอเสีย" : "TIP: CHECK O2/A-F SENSOR";
  if (strstr(line, "CKP") || strstr(line, "CMP")) return th ? "แนะนำ: ตรวจสัญญาณรอบเครื่อง" : "TIP: CHECK CKP/CMP";
  if (strstr(line, "BAT") || strstr(line, "ไฟ")) return th ? "แนะนำ: ตรวจไฟเลี้ยง/กราวด์" : "TIP: CHECK POWER/GROUND";
  return th ? "แนะนำ: ตรวจปลั๊กและสายไฟ" : "TIP: CHECK PLUGS/WIRING";
}

static void sgLvUpdateGauge(uint32_t now) {
  char b[24];

  // RPM vector: จำกัดประมาณ 50 FPS และอัปเดตเฉพาะเมื่อค่า/โซนสีเปลี่ยน
  if (now - sgLvLastFastMs >= 20UL) {
    sgLvLastFastMs = now;
    const int8_t zone = sgGauge.rpm >= 8000 ? 2 : (sgGauge.rpm >= 6000 ? 1 : 0);
    const lv_color_t rpmColor = sgLvRpmColorForZone(zone);
    if ((int32_t)sgGauge.rpm != sgLvLastVectorRpm || zone != sgLvLastRpmColorZone) {
      sgLvLastVectorRpm = sgGauge.rpm;
      sgLvLastRpmColorZone = zone;
      sgLvUpdateVectorRpm(sgGauge.rpm, rpmColor);
      sgLvUpdateRpmBars(sgGauge.rpm);
    }
  }

  if (now - sgLvLastMediumMs >= 40UL) {
    sgLvLastMediumMs = now;
    const int32_t tps0 = (int32_t)(sgGauge.tps + (sgGauge.tps >= 0.0f ? 0.5f : -0.5f));
    if (tps0 != sgLvLastPage1Tps0) {
      sgLvLastPage1Tps0 = tps0;
      snprintf(b, sizeof(b), "%.0f%%", sgGauge.tps);
      sgLvSetText(sgLvTps, b);
    }
    const int32_t ign10 = (int32_t)(sgGauge.ignitionDeg * 10.0f + (sgGauge.ignitionDeg >= 0.0f ? 0.5f : -0.5f));
    if (ign10 != sgLvLastPage1Ign10) {
      sgLvLastPage1Ign10 = ign10;
      snprintf(b, sizeof(b), "%.1f °", sgGauge.ignitionDeg);
      sgLvSetText(sgLvIgn, b);
    }
    const int32_t afr10 = (int32_t)(sgGauge.afr * 10.0f + 0.5f);
    const int8_t afrZone = sgGauge.afr <= 0.1f ? 3 : (sgGauge.afr > 15.0f ? 2 : (sgGauge.afr < 10.0f ? 1 : 0));
    if (afr10 != sgLvLastVectorAfr10 || afrZone != sgLvLastAfrColorZone) {
      sgLvLastVectorAfr10 = afr10;
      sgLvLastAfrColorZone = afrZone;
      sgLvUpdateVectorAfr(sgGauge.afr, sgLvAfrColor(sgGauge.afr));
    }
  }

  if (now - sgLvLastSlowMs >= 100UL) {
    sgLvLastSlowMs = now;
    const int32_t bat10 = (int32_t)(sgGauge.batteryV * 10.0f + (sgGauge.batteryV >= 0.0f ? 0.5f : -0.5f));
    if (bat10 != sgLvLastPage1Bat10) {
      sgLvLastPage1Bat10 = bat10;
      snprintf(b, sizeof(b), "%.1f V", sgGauge.batteryV);
      sgLvSetText(sgLvBat, b);
    }
    const int32_t temp0 = (int32_t)sgGauge.tempC;
    if (temp0 != sgLvLastPage1Temp) {
      sgLvLastPage1Temp = temp0;
      snprintf(b, sizeof(b), "%d °C", (int)sgGauge.tempC);
      sgLvSetText(sgLvTemp, b);
    }
    const int8_t batZone = (sgGauge.batteryV < 10.0f || sgGauge.batteryV > 15.0f) ? 2 : (sgGauge.batteryV < 11.5f ? 1 : 0);
    if (batZone != sgLvLastBatColorZone) {
      sgLvLastBatColorZone = batZone;
      sgLvSetTextColorIfChanged(sgLvBat, sgLvBatteryColor(sgGauge.batteryV));
    }
    const int8_t tempZone = sgGauge.tempC >= 120 ? 2 : (sgGauge.tempC >= 100 ? 1 : 0);
    if (tempZone != sgLvLastTempColorZone) {
      sgLvLastTempColorZone = tempZone;
      sgLvSetTextColorIfChanged(sgLvTemp, sgLvTempColor((int)sgGauge.tempC));
    }
    sgLvUpdateAdaptiveDashboard();
  }
}

static bool sgLvDataValueReady(SgValueId id, bool auxTable) {
  if (!sgProfile || !sgGauge.ecuConnected) return false;
  if (!sgValueValidated(sgProfile->id, id)) return false;
  if (auxTable) return sgGauge.t20Ok;
  return sgGauge.t17Ok;
}

static void sgLvDataSetVisible(uint8_t index, bool visible, uint8_t visibleIndex, uint8_t visibleCount) {
  if (!sgLvDataPanels[index]) return;
  if (!visible) {
    sgLvSetHiddenIfChanged(sgLvDataPanels[index], true);
    return;
  }

  // สองคอลัมน์แบบอ่านจากซ้ายไปขวาในแต่ละแถว และยืดเต็มพื้นที่โดยไม่มี footer
  const uint8_t rows = (visibleCount + 1U) / 2U;
  const int topY = 28;
  const int usableH = 207;
  int step = rows ? (usableH / rows) : 24;
  if (step > 35) step = 35;
  if (step < 19) step = 19;
  const int panelH = step - 2;
  const int col = visibleIndex & 1U;
  const int row = visibleIndex >> 1U;

  sgLvSetPosIfChanged(sgLvDataPanels[index], 5 + col * 158, topY + row * step);
  sgLvSetSizeIfChanged(sgLvDataPanels[index], 152, panelH);

  const int textY = panelH >= 30 ? 5 : (panelH >= 24 ? 3 : 2);
  sgLvSetYIfChanged(sgLvDataNames[index], textY);
  sgLvSetYIfChanged(sgLvDataLines[index], textY);
  sgLvSetTextFontIfChanged(sgLvDataNames[index],
                             panelH >= 28 ? &lv_font_montserrat_14 : &lv_font_montserrat_12);
  sgLvSetTextFontIfChanged(sgLvDataLines[index],
                             panelH >= 28 ? &lv_font_montserrat_18 : &lv_font_montserrat_14);
  const bool primary = (index == 0 || index == 1 || index == 3 || index == 10 ||
                        index == 11 || index == 12 || index == 13 || index == 15 || index == 16);
  sgLvSetTextColorIfChanged(sgLvDataNames[index],
                              primary ? sgLvAccentColor() : SG_LV_DIM);
  sgLvSetBorderOpaIfChanged(sgLvDataPanels[index],
                              primary ? LV_OPA_COVER : LV_OPA_60);
  sgLvSetHiddenIfChanged(sgLvDataPanels[index], false);
}

static void sgLvUpdateData() {
  char b[40];
  uint8_t shown = 0;

  // แสดงเฉพาะค่าที่โปรไฟล์รองรับและได้รับเฟรมข้อมูลจริงแล้ว
  bool ready[18] = {
    sgLvDataValueReady(SG_VAL_RPM, false),
    sgLvDataValueReady(SG_VAL_TPS, false),
    sgLvDataValueReady(SG_VAL_TPS_VOLT, false),
    sgLvDataValueReady(SG_VAL_ECT, false),
    sgLvDataValueReady(SG_VAL_ECT_VOLT, false),
    sgLvDataValueReady(SG_VAL_IAT, false),
    sgLvDataValueReady(SG_VAL_IAT_VOLT, false),
    sgLvDataValueReady(SG_VAL_MAP, false),
    sgLvDataValueReady(SG_VAL_MAP_VOLT, false),
    sgLvDataValueReady(SG_VAL_SPEED, false),
    sgLvDataValueReady(SG_VAL_BATTERY, false),
    sgLvDataValueReady(SG_VAL_INJECTOR, false),
    sgLvDataValueReady(SG_VAL_IGNITION, false),
    sgLvDataValueReady(SG_VAL_AFR, true),
    sgLvDataValueReady(SG_VAL_O2, true),
    sgGauge.ecuConnected && sgGauge.t20Ok,
    sgGauge.ecuConnected && sgGauge.ltftValid,
    sgLvDataValueReady(SG_VAL_IACV, false)
  };

  uint8_t readyCount = 0;
  for (uint8_t i = 0; i < 18; ++i) if (ready[i]) readyCount++;

  // วางค่าตามลำดับความสำคัญ โดยยังใช้ readiness/capability ของ index เดิมทั้งหมด
  for (uint8_t displayPos = 0; displayPos < 18; ++displayPos) {
    const uint8_t dataIndex = SG_LV_DATA_DISPLAY_ORDER[displayPos];
    sgLvDataSetVisible(dataIndex, ready[dataIndex], shown, readyCount);
    if (ready[dataIndex]) shown++;
  }

  if (sgLvDataEmpty) {
    if (shown == 0) lv_obj_clear_flag(sgLvDataEmpty, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(sgLvDataEmpty, LV_OBJ_FLAG_HIDDEN);
  }

  const int32_t shownValue[18] = {
    (int32_t)sgGauge.rpm,
    sgLvQuantizeSigned(sgGauge.tps, 10.0f),
    sgLvQuantizeSigned(sgGauge.tpsVolt, 100.0f),
    (int32_t)sgGauge.tempC,
    sgLvQuantizeSigned(sgGauge.ectVolt, 100.0f),
    (int32_t)sgGauge.iatC,
    sgLvQuantizeSigned(sgGauge.iatVolt, 100.0f),
    sgLvQuantizeSigned(sgGauge.mapKpa, 10.0f),
    sgLvQuantizeSigned(sgGauge.mapVolt, 100.0f),
    (int32_t)sgGauge.speedKmh,
    sgLvQuantizeSigned(sgGauge.batteryV, 10.0f),
    sgLvQuantizeSigned(sgGauge.injectorMs, 100.0f),
    sgLvQuantizeSigned(sgGauge.ignitionDeg, 10.0f),
    sgLvQuantizeSigned(sgGauge.afr, 100.0f),
    sgLvQuantizeSigned(sgGauge.o2Volt, 100.0f),
    sgLvQuantizeSigned(sgGauge.stftPct, 10.0f),
    sgLvQuantizeSigned(sgGauge.ltftPct, 10.0f),
    (int32_t)sgGauge.iacvPulse
  };

  for (uint8_t i = 0; i < 18; ++i) {
    if (!ready[i] || sgLvLastPage2Value[i] == shownValue[i]) continue;
    sgLvLastPage2Value[i] = shownValue[i];
    switch (i) {
      case 0:  snprintf(b,sizeof(b),"%u",sgGauge.rpm); break;
      case 1:  snprintf(b,sizeof(b),"%.1f %%",sgGauge.tps); break;
      case 2:  snprintf(b,sizeof(b),"%.2f V",sgGauge.tpsVolt); break;
      case 3:  snprintf(b,sizeof(b),"%d °C",sgGauge.tempC); break;
      case 4:  snprintf(b,sizeof(b),"%.2f V",sgGauge.ectVolt); break;
      case 5:  snprintf(b,sizeof(b),"%d °C",sgGauge.iatC); break;
      case 6:  snprintf(b,sizeof(b),"%.2f V",sgGauge.iatVolt); break;
      case 7:  snprintf(b,sizeof(b),"%.1f kPa",sgGauge.mapKpa); break;
      case 8:  snprintf(b,sizeof(b),"%.2f V",sgGauge.mapVolt); break;
      case 9:  snprintf(b,sizeof(b),"%u km/h",sgGauge.speedKmh); break;
      case 10: snprintf(b,sizeof(b),"%.1f V",sgGauge.batteryV); break;
      case 11: snprintf(b,sizeof(b),"%.2f ms",sgGauge.injectorMs); break;
      case 12: snprintf(b,sizeof(b),"%.1f°",sgGauge.ignitionDeg); break;
      case 13: snprintf(b,sizeof(b),"%.2f",sgGauge.afr); break;
      case 14: snprintf(b,sizeof(b),"%.2f V",sgGauge.o2Volt); break;
      case 15: snprintf(b,sizeof(b),"%+.1f %%",sgGauge.stftPct); break;
      case 16: snprintf(b,sizeof(b),"%+.1f %%",sgGauge.ltftPct); break;
      default: snprintf(b,sizeof(b),"%u",sgGauge.iacvPulse); break;
    }
    sgLvSetText(sgLvDataLines[i], b);
  }
}

static const char* sgLvDtcStatusLocalized() {
  if (!sgLanguageThai()) return sgDtc.status;
  if (strstr(sgDtc.status, "NO CODE")) return sgTextThai(SG_TEXT_NO_CODE);
  if (strstr(sgDtc.status, "OK")) return sgTextThai(SG_TEXT_SYSTEM_NORMAL);
  if (strstr(sgDtc.status, "OFFLINE")) return sgTextThai(SG_TEXT_OFFLINE);
  if (strstr(sgDtc.status, "DISABLED")) return sgTextThai(SG_TEXT_DTC_CLEAR_DISABLED);
  if (strstr(sgDtc.status, "RUN") || strstr(sgDtc.status, "LOCK")) return sgTextThai(SG_TEXT_READ_DISABLED_ENGINE);
  return sgDtc.status;
}

static void sgLvSetDtcMain(const char *text, lv_color_t color) {
  sgLvSetText(sgLvDtcLines[0], text);
  sgLvSetText(sgLvDtcMainShadow, text);
  if (sgLvDtcLines[0]) lv_obj_set_style_text_color(sgLvDtcLines[0], color, LV_PART_MAIN);
  if (sgLvDtcMainShadow) lv_obj_set_style_text_color(sgLvDtcMainShadow, color, LV_PART_MAIN);
}

static void sgLvUpdateDtc() {
  if (!sgLvDtcLines[0]) return;

  // ล้างรายการก่อนกำหนด state ใหม่
  for (int i = 1; i < 6; ++i) sgLvSetText(sgLvDtcLines[i], "");

  if (sgDtc.busy) {
    sgLvSetDtcMain(sgLanguageThai() ? "กำลังตรวจโค้ด..." : "SCANNING DTC...", SG_LV_CYAN);
    sgLvSetText(sgLvDtcLines[1], sgLanguageThai() ? "รอสักครู่" : "PLEASE WAIT");
    if (sgLvDtcSpinner) lv_obj_clear_flag(sgLvDtcSpinner, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  if (sgLvDtcSpinner) lv_obj_add_flag(sgLvDtcSpinner, LV_OBJ_FLAG_HIDDEN);

  if (!sgDtc.readOk) {
    const bool hasAttemptStatus = sgDtc.status[0] != '\0' && strstr(sgDtc.status, "READY") == nullptr;
    if (hasAttemptStatus) {
      sgLvSetDtcMain(sgLanguageThai() ? "อ่านโค้ดไม่สำเร็จ" : "DTC READ FAILED", SG_LV_ORANGE);
      sgLvSetText(sgLvDtcLines[1], sgLvDtcStatusLocalized());
      sgLvSetText(sgLvDtcLines[5], sgLanguageThai() ? "ค้าง 5 วิ เพื่อลองใหม่" : "HOLD 5s TO RETRY");
      return;
    }
    sgLvSetDtcMain(sgLanguageThai() ? "พร้อมตรวจโค้ด" : "HOLD TO READ", SG_LV_YELLOW);
    sgLvSetText(sgLvDtcLines[1], sgLanguageThai() ? "ดับเครื่องก่อนตรวจโค้ด" : "IGNITION ON, ENGINE OFF");
    sgLvSetText(sgLvDtcLines[5], sgLanguageThai() ? "ค้าง 5 วิ เพื่ออ่าน" : "HOLD FOR 5 SECONDS");
    return;
  }

  if (sgDtc.count == 0) {
    sgLvSetDtcMain(sgLanguageThai() ? "ไม่พบโค้ดปัญหา" : "NO FAULT CODE FOUND", SG_LV_GREEN);
    sgLvSetText(sgLvDtcLines[1], sgLanguageThai() ? "ระบบปกติ" : "TURN IGNITION OFF & ON");
    sgLvSetText(sgLvDtcLines[5], sgLanguageThai() ? "สตาร์ตเครื่องเพื่อกลับหน้าหลัก" : "START ENGINE TO EXIT");
    return;
  }

  sgLvSetDtcMain(sgLanguageThai() ? "พบโค้ดปัญหา" : "FAULT DETECTED", SG_LV_RED);
  sgLvSetText(sgLvDtcLines[1], sgLanguageThai() ? "ดูรายละเอียดด้านล่าง" : "SEE CODES BELOW");
  for (int i = 2; i < 5; ++i) {
    const int idx = i - 2;
    sgLvSetText(sgLvDtcLines[i], idx < sgDtc.count ? sgDtc.lines[idx] : "");
  }
  const bool showAdvice = ((millis() / 2200UL) & 1UL) != 0UL;
  const char *advice = sgLvDtcAdviceText();
  sgLvSetText(sgLvDtcLines[5], (showAdvice && advice) ? advice :
              (sgLanguageThai() ? "ค้าง 5 วิ เพื่อลบ" : "HOLD 5s TO CLEAR CODES"));
}

// เรียกจาก DTC engine ระหว่างคำสั่งแบบ blocking เพื่อให้ spinner ยังเคลื่อนไหว
void sgUiDtcLoadingPump() {
  if (!sgLvReady || sgPage != SG_PAGE_DTC) return;
  const uint32_t now = millis();
  const uint32_t d = now - sgLvLastTickMs;
  sgLvLastTickMs = now;
  lv_tick_inc(d);
  sgLvUpdateDtc();
  lv_timer_handler();
}



static void sgFormatRemain(char *out, size_t len, uint32_t dueMs) {
  if (!out || len == 0) return;
  const uint32_t now = millis();
  uint32_t remain = 0;
  if (dueMs != 0 && (int32_t)(dueMs - now) > 0) remain = dueMs - now;
  const uint32_t sec = (remain + 999UL) / 1000UL;
  snprintf(out, len, "%lu:%02lu", (unsigned long)(sec / 60UL), (unsigned long)(sec % 60UL));
}

static void sgBtStatusDisplay(char *out, size_t len) {
  if (!out || len == 0) return;
  if (!sgBtReady) {
    snprintf(out, len, sgLanguageThai() ? "BT: ปิด" : "BT: OFF");
    return;
  }
  if (sgBtClientConnected) {
    snprintf(out, len, sgLanguageThai() ? "BT: เชื่อมต่อแล้ว" : "BT: LINKED");
    return;
  }
  char t[8];
  if (sgBtConnectWindowActive) {
    sgFormatRemain(t, sizeof(t), sgBtConnectWindowUntilMs);
    snprintf(out, len, sgLanguageThai() ? "BT: รอแอพ %s" : "BT: WAIT APP %s", t);
    return;
  }
  if (sgBtIdleOffDueMs != 0) {
    sgFormatRemain(t, sizeof(t), sgBtIdleOffDueMs);
    snprintf(out, len, sgLanguageThai() ? "BT: รอเชื่อมต่อใหม่ %s" : "BT: RECONNECT %s", t);
    return;
  }
  snprintf(out, len, sgLanguageThai() ? "BT: รอแอพ" : "BT: WAIT APP");
}

static void sgLvUpdateSetting() {
  if (!sgLvSettingLines[0]) return;

  uint32_t visualKey = 0;
  visualKey = visualKey * 4U + (uint32_t)sgSettingUiMode;
  visualKey = visualKey * 8U + (uint32_t)sgSettingItem;
  visualKey = visualKey * 2U + (sgFactoryResetConfirmActive ? 1U : 0U);
  visualKey = visualKey * 2U + (sgFactoryResetConfirmOk ? 1U : 0U);
  visualKey = visualKey * 2U + (sgLanguageThai() ? 1U : 0U);
  visualKey = visualKey * 4U + (uint32_t)sgThemeMode;
  visualKey = visualKey * 8U + (uint32_t)sgBrightnessMode;
  visualKey = visualKey * 2U + (sgAutoDtcEnabled ? 1U : 0U);
  visualKey = visualKey * 4U + (uint32_t)sgSmoothLevel;
  visualKey = visualKey * 8U + (uint32_t)sgAccentTheme;
  visualKey = visualKey * 8U + (uint32_t)sgFuelType;
  if (visualKey == sgLvSettingVisualKey) return;
  sgLvSettingVisualKey = visualKey;

  if (sgFactoryResetConfirmActive) {
    sgLvSetTextColorIfChanged(sgLvSettingLines[0], sgFactoryResetConfirmOk ? SG_LV_WHITE : SG_LV_YELLOW);
    if (sgLvSettingLines[1]) sgLvSetTextColorIfChanged(sgLvSettingLines[1], sgFactoryResetConfirmOk ? SG_LV_YELLOW : SG_LV_WHITE);
    return;
  }
  sgLvSetTextFontIfChanged(sgLvSettingLines[0], sgLvUiFont());
  sgLvSetText(sgLvSettingLines[0], sgDisplayModeText());
  sgLvSetTextFontIfChanged(sgLvSettingLines[1], sgLvUiFont());
  sgLvSetText(sgLvSettingLines[1], sgLanguageThai() ? sgTextThai(SG_TEXT_THAI_LANGUAGE) : "ENGLISH");
  sgLvSetTextFontIfChanged(sgLvSettingLines[2], sgLvUiFont());
  sgLvSetText(sgLvSettingLines[2], sgAutoDtcEnabled ? (sgLanguageThai() ? "เปิด" : "ON") : (sgLanguageThai() ? "ปิด" : "OFF"));
  sgLvSetTextFontIfChanged(sgLvSettingLines[3], sgLvUiFont());
  sgLvSetText(sgLvSettingLines[3], sgSmoothLevelText());
  sgLvSetTextFontIfChanged(sgLvSettingLines[4], sgLvUiFont());
  sgLvSetText(sgLvSettingLines[4], sgAccentThemeText());
  sgLvSetTextColorIfChanged(sgLvSettingLines[4], sgLvAccentColor());
  sgLvSetTextFontIfChanged(sgLvSettingLines[SG_SETTING_FUEL_TYPE], sgLvUiFont());
  sgLvSetText(sgLvSettingLines[SG_SETTING_FUEL_TYPE], sgFuelTypeText());
  sgLvSetTextFontIfChanged(sgLvSettingLines[SG_SETTING_FACTORY_RESET], sgLvUiFont());
  sgLvSetText(sgLvSettingLines[SG_SETTING_FACTORY_RESET], sgLanguageThai() ? "คืนค่าโรงงาน" : "FACTORY RESET");

  for (int i = 0; i < SG_SETTING_ITEM_COUNT; ++i) {
    const bool sel = sgSettingUiMode != SG_SETTING_VIEW && sgSettingItem == i;
    if (sgLvSettingLines[i]) sgLvSetTextColorIfChanged(sgLvSettingLines[i], sel ? SG_LV_YELLOW : SG_LV_WHITE);
    if (sgLvSettingPanels[i]) {
      const lv_color_t rowAccent = (i == SG_SETTING_FACTORY_RESET) ? SG_LV_ORANGE : sgLvAccentColor();
      lv_obj_set_style_border_color(sgLvSettingPanels[i], rowAccent, LV_PART_MAIN);
      lv_obj_set_style_border_opa(sgLvSettingPanels[i], sel ? LV_OPA_COVER : (i == SG_SETTING_FACTORY_RESET ? LV_OPA_40 : LV_OPA_20), LV_PART_MAIN);
      lv_obj_set_style_bg_color(sgLvSettingPanels[i], sel ? lv_color_mix(rowAccent, SG_LV_BG, LV_OPA_20) : sgLvThemePanelColor(), LV_PART_MAIN);
      lv_obj_set_style_bg_opa(sgLvSettingPanels[i], sel ? LV_OPA_COVER : LV_OPA_70, LV_PART_MAIN);
    }
  }
  if (sgLvSettingLines[SG_SETTING_ACCENT_COLOR] && !(sgSettingUiMode != SG_SETTING_VIEW && sgSettingItem == SG_SETTING_ACCENT_COLOR)) {
    sgLvSetTextColorIfChanged(sgLvSettingLines[SG_SETTING_ACCENT_COLOR], sgLvAccentColor());
  }
}

void sgUiInvalidate() { sgLvNeedBuild = true; }

void sgUiBegin() {
  sgLvApplyThemePalette();
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  lv_init();
  lv_disp_draw_buf_init(&sgLvDrawBuf, sgLvBuf1, nullptr, 320 * 16);
  lv_disp_drv_init(&sgLvDispDrv);
  sgLvDispDrv.hor_res = 320;
  sgLvDispDrv.ver_res = 240;
  sgLvDispDrv.flush_cb = sgLvFlush;
  sgLvDispDrv.draw_buf = &sgLvDrawBuf;
  lv_disp_drv_register(&sgLvDispDrv);

  sgLvLastTickMs = millis();
  sgLvReady = true;
  sgLvNeedBuild = true;
}

void sgUiTask() {
  if (!sgLvReady) return;
  const uint32_t now = millis();

  if (sgUiBootAnimationActive()) {
    const uint32_t d = now - sgLvLastTickMs;
    sgLvLastTickMs = now;
    lv_tick_inc(d);
    lv_timer_handler();
    return;
  }

  // หลังตรวจแล้วไม่พบปัญหา: ถ้าผู้ใช้สตาร์ตรถ ให้ซ่อนหน้า DTC อัตโนมัติ
  // ถ้าพบ DTC จะไม่ซ่อน เพื่อให้ผู้ใช้ยังเห็นโค้ดปัญหา
  if (sgPage == SG_PAGE_DTC && sgDtc.readOk && sgDtc.count == 0 && !sgDtc.busy && sgGauge.rpm > 0) {
    sgSetPage(SG_PAGE_GAUGE);
  }
  const uint32_t d = now - sgLvLastTickMs;
  sgLvLastTickMs = now;
  lv_tick_inc(d);

  if (sgLvNeedBuild || sgLvLastPage != (int8_t)sgPage) sgLvBuildPage();

  sgLvUpdateCommon();
  if (now - sgLvLastDynamicMs >= SG_UI_REFRESH_PERIOD_MS) { // cadence สมดุล ลดภาระ SPI/K-Line
    sgLvLastDynamicMs = now;
    if (sgPage == SG_PAGE_GAUGE) sgLvUpdateGauge(now);
    else if (sgPage == SG_PAGE_DATA) sgLvUpdateData();
    else if (sgPage == SG_PAGE_DTC) sgLvUpdateDtc();
    else if (sgPage == SG_PAGE_SETTING) sgLvUpdateSetting();
  }
  // Overlay ต้องถูกยกขึ้นหลังอัปเดตทุก Widget เพื่อไม่ให้ข้อความกดค้าง/Notice ถูกบัง
  if (sgLvHold && !lv_obj_has_flag(sgLvHold, LV_OBJ_FLAG_HIDDEN)) lv_obj_move_foreground(sgLvHold);
  if (sgLvNotice && !lv_obj_has_flag(sgLvNotice, LV_OBJ_FLAG_HIDDEN)) lv_obj_move_foreground(sgLvNotice);
  lv_timer_handler();
}
