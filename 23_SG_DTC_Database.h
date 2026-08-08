#pragma once
// SUN DIAG GAUGE v1.1.1 BETA13 DTC Thai display-language refinement
// Source concept: HondaECU V330 DTC text pairs; separated by vehicle group for safe future override.
// Unknown code policy: never guess. Display UNKNOWN DTC when no exact pair exists.

enum SgDtcVehicleGroup : uint8_t {
  SG_DTC_GROUP_COMMON = 0,
  SG_DTC_GROUP_LEAD_GIORNO = 1,
  SG_DTC_GROUP_PCX_CLICK = 2,
  SG_DTC_GROUP_WAVE_CUB = 3
};

struct SgDtcDefinition {
  uint8_t mainCode;
  uint8_t subCode;
  const char *sensorTH;
  const char *sensorEN;
  const char *faultTH;
  const char *faultEN;
};

#define DTC(m,s,sTH,sEN,fTH,fEN) {m,s,sTH,sEN,fTH,fEN}

static const SgDtcDefinition SG_DTC_COMMON_DB[] = {
  DTC(1,1,"MAP","MAP","สัญญาณต่ำ","LOW VOLTAGE"),
  DTC(1,2,"MAP","MAP","สัญญาณสูง","HIGH VOLTAGE"),
  DTC(2,1,"MAP","MAP","ค่าอ่านผิดปกติ","PERFORMANCE"),
  DTC(7,1,"ECT","ECT","สัญญาณต่ำ","LOW VOLTAGE"),
  DTC(7,2,"ECT","ECT","สัญญาณสูง","HIGH VOLTAGE"),
  DTC(8,1,"TPS","TP","สัญญาณต่ำ","LOW VOLTAGE"),
  DTC(8,2,"TPS","TP","สัญญาณสูง","HIGH VOLTAGE"),
  DTC(9,1,"IAT","IAT","สัญญาณต่ำ","LOW VOLTAGE"),
  DTC(9,2,"IAT","IAT","สัญญาณสูง","HIGH VOLTAGE"),
  DTC(11,1,"VS","VS","ไม่มีสัญญาณ","NO SIGNAL"),
  DTC(12,1,"หัวฉีด","INJECTOR 1","วงจรผิดปกติ","CIRCUIT"),
  DTC(18,1,"CMP","CMP","ไม่มีสัญญาณ","NO SIGNAL"),
  DTC(19,1,"CKP","CKP","ไม่มีสัญญาณ","NO SIGNAL"),
  DTC(21,1,"O2","O2","สัญญาณต่ำ","LOW VOLTAGE"),
  DTC(21,2,"O2","O2","สัญญาณสูง","HIGH VOLTAGE"),
  DTC(23,1,"O2 HEATER","O2 HEATER","วงจรผิดปกติ","MALFUNCTION"),
  DTC(29,1,"IACV","IACV","วงจรผิดปกติ","CIRCUIT"),
  DTC(33,2,"ECM EEPROM","ECM EEPROM","หน่วยความจำผิดปกติ","MALFUNCTION"),
  DTC(36,1,"A/F","A/F","เซ็นเซอร์ผิดปกติ","SENSOR"),
  DTC(38,1,"A/F HEATER","A/F HEATER","วงจรผิดปกติ","MALFUNCTION"),
  DTC(86,1,"ECU","SERIAL COM","สื่อสารไม่ได้","MALFUNCTION"),
  DTC(91,1,"IGN COIL","IGN COIL","วงจรคอยล์ผิดปกติ","PRIMARY CIRCUIT")
};

// Lead/Giorno: กลุ่มรถออโต้ Shindengen ขนาดเล็ก เน้นหัวฉีดเดี่ยวและระบบเดินเบา
static const SgDtcDefinition SG_DTC_LEAD_GIORNO_DB[] = {
  DTC(12,1,"หัวฉีด","INJECTOR","วงจรผิดปกติ","CIRCUIT"),
  DTC(29,1,"IACV","IACV","วงจรผิดปกติ","CIRCUIT"),
  DTC(54,1,"BANK ANGLE","BANK ANGLE","สัญญาณต่ำ","LOW VOLTAGE"),
  DTC(54,2,"BANK ANGLE","BANK ANGLE","สัญญาณสูง","HIGH VOLTAGE"),
  DTC(88,1,"EVAP","EVAP PURGE","โซลินอยด์ผิดปกติ","SOLENOID")
};

// PCX/Click: กลุ่มออโต้ที่อาจมี O2/A-F heater และ EVAP ตามปี/ตลาด
static const SgDtcDefinition SG_DTC_PCX_CLICK_DB[] = {
  DTC(12,1,"หัวฉีด","INJECTOR","วงจรผิดปกติ","CIRCUIT"),
  DTC(21,1,"O2","O2","สัญญาณต่ำ","LOW VOLTAGE"),
  DTC(21,2,"O2","O2","สัญญาณสูง","HIGH VOLTAGE"),
  DTC(23,1,"O2 HEATER","O2 HEATER","วงจรผิดปกติ","MALFUNCTION"),
  DTC(36,1,"A/F","A/F","เซ็นเซอร์ผิดปกติ","SENSOR"),
  DTC(38,1,"A/F HEATER","A/F HEATER","วงจรผิดปกติ","MALFUNCTION"),
  DTC(54,1,"BANK ANGLE","BANK ANGLE","สัญญาณต่ำ","LOW VOLTAGE"),
  DTC(54,2,"BANK ANGLE","BANK ANGLE","สัญญาณสูง","HIGH VOLTAGE"),
  DTC(88,1,"EVAP","EVAP PURGE","โซลินอยด์ผิดปกติ","SOLENOID")
};

// Wave/Super Cub: กลุ่มเกียร์/Cub เน้นหัวฉีดเดี่ยว ความเร็วรถ และสัญญาณ CKP/CMP
static const SgDtcDefinition SG_DTC_WAVE_CUB_DB[] = {
  DTC(11,1,"VS","VS","ไม่มีสัญญาณ","NO SIGNAL"),
  DTC(12,1,"หัวฉีด","INJECTOR","วงจรผิดปกติ","CIRCUIT"),
  DTC(18,1,"CMP","CMP","ไม่มีสัญญาณ","NO SIGNAL"),
  DTC(19,1,"CKP","CKP","ไม่มีสัญญาณ","NO SIGNAL"),
  DTC(54,1,"BANK ANGLE","BANK ANGLE","สัญญาณต่ำ","LOW VOLTAGE"),
  DTC(54,2,"BANK ANGLE","BANK ANGLE","สัญญาณสูง","HIGH VOLTAGE")
};

#undef DTC

template <size_t N>
inline const SgDtcDefinition* sgDtcLookupTable(const SgDtcDefinition (&table)[N], uint8_t m, uint8_t s) {
  for (size_t i = 0; i < N; ++i) {
    if (table[i].mainCode == m && table[i].subCode == s) return &table[i];
  }
  return nullptr;
}

inline SgDtcVehicleGroup sgDtcVehicleGroupForProfile(SgProfileId p) {
  switch (p) {
    case SG_PROFILE_LEAD125:
    case SG_PROFILE_GIORNO:
      return SG_DTC_GROUP_LEAD_GIORNO;
    case SG_PROFILE_PCX150:
    case SG_PROFILE_PCX160:
    case SG_PROFILE_CLICK160:
      return SG_DTC_GROUP_PCX_CLICK;
    case SG_PROFILE_WAVE110I:
    case SG_PROFILE_WAVE125I:
    case SG_PROFILE_SUPERCUB:
      return SG_DTC_GROUP_WAVE_CUB;
    default:
      return SG_DTC_GROUP_COMMON;
  }
}

inline const SgDtcDefinition* sgDtcLookup(uint8_t m, uint8_t s, SgProfileId p) {
  const SgDtcVehicleGroup g = sgDtcVehicleGroupForProfile(p);
  const SgDtcDefinition *d = nullptr;
  if (g == SG_DTC_GROUP_LEAD_GIORNO) d = sgDtcLookupTable(SG_DTC_LEAD_GIORNO_DB, m, s);
  else if (g == SG_DTC_GROUP_PCX_CLICK) d = sgDtcLookupTable(SG_DTC_PCX_CLICK_DB, m, s);
  else if (g == SG_DTC_GROUP_WAVE_CUB) d = sgDtcLookupTable(SG_DTC_WAVE_CUB_DB, m, s);
  if (d) return d;
  return sgDtcLookupTable(SG_DTC_COMMON_DB, m, s);
}
