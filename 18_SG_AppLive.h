#pragma once
/*
  18_SG_AppLive.h
  SUN DIAG GAUGE Stable App Fast Packet Patch

  หน้าที่:
  - ส่งข้อมูลที่ Decode แล้วไปแอพมือถือเป็น JSON สั้น 1 บรรทัด
  - ไม่อ่าน ECU เพิ่ม ไม่เปลี่ยน Polling และไม่แตะ Decode/Offset/Formula
  - ไม่มี delay และทำงานเฉพาะเมื่อ Bluetooth พร้อมและมี Client เชื่อมต่อ

  หมายเหตุสำคัญ:
  Bluetooth Classic SPP มี data stream เดียว หาก SG_APP_LIVE_EXCLUSIVE_BT = 1
  Bluetooth จะส่งเฉพาะ JSON ของแอพ ส่วน Engineering Monitor Log ยังออก USB Serial
  เพื่อไม่ให้ข้อความ Debug ปนใน JSON
*/

static uint32_t sgAppLastSendMs = 0;
static uint32_t sgAppSeq = 0;

#if SG_APP_AUTH_ENABLE
static bool sgAppAuthOk = false;
static uint8_t sgAppAuthFails = 0;
static uint32_t sgAppLastAuthPromptMs = 0;
static char sgAppAuthLine[24];
static uint8_t sgAppAuthLineLen = 0;
#endif

// ทำชื่อรุ่นให้ปลอดภัยสำหรับ JSON โดยไม่ใช้ String และไม่จองหน่วยความจำแบบ Dynamic
static void sgAppJsonSafeModel(char *out, size_t outLen, const char *src) {
  if (!out || outLen == 0) return;
  out[0] = '\0';
  if (!src) return;

  size_t w = 0;
  for (size_t r = 0; src[r] != '\0' && w + 1 < outLen; ++r) {
    const char c = src[r];

    // ป้องกัน JSON แตกจาก quote/backslash; ชื่อรุ่นปกติแทบไม่เข้ากรณีนี้
    if ((c == '"' || c == '\\') && w + 2 < outLen) {
      out[w++] = '\\';
      out[w++] = c;
    } else if ((uint8_t)c >= 0x20) {
      out[w++] = c;
    }
  }
  out[w] = '\0';
}


#if SG_APP_AUTH_ENABLE
static bool sgAppPinMatches(const char *line) {
  if (!line) return false;

  // รองรับทั้ง PIN:1234, AUTH:1234 และ 1234 เพื่อให้ฝั่งแอพเขียนง่าย
  if (strncmp(line, "PIN:", 4) == 0) line += 4;
  else if (strncmp(line, "AUTH:", 5) == 0) line += 5;

  // ตัดช่องว่างหน้ารหัส
  while (*line == ' ' || *line == '\t') line++;

  char pin[8];
  size_t w = 0;
  while (*line && *line != '\r' && *line != '\n' && *line != ' ' && w + 1 < sizeof(pin)) {
    pin[w++] = *line++;
  }
  pin[w] = '\0';
  return strcmp(pin, SG_APP_AUTH_PIN) == 0;
}

// ใช้ reset เฉพาะตอนเริ่มระบบ/กรณีต้องการล็อกใหม่จริง ๆ
// ไม่เรียกตอน Bluetooth หลุด เพื่อให้ reconnect แล้วไม่ต้องใส่ PIN ซ้ำ
static void sgAppAuthReset() {
  sgAppAuthOk = false;
  sgAppAuthFails = 0;
  sgAppAuthLineLen = 0;
  sgAppAuthLine[0] = '\0';
}

static void sgAppAuthProcessInput() {
  while (SG_BT.available() > 0) {
    const char c = (char)SG_BT.read();

    if (c == '\r' || c == '\n') {
      if (sgAppAuthLineLen == 0) continue;
      sgAppAuthLine[sgAppAuthLineLen] = '\0';

      if (sgAppPinMatches(sgAppAuthLine)) {
        sgAppAuthOk = true;
        sgAppAuthFails = 0;
#if SG_APP_AUTH_REMEMBER_TRUSTED
        sgSettingsSaveAppAuthTrusted(true);
#endif
        SG_BT.println("{\"type\":\"auth\",\"ok\":true,\"trusted\":true}");
      } else {
        if (sgAppAuthFails < 255) sgAppAuthFails++;
        SG_BT.println("{\"type\":\"auth\",\"ok\":false}");
      }

      sgAppAuthLineLen = 0;
      sgAppAuthLine[0] = '\0';
      continue;
    }

    if ((uint8_t)c < 0x20) continue;
    if (sgAppAuthLineLen + 1 < sizeof(sgAppAuthLine)) {
      sgAppAuthLine[sgAppAuthLineLen++] = c;
    } else {
      // ยาวผิดปกติ ให้เริ่มรับใหม่ ป้องกัน buffer เต็ม
      sgAppAuthLineLen = 0;
      sgAppAuthLine[0] = '\0';
    }
  }
}

static bool sgAppAuthGateOpen(uint32_t now) {
#if SG_APP_AUTH_REMEMBER_TRUSTED
  // ใส่ PIN ถูกครั้งแรกแล้วจำไว้ใน NVS: รีบูต/BT หลุดแล้วไม่ต้องใส่ซ้ำ
  if (sgAppAuthTrusted) {
    sgAppAuthOk = true;
    return true;
  }
#endif
  sgAppAuthProcessInput();
  if (sgAppAuthOk) return true;

  if ((uint32_t)(now - sgAppLastAuthPromptMs) >= SG_APP_AUTH_PROMPT_MS) {
    sgAppLastAuthPromptMs = now;
    SG_BT.println("{\"type\":\"auth\",\"status\":\"required\",\"pin_len\":4,\"remember\":true}");
  }
  return false;
}
#endif

void sgAppLiveTask() {
#if SG_APP_LIVE_ENABLE && SG_BT_MONITOR_ENABLE
  // Bluetooth จะเริ่มเมื่อระบบเดิมเปิด sgBtReady สำเร็จเท่านั้น
  if (!sgBtReady) return;
  if (!SG_BT.hasClient()) {
#if SG_APP_AUTH_ENABLE
    // BETA8 Trusted: Bluetooth หลุดแล้วต่อใหม่ไม่ต้องใส่ PIN ซ้ำ
    // และถ้าเคยใส่ถูกแล้ว จะจำ trusted ใน NVS แม้ปิด/เปิดใหม่
    sgAppAuthLineLen = 0;
    sgAppAuthLine[0] = '\0';
#endif
    return;
  }

  const uint32_t now = millis();
#if SG_APP_AUTH_ENABLE
  if (!sgAppAuthGateOpen(now)) return;
#endif

  if ((uint32_t)(now - sgAppLastSendMs) < SG_APP_LIVE_PERIOD_MS) return;
  sgAppLastSendMs = now;

  char modelSafe[48];
  sgAppJsonSafeModel(modelSafe, sizeof(modelSafe), sgModelName);

  // v2.2.1: เพิ่ม STFT/LTFT สำหรับแอพ โดยใช้ค่าที่ Decode แล้วเท่านั้น
  // - stft/ltft ส่งเป็นเปอร์เซ็นต์จริง ไม่ใส่เครื่องหมาย % ใน JSON
  // - *_ok ช่วยให้แอพรู้ว่าควรแสดงค่า หรือซ่อน/ขึ้น -- เมื่อ ECU ยังไม่ตอบตารางนั้น
  // - ไม่อ่าน ECU เพิ่ม และไม่แตะสูตร Decode/Offset เดิม
  const bool stftOk = sgGauge.ecuConnected && sgGauge.t20Ok;
  const bool ltftOk = sgGauge.ecuConnected && sgGauge.ltftValid;

  // 320 byte เผื่อ field stft/ltft และ *_ok โดยไม่ใช้ String ต่อข้อความ
  char packet[320];
  const int written = snprintf(
    packet,
    sizeof(packet),
    "{\"type\":\"live\",\"seq\":%lu,\"rpm\":%u,\"afr\":%.2f,\"ect\":%d,\"tps\":%.1f,\"bat\":%.2f,\"ign\":%.1f,\"inj\":%.2f,\"stft\":%.1f,\"ltft\":%.1f,\"stft_ok\":%s,\"ltft_ok\":%s,\"ecu\":\"%s\",\"model\":\"%s\"}",
    (unsigned long)(++sgAppSeq),
    (unsigned int)sgGauge.rpm,
    (double)sgGauge.afr,
    (int)(sgVehicleDataV1.mainPublished ? sgVehicleDataV1.coolantC : sgGauge.tempC),
    (double)(sgVehicleDataV1.mainPublished ? sgVehicleDataV1.throttlePositionPct : sgGauge.tps),
    (double)(sgVehicleDataV1.mainPublished ? sgVehicleDataV1.batteryV : sgGauge.batteryV),
    (double)(sgVehicleDataV1.mainPublished ? sgVehicleDataV1.ignitionDeg : sgGauge.ignitionDeg),
    (double)(sgVehicleDataV1.mainPublished ? sgVehicleDataV1.injectorMs : sgGauge.injectorMs),
    (double)sgGauge.stftPct,
    (double)sgGauge.ltftPct,
    stftOk ? "true" : "false",
    ltftOk ? "true" : "false",
    sgGauge.ecuConnected ? "ON" : "OFF",
    modelSafe
  );

  // ส่งเฉพาะ packet ที่สร้างครบ ไม่ส่ง JSON ตัดกลาง
  if (written > 0 && written < (int)sizeof(packet)) {
    SG_BT.println(packet);  // 1 packet ต่อ 1 บรรทัด จบด้วย CRLF/newline
  }
#endif
}
