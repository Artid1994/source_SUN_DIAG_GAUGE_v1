#pragma once
/*
  11_SG_Backlight.h
  ระบบปรับความสว่างจอสำหรับ ESP32-2432S028

  ปรับปรุง Auto Brightness:
  - เพิ่ม Manual 8 ระดับ แทน low/mid/high เพื่อจูนจอจริงได้ละเอียดขึ้น
  - Auto ใช้ mapping ต่อเนื่อง 95-255 พร้อม smoothing และ hysteresis
  - สว่างขึ้นเร็ว แต่หรี่ลงช้า ลดอาการจอวูบวาบเวลาแสงเปลี่ยนเร็ว
  - ไม่แตะ ECU / K-Line / Decode / DTC / Bluetooth
*/


const char* sgThemeModeText() {
  if (sgThemeMode == 1) return sgLanguageThai() ? "กลางคืน" : "NIGHT";
  if (sgThemeMode == 2) return sgLanguageThai() ? "กลางวัน" : "DAY";
  return sgLanguageThai() ? "อัตโนมัติ" : "AUTO";
}

static void sgThemeApplyActive(bool day) {
  if (sgDayThemeActive == day) return;
  sgDayThemeActive = day;
  sgUiInvalidate();
}

void sgThemeNextMode() {
  sgThemeMode = (uint8_t)((sgThemeMode + 1U) % 3U);
  if (sgThemeMode == 1) sgThemeApplyActive(false);
  else if (sgThemeMode == 2) sgThemeApplyActive(true);
  else {
#if SG_LIGHT_SENSOR_ENABLE
    sgThemeApplyActive(sgLightRaw >= 2300);
#else
    sgThemeApplyActive(false);
#endif
  }
  sgSettingsSaveUserSettings();
}

const char* sgBrightnessModeText() {
  // โหมดความสว่างละเอียดขึ้น
  // 0 = AUTO, 1-5 = ระดับ Manual ไล่จากมืดไปสว่าง
  static char b[16];
  if (sgBrightnessMode == 0) return sgLanguageThai() ? "อัตโนมัติ" : "AUTO";
  snprintf(b, sizeof(b), "%u", (unsigned)sgBrightnessMode);
  return b;
}

uint8_t sgBrightnessFixedDuty() {
  static const uint8_t kDuty[5] = {
    SG_BL_LEVEL1_DUTY, SG_BL_LEVEL3_DUTY, SG_BL_LEVEL5_DUTY,
    SG_BL_LEVEL7_DUTY, SG_BL_LEVEL8_DUTY
  };
  if (sgBrightnessMode >= 1 && sgBrightnessMode <= 5) return kDuty[sgBrightnessMode - 1];
  return SG_BL_HIGH_DUTY;
}

uint8_t sgBacklightPwmDuty(uint8_t duty) {
#if SG_BL_ACTIVE_LOW
  return 255 - duty;
#else
  return duty;
#endif
}

void sgBacklightWrite(uint8_t duty) {
  duty = constrain(duty, 0, 255);
  sgBacklightDuty = duty;
  const uint8_t pwmDuty = sgBacklightPwmDuty(duty);

#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(TFT_BACKLIGHT_PIN, pwmDuty);
#else
  ledcWrite(SG_BL_PWM_CHANNEL, pwmDuty);
#endif
}

#if SG_LIGHT_SENSOR_ENABLE
static uint16_t sgLightReadAveraged() {
  // อ่านหลายตัวอย่างแบบเบา ๆ ใช้เวลาไม่นาน แต่ช่วยลดค่าแกว่งจาก ADC
  uint32_t sum = 0;
  for (uint8_t i = 0; i < 12; ++i) {
    sum += analogRead(SG_LIGHT_SENSOR_PIN);
    delayMicroseconds(100);
  }
  return (uint16_t)(sum / 12U);
}

static uint8_t sgAutoBrightnessTarget(uint16_t raw) {
  uint16_t rawForMap = raw;
#if SG_LIGHT_RAW_DARK_HIGH
  rawForMap = 4095 - raw;
#endif

  // Auto แบบต่อเนื่อง แต่แบ่งช่วงให้เหมือน 8 ระดับภายใน
  // ช่วงกลางคืน/ในอาคารไม่สว่างเกินไป ส่วนกลางแดดเร่งขึ้นเต็มได้เร็ว
  static const uint8_t dutyTable[9] = {
    SG_BL_LEVEL1_DUTY, SG_BL_LEVEL1_DUTY, SG_BL_LEVEL2_DUTY, SG_BL_LEVEL3_DUTY,
    SG_BL_LEVEL4_DUTY, SG_BL_LEVEL5_DUTY, SG_BL_LEVEL6_DUTY, SG_BL_LEVEL7_DUTY, SG_BL_LEVEL8_DUTY
  };
  uint8_t idx = rawForMap / 512;
  if (idx > 8) idx = 8;
  uint8_t base = dutyTable[idx];

  // interpolate เบา ๆ ระหว่างระดับ เพื่อไม่ให้กระโดดเป็นขั้นชัดเกินไป
  uint16_t frac = rawForMap & 0x01FF;
  uint8_t next = dutyTable[idx < 8 ? idx + 1 : idx];
  int target = base + ((int)(next - base) * (int)frac) / 512;
  target = constrain(target, SG_BL_LEVEL1_DUTY, SG_BL_LEVEL8_DUTY);
  return (uint8_t)target;
}
#endif

void sgBacklightBegin() {
  sgDayThemeActive = (sgThemeMode == 2);
  pinMode(TFT_BACKLIGHT_PIN, OUTPUT);

#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(TFT_BACKLIGHT_PIN, SG_BL_PWM_FREQ, SG_BL_PWM_RESOLUTION);
#else
  ledcSetup(SG_BL_PWM_CHANNEL, SG_BL_PWM_FREQ, SG_BL_PWM_RESOLUTION);
  ledcAttachPin(TFT_BACKLIGHT_PIN, SG_BL_PWM_CHANNEL);
#endif

#if SG_LIGHT_SENSOR_ENABLE
  pinMode(SG_LIGHT_SENSOR_PIN, INPUT);
#endif

  // เปิดเครื่องให้สว่างก่อน แล้วค่อยให้ AUTO ปรับทีหลัง ลดอาการจอมืดตอนบูตกลางแดด
  if (sgBrightnessMode == 0) sgBacklightWrite(SG_BL_HIGH_DUTY);
  else sgBacklightWrite(sgBrightnessFixedDuty());
}

void sgScreenSetOff(bool off) {
  sgScreenOff = off;
  if (off) {
    sgBacklightWrite(0);
  } else {
    if (sgBrightnessMode == 0) sgBacklightWrite(SG_BL_HIGH_DUTY);
    else sgBacklightWrite(sgBrightnessFixedDuty());
  }
}

inline bool sgScreenIsOff() { return sgScreenOff; }

void sgBrightnessNextMode() {
  sgBrightnessMode++;
  if (sgBrightnessMode > 5) sgBrightnessMode = 0;
  sgSettingsSaveBrightness();

  // เปลี่ยนแสงทันทีตอนกดปุ่ม ไม่ต้องรอ task
  if (sgThemeMode == 2) {
    sgBacklightWrite(SG_BL_LEVEL8_DUTY);
    return;
  }

  if (sgBrightnessMode != 0 && sgThemeMode != 0) {
    sgBacklightWrite(sgBrightnessFixedDuty());
  } else {
#if SG_LIGHT_SENSOR_ENABLE
    sgLightRaw = sgLightReadAveraged();
    sgBacklightWrite(sgAutoBrightnessTarget(sgLightRaw));
#else
    sgBacklightWrite(SG_BL_HIGH_DUTY);
#endif
  }
}


const char* sgDisplayModeText() {
  static char b[24];
  if (sgThemeMode == 0) return sgLanguageThai() ? "อัตโนมัติ" : "AUTO";
  if (sgThemeMode == 2) return sgLanguageThai() ? "กลางวัน" : "DAY";
  snprintf(b, sizeof(b), sgLanguageThai() ? "กลางคืน %u" : "NIGHT %u", (unsigned)(sgBrightnessMode == 0 ? 1 : sgBrightnessMode));
  return b;
}

void sgDisplayNextMode() {
  // รอบเดียวรวมโหมดภาพและความสว่าง: AUTO -> NIGHT 1..5 -> DAY -> AUTO
  if (sgThemeMode == 0) {
    sgThemeMode = 1;
    sgBrightnessMode = 1;
  } else if (sgThemeMode == 1 && sgBrightnessMode < 5) {
    sgBrightnessMode++;
  } else if (sgThemeMode == 1) {
    sgThemeMode = 2;
    sgBrightnessMode = 5;
  } else {
    sgThemeMode = 0;
    sgBrightnessMode = 0;
  }
  if (sgThemeMode == 2) {
    sgThemeApplyActive(true);
    sgBacklightWrite(SG_BL_LEVEL8_DUTY);
  } else if (sgThemeMode == 1) {
    sgThemeApplyActive(false);
    sgBacklightWrite(sgBrightnessFixedDuty());
  } else {
#if SG_LIGHT_SENSOR_ENABLE
    sgLightRaw = sgLightReadAveraged();
    sgThemeApplyActive(sgLightRaw >= 2300);
    sgBacklightWrite(sgAutoBrightnessTarget(sgLightRaw));
#else
    sgThemeApplyActive(false);
    sgBacklightWrite(SG_BL_HIGH_DUTY);
#endif
  }
  sgSettingsSaveUserSettings();
}

// ตัวแปรจากระบบปุ่ม ใช้กัน ADC กระชากจาก TTP223 ระหว่างกดรัว
extern bool sgBtnPressed;
extern uint32_t sgBtnLastActionMs;

void sgBacklightTask() {
  static uint32_t lastMs = 0;
  static float smoothDuty = SG_BL_HIGH_DUTY;
  static bool autoThemeCandidate = false;
  static bool autoThemeCandidateValid = false;
  static uint32_t autoThemeCandidateSinceMs = 0;

  const uint32_t now = millis();
  if (sgScreenOff) {
    if (sgBacklightDuty != 0) sgBacklightWrite(0);
    return;
  }
  if (now - lastMs < SG_BL_UPDATE_MS) return;
  lastMs = now;

  if (sgThemeMode == 2) {
    sgBacklightWrite(SG_BL_LEVEL8_DUTY);
    smoothDuty = sgBacklightDuty;
    return;
  }

  if (sgBrightnessMode != 0 && sgThemeMode != 0) {
    sgBacklightWrite(sgBrightnessFixedDuty());
    smoothDuty = sgBacklightDuty;
    return;
  }

#if SG_LIGHT_SENSOR_ENABLE
  sgLightRaw = sgLightReadAveraged();
  if (sgThemeMode == 0) {
    // TTP223/การกดรัวอาจทำให้ ADC แสงแกว่งชั่วคราว จึงพักการสลับธีม 1.2 วินาทีหลังปุ่มทำงาน
    const bool buttonQuiet = !sgBtnPressed && (uint32_t)(now - sgBtnLastActionMs) >= 1200UL;
    bool requestedDay = sgDayThemeActive;
    if (!sgDayThemeActive && sgLightRaw >= 2600) requestedDay = true;
    else if (sgDayThemeActive && sgLightRaw <= 1800) requestedDay = false;

    if (!buttonQuiet) {
      autoThemeCandidateValid = false;
    } else if (requestedDay != sgDayThemeActive) {
      if (!autoThemeCandidateValid || autoThemeCandidate != requestedDay) {
        autoThemeCandidate = requestedDay;
        autoThemeCandidateSinceMs = now;
        autoThemeCandidateValid = true;
      } else if ((uint32_t)(now - autoThemeCandidateSinceMs) >= 1500UL) {
        sgThemeApplyActive(autoThemeCandidate);
        autoThemeCandidateValid = false;
      }
    } else {
      autoThemeCandidateValid = false;
    }
  } else {
    autoThemeCandidateValid = false;
    sgThemeApplyActive(sgThemeMode == 2);
  }
  if (sgDayThemeActive) {
    sgBacklightWrite(SG_BL_LEVEL8_DUTY);
    smoothDuty = sgBacklightDuty;
    return;
  }
  const uint8_t target = sgAutoBrightnessTarget(sgLightRaw);

  // เจอแสงจ้าให้เพิ่มเร็ว, ตอนมืดค่อย ๆ หรี่ ลด flicker/วูบวาบ
  const float alpha = ((float)target > smoothDuty) ? SG_BL_AUTO_BRIGHTEN_ALPHA : SG_BL_AUTO_DIM_ALPHA;
  smoothDuty += ((float)target - smoothDuty) * alpha;

  const int duty = constrain((int)(smoothDuty + 0.5f), SG_BL_MIN_DUTY, SG_BL_HIGH_DUTY);
  if (abs(duty - (int)sgBacklightDuty) >= SG_BL_AUTO_DEADBAND) {
    sgBacklightWrite((uint8_t)duty);
  }
#else
  sgBacklightWrite(SG_BL_HIGH_DUTY);
#endif
}
