#pragma once

// SUN DIAG GAUGE Smart Warning / Health Estimate
// - ประเมินจากค่าที่ ECU อ่านได้จริงเท่านั้น
// - ไม่ส่งคำสั่งเขียน ECU
// - History เก็บใน RAM แบบวงแหวน 20 รายการ ไม่เขียน Flash/NVS

enum SgSmartSeverity : uint8_t {
  SG_SMART_OK = 0,
  SG_SMART_INFO = 1,
  SG_SMART_WARNING = 2,
  SG_SMART_DANGER = 3
};

enum SgSmartWarningType : uint8_t {
  SG_WARN_NONE = 0,
  SG_WARN_KLINE,
  SG_WARN_BAT_LOW,
  SG_WARN_BAT_HIGH,
  SG_WARN_TEMP_HIGH,
  SG_WARN_TEMP_DANGER,
  SG_WARN_AFR_LEAN,
  SG_WARN_AFR_RICH
};

struct SgWarningRecord {
  uint32_t timeMs;
  int16_t value10;
  uint8_t type;
  uint8_t severity;
};

static const uint8_t SG_WARNING_HISTORY_SIZE = 20;
SgWarningRecord sgWarningHistory[SG_WARNING_HISTORY_SIZE] = {};
uint8_t sgWarningHistoryHead = 0;
uint8_t sgWarningHistoryCount = 0;
uint8_t sgVehicleHealthScore = 100;
SgSmartWarningType sgActiveWarning = SG_WARN_NONE;
SgSmartSeverity sgActiveWarningSeverity = SG_SMART_OK;
int16_t sgActiveWarningValue10 = 0;

static void sgWarningHistoryPush(SgSmartWarningType type, SgSmartSeverity severity, int16_t value10) {
  SgWarningRecord &r = sgWarningHistory[sgWarningHistoryHead];
  r.timeMs = millis();
  r.value10 = value10;
  r.type = (uint8_t)type;
  r.severity = (uint8_t)severity;
  sgWarningHistoryHead = (uint8_t)((sgWarningHistoryHead + 1U) % SG_WARNING_HISTORY_SIZE);
  if (sgWarningHistoryCount < SG_WARNING_HISTORY_SIZE) sgWarningHistoryCount++;
#if SG_DEBUG_ENABLE
  SG_PRINT("[SMART] type="); SG_PRINT((int)type);
  SG_PRINT(" severity="); SG_PRINT((int)severity);
  SG_PRINT(" value10="); SG_PRINTLN((int)value10);
#endif
}

static const char* sgSmartWarningText(bool thai) {
  switch (sgActiveWarning) {
    case SG_WARN_KLINE:       return thai ? "K-LINE ไม่นิ่ง" : "K-LINE UNSTABLE";
    case SG_WARN_BAT_LOW:     return thai ? "แบตเตอรี่ต่ำ" : "BATTERY LOW";
    case SG_WARN_BAT_HIGH:    return thai ? "แบตเตอรี่สูง" : "BATTERY HIGH";
    case SG_WARN_TEMP_HIGH:   return thai ? "อุณหภูมิสูง" : "TEMP HIGH";
    case SG_WARN_TEMP_DANGER: return thai ? "อุณหภูมิอันตราย" : "TEMP DANGER";
    case SG_WARN_AFR_LEAN:    return thai ? "ส่วนผสมบาง" : "AFR LEAN";
    case SG_WARN_AFR_RICH:    return thai ? "ส่วนผสมหนา" : "AFR RICH";
    default:                  return "";
  }
}

static void sgSmartHealthTask() {
  static uint32_t lastMs = 0;
  const uint32_t now = millis();
  if ((uint32_t)(now - lastMs) < 250UL) return;
  lastMs = now;

  SgSmartWarningType nextType = SG_WARN_NONE;
  SgSmartSeverity nextSeverity = SG_SMART_OK;
  int16_t nextValue10 = 0;
  uint8_t score = 100;

  if (!sgGauge.ecuConnected) {
    score = 0;
  } else {
    const float batteryV = sgVehicleDataV1.mainPublished ? sgVehicleDataV1.batteryV : sgGauge.batteryV;
    const int16_t coolantC = sgVehicleDataV1.mainPublished ? sgVehicleDataV1.coolantC : sgGauge.tempC;

    if (sgConnectionHealth == SG_HEALTH_UNSTABLE) {
      nextType = SG_WARN_KLINE;
      nextSeverity = SG_SMART_WARNING;
      score = (score > 15U) ? (uint8_t)(score - 15U) : 0U;
    }

    if (batteryV > 0.1f) {
      if (batteryV < 10.0f) {
        nextType = SG_WARN_BAT_LOW; nextSeverity = SG_SMART_DANGER;
        nextValue10 = (int16_t)(batteryV * 10.0f);
        score = (score > 30U) ? (uint8_t)(score - 30U) : 0U;
      } else if (batteryV > 15.0f) {
        nextType = SG_WARN_BAT_HIGH; nextSeverity = SG_SMART_DANGER;
        nextValue10 = (int16_t)(batteryV * 10.0f);
        score = (score > 30U) ? (uint8_t)(score - 30U) : 0U;
      } else if (batteryV < 11.5f) {
        if (nextSeverity < SG_SMART_WARNING) {
          nextType = SG_WARN_BAT_LOW; nextSeverity = SG_SMART_WARNING;
          nextValue10 = (int16_t)(batteryV * 10.0f);
        }
        score = (score > 12U) ? (uint8_t)(score - 12U) : 0U;
      }
    }

    if (coolantC >= 120) {
      nextType = SG_WARN_TEMP_DANGER; nextSeverity = SG_SMART_DANGER;
      nextValue10 = (int16_t)(coolantC * 10);
      score = (score > 40U) ? (uint8_t)(score - 40U) : 0U;
    } else if (coolantC >= 100) {
      if (nextSeverity < SG_SMART_WARNING) {
        nextType = SG_WARN_TEMP_HIGH; nextSeverity = SG_SMART_WARNING;
        nextValue10 = (int16_t)(coolantC * 10);
      }
      score = (score > 20U) ? (uint8_t)(score - 20U) : 0U;
    }

    // AFR เตือนเมื่อมีเฟรม Aux จริงและเครื่องทำงาน เพื่อลด false warning ตอนยังไม่พร้อม
    if (sgGauge.t20Ok && sgGauge.rpm >= 1000U && sgGauge.afr > 0.1f) {
      if (sgGauge.afr > 15.0f) {
        if (nextSeverity < SG_SMART_WARNING) {
          nextType = SG_WARN_AFR_LEAN; nextSeverity = SG_SMART_WARNING;
          nextValue10 = (int16_t)(sgGauge.afr * 10.0f);
        }
        score = (score > 15U) ? (uint8_t)(score - 15U) : 0U;
      } else if (sgGauge.afr < 10.0f) {
        if (nextSeverity < SG_SMART_WARNING) {
          nextType = SG_WARN_AFR_RICH; nextSeverity = SG_SMART_WARNING;
          nextValue10 = (int16_t)(sgGauge.afr * 10.0f);
        }
        score = (score > 15U) ? (uint8_t)(score - 15U) : 0U;
      }
    }
  }

  sgVehicleHealthScore = score;

  // Warning Confirmation
  // - ค่าผิดปกติทั่วไป/AFR ต้องต่อเนื่อง 1.25 วินาที
  // - TEMP DANGER ยืนยันเร็ว 0.25 วินาที
  // - ค่ากลับปกติต้องต่อเนื่อง 0.75 วินาทีก่อนยกเลิก
  static SgSmartWarningType pendingType = SG_WARN_NONE;
  static SgSmartSeverity pendingSeverity = SG_SMART_OK;
  static int16_t pendingValue10 = 0;
  static uint32_t pendingSinceMs = 0;

  if (nextType == sgActiveWarning && nextSeverity == sgActiveWarningSeverity) {
    pendingType = SG_WARN_NONE;
    pendingSeverity = SG_SMART_OK;
    pendingSinceMs = 0;
    sgActiveWarningValue10 = nextValue10;
    return;
  }

  if (nextType != pendingType || nextSeverity != pendingSeverity) {
    pendingType = nextType;
    pendingSeverity = nextSeverity;
    pendingValue10 = nextValue10;
    pendingSinceMs = now;
    return;
  }

  pendingValue10 = nextValue10;
  uint32_t confirmMs = SG_WARNING_CONFIRM_MS;
  if (pendingType == SG_WARN_NONE) confirmMs = SG_WARNING_CLEAR_CONFIRM_MS;
  else if (pendingType == SG_WARN_TEMP_DANGER) confirmMs = SG_WARNING_TEMP_DANGER_CONFIRM_MS;

  if ((uint32_t)(now - pendingSinceMs) < confirmMs) return;

  sgActiveWarning = pendingType;
  sgActiveWarningSeverity = pendingSeverity;
  sgActiveWarningValue10 = pendingValue10;
  if (sgActiveWarning != SG_WARN_NONE) {
    sgWarningHistoryPush(sgActiveWarning, sgActiveWarningSeverity, sgActiveWarningValue10);
  }
  pendingType = SG_WARN_NONE;
  pendingSeverity = SG_SMART_OK;
  pendingSinceMs = 0;
}
