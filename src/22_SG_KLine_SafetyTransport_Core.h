#pragma once
// ================================================================
// HondaECU V330 SAFE CORE INTEGRATION
// นำมาเฉพาะ transport/diagnostic behavior ที่เหมาะกับ Smart Gauge:
// - echo separation, dynamic length, checksum, timeout/retry
// - session state, detailed command result, raw TX/RX logger
// - ห้าม Flash/EEPROM/RAM/Security/Programming commands
// ================================================================

enum SgV330Session : uint8_t {
  SG_V330_SESSION_DISCONNECTED = 0,
  SG_V330_SESSION_INITIALIZING,
  SG_V330_SESSION_REALTIME,
  SG_V330_SESSION_DIAGNOSTIC,
  SG_V330_SESSION_CLEARING_DTC,
  SG_V330_SESSION_RECOVERING
};

enum SgV330Result : uint8_t {
  SG_V330_OK = 0,
  SG_V330_TIMEOUT,
  SG_V330_ECHO_ONLY,
  SG_V330_ECHO_MISMATCH,
  SG_V330_BAD_LENGTH,
  SG_V330_BAD_CHECKSUM,
  SG_V330_NEGATIVE_RESPONSE,
  SG_V330_BUFFER_OVERFLOW,
  SG_V330_BLOCKED_UNSAFE_COMMAND
};

struct SgV330Reply {
  SgV330Result result;
  uint8_t raw[64];
  uint8_t rawLen;
  uint8_t echoLen;
  const uint8_t *reply;
  uint8_t replyLen;
  const uint8_t *payload;
  uint8_t payloadLen;
  uint8_t attempts;
};

SgV330Session sgV330Session = SG_V330_SESSION_DISCONNECTED;
SgV330Result sgV330LastResult = SG_V330_TIMEOUT;
uint32_t sgV330CommandCount = 0;
uint32_t sgV330RetryCount = 0;
uint32_t sgV330BadLengthCount = 0;
uint32_t sgV330EchoErrorCount = 0;
uint32_t sgV330NegativeResponseCount = 0;
bool sgV330BusLocked = false;
uint32_t sgV330CommandStartedMs = 0;
uint32_t sgV330LastPacketHash = 0;
uint32_t sgV330LastPacketMs = 0;


uint8_t sgV330QualityPercent() {
  const uint32_t bad = sgRxTimeoutCount + sgChecksumErrorCount + sgV330BadLengthCount + sgV330EchoErrorCount + sgDuplicatePacketCount;
  const uint32_t total = sgRxOkCount + bad;
  if (total == 0) return 0;
  const uint32_t q = (sgRxOkCount * 100UL) / total;
  return (uint8_t)(q > 100 ? 100 : q);
}
uint32_t sgV330Hash(const uint8_t *d, uint8_t n){ uint32_t h=2166136261UL; for(uint8_t i=0;i<n;i++){h^=d[i];h*=16777619UL;} return h; }
void sgV330UartWatchdog(){ if(sgV330BusLocked && millis()-sgV330CommandStartedMs>SG_V330_UART_WATCHDOG_MS){ sgKLineFlushRx(); sgV330BusLocked=false; sgStalePacketCount++; } }

const char *sgV330ResultName(SgV330Result r) {
  switch (r) {
    case SG_V330_OK: return "OK";
    case SG_V330_TIMEOUT: return "TIMEOUT";
    case SG_V330_ECHO_ONLY: return "ECHO_ONLY";
    case SG_V330_ECHO_MISMATCH: return "ECHO_MISMATCH";
    case SG_V330_BAD_LENGTH: return "BAD_LENGTH";
    case SG_V330_BAD_CHECKSUM: return "BAD_CHECKSUM";
    case SG_V330_NEGATIVE_RESPONSE: return "NEGATIVE_RESPONSE";
    case SG_V330_BUFFER_OVERFLOW: return "BUFFER_OVERFLOW";
    case SG_V330_BLOCKED_UNSAFE_COMMAND: return "BLOCKED_UNSAFE_COMMAND";
    default: return "UNKNOWN";
  }
}

void sgV330SetSession(SgV330Session state) {
  if (sgV330Session == state) return;
  sgV330Session = state;
#if SG_DEBUG_ENABLE
  SG_PRINT("[V330 SESSION] "); SG_PRINTLN((int)state);
#endif
}

// Safety firewall: ชุดรวมนี้อนุญาตเฉพาะบริการอ่าน/วินิจฉัยที่ใช้ใน SUN DIAG GAUGE
// Safety firewall: อนุญาตเฉพาะบริการอ่าน/วินิจฉัยที่อนุญาตโดย SUN DIAG GAUGE
// ห้าม: Flash/EEPROM/RAM/Security/Programming/Write-to-EEPROM
//  - 0x72 command header (ISO9141 K-Line identifier)
//  - 0x73: Present/DTC current/aided (allowed: read-only)
//  - 0x74: Future DTC (allowed: read-only)
//  - 0x60 0x03: Clear DTC (allowed: clearing only)
//  - 0x71, 0x72, 0x00: Realtime data, ECU ID, session (allowed)
bool sgV330CommandAllowed(const uint8_t *tx, uint8_t txLen) {
  if (!tx || txLen < 4 || tx[0] != 0x72) return false;
  const uint8_t service = tx[2];
  if (service == 0x73 || service == 0x74) return true;          // Past/Current DTC
  if (service == 0x60 && tx[3] == 0x03) return true;            // Clear DTC only
  if (service == 0x71 || service == 0x72 || service == 0x00) return true; // realtime/ID/session
  return false;
}

void sgV330RawLog(const char *tag, const uint8_t *data, uint8_t len) {
#if SG_V330_RAW_LOG_ENABLE
  SG_PRINT("[V330 RAW] "); SG_PRINT(tag); SG_PRINT(" len="); SG_PRINT(len); SG_PRINT(" : ");
  for (uint8_t i = 0; i < len; ++i) {
    if (data[i] < 0x10) SG_PRINT('0');
    SG_PRINT_HEX(data[i]);
    if (i + 1 < len) SG_PRINT(' ');
  }
  SG_PRINTLN("");
#else
  (void)tag; (void)data; (void)len;
#endif
}

bool sgV330ChecksumValid(const uint8_t *data, uint8_t len) {
  if (!data || len < 2) return false;
  uint8_t sum = 0;
  for (uint8_t i = 0; i < len; ++i) sum = (uint8_t)(sum + data[i]);
  return sum == 0;
}

bool sgV330LooksNegative(const uint8_t *payload, uint8_t payloadLen) {
  if (!payload || payloadLen == 0) return false;
  // รองรับรูปแบบ negative response ที่พบบ่อย โดยไม่ตีความ payload DTC ปกติเป็น error
  return payload[0] == 0x7F || payload[0] == 0xFF;
}

SgV330Result sgV330ParseCaptured(const uint8_t *tx, uint8_t txLen,
                                 const uint8_t *raw, uint8_t rawLen,
                                 SgV330Reply &out) {
  out.rawLen = rawLen;
  out.echoLen = txLen;
  out.reply = nullptr;
  out.replyLen = 0;
  out.payload = nullptr;
  out.payloadLen = 0;
  if (rawLen == 0) return SG_V330_TIMEOUT;
  if (rawLen > sizeof(out.raw)) return SG_V330_BUFFER_OVERFLOW;
  memcpy(out.raw, raw, rawLen);
  if (rawLen < txLen) return SG_V330_ECHO_MISMATCH;
  if (memcmp(raw, tx, txLen) != 0) return SG_V330_ECHO_MISMATCH;
  if (rawLen == txLen) return SG_V330_ECHO_ONLY;

  const uint8_t *reply = out.raw + txLen;
  uint8_t replyLen = rawLen - txLen;
  out.reply = reply;
  out.replyLen = replyLen;

  if (replyLen < 4 || reply[0] != 0x02) return SG_V330_BAD_LENGTH;
  const uint8_t declaredLen = reply[1];
  if (declaredLen < 4 || declaredLen > replyLen) return SG_V330_BAD_LENGTH;
  // หากมี byte เกินจากเฟรม ให้ถือว่าเป็น noise/เฟรมซ้อน ไม่ decode
  if (declaredLen != replyLen) return SG_V330_BAD_LENGTH;
  if (!sgV330ChecksumValid(reply, replyLen)) return SG_V330_BAD_CHECKSUM;

  out.payload = reply + 2;
  out.payloadLen = replyLen - 3; // marker + length + checksum
  if (sgV330LooksNegative(out.payload, out.payloadLen)) return SG_V330_NEGATIVE_RESPONSE;
  return SG_V330_OK;
}

SgV330Result sgV330SendCommand(const uint8_t *tx, uint8_t txLen,
                               SgV330Reply &out, uint16_t timeoutMs,
                               const char *tag, uint8_t retries = SG_V330_RETRY_COUNT) {
  sgV330UartWatchdog();
  memset(&out, 0, sizeof(out));
  if (sgV330BusLocked) { out.result=SG_V330_TIMEOUT; return out.result; }
  sgV330BusLocked=true; sgV330CommandStartedMs=millis();
  out.result = SG_V330_TIMEOUT;
  if (!sgV330CommandAllowed(tx, txLen)) {
    out.result = SG_V330_BLOCKED_UNSAFE_COMMAND;
    sgV330LastResult = out.result;
    SG_PRINTLN("[V330 SAFETY] blocked command outside SmartGauge whitelist");
    sgV330BusLocked=false; return out.result;
  }

  for (uint8_t attempt = 0; attempt <= retries; ++attempt) {
    out.attempts = attempt + 1;
    sgV330CommandCount++;
    sgV330RawLog("TX", tx, txLen);
    uint8_t n = sgKLineSendCapture(tx, txLen, out.raw, sizeof(out.raw), timeoutMs, tag);
    sgV330RawLog("RX", out.raw, n);
    out.result = sgV330ParseCaptured(tx, txLen, out.raw, n, out);
    sgV330LastResult = out.result;

    if (out.result == SG_V330_OK) {
      const uint32_t h=sgV330Hash(out.reply,out.replyLen);
      if (h==sgV330LastPacketHash && millis()-sgV330LastPacketMs<SG_V330_DUPLICATE_WINDOW_MS) sgDuplicatePacketCount++;
      sgV330LastPacketHash=h; sgV330LastPacketMs=millis(); sgRxOkCount++;
    }
    if (out.result == SG_V330_OK || out.result == SG_V330_NEGATIVE_RESPONSE ||
        out.result == SG_V330_BLOCKED_UNSAFE_COMMAND) break;

    if (out.result == SG_V330_BAD_CHECKSUM) sgChecksumErrorCount++;
    else if (out.result == SG_V330_BAD_LENGTH) sgV330BadLengthCount++;
    else if (out.result == SG_V330_ECHO_MISMATCH || out.result == SG_V330_ECHO_ONLY) sgV330EchoErrorCount++;
    else if (out.result == SG_V330_TIMEOUT) sgRxTimeoutCount++;

    if (attempt < retries) {
      sgV330RetryCount++;
      sgKLineFlushRx();
      delay(SG_V330_RETRY_DELAY_MS);
    }
  }

  if (out.result == SG_V330_NEGATIVE_RESPONSE) sgV330NegativeResponseCount++;
  SG_PRINT("[V330 RESULT] "); SG_PRINTLN(sgV330ResultName(out.result));
  sgV330BusLocked=false;
  return out.result;
}
