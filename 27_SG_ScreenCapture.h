#pragma once
/*
  27_SG_ScreenCapture.h
  Developer-only USB screenshot capture for UI work.
  - Uses the LVGL shadow framebuffer maintained in 07_SG_UI.h
  - Does not read or write ECU/K-Line data
  - Protocol command from USB Serial: SGSHOT
*/

#if SG_SCREEN_CAPTURE_ENABLE
static char sgScreenCaptureCmdBuf[8];
static uint8_t sgScreenCaptureCmdLen = 0;

static void sgScreenCaptureSendFrame() {
  Serial.write((const uint8_t *)"SGSHOT1", 7);
  const uint16_t w = 160;
  const uint16_t h = 120;
  const uint32_t frameId = sgScreenCaptureFrameId;
  Serial.write((const uint8_t *)&w, sizeof(w));
  Serial.write((const uint8_t *)&h, sizeof(h));
  Serial.write((const uint8_t *)&frameId, sizeof(frameId));

  for (uint16_t y = 0; y < h; ++y) {
    for (uint16_t x = 0; x < w; ++x) {
      const uint16_t px = tft.readPixel((int32_t)x * 2, (int32_t)y * 2);
      Serial.write((const uint8_t *)&px, sizeof(px));
    }
    delay(0);
  }
  Serial.write((const uint8_t *)"SGDONE", 6);
}

void sgScreenCaptureTask() {
  while (Serial.available() > 0) {
    const char ch = (char)Serial.read();
    if (ch == '\r' || ch == '\n') {
      sgScreenCaptureCmdBuf[sgScreenCaptureCmdLen] = '\0';
      if (strcmp(sgScreenCaptureCmdBuf, "SGSHOT") == 0) {
        sgScreenCaptureCmdLen = 0;
        sgScreenCaptureSendFrame();
        return;
      }
      sgScreenCaptureCmdLen = 0;
      continue;
    }

    if (sgScreenCaptureCmdLen < sizeof(sgScreenCaptureCmdBuf) - 1) {
      sgScreenCaptureCmdBuf[sgScreenCaptureCmdLen++] = ch;
    } else {
      sgScreenCaptureCmdLen = 0;
    }
  }
}
#else
void sgScreenCaptureTask() {}
#endif
