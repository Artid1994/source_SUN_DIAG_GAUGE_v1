#pragma once
/*
  SUN DIAG GAUGE v1.0.1 Decode Split
  แยก Decode ตามกลุ่มรถเพื่อทดสอบ/ปรับฐานข้อมูลทีละกลุ่ม
*/

// ======================== REALTIME DECODE ====================
uint8_t sgMainFrame[40];
uint8_t sgAuxFrame[24];
uint8_t sgMainPayload[32];
uint8_t sgAuxPayload[16];
uint8_t sgMainPayloadLen = 0;
uint8_t sgAuxPayloadLen = 0;

bool sgMainHasByte(uint8_t at) { return at < sgMainPayloadLen; }
bool sgAuxHasByte(uint8_t at)  { return at < sgAuxPayloadLen; }
uint8_t sgMainByte(uint8_t at) { return sgMainHasByte(at) ? sgMainPayload[at] : 0; }
uint8_t sgAuxByte(uint8_t at)  { return sgAuxHasByte(at) ? sgAuxPayload[at] : 0; }

uint16_t sgMainU16BE(uint8_t at) {
  if ((uint16_t)at + 1 >= sgMainPayloadLen) return 0;
  return ((uint16_t)sgMainPayload[at] << 8) | sgMainPayload[at + 1];
}

uint16_t sgAuxU16BE(uint8_t at) {
  if ((uint16_t)at + 1 >= sgAuxPayloadLen) return 0;
  return ((uint16_t)sgAuxPayload[at] << 8) | sgAuxPayload[at + 1];
}

uint8_t sgMainProfileOffset(uint8_t adxOffset) {
  if (sgProfile->offsetsIncludeEcho) {
    return adxOffset >= sgProfile->tableMainLen ? (uint8_t)(adxOffset - sgProfile->tableMainLen) : 0;
  }
  return adxOffset;
}

uint8_t sgAuxProfileOffset(uint8_t adxOffset) {
  if (sgProfile->offsetsIncludeEcho) {
    return adxOffset >= sgProfile->tableAuxLen ? (uint8_t)(adxOffset - sgProfile->tableAuxLen) : 0;
  }
  return adxOffset;
}


bool sgValidateProfileOffsets(const SgProfile *profile) {
  if (!profile) return false;

  auto chk = [](const char *name, uint8_t offset, uint8_t payloadLen, bool u16Value) -> bool {
    const uint8_t need = u16Value ? 2 : 1;
    if ((uint16_t)offset + need > payloadLen) {
      SG_PRINT("[VALIDATION] "); SG_PRINT(name); SG_PRINT(" offset out of bounds: 0x");
      SG_PRINT_HEX(offset);
      SG_PRINT(" len="); SG_PRINTLN(payloadLen);
      return false;
    }
    return true;
  };

  const uint8_t rpmOfs = profile->offsetsIncludeEcho ? (profile->rpmOffset >= profile->tableMainLen ? profile->rpmOffset - profile->tableMainLen : 0) : profile->rpmOffset;
  const uint8_t tpsOfs = profile->offsetsIncludeEcho ? (profile->tpsOffset >= profile->tableMainLen ? profile->tpsOffset - profile->tableMainLen : 0) : profile->tpsOffset;
  const uint8_t ectOfs = profile->offsetsIncludeEcho ? (profile->tempOffset >= profile->tableMainLen ? profile->tempOffset - profile->tableMainLen : 0) : profile->tempOffset;
  const uint8_t batOfs = profile->offsetsIncludeEcho ? (profile->batteryOffset >= profile->tableMainLen ? profile->batteryOffset - profile->tableMainLen : 0) : profile->batteryOffset;
  const uint8_t injOfs = profile->offsetsIncludeEcho ? (profile->injectorOffset >= profile->tableMainLen ? profile->injectorOffset - profile->tableMainLen : 0) : profile->injectorOffset;
  const uint8_t ignOfs = profile->offsetsIncludeEcho ? (profile->ignOffset >= profile->tableMainLen ? profile->ignOffset - profile->tableMainLen : 0) : profile->ignOffset;
  const uint8_t afrOfs = profile->offsetsIncludeEcho ? (profile->afrOffset >= profile->tableAuxLen ? profile->afrOffset - profile->tableAuxLen : 0) : profile->afrOffset;
  const uint8_t stftOfs = profile->offsetsIncludeEcho ? (profile->stftOffset >= profile->tableAuxLen ? profile->stftOffset - profile->tableAuxLen : 0) : profile->stftOffset;

  bool ok = true;
  ok &= chk("RPM", rpmOfs, profile->mainReplyLen, true);
  ok &= chk("TPS", tpsOfs, profile->mainReplyLen, false);
  ok &= chk("ECT", ectOfs, profile->mainReplyLen, false);
  ok &= chk("BAT", batOfs, profile->mainReplyLen, false);
  ok &= chk("INJ", injOfs, profile->mainReplyLen, true);
  ok &= chk("IGN", ignOfs, profile->mainReplyLen, false);
  ok &= chk("AFR/O2", afrOfs, profile->auxReplyLen, false);
  ok &= chk("STFT", stftOfs, profile->auxReplyLen, false);

  if (ok) SG_PRINTLN("[PROFILE] offsets validated OK");
  else SG_PRINTLN("[PROFILE] OFFSET VALIDATION FAILED - decode locked");
  return ok;
}


void sgApplyRpmSmooth() {
  if (sgGauge.rpmFiltered <= 0.1f || sgGauge.rpmRaw == 0) sgGauge.rpmFiltered = (float)sgGauge.rpmRaw;
  else sgGauge.rpmFiltered += ((float)sgGauge.rpmRaw - sgGauge.rpmFiltered) * sgRpmDisplayAlpha(sgSmoothLevel);
  uint16_t rounded = (uint16_t)(sgGauge.rpmFiltered + (SG_RPM_DISPLAY_STEP / 2));
  sgGauge.rpm = (rounded / SG_RPM_DISPLAY_STEP) * SG_RPM_DISPLAY_STEP;
}


// ======================== AFR DISPLAY SMOOTHING ========================
// เก็บสูตร AFR เดิมจาก Database ไว้เหมือนเดิม 100%
// ฟังก์ชันนี้กรองเฉพาะค่าที่ส่งไปแสดงผลบน UI/Monitor
// หลักการ:
// - ค่าดิบหลัง decode เก็บไว้ที่ afrRawDecoded เหมือนเดิม
// - noise เล็ก ๆ ต่ำกว่า SG_AFR_NOISE_DEADBAND จะไม่ดึงตัวเลขให้สั่น
// - ถ้าค่าเปลี่ยนจริงมาก ๆ จะใช้ fast alpha เพื่อให้ค่าตามทัน ไม่หน่วงจนหลอกตา
void sgApplyAfrSmooth(float decodedAfr) {
  decodedAfr = sgFuelScaledAfr(decodedAfr);  // สเกล AFR ตามเชื้อเพลิงที่เลือก
  sgGauge.afrRawDecoded = decodedAfr;

  if (decodedAfr < 5.0f || decodedAfr > 25.0f) {
    sgGauge.afr = 0.0f;
    sgGauge.afrFilterReady = false;
    return;
  }

  if (!sgGauge.afrFilterReady) {
    sgGauge.afrFiltered = decodedAfr;
    sgGauge.afrFilterReady = true;
  } else {
    float delta = decodedAfr - sgGauge.afrFiltered;
    float absDelta = fabsf(delta);

    if (absDelta >= SG_AFR_NOISE_DEADBAND) {
      const float alpha = (absDelta >= SG_AFR_FAST_DELTA) ? sgAfrDisplayFastAlpha() : sgAfrDisplayAlpha(sgSmoothLevel);
      float correction = delta * alpha;
      const float maxStep = sgAfrDisplayMaxStep();
      if (correction > maxStep) correction = maxStep;
      else if (correction < -maxStep) correction = -maxStep;
      sgGauge.afrFiltered += correction;
    }
  }

  sgGauge.afr = sgGauge.afrFiltered;
}

void sgClampAfrStft() {
  if (sgGauge.afr < 5.0f || sgGauge.afr > 25.0f) sgGauge.afr = 0.0f;
  if (sgGauge.stftPct < -100.0f || sgGauge.stftPct > 100.0f) sgGauge.stftPct = 0.0f;
  if (sgGauge.ltftPct < -100.0f || sgGauge.ltftPct > 100.0f) sgGauge.ltftPct = 0.0f;
}

// แปลงค่า Trim จากสูตร ADX แบบ Ratio ให้เป็น % สำหรับไล่รถ
// ตัวอย่าง: ratio 1.00 = 0%, ratio 1.05 = +5%, ratio 0.95 = -5%
// หมายเหตุ: ยังรักษาสูตร ADX เดิมไว้ก่อน แล้วค่อยแปลงเพื่อการแสดงผล
float sgTrimRatioToPercent(float ratio) {
  return (ratio - 1.0f) * 100.0f;
}

float sgTrimNonPcxRatio(uint8_t raw) {
  return ((float)raw / 128.0f) * 0.998f;
}

float sgTrimPcxRatio(uint8_t raw) {
  return (float)raw / 128.258f;
}

float sgPcxClickO2AdxToAfr(float o2Adx) {
  struct AfrLookupPoint {
    float input;
    float output;
  };

  static const AfrLookupPoint lookup[] = {
    {0.000f, 20.000f},
    {0.020f, 18.700f},
    {0.028f, 18.000f},
    {0.042f, 17.000f},
    {0.098f, 16.100f},
    {0.351f, 15.000f},
    {0.6925f, 14.000f},
    {0.800f, 13.000f},
    {0.870f, 12.000f},
    {0.936f, 11.000f},
    {1.014f, 10.000f},
  };

  if (o2Adx <= lookup[0].input) return lookup[0].output;

  const uint8_t count = sizeof(lookup) / sizeof(lookup[0]);
  for (uint8_t i = 1; i < count; ++i) {
    if (o2Adx <= lookup[i].input) {
      const float span = lookup[i].input - lookup[i - 1].input;
      if (span <= 0.0f) return lookup[i].output;
      const float t = (o2Adx - lookup[i - 1].input) / span;
      return lookup[i - 1].output + ((lookup[i].output - lookup[i - 1].output) * t);
    }
  }

  return lookup[count - 1].output;
}

// LTFT #1 ใช้เฉพาะ Lead/Giorno และกลุ่ม Wave/Cub ตาม ADX กลุ่ม non-PCX/Click.
// Table17 payload[0x12] เป็น LT Fuel Trim #1; ไม่ใช่ Battery.
// สูตร ADX ที่ล็อคไว้: (X / 128) * 0.998
// ค่าที่ส่งให้ UI/Monitor เป็นเปอร์เซ็นต์ชดเชย: (ratio - 1) * 100
void sgDecodeLtftNonPcx() {
  sgGauge.ltftRaw = 0;
  sgGauge.ltftPct = 0.0f;
  sgGauge.ltftValid = false;

  static const uint8_t SG_LTFT_T17_OFFSET = 0x12;
  if (!sgMainHasByte(SG_LTFT_T17_OFFSET)) return;

  const uint8_t raw = sgMainByte(SG_LTFT_T17_OFFSET);
  const float ratio = sgTrimNonPcxRatio(raw);
  const float trimPct = sgTrimRatioToPercent(ratio);

  sgGauge.ltftRaw = raw;
  sgGauge.ltftPct = trimPct;
  sgGauge.ltftValid = true;
}


// ======================== DECODE: LEAD / GIORNO ========================
/*
  Decode Group: LEAD / GIORNO
  ADX: LEAD GRIONO BLUE.adx
  Validation: Lead125 log จริง ใช้งานเป็นฐานก่อน แล้วค่อยปรับจากรถจริงเพิ่ม
*/
void sgDecodeMain_LeadGiorno() {
  // ใช้ไฟล์ ADX หลัก Lead/Giorno/Wave/Super Cub ตรงตาม TunePro RT
  // Table17 reply: packetoffsetinbody=5, payload 24 bytes
  sgGauge.rpmRaw = sgMainU16BE(0x04);                 // ENGINESPEED: X
  sgApplyRpmSmooth();

  sgGauge.tpsVolt = (float)sgMainByte(0x06) * (5.0f / 256.0f); // TPSENSORVOLT: X*5/256
  sgGauge.tps = constrain((float)sgMainByte(0x07) * 0.5f, 0.0f, 100.0f); // TPSENSORANGLE: X*0.5

  sgGauge.eotVolt = (float)sgMainByte(0x08) * (5.0f / 256.0f); // EOTSENSORVOLT: X*5/256
  sgGauge.eotC = (int16_t)sgMainByte(0x09) - 40;               // EOTSENSORTEMP: X-40
  // FIX: Lead/Giorno ใช้ตำแหน่ง ECT เดิมที่เคยทดสอบผ่าน ไม่ใช้ตำแหน่ง ECT จาก ADX ล่าสุด
  // สูตรยังเป็นสูตรเดิมของ Honda: raw - 40
  sgGauge.ectVolt = (float)sgMainByte(0x08) * (5.0f / 256.0f); // ค่าแรงดันอ้างอิงเดิม
  sgGauge.tempC = (int16_t)sgMainByte(0x0D) - 40;              // ECT OLD WORKING: payload[0x0D] - 40

  sgGauge.batteryV = (float)sgMainByte(0x0E) / 10.0f;          // BATTERYVOLT: X/10
  sgGauge.injectorMs = (float)sgMainU16BE(0x0F) / 250.0f;      // INJECTORDURATI: X/250
  sgGauge.injectorDutyPct = ((float)sgGauge.injectorMs * (float)sgGauge.rpmRaw) / 1200.0f; // INJECTORDUTY: (X*Y)/1200
  sgGauge.ignitionDeg = ((float)sgMainByte(0x11) / 2.0f) - 64.0f; // IGNITIONADVANC: (X/2)-64
  sgDecodeLtftNonPcx(); // LTFUELTRIM#1: X/128*0.998

  sgGauge.iatVolt = 0.0f;
  sgGauge.iatC = 0;
  sgGauge.mapVolt = 0.0f;
  sgGauge.mapKpa = 0.0f;
  sgGauge.speedKmh = 0;
  sgGauge.iacvPulse = 0;
  sgGauge.iacvFlow = 0.0f;
  sgGauge.o2HeaterVolt = 0.0f;
}

void sgDecodeAux_LeadGiorno() {
  // Table20 reply: packetoffsetinbody=5, payload 8 bytes
  sgGauge.afrRaw = sgAuxByte(0x04);
  sgGauge.stftRaw = sgAuxByte(0x05);

  // LOCKED WORKING FIX: Lead/Giorno AFR หน้า 1/หน้า 2 ต้องใช้สูตรเดิมที่เคยแสดงผลได้จริง
  // O2 Sensor Voltage ตาม ADX: (X / 128) * 2.5
  // AFR_1 เดิมที่ใช้งานได้กับ Lead125: (X * -0.17857) + 20
  // ห้ามเปลี่ยนเป็น o2Afr ตรง ๆ เพราะค่าจะอยู่ช่วง 0-2.5 แล้ว UI จะมองว่า AFR invalid และไม่แสดง
  sgGauge.o2Volt = (float)sgGauge.afrRaw * (2.5f / 128.0f);
  const float leadAfr = 20.0f - ((float)sgGauge.afrRaw * 0.17857f);
  sgApplyAfrSmooth(leadAfr);

  sgGauge.stftPct = sgTrimRatioToPercent(sgTrimNonPcxRatio(sgGauge.stftRaw)); // STFUELTRIM#1: (X/128*0.998 - 1) * 100
  sgGauge.o2HeaterVolt = 0.0f;
}

// ======================== DECODE: PCX / CLICK ========================
/*
  Decode Group: PCX / CLICK
  ADX: pXC150-160.adx
  หมายเหตุ: offset ของกลุ่มนี้อ้างอิงเฟรมเต็มที่มี echo จึงต้องผ่าน sgMainProfileOffset()/sgAuxProfileOffset()
*/
void sgDecodeMain_PCXClick() {
  // ใช้ไฟล์ ADX PCX150/PCX160/Click160 ตรงตาม TunePro RT
  // Command 1 reply: 30 bytes, ADX packet offsets include 5-byte echoed command/header.
  sgGauge.rpmRaw = sgMainU16BE(sgMainProfileOffset(0x09));      // RPM breakpoint: X, 16-bit
  sgApplyRpmSmooth();

  sgGauge.tpsVolt = (float)sgMainByte(sgMainProfileOffset(0x0B)) * (5.0f / 256.377f); // TP sensor[V]: X*5/256.377
  sgGauge.tps = constrain((float)sgMainByte(sgMainProfileOffset(0x0C)) / 2.0f, 0.0f, 100.0f); // TP sensor: X/2

  sgGauge.ectVolt = (float)sgMainByte(sgMainProfileOffset(0x0D)) * (5.0f / 256.377f); // ECT sensor[V]
  sgGauge.tempC = (int16_t)sgMainByte(sgMainProfileOffset(0x0E)) - 40;                // ECT sensor[C]: X-40
  sgGauge.iatVolt = (float)sgMainByte(sgMainProfileOffset(0x0F)) * (5.0f / 256.377f); // IAT sensor[V]
  sgGauge.iatC = (int16_t)sgMainByte(sgMainProfileOffset(0x10)) - 40;                 // IAT sensor[C]: X-40
  sgGauge.mapVolt = (float)sgMainByte(sgMainProfileOffset(0x11)) * (5.0f / 256.377f); // MAP sensor[V]
  sgGauge.mapKpa = (float)sgMainByte(sgMainProfileOffset(0x12));                      // MAP sensor[kPa]: X

  sgGauge.batteryV = (float)sgMainByte(sgMainProfileOffset(0x15)) / 10.0f;            // Battery[V]: X/10
  sgGauge.speedKmh = sgMainByte(sgMainProfileOffset(0x16));                           // Bike speed: X
  sgGauge.injectorMs = (float)sgMainU16BE(sgMainProfileOffset(0x17)) / 250.0f;        // Injector[MS]: X/250
  sgGauge.injectorDutyPct = ((float)sgGauge.injectorMs * (float)sgGauge.rpmRaw) / 1200.0f; // Injectorduty: (X*Y)/1200
  sgGauge.ignitionDeg = ((float)sgMainByte(sgMainProfileOffset(0x19)) - 128.0f) / 2.0f; // spark advance: (X-128)/2
  sgGauge.iacvPulse = (uint16_t)sgMainByte(sgMainProfileOffset(0x1A)) * 2U;           // IACV pulse counter: X*2
  sgGauge.iacvFlow = (float)sgMainByte(sgMainProfileOffset(0x1B)) / 13.0121f;         // IACV valve: X/13.0121

  sgGauge.eotVolt = 0.0f;
  sgGauge.eotC = 0;
  sgGauge.ltftRaw = 0;
  sgGauge.ltftPct = 0.0f;
  sgGauge.ltftValid = false;
}

void sgDecodeAux_PCXClick() {
  // Command 2 reply: 13 bytes, ADX packet offsets include 5-byte echoed command/header.
  sgGauge.afrRaw = sgAuxByte(sgAuxProfileOffset(0x09));
  sgGauge.stftRaw = sgAuxByte(sgAuxProfileOffset(0x0A));

  const float o2Adx = ((float)sgGauge.afrRaw / 753.747f) * 14.7f; // O2sensor[V]: X/753.747*14.7
  const float afrExact = sgPcxClickO2AdxToAfr(o2Adx);             // AFR: ADX LOOKUP(AFR) est.
  sgGauge.o2Volt = o2Adx;
  sgApplyAfrSmooth(afrExact);

  sgGauge.stftPct = sgTrimRatioToPercent(sgTrimPcxRatio(sgGauge.stftRaw)); // O2 stft: (X/128.258 - 1) * 100
  sgGauge.o2HeaterVolt = (float)sgAuxByte(sgAuxProfileOffset(0x0B)) * (5.0f / 256.377f); // O2heater[V]
}

// ======================== DECODE: WAVE / CUB ========================
/*
  Decode Group: WAVE / SUPER CUB
  ADX: Real-Time wave. tanasanst17.adx
  Status: ใช้สูตรฐาน S4 ก่อน ต้องยืนยันด้วย log รถจริงแยกรุ่น
  Update: tempOffset ถูกย้ายไป 0x0B ใน ProfileDB ตามไฟล์ ADX note ที่ส่งมา
*/
void sgDecodeMain_WaveCub() {
  // ใช้ไฟล์ ADX หลัก Lead/Giorno/Wave/Super Cub ตรงตาม TunePro RT
  // Table17 reply: packetoffsetinbody=5, payload 24 bytes
  sgGauge.rpmRaw = sgMainU16BE(0x04);                 // ENGINESPEED: X
  sgApplyRpmSmooth();

  sgGauge.tpsVolt = (float)sgMainByte(0x06) * (5.0f / 256.0f); // TPSENSORVOLT: X*5/256
  sgGauge.tps = constrain((float)sgMainByte(0x07) * 0.5f, 0.0f, 100.0f); // TPSENSORANGLE: X*0.5

  sgGauge.eotVolt = (float)sgMainByte(0x08) * (5.0f / 256.0f); // EOTSENSORVOLT: X*5/256
  sgGauge.eotC = (int16_t)sgMainByte(0x09) - 40;               // EOTSENSORTEMP: X-40
  sgGauge.ectVolt = (float)sgMainByte(0x0A) * (5.0f / 256.0f); // ECTSENSORVOLT: X*5/256
  sgGauge.tempC = (int16_t)sgMainByte(0x0B) - 40;              // ECTSENSORTEMP: X-40

  sgGauge.batteryV = (float)sgMainByte(0x0E) / 10.0f;          // BATTERYVOLT: X/10
  sgGauge.injectorMs = (float)sgMainU16BE(0x0F) / 250.0f;      // INJECTORDURATI: X/250
  sgGauge.injectorDutyPct = ((float)sgGauge.injectorMs * (float)sgGauge.rpmRaw) / 1200.0f; // INJECTORDUTY: (X*Y)/1200
  sgGauge.ignitionDeg = ((float)sgMainByte(0x11) / 2.0f) - 64.0f; // IGNITIONADVANC: (X/2)-64
  sgDecodeLtftNonPcx(); // LTFUELTRIM#1: X/128*0.998

  sgGauge.iatVolt = 0.0f;
  sgGauge.iatC = 0;
  sgGauge.mapVolt = 0.0f;
  sgGauge.mapKpa = 0.0f;
  sgGauge.speedKmh = 0;
  sgGauge.iacvPulse = 0;
  sgGauge.iacvFlow = 0.0f;
  sgGauge.o2HeaterVolt = 0.0f;
}

void sgDecodeAux_WaveCub() {
  // Table20 reply: packetoffsetinbody=5, payload 8 bytes
  sgGauge.afrRaw = sgAuxByte(0x04);
  sgGauge.stftRaw = sgAuxByte(0x05);

  const float o2Afr = ((float)sgGauge.afrRaw / 128.0f) * 2.5f; // O2SENSORVOLT และ AIRFUELRATIO: X/128*2.5
  sgGauge.o2Volt = o2Afr;
  sgGauge.afrRawDecoded = o2Afr;
  // ตาม ADX ค่านี้ไม่ใช่ wideband AFR 10-20 แบบเดิม จึงไม่ส่งเข้า AFR warning/filter เดิม
  sgGauge.afr = 0.0f;
  sgGauge.afrFilterReady = false;

  sgGauge.stftPct = sgTrimRatioToPercent(sgTrimNonPcxRatio(sgGauge.stftRaw)); // STFUELTRIM#1: (X/128*0.998 - 1) * 100
  sgGauge.o2HeaterVolt = 0.0f;
}


void sgDecodeMain() {
  switch (sgProfile->id) {
    case SG_PROFILE_LEAD125:
    case SG_PROFILE_GIORNO:
      sgDecodeMain_LeadGiorno();
      break;
    case SG_PROFILE_PCX150:
    case SG_PROFILE_PCX160:
    case SG_PROFILE_CLICK160:
      sgDecodeMain_PCXClick();
      break;
    case SG_PROFILE_WAVE110I:
    case SG_PROFILE_WAVE125I:
    case SG_PROFILE_SUPERCUB:
      sgDecodeMain_WaveCub();
      break;
    case SG_PROFILE_UNKNOWN:
      if (sgGenericFallbackActive) sgDecodeMain_LeadGiorno();
      break;
    default:
      break;
  }
}

void sgDecodeAux() {
  switch (sgProfile->id) {
    case SG_PROFILE_LEAD125:
    case SG_PROFILE_GIORNO:
      sgDecodeAux_LeadGiorno();
      break;
    case SG_PROFILE_PCX150:
    case SG_PROFILE_PCX160:
    case SG_PROFILE_CLICK160:
      sgDecodeAux_PCXClick();
      break;
    case SG_PROFILE_WAVE110I:
    case SG_PROFILE_WAVE125I:
    case SG_PROFILE_SUPERCUB:
      sgDecodeAux_WaveCub();
      break;
    case SG_PROFILE_UNKNOWN:
      if (sgGenericFallbackActive) sgDecodeAux_LeadGiorno();
      break;
    default:
      break;
  }
}

bool sgReadProfileTable(const uint8_t *tx, uint8_t txLen, uint8_t *frame, uint8_t frameCap,
                        uint8_t expectedReplyLen, uint16_t timeoutMs, const char *tag,
                        uint8_t *payload, uint8_t *payloadLen) {
  const uint8_t fallbackExpected = txLen + expectedReplyLen;
  if (fallbackExpected > frameCap) return false;
  bool ok = sgKLineSendReadExact(tx, txLen, frame, fallbackExpected, timeoutMs, tag);
  if (!ok) return false;

  uint8_t replyLen = frame[txLen + 1];
  if (frame[txLen] != 0x02 || replyLen < 2 || (uint16_t)txLen + replyLen > frameCap) {
    replyLen = expectedReplyLen;
  }
  const uint8_t copyLen = replyLen > 0 ? replyLen - 1 : 0; // ไม่ส่ง checksum เข้า decoder
  memcpy(payload, frame + txLen, copyLen);
  *payloadLen = copyLen;
  return true;
}

bool sgReadTable17() {
  bool ok = sgReadProfileTable(sgProfile->tableMainCmd, sgProfile->tableMainLen, sgMainFrame, sizeof(sgMainFrame),
                               sgProfile->mainReplyLen, SG_T17_TIMEOUT_MS, "[MAIN TX]", sgMainPayload, &sgMainPayloadLen);
  sgGauge.t17Ok = ok;
  if (!ok) {
    if (++sgConsecutiveFail >= SG_MAX_CONSECUTIVE_FAIL) {
      sgScheduleReconnect("REALTIME LINK LOST");
    }
    return false;
  }
  if (!sgProfileOffsetsOk) return false;
  sgDecodeMain();
  sgGauge.lastT17Ms = millis();
  sgPublishVehicleMainSnapshot();
  sgT17Count++;
  return true;
}


bool sgReadTable20() {
  bool ok = sgReadProfileTable(sgProfile->tableAuxCmd, sgProfile->tableAuxLen, sgAuxFrame, sizeof(sgAuxFrame),
                               sgProfile->auxReplyLen, SG_T20_TIMEOUT_MS, "[AUX TX]", sgAuxPayload, &sgAuxPayloadLen);
  sgGauge.t20Ok = ok;
  if (!ok) {
    if (++sgConsecutiveFail >= SG_MAX_CONSECUTIVE_FAIL) {
      sgScheduleReconnect("REALTIME LINK LOST");
    }
    return false;
  }
  if (!sgProfileOffsetsOk) return false;
  sgDecodeAux();
  sgGauge.lastT20Ms = millis();
  sgT20Count++;
  return true;
}
