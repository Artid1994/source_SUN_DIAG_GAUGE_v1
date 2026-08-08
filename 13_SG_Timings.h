#pragma once
/*
  13_SG_Timings.h
  รวมค่า timing/timeout ทั้งโปรเจกต์ไว้ที่เดียว
  - ไม่เปลี่ยนลอจิกเดิม
  - macro เก่ายังคงเป็น alias เพื่อไม่ต้องแก้โค้ดกระจายหลายไฟล์
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
