#pragma once
// ================================================================
// AlexMapX Live Data Beta / No-ADX Research Hook
// Source concept from AlexMapX app.asar ecm-connection.js:
// - ECM Type 1/4/7 probe commands
// - direct Live Data formulas for RPM/TPS/ECT/BAT/INJ/IGN/LTFT/O2
// Safety: read-only only. Disabled automatically in USER build.
// ================================================================

struct SgAlexMapXDecoded {
  bool valid;
  uint8_t ecmType;
  uint16_t rpm;
  float tpsVolt;
  float tpsDeg;
  int16_t ectC;
  float batteryV;
  float injectorMs;
  float ignitionDeg;
  float ltftPct;
  float o2Volt;
  float afr;
};

SgAlexMapXDecoded sgAlexMapXLast;
uint8_t sgAlexMapXEcmType = 0;
uint32_t sgAlexMapXReadCount = 0;

static const uint8_t SG_AMX_INIT[]  = {0x72,0x05,0x71,0x00,0x18};
static const uint8_t SG_AMX_TYPE1[] = {0x72,0x07,0x72,0x11,0x00,0x14,0xF0};
static const uint8_t SG_AMX_TYPE4[] = {0x72,0x07,0x72,0x17,0x00,0x0F,0xEF};
static const uint8_t SG_AMX_TYPE7[] = {0x72,0x07,0x72,0x13,0x00,0x0E,0xF4};
static const uint8_t SG_AMX_O2[]    = {0x72,0x07,0x72,0x20,0x00,0x0B,0xEA};

inline uint8_t sgAmxPayloadU8(const SgV330Reply &r, uint8_t idx) {
  return (r.payload && idx < r.payloadLen) ? r.payload[idx] : 0;
}

inline uint16_t sgAmxPayloadU16(const SgV330Reply &r, uint8_t hi, uint8_t lo) {
  return ((uint16_t)sgAmxPayloadU8(r, hi) << 8) | sgAmxPayloadU8(r, lo);
}

inline float sgAmxAfrFromO2Volt(float v) {
  static const float volt[] = {0.000f,0.020f,0.040f,0.100f,0.350f,0.700f,0.800f,0.900f,0.950f,1.000f};
  static const float afr[]  = {20.0f,18.0f,17.0f,16.0f,15.0f,14.0f,13.0f,12.0f,11.0f,10.0f};
  if (v <= volt[0]) return afr[0];
  for (uint8_t i = 1; i < 10; ++i) {
    if (v <= volt[i]) {
      const float t = (v - volt[i-1]) / (volt[i] - volt[i-1]);
      return afr[i-1] + t * (afr[i] - afr[i-1]);
    }
  }
  return afr[9];
}

inline bool sgAlexMapXDecodeMain(const SgV330Reply &r, uint8_t type, SgAlexMapXDecoded &out) {
  if (!r.payload || r.payloadLen < 24) return false;
  memset(&out, 0, sizeof(out));
  out.valid = true;
  out.ecmType = type;
  // Payload offsets below are converted from AlexMapX raw-string offsets after echo/header removal.
  out.rpm = sgAmxPayloadU16(r, 4, 5);
  out.tpsVolt = sgAmxPayloadU8(r, 6) * 195.0f / 10.0f / 1000.0f;
  out.tpsDeg = sgAmxPayloadU8(r, 7) * 0.5f;
  out.ectC = (int16_t)sgAmxPayloadU8(r, 9) - 40;
  if (type == 1) {
    out.batteryV = sgAmxPayloadU8(r, 16) / 10.0f;
    out.injectorMs = sgAmxPayloadU16(r, 18, 19) / 250.0f;
    out.ignitionDeg = ((int16_t)sgAmxPayloadU8(r, 20) - 128) * 0.5f;
    out.ltftPct = sgAmxPayloadU8(r, 38) * 39.0f / 50.0f - 100.0f;
  } else {
    out.batteryV = sgAmxPayloadU8(r, 14) / 10.0f;
    out.injectorMs = sgAmxPayloadU16(r, 15, 16) / 250.0f;
    out.ignitionDeg = ((int16_t)sgAmxPayloadU8(r, 17) - 128) * 0.5f;
    out.ltftPct = (type == 4) ? (sgAmxPayloadU8(r, 18) * 39.0f / 50.0f - 100.0f) : 0.0f;
  }
  return true;
}

inline bool sgAlexMapXDecodeO2(const SgV330Reply &r, SgAlexMapXDecoded &out) {
  if (!r.payload || r.payloadLen < 5) return false;
  const uint8_t raw = sgAmxPayloadU8(r, 4);
  out.o2Volt = raw / 753.75f * 14.7f;
  out.afr = sgAmxAfrFromO2Volt(out.o2Volt);
  return true;
}

inline bool sgAlexMapXDetectType() {
#if SG_ALEXMAPX_LIVEDATA_BETA_ENABLE
  SgV330Reply r;
  (void)sgV330SendCommand(SG_AMX_INIT, sizeof(SG_AMX_INIT), r, SG_DTC_TIMEOUT_MS, "[AMX INIT]", 1);
  if (sgV330SendCommand(SG_AMX_TYPE7, sizeof(SG_AMX_TYPE7), r, SG_DTC_TIMEOUT_MS, "[AMX TYPE7]", 1) == SG_V330_OK) { sgAlexMapXEcmType = 7; return true; }
  if (sgV330SendCommand(SG_AMX_TYPE4, sizeof(SG_AMX_TYPE4), r, SG_DTC_TIMEOUT_MS, "[AMX TYPE4]", 1) == SG_V330_OK) { sgAlexMapXEcmType = 4; return true; }
  sgAlexMapXEcmType = 1;
  return true;
#else
  return false;
#endif
}

inline bool sgAlexMapXReadOnce() {
#if SG_ALEXMAPX_LIVEDATA_BETA_ENABLE
  if (!sgAlexMapXEcmType && !sgAlexMapXDetectType()) return false;
  const uint8_t *cmd = sgAlexMapXEcmType == 7 ? SG_AMX_TYPE7 : (sgAlexMapXEcmType == 4 ? SG_AMX_TYPE4 : SG_AMX_TYPE1);
  const uint8_t len = sgAlexMapXEcmType == 7 ? sizeof(SG_AMX_TYPE7) : (sgAlexMapXEcmType == 4 ? sizeof(SG_AMX_TYPE4) : sizeof(SG_AMX_TYPE1));
  SgV330Reply r;
  if (sgV330SendCommand(cmd, len, r, SG_DTC_TIMEOUT_MS, "[AMX LIVE]", 1) != SG_V330_OK) return false;
  if (!sgAlexMapXDecodeMain(r, sgAlexMapXEcmType, sgAlexMapXLast)) return false;
  sgAlexMapXReadCount++;
  if ((sgAlexMapXReadCount % SG_ALEXMAPX_POLL_O2_EVERY) == 0) {
    SgV330Reply o2;
    if (sgV330SendCommand(SG_AMX_O2, sizeof(SG_AMX_O2), o2, SG_DTC_TIMEOUT_MS, "[AMX O2]", 1) == SG_V330_OK) {
      (void)sgAlexMapXDecodeO2(o2, sgAlexMapXLast);
    }
  }
  return true;
#else
  return false;
#endif
}
