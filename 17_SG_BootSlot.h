#pragma once
/*
  17_SG_BootSlot.h — SUN DIAG GAUGE ป้องกันอาการ ESP32 สลับไปบูต firmware เก่าที่ค้างอยู่ใน OTA partition
  หลังจาก firmware ใหม่นี้บูตสำเร็จ จะล็อก boot partition ปัจจุบันให้เป็นตัวหลัก
*/
#include <esp_ota_ops.h>

void sgBootSlotLockCurrent() {
  const esp_partition_t *running = esp_ota_get_running_partition();
  if (!running) return;
  esp_err_t err = esp_ota_set_boot_partition(running);
  if (err == ESP_OK) {
    SG_PRINT("[BOOT] locked current partition: ");
    SG_PRINTLN(running->label ? running->label : "unknown");
  } else {
    SG_PRINT("[BOOT] lock partition failed err=");
    SG_PRINTLN((int)err);
  }
}
