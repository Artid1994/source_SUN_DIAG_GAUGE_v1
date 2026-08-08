#pragma once
/*
  SUN DIAG GAUGE v1.0.0 Profile Database
  ฐานข้อมูลแยกตามกลุ่มรถ/แหล่ง ADX เพื่อให้ไล่ปรับจาก log รถจริงง่ายขึ้น

  Lead / Giorno     -> Database/DB_Lead_Giorno.h    -> LEAD GRIONO BLUE.adx
  PCX150 / PCX160 / Click160 -> Database/DB_PCX_Click.h -> pXC150-160.adx
  Wave / Super Cub  -> Database/DB_Wave_Cub.h        -> Real-Time wave. tanasanst17.adx

  กฎหลัก:
  - ระบุรุ่นจาก ECU ID เท่านั้น
  - ECU ID ซ้ำ/ไม่ชัดเจน: แสดง "ECU ไม่รองรับ" และไม่ Decode
  - ECU ID ไม่พบฐานข้อมูล: ใช้ Generic fallback แบบจำกัด เพื่อช่วยเก็บ log
*/

// ======================== COMMANDS / PROFILE DB =============
const uint8_t SG_WAKEUP_FE[]      = { 0xFE, 0x04, 0x72, 0x8C };
const uint8_t SG_CONNECT_AN[]     = { 0x2C, 0x41, 0x4E, 0x23 };
const uint8_t SG_CONNECT_TN[]     = { 0x2C, 0x54, 0x4E, 0x23 };
const uint8_t SG_SESSION_CMD[]    = { 0x72, 0x05, 0x00, 0xF0, 0x99 };
const uint8_t SG_ECU_ID_CMD[]     = { 0x72, 0x07, 0x72, 0x00, 0x00, 0x05, 0x10 };
const uint8_t SG_T17_CMD[]        = { 0x72, 0x05, 0x71, 0x17, 0x01 };
const uint8_t SG_T20_CMD[]        = { 0x72, 0x05, 0x71, 0x20, 0xF8 };
const uint8_t SG_T11_CMD[]        = { 0x72, 0x05, 0x71, 0x11, 0x07 };
// DTC ไม่ใช้คำสั่งจาก ADX แล้ว
// ระบบใหม่สร้างแพ็กเกตแบบ HondaECU V330 ขณะทำงาน:
// Current = service 0x74, page 0x01-0x0B
// Past    = service 0x73, page 0x01-0x0B
// Clear   = service 0x60, subcommand 0x03


// ======================== DATABASE: LEAD / GIORNO ========================
#pragma once
/*
  SUN DIAG GAUGE Database Group: LEAD125 / GIORNO
  ADX Source: LEAD GRIONO BLUE.adx
  ลำดับ Macro ตาม TunePro RT: Connect TN -> T17 -> T20
  แยก Profile เป็นชื่อรุ่นจริง เพื่อให้ Dashboard/Monitor แสดงผลตรงตาม ECU ID
*/

// -------- DB_Lead125.h --------
#define SG_DB_MODEL_LEAD125_ECU_MAP \
  { "0104780D01", SG_PROFILE_LEAD125, "LEAD125", true, false }, \
  { "0105950D01", SG_PROFILE_LEAD125, "LEAD125", true, false }


// -------- DB_Giorno.h --------
#define SG_DB_MODEL_GIORNO_ECU_MAP \
  { "01052A0D01", SG_PROFILE_GIORNO, "GIORNO+125", true, false }


#define SG_DB_LEAD_GIORNO_PROFILES \
  { SG_PROFILE_LEAD125, "LEAD125", "Lead125", SG_CONNECT_TN, 4, SG_T17_CMD, 5, SG_T20_CMD, 5, \
    24, 8, false, 0x04, 0x07, 0x0D, 0x0E, 0x0F, 0x11, 0x04, 0x05, false, true, 1 }, \
  { SG_PROFILE_GIORNO, "GIORNO", "Giorno", SG_CONNECT_TN, 4, SG_T17_CMD, 5, SG_T20_CMD, 5, \
    24, 8, false, 0x04, 0x07, 0x0D, 0x0E, 0x0F, 0x11, 0x04, 0x05, false, true, 1 }

#define SG_DB_LEAD_GIORNO_ECU_MAP \
  SG_DB_MODEL_LEAD125_ECU_MAP, \
  SG_DB_MODEL_GIORNO_ECU_MAP

// ======================== DATABASE: PCX / CLICK =========================
#pragma once
/*
  SUN DIAG GAUGE Database Group: PCX150 / PCX160 / CLICK160
  ADX Source: PCX150 PCX160 Click160.adx
  ลำดับ Macro ตาม TunePro RT: Command1 72 05 71 11 -> Command2 72 05 71 20
  แยก Profile เป็นชื่อรุ่นจริง เพื่อให้ Dashboard/Monitor แสดงผลตรงตาม ECU ID
*/

// -------- DB_PCX150.h --------
#define SG_DB_MODEL_PCX150_ECU_MAP \
  { "0101990D01", SG_PROFILE_PCX150, "PCX150", true, false }, \
  { "0102180D01", SG_PROFILE_PCX150, "PCX150", true, false }, \
  { "0102990D01", SG_PROFILE_PCX150, "PCX150", true, false }


// -------- DB_PCX160.h --------
#define SG_DB_MODEL_PCX160_ECU_MAP \
  { "0903CF0D01", SG_PROFILE_PCX160, "PCX160 e:HEV", true, false }, \
  { "0103CF0D01", SG_PROFILE_PCX160, "PCX160", true, false }, \
  { "0103D00D01", SG_PROFILE_PCX160, "PCX160", true, false }, \
  { "0105040D01", SG_PROFILE_PCX160, "PCX160", true, false }, \
  { "0105050D01", SG_PROFILE_PCX160, "PCX160", true, false }, \
  { "0105B40D01", SG_PROFILE_PCX160, "PCX160", true, false }, \
  { "0105B30D01", SG_PROFILE_PCX160, "PCX160", true, false }


// -------- DB_Click160.h --------
#define SG_DB_MODEL_CLICK160_ECU_MAP \
  { "01046D0D01", SG_PROFILE_CLICK160, "CLICK160", true, false }


#define SG_DB_PCX_CLICK_PROFILES \
  { SG_PROFILE_PCX150, "PCX150", "PCX150", SG_CONNECT_AN, 4, SG_T11_CMD, 5, SG_T20_CMD, 5, \
    30, 13, true, 0x09, 0x0C, 0x0E, 0x15, 0x17, 0x19, 0x09, 0x0A, true, false, 1 }, \
  { SG_PROFILE_PCX160, "PCX160", "PCX160", SG_CONNECT_AN, 4, SG_T11_CMD, 5, SG_T20_CMD, 5, \
    30, 13, true, 0x09, 0x0C, 0x0E, 0x15, 0x17, 0x19, 0x09, 0x0A, true, false, 1 }, \
  { SG_PROFILE_CLICK160, "CLICK160", "Click160", SG_CONNECT_AN, 4, SG_T11_CMD, 5, SG_T20_CMD, 5, \
    30, 13, true, 0x09, 0x0C, 0x0E, 0x15, 0x17, 0x19, 0x09, 0x0A, true, false, 1 }

#define SG_DB_PCX_CLICK_ECU_MAP \
  SG_DB_MODEL_PCX150_ECU_MAP, \
  SG_DB_MODEL_PCX160_ECU_MAP, \
  SG_DB_MODEL_CLICK160_ECU_MAP

// ======================== DATABASE: WAVE / CUB ==========================
#pragma once
/*
  SUN DIAG GAUGE Database Group: WAVE110i / WAVE125i / SUPER CUB
  ADX Source: Lead/Giorno/Wave/Super Cub.adx
  แยก Profile เป็นชื่อรุ่นจริง แทน GEAR_S4 / GENERIC

  ใช้ TunePro RT ADX ตรงตัว:
  - Connect TN -> T17 -> T20
  - T17: RPM 0x04, TPS Volt 0x06, TPS 0x07, EOT 0x09, ECT 0x0B, BAT 0x0E, INJ 0x0F, IGN 0x11, LTFT 0x12
  - T20: O2/AFR 0x04, STFT 0x05
*/

// -------- DB_Wave110i.h --------
#define SG_DB_MODEL_WAVE110I_ECU_MAP \
  { "0100B20D01", SG_PROFILE_WAVE110I, "WAVE110i", true, false }, \
  { "0100CE0D02", SG_PROFILE_WAVE110I, "WAVE110i", true, false }, \
  { "0101610D01", SG_PROFILE_WAVE110I, "WAVE110i", true, false }, \
  { "0102B90D01", SG_PROFILE_WAVE110I, "WAVE110i", true, false }, \
  { "01034C0D01", SG_PROFILE_WAVE110I, "WAVE110i", true, false }, \
  { "01040F0D01", SG_PROFILE_WAVE110I, "WAVE110i", true, false }, \
  { "0105550D01", SG_PROFILE_WAVE110I, "WAVE110i", true, false }


// -------- DB_Wave125i.h --------
#define SG_DB_MODEL_WAVE125I_ECU_MAP \
  { "01033E0D01", SG_PROFILE_WAVE125I, "WAVE125i", true, false }, \
  { "01034F0D01", SG_PROFILE_WAVE125I, "WAVE125i", true, false }, \
  { "0102580D01", SG_PROFILE_WAVE125I, "WAVE125i", true, false }, \
  { "0101B30D01", SG_PROFILE_WAVE125I, "WAVE125i", true, false }, \
  { "01047B0D01", SG_PROFILE_WAVE125I, "WAVE125i", true, false }


// -------- DB_SuperCub.h --------
#define SG_DB_MODEL_SUPER_CUB_ECU_MAP \
  { "0101BA0D01", SG_PROFILE_SUPERCUB, "DREAM110", true, false }, \
  { "0102B50D01", SG_PROFILE_SUPERCUB, "DREAM CUB110", true, false }, \
  { "0103FA0D01", SG_PROFILE_SUPERCUB, "SUPER CUB110", true, false }, \
  { "0105260D01", SG_PROFILE_SUPERCUB, "SUPER CUB110", true, false }


#define SG_DB_WAVE_CUB_PROFILES \
  { SG_PROFILE_WAVE110I, "WAVE110i", "Wave110i", SG_CONNECT_TN, 4, SG_T17_CMD, 5, SG_T20_CMD, 5, \
    24, 8, false, 0x04, 0x07, 0x0B, 0x0E, 0x0F, 0x11, 0x04, 0x05, false, false, 1 }, \
  { SG_PROFILE_WAVE125I, "WAVE125i", "Wave125i", SG_CONNECT_TN, 4, SG_T17_CMD, 5, SG_T20_CMD, 5, \
    24, 8, false, 0x04, 0x07, 0x0B, 0x0E, 0x0F, 0x11, 0x04, 0x05, false, false, 1 }, \
  { SG_PROFILE_SUPERCUB, "SUPERCUB", "SuperCub", SG_CONNECT_TN, 4, SG_T17_CMD, 5, SG_T20_CMD, 5, \
    24, 8, false, 0x04, 0x07, 0x0B, 0x0E, 0x0F, 0x11, 0x04, 0x05, false, false, 1 }

#define SG_DB_WAVE_CUB_ECU_MAP \
  SG_DB_MODEL_WAVE110I_ECU_MAP, \
  SG_DB_MODEL_WAVE125I_ECU_MAP, \
  SG_DB_MODEL_SUPER_CUB_ECU_MAP, \
  { "01010E0D01", SG_PROFILE_UNKNOWN, "AMBIGUOUS WAVE/CUB", false, true }, \
  { "0101880D01", SG_PROFILE_WAVE110I, "WAVE110i", true, false }



const SgProfile sgProfiles[] = {
  // UNKNOWN / SAFE LOCK
  { SG_PROFILE_UNKNOWN, "UNKNOWN", "SCAN", SG_CONNECT_AN, 4, SG_T17_CMD, 5, SG_T20_CMD, 5,
    24, 8, false, 0x04, 0x07, 0x0D, 0x0E, 0x0F, 0x11, 0x04, 0x05, false, false, 1 },
  SG_DB_LEAD_GIORNO_PROFILES,
  SG_DB_PCX_CLICK_PROFILES,
  SG_DB_WAVE_CUB_PROFILES
};
const uint8_t SG_PROFILE_COUNT = sizeof(sgProfiles) / sizeof(sgProfiles[0]);

const SgEcuIdMap sgEcuIdMap[] = {
  SG_DB_LEAD_GIORNO_ECU_MAP,
  SG_DB_PCX_CLICK_ECU_MAP,
  SG_DB_WAVE_CUB_ECU_MAP
};
const uint8_t SG_ECU_ID_MAP_COUNT = sizeof(sgEcuIdMap) / sizeof(sgEcuIdMap[0]);

const SgProfile *sgProfile = &sgProfiles[0];
char sgEcuIdHex[16] = "UNKNOWN";
char sgModelName[28] = "AUTO SCAN";
bool sgEcuIdReadDone = false;
bool sgRuntimeProfileEnabled = false;
bool sgEcuUnsupported = false;
bool sgEcuAmbiguous = false;

// สถานะที่เพิ่มจาก Improvement Plan
// ต้องประกาศเป็น global เพราะ K-Line / Decode / Monitor / UI ใช้ร่วมกัน
bool sgProfileOffsetsOk = false;       // true = offset ของโปรไฟล์ผ่านการตรวจขอบเขต payload
bool sgGenericFallbackActive = false;  // true = ECU ID ไม่รู้จัก แต่ใช้ Generic fallback เพื่อช่วย debug
