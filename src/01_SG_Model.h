#pragma once
// ========================= MODEL ==========================
enum SgPage : uint8_t {
  SG_PAGE_GAUGE = 0,
  SG_PAGE_DATA  = 1,
  SG_PAGE_DTC   = 2,        // หน้าอ่านและล้าง DTC รวมหน้าเดียว
  SG_PAGE_SETTING = 3,
  SG_PAGE_SYSTEM = 4        // หน้า System Info / K-Line diagnostics
};


enum SgConnectionHealth : uint8_t {
  SG_HEALTH_LOST = 0,
  SG_HEALTH_UNSTABLE = 1,
  SG_HEALTH_GOOD = 2
};

enum SgProfileId : uint8_t {
  SG_PROFILE_UNKNOWN = 0,

  // Auto / CVT - แสดงชื่อรุ่นจริงบน Dashboard
  SG_PROFILE_LEAD125,
  SG_PROFILE_GIORNO,
  SG_PROFILE_PCX150,
  SG_PROFILE_PCX160,
  SG_PROFILE_CLICK160,

  // Gear / Cub - แสดงชื่อรุ่นจริงบน Dashboard
  SG_PROFILE_WAVE110I,
  SG_PROFILE_WAVE125I,
  SG_PROFILE_SUPERCUB
};


// ========================= VALIDATION STATUS ==========================
// ใช้กฎ SUN DIAG GAUGE v1.0 Stable: ค่าไหนยังไม่ผ่าน Log รถจริง จะไม่แสดงบน Dashboard/Data
enum SgValueId : uint8_t {
  SG_VAL_RPM = 0,
  SG_VAL_RPM_RAW,
  SG_VAL_TPS,
  SG_VAL_TPS_VOLT,
  SG_VAL_ECT,
  SG_VAL_ECT_VOLT,
  SG_VAL_INJECTOR,
  SG_VAL_IGNITION,
  SG_VAL_BATTERY,
  SG_VAL_AFR,
  SG_VAL_O2,
  SG_VAL_STFT,
  SG_VAL_LTFT,
  SG_VAL_IAT,
  SG_VAL_IAT_VOLT,
  SG_VAL_MAP,
  SG_VAL_MAP_VOLT,
  SG_VAL_SPEED,
  SG_VAL_IACV
};

inline bool sgValueValidated(SgProfileId profile, SgValueId value) {
  // Lead125: ผ่านจาก Log รถจริงแล้วเฉพาะชุดนี้
  // Live log 2026-07-25: AFR ใช้ได้เมื่อ sensor ให้ช่วง valid; ECT voltage ยังไม่ควรแสดงเป็นค่าจริง
  if (profile == SG_PROFILE_LEAD125) {
    switch (value) {
      case SG_VAL_RPM:
      case SG_VAL_RPM_RAW:
      case SG_VAL_TPS:
      case SG_VAL_TPS_VOLT:
      case SG_VAL_ECT:
      case SG_VAL_INJECTOR:
      case SG_VAL_IGNITION:
      case SG_VAL_BATTERY:
      case SG_VAL_AFR:
        return true;
      default:
        return false;
    }
  }

  // รุ่นอื่นยังรอ Log รถจริง ให้แสดงเฉพาะค่าพื้นฐานที่คาดว่าปลอดภัยที่สุด
  switch (value) {
    case SG_VAL_RPM:
    case SG_VAL_RPM_RAW:
    case SG_VAL_TPS:
    case SG_VAL_ECT:
    case SG_VAL_INJECTOR:
    case SG_VAL_IGNITION:
      return true;
    default:
      return false;
  }
}

struct SgProfile {
  SgProfileId id;
  const char *code;
  const char *uiName;
  const uint8_t *connectCmd;
  uint8_t connectLen;
  const uint8_t *tableMainCmd;
  uint8_t tableMainLen;
  const uint8_t *tableAuxCmd;
  uint8_t tableAuxLen;
  uint8_t mainReplyLen;  // ความยาว fallback หลัง echo (ใช้เมื่อ ECU ไม่ส่ง length header)
  uint8_t auxReplyLen;
  bool offsetsIncludeEcho; // ADX ของ PCX/Click ระบุ offset อ้างอิงจากเฟรมเต็มที่มี echo 5 byte
  uint8_t rpmOffset;
  uint8_t tpsOffset;
  uint8_t tempOffset;
  uint8_t batteryOffset;
  uint8_t injectorOffset;
  uint8_t ignOffset;
  uint8_t afrOffset;
  uint8_t stftOffset;
  bool pcxLayout;
  bool leadVerifiedOffsets;
  uint8_t decodeVersion;  // เวอร์ชัน offset/formula สำหรับ Monitor/Validation
};

struct SgEcuIdMap {
  const char *ecuId;      // 5 bytes as HEX, extracted after ECU-ID reply
  SgProfileId profileId;
  const char *model;
  bool runtimeAllowed;    // false = ECU ID ซ้ำ/ไม่ชัดเจน ห้าม decode
  bool ambiguous;         // true = ข้อมูลไม่ชัดเจน ต้องขึ้น ECU ไม่รองรับ
};

struct GaugeData {
  uint16_t rpm;          // ค่าที่แสดงผลบนจอ (smooth + round)
  uint16_t rpmRaw;       // ค่าดิบจาก ECU
  float rpmFiltered;     // ค่ากลางสำหรับลดการกระโดดของตัวเลข
  // [BETA STEP 6] Peak Hold tracking: maximum RPM, maximum ECT, minimum battery voltage
  uint16_t peakRpm;      // Maximum RPM during active session
  int16_t peakEct;       // Maximum ECT coolant temperature during active session
  float minBatt;         // Minimum battery voltage during active session
  float tps;
  int16_t tempC;
  float batteryV;
  float injectorMs;
  float injectorDutyPct; // Injector duty [%] = injectorMs * RPM / 1200 ตาม ADX
  float ignitionDeg;
  float afr;               // AFR ที่ผ่าน smoothing สำหรับ UI
  float afrRawDecoded;     // AFR จากสูตร Database ก่อน smoothing
  float afrFiltered;       // state ของ filter
  bool afrFilterReady;
  float o2Volt;       // O2 / AFR ตามสูตร ADX
  float o2HeaterVolt; // O2 heater voltage ตาม ADX PCX/Click
  float stftPct;      // ST Fuel Trim แสดงเป็นเปอร์เซ็นต์ชดเชยน้ำมัน: (ratio - 1) * 100
  float ltftPct;      // LT Fuel Trim แสดงเป็นเปอร์เซ็นต์ชดเชยน้ำมัน: (ratio - 1) * 100
  uint8_t ltftRaw;    // Raw Table17 offset 0x12
  bool ltftValid;     // Core ยืนยันว่ามี byte และค่าผ่านช่วงที่อนุญาต

  // ค่ารายละเอียดจาก Table17/Table11/Table20 สำหรับหน้า DATA
  float tpsVolt;
  float eotVolt;
  int16_t eotC;
  float ectVolt;
  float iatVolt;
  int16_t iatC;
  float mapVolt;
  float mapKpa;
  uint8_t speedKmh;
  uint16_t iacvPulse;
  float iacvFlow;
  uint16_t afrRaw;
  uint8_t stftRaw;

  bool ecuConnected;
  bool t17Ok;
  bool t20Ok;
  uint32_t lastT17Ms;
  uint32_t lastT20Ms;
};

struct DtcData {
  char status[32];
  char lines[12][96];   // Current/Past + code + sensor + fault (TH/EN); 96 เผื่อข้อความไทยเต็ม
  uint8_t count;
  uint8_t currentCount;
  uint8_t pastCount;
  bool readOk;
  bool clearOk;
  bool busy;
  uint32_t lastActionMs;
  bool clearUsedThisIgnition;
  uint8_t visibleOffset;
};

struct GaugeSettings {
  uint8_t smoothing;
  uint8_t fuelType;
  uint8_t fontSize;
  // [BETA STEP 2] Alert thresholds
  uint16_t ectAlertTempC;
  float afrLeanAlertThres;
  float afrRichAlertThres;
};

TFT_eSPI tft = TFT_eSPI();
HardwareSerial KLINE(2);
GaugeData sgGauge;
DtcData sgDtc;
SgPage sgPage = SG_PAGE_GAUGE;

// MAIN = command echo 5 + reply body ตาม ADX
// AUX  = command echo 5 + reply body ตาม ADX
uint8_t sgT17Frame[40];
uint8_t sgT20Frame[24];
uint8_t sgT17Payload[32];
uint8_t sgT20Payload[16];

uint32_t sgRxTimeoutCount = 0;
uint32_t sgChecksumErrorCount = 0;
uint32_t sgRxOkCount = 0;
uint32_t sgReconnectCount = 0;
uint32_t sgDuplicatePacketCount = 0;
uint32_t sgStalePacketCount = 0;
uint32_t sgT17Count = 0;
uint32_t sgT20Count = 0;
uint32_t sgWakeCount = 0;
uint8_t sgWakeFailCount = 0;
bool sgWakeLocked = false;
uint32_t sgLastConnectTryMs = 0;
uint32_t sgLastKLineCmdDoneMs = 0;
uint32_t sgLastDebugMs = 0;
uint8_t sgPollStep = 0;
uint8_t sgConsecutiveFail = 0;
uint32_t sgLastGoodFrameMs = 0;
SgConnectionHealth sgConnectionHealth = SG_HEALTH_LOST;
bool sgBtClientConnected = false;
bool sgButtonStuckFault = false;
uint8_t sgReconnectBackoffLevel = 0;
uint32_t sgEcuDisconnectedSinceMs = 0;
bool sgEcuRestartRequired = false;
uint32_t sgEcuLostSinceMs = 0;


// Startup Diagnostic Check ทำงานเพียงครั้งเดียวต่อการเปิดเครื่อง
enum SgStartupDtcState : uint8_t {
  SG_STARTUP_DTC_WAIT_ECU = 0,
  SG_STARTUP_DTC_WAIT_STABLE,
  SG_STARTUP_DTC_RUNNING,
  SG_STARTUP_DTC_DONE,
  SG_STARTUP_DTC_SKIPPED
};
SgStartupDtcState sgStartupDtcState = SG_STARTUP_DTC_WAIT_ECU;
uint32_t sgStartupDtcBootMs = 0;
uint32_t sgStartupDtcStableSinceMs = 0;
bool sgStartupDtcFoundCode = false;

// ลำดับเริ่มต้นและการเชื่อมใหม่: ห้ามปลุก ECU จนกว่า Boot Animation จะจบ
bool sgBootSequenceDone = false;
bool sgReconnectPending = false;
uint32_t sgReconnectNotBeforeMs = 0;


// โหมด OTA: ระหว่างทำงานห้าม K-Line wakeup/poll เพื่อไม่รบกวนการอัปโหลด
bool sgOtaModeActive = false;

// โหมดความสว่างหน้า Setting
// 0=AUTO, 1-5=ระดับความสว่างคงที่
uint8_t sgBrightnessMode = 0;
uint8_t sgBacklightDuty = SG_BL_HIGH_DUTY;
uint16_t sgLightRaw = 0;

// ปิดเฉพาะไฟหน้าจอ ขณะ Core/K-Line/Bluetooth ยังทำงานตามปกติ
bool sgScreenOff = false;

// ข้อความแจ้งสถานะสั้น ๆ บนจอ เช่น BT Monitor ON / WIFI OTA ON
char sgUiNotice[32] = "";
uint32_t sgUiNoticeUntilMs = 0;

// ระบบข้อความแจ้งเตือนแบบแปลภาษาได้
uint8_t sgUiNoticeTextId = 0;
