#pragma once
/*
  Wi-Fi OTA removed from the main SUN DIAG GAUGE build to reduce Flash usage.
  Compatibility stubs remain because the existing input/UI modules still
  reference these function names. No WiFi/ArduinoOTA/Update code is linked.
*/

bool sgOtaReady = false;
bool sgOtaApMode = false;
char sgOtaNetworkName[1] = "";

inline void sgOtaBeginService() {}
inline void sgOtaEnterMode() {
  sgUiShowNotice("OTA REMOVED", 1800);
}
inline void sgOtaExitMode() {}
inline void sgOtaTask() {}
