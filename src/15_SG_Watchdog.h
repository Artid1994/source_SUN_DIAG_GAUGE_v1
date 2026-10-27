#pragma once
/*
  15_SG_Watchdog.h
  Hardware Watchdog สำหรับป้องกัน loop/main task ค้าง
*/
#if SG_WATCHDOG_ENABLE
  #include <esp_task_wdt.h>
#endif

void sgWatchdogBegin() {
#if SG_WATCHDOG_ENABLE
  #if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    esp_task_wdt_config_t twdt_config = {};
    twdt_config.timeout_ms = SG_WATCHDOG_TIMEOUT_SEC * 1000;
    twdt_config.idle_core_mask = (1 << portNUM_PROCESSORS) - 1;
    twdt_config.trigger_panic = true;
    esp_task_wdt_init(&twdt_config);
  #else
    esp_task_wdt_init(SG_WATCHDOG_TIMEOUT_SEC, true);
  #endif
  esp_task_wdt_add(NULL);
  SG_PRINT("[WDT] enabled timeout="); SG_PRINT(SG_WATCHDOG_TIMEOUT_SEC); SG_PRINTLN("s");
#endif
}

void sgWatchdogReset() {
#if SG_WATCHDOG_ENABLE
  esp_task_wdt_reset();
#endif
}
