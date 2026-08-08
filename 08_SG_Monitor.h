#pragma once
/*
  08_SG_Monitor.h
  SUN DIAG GAUGE Engineering Monitor v2
  - จัด Log ให้อ่านง่ายสำหรับเทียบ ADX / หา Offset จากรถจริง
  - ใช้ SG_PRINT/SG_PRINTLN/SG_PRINT_HEX เพื่อให้แยก USB Serial กับ Bluetooth App JSON ได้
  - Read-only monitor เท่านั้น ไม่มีคำสั่งเขียน ECU / ไม่มี Seed-Key / ไม่มี Flash
*/

void sgMonitorPrintHexByte(uint8_t b) {
#if SG_DEBUG_ENABLE
  if (b < 16) SG_PRINT('0');
  SG_PRINT_HEX(b);
#endif
}

void sgMonitorPrintHexLine(const uint8_t *payload, uint8_t len) {
#if SG_DEBUG_ENABLE
  for (uint8_t i = 0; i < len; ++i) {
    sgMonitorPrintHexByte(payload[i]);
    if (i + 1 < len) SG_PRINT(' ');
  }
  SG_PRINTLN("");
#endif
}

void sgMonitorPrintPayloadInline(const char *label, const uint8_t *payload, uint8_t len) {
#if SG_DEBUG_ENABLE && SG_MONITOR_RAW_ENABLE
  SG_PRINT(label); SG_PRINT(" ["); SG_PRINT(len); SG_PRINT("] ");
  sgMonitorPrintHexLine(payload, len);
#endif
}

void sgMonitorPrintPayloadRows(const char *label, const uint8_t *payload, uint8_t len) {
#if SG_DEBUG_ENABLE && SG_MONITOR_RAW_ENABLE
  SG_PRINT(label); SG_PRINT(" ["); SG_PRINT(len); SG_PRINTLN("]");

  for (uint8_t i = 0; i < len; ++i) {
    if ((i % 8) == 0) {
      SG_PRINT("  ");
      if (i < 16) SG_PRINT('0');
      SG_PRINT_HEX(i);
      SG_PRINT(" : ");
    }

    sgMonitorPrintHexByte(payload[i]);

    if (((i + 1) % 8) == 0 || i + 1 == len) SG_PRINTLN("");
    else SG_PRINT(' ');
  }
#endif
}

void sgMonitorPrintFrameRows(const char *label, const uint8_t *frame, uint8_t len) {
#if SG_DEBUG_ENABLE && SG_MONITOR_RAW_ENABLE
  SG_PRINTLN(label);
  sgMonitorPrintPayloadRows("FRAME", frame, len);
#endif
}

void sgMonitorPrintDecodedBlock() {
#if SG_DEBUG_ENABLE && SG_MONITOR_DATA_ENABLE
  SG_PRINTLN("Decoded");
  SG_PRINT("  RPM      : "); SG_PRINT(sgGauge.rpm); SG_PRINT(" rpm");
  SG_PRINT("  RAW="); SG_PRINTLN(sgGauge.rpmRaw);

  SG_PRINT("  TPS      : "); SG_PRINT(sgVehicleDataV1.mainPublished ? sgVehicleDataV1.throttlePositionPct : sgGauge.tps); SG_PRINT(" %");
  SG_PRINT("  TPS_V="); SG_PRINT(sgGauge.tpsVolt); SG_PRINTLN(" V");

  SG_PRINT("  TEMP/ECT : "); SG_PRINT(sgVehicleDataV1.mainPublished ? sgVehicleDataV1.coolantC : sgGauge.tempC); SG_PRINT(" C");
  SG_PRINT("  ECT_V="); SG_PRINT(sgGauge.ectVolt); SG_PRINTLN(" V");

  SG_PRINT("  EOT      : "); SG_PRINT(sgGauge.eotC); SG_PRINT(" C");
  SG_PRINT("  EOT_V="); SG_PRINT(sgGauge.eotVolt); SG_PRINTLN(" V");

  SG_PRINT("  INJECTOR : "); SG_PRINT(sgVehicleDataV1.mainPublished ? sgVehicleDataV1.injectorMs : sgGauge.injectorMs); SG_PRINT(" ms");
  SG_PRINT("  DUTY="); SG_PRINT(sgGauge.injectorDutyPct); SG_PRINTLN(" %");
  SG_PRINT("  IGN      : "); SG_PRINT(sgVehicleDataV1.mainPublished ? sgVehicleDataV1.ignitionDeg : sgGauge.ignitionDeg); SG_PRINTLN(" deg");
  SG_PRINT("  BATTERY  : "); SG_PRINT(sgVehicleDataV1.mainPublished ? sgVehicleDataV1.batteryV : sgGauge.batteryV); SG_PRINTLN(" V");

  SG_PRINT("  AFR      : "); SG_PRINT(sgGauge.afr);
  SG_PRINT("  RAW="); SG_PRINTLN(sgGauge.afrRawDecoded);

  SG_PRINT("  O2/AFR   : "); SG_PRINT(sgGauge.o2Volt); SG_PRINTLN(" ADX");
  SG_PRINT("  O2 HEAT  : "); SG_PRINT(sgGauge.o2HeaterVolt); SG_PRINTLN(" V");
  SG_PRINT("  STFT     : "); SG_PRINT(sgGauge.stftPct); SG_PRINT(" %");
  SG_PRINT("  RAW="); SG_PRINTLN(sgGauge.stftRaw);
  SG_PRINT("  LTFT     : "); SG_PRINT(sgGauge.ltftPct);
  SG_PRINT(" %  RAW="); SG_PRINT(sgGauge.ltftRaw);
  SG_PRINT(" VALID="); SG_PRINTLN(sgGauge.ltftValid ? 1 : 0);

  SG_PRINT("  IAT      : "); SG_PRINT(sgGauge.iatC); SG_PRINT(" C");
  SG_PRINT("  IAT_V="); SG_PRINT(sgGauge.iatVolt); SG_PRINTLN(" V");

  SG_PRINT("  MAP      : "); SG_PRINT(sgGauge.mapKpa); SG_PRINT(" kPa");
  SG_PRINT("  MAP_V="); SG_PRINT(sgGauge.mapVolt); SG_PRINTLN(" V");

  SG_PRINT("  SPEED    : "); SG_PRINT(sgGauge.speedKmh); SG_PRINTLN(" km/h");
  SG_PRINT("  IACV     : "); SG_PRINT(sgGauge.iacvPulse);
  SG_PRINT(" / FLOW="); SG_PRINTLN(sgGauge.iacvFlow);
#endif
}

void sgMonitorDataTick() {
#if SG_DEBUG_ENABLE && SG_MONITOR_DATA_ENABLE
  const uint32_t now = millis();
  if (now - sgLastDebugMs < SG_MONITOR_DATA_PERIOD_MS) return;
  sgLastDebugMs = now;

  SG_PRINTLN("========================================================");
  SG_PRINTLN("SUN DIAG GAUGE ENGINEERING MONITOR v2");
  SG_PRINTLN("========================================================");

  SG_PRINT("Build    : "); SG_PRINTLN(SG_BUILD_LABEL);
  SG_PRINT("Firmware : "); SG_PRINT(SG_FIRMWARE_VERSION);
  SG_PRINT(" / DB : "); SG_PRINTLN(SG_DB_VERSION);

  SG_PRINT("Vehicle  : "); SG_PRINTLN(sgModelName);
  SG_PRINT("Profile  : "); SG_PRINTLN(sgProfile ? sgProfile->code : "UNKNOWN");
  SG_PRINT("ECU ID   : "); SG_PRINTLN(sgEcuIdHex);
  {
    const SgEcuIdentityRecord *idRef = sgIdentityFirstByEcuId(sgEcuIdHex);
    const uint8_t idCount = sgIdentityCountByEcuId(sgEcuIdHex);
    if (idRef) {
      SG_PRINT("ID REF   : "); SG_PRINTLN(idRef->model);
      SG_PRINT("PART REF : ");
      if (idCount == 1) SG_PRINTLN(idRef->partNumber);
      else { SG_PRINT("MULTIPLE ("); SG_PRINT(idCount); SG_PRINTLN(")"); }
      SG_PRINT("VERIFY   : "); SG_PRINT(idRef->verifyStatus); SG_PRINT(" / "); SG_PRINTLN(idRef->confidence);
    }
  }
  SG_PRINT("Runtime  : "); SG_PRINT(sgRuntimeProfileEnabled ? "ON" : "LOCKED");
  if (sgGenericFallbackActive) SG_PRINT(" (GENERIC FALLBACK)");
  SG_PRINTLN("");
  SG_PRINT("Decode   : v"); SG_PRINT(sgProfile ? sgProfile->decodeVersion : 0);
  SG_PRINT(" / Offset "); SG_PRINTLN(sgProfileOffsetsOk ? "OK" : "ERROR");

  SG_PRINTLN("--------------------------------------------------------");
  SG_PRINTLN("Status");
  SG_PRINT("  Link      : "); SG_PRINTLN(sgGauge.ecuConnected ? "OK" : "NO");
  SG_PRINT("  Main/Aux  : "); SG_PRINT(sgGauge.t17Ok ? "OK" : "NO");
  SG_PRINT(" / "); SG_PRINTLN(sgGauge.t20Ok ? "OK" : "NO");
  SG_PRINT("  Count     : MAIN="); SG_PRINT(sgT17Count);
  SG_PRINT("  AUX="); SG_PRINTLN(sgT20Count);
  SG_PRINT("  Error     : RX_TO="); SG_PRINT(sgRxTimeoutCount);
  SG_PRINT("  CRC="); SG_PRINT(sgChecksumErrorCount);
  SG_PRINT("  FAIL="); SG_PRINTLN(sgConsecutiveFail);
  SG_PRINT("  Wake      : FAIL="); SG_PRINT(sgWakeFailCount);
  SG_PRINT("  LOCK="); SG_PRINTLN(sgWakeLocked ? 1 : 0);

  SG_PRINTLN("--------------------------------------------------------");
  sgMonitorPrintDecodedBlock();

  SG_PRINTLN("--------------------------------------------------------");
  SG_PRINTLN("Payload Main / Table17");
  sgMonitorPrintPayloadRows("PAYLOAD17", sgMainPayload, sgMainPayloadLen);

  SG_PRINTLN("--------------------------------------------------------");
  SG_PRINTLN("--------------------------------------------------------");
  SG_PRINTLN("Payload Aux / Table20");
  sgMonitorPrintPayloadRows("PAYLOAD20", sgAuxPayload, sgAuxPayloadLen);

  SG_PRINTLN("--------------------------------------------------------");
  SG_PRINTLN("Raw Frame Snapshot");
  sgMonitorPrintPayloadInline("MAIN_FRAME", sgMainFrame, sgProfile ? (uint8_t)(sgProfile->tableMainLen + sgProfile->mainReplyLen) : 0);
  sgMonitorPrintPayloadInline("AUX_FRAME ", sgAuxFrame,  sgProfile ? (uint8_t)(sgProfile->tableAuxLen + sgProfile->auxReplyLen) : 0);

  SG_PRINTLN("========================================================");
#endif
}
