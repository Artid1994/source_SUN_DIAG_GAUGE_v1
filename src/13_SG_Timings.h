#pragma once
/* 13_SG_Timings.h
   Project-wide timing/timeout configuration constants.
   K-Line wakeup and protocol timing (Sony Honda diagnostic gauge):
   - FE slow init wakeup: 70ms LOW (SG_ECU_WAKEUP_LOW_PULSE_MS=70) + 130ms HIGH
     (SG_ECU_WAKEUP_HIGH_HOLD_MS=130) = 200ms total K-Line wakeup strobe.
   - Wake RX timeout: 180ms per capture attempt (SG_KLINE_WAKEUP_RX_MS).
   - Frame gap: 12ms before next frame check (SG_KLINE_RX_RESPONSE_GAP_MS).
   - Inter-command gap: 18ms (SG_KLINE_INTER_CMD_GAP_MS).
   - Command response timeout: 130ms (SG_KLINE_CMD_RESPONSE_TIMEOUT).
   - ECU ID timeout: 180ms (SG_KLINE_ECU_ID_TIMEOUT_MS).
   - DTC timeout: 400ms (SG_KLINE_DTC_TIMEOUT_MS).

   ECU wake/reconnect timing:
   - Connect retry: 1000ms (SG_ECU_CONNECT_RETRY_MS).
   - Reconnect pause: 3000ms (SG_ECU_RECONNECT_PAUSE_MS).
   - Wake fail pause: 5000ms (SG_ECU_WAKEUP_FAIL_PAUSE_MS).
*/


// K-Line Communication Timing
#define SG_KLINE_RX_RESPONSE_GAP_MS     12
#define SG_KLINE_CMD_RESPONSE_TIMEOUT   130
#define SG_KLINE_AUX_RESPONSE_TIMEOUT   110
#define SG_KLINE_ECU_ID_TIMEOUT_MS      180
#define SG_KLINE_DTC_TIMEOUT_MS         400   // HondaECU V330 DTC command timeout
#define SG_KLINE_WAKEUP_RX_MS           180
#define SG_KLINE_INTER_CMD_GAP_MS       18

// ECU Wake / Reconnect Timing
#define SG_ECU_WAKEUP_LOW_PULSE_MS      70
#define SG_ECU_WAKEUP_HIGH_HOLD_MS      130
#define SG_ECU_CONNECT_RETRY_MS         1000
#define SG_ECU_RECONNECT_PAUSE_MS       3000
#define SG_ECU_WAKEUP_FAIL_PAUSE_MS     5000

// Boot / UI Timing
#define SG_BOOT_ANIMATION_DURATION_MS   3400
#define SG_UI_REFRESH_PERIOD_MS         12
#define SG_UI_DATA_REFRESH_PERIOD_MS    35
#define SG_UI_DTC_REFRESH_PERIOD_MS     250
#define SG_UI_DEBUG_PERIOD_MS           1000
#define SG_MONITOR_OUTPUT_PERIOD_MS     1000

// Backlight Timing
#define SG_BACKLIGHT_UPDATE_PERIOD_MS   350

// OTA Timing
#define SG_WIFI_CONNECT_TIMEOUT_MS      8000

// Backward compatible aliases ใช้ชื่อเดิมให้ compile ได้ทุกไฟล์
#define SG_RESPONSE_GAP_MS              SG_KLINE_RX_RESPONSE_GAP_MS
#define SG_T17_TIMEOUT_MS               SG_KLINE_CMD_RESPONSE_TIMEOUT
#define SG_T20_TIMEOUT_MS               SG_KLINE_AUX_RESPONSE_TIMEOUT
#define SG_ECU_ID_TIMEOUT_MS            SG_KLINE_ECU_ID_TIMEOUT_MS
#define SG_DTC_TIMEOUT_MS               SG_KLINE_DTC_TIMEOUT_MS
#define SG_WAKE_RX_MS                   SG_KLINE_WAKEUP_RX_MS
#define SG_KLINE_GAP_MS                 SG_KLINE_INTER_CMD_GAP_MS
#define SG_CONNECT_RETRY_MS             SG_ECU_CONNECT_RETRY_MS
#define SG_RECONNECT_PAUSE_MS           SG_ECU_RECONNECT_PAUSE_MS
#define SG_WAKE_FAIL_PAUSE_MS           SG_ECU_WAKEUP_FAIL_PAUSE_MS
#define SG_BOOT_ANIMATION_MS            SG_BOOT_ANIMATION_DURATION_MS
#define SG_UI_PERIOD_MS                 SG_UI_REFRESH_PERIOD_MS
#define SG_DATA_UI_PERIOD_MS            SG_UI_DATA_REFRESH_PERIOD_MS
#define SG_DEBUG_PERIOD_MS              SG_UI_DEBUG_PERIOD_MS
#define SG_MONITOR_DATA_PERIOD_MS       SG_MONITOR_OUTPUT_PERIOD_MS
#define SG_BL_UPDATE_MS                 SG_BACKLIGHT_UPDATE_PERIOD_MS
