#pragma once

#ifndef SG_VEHICLE_DATA_VERIFY
#define SG_VEHICLE_DATA_VERIFY 0
#endif

struct SharedVehicleDataV1 {
  uint16_t rpmRaw;
  float throttlePositionPct;
  int16_t coolantC;
  float batteryV;
  float injectorMs;
  float ignitionDeg;
  uint32_t lastMainUpdateMs;
  bool mainPublished;
};

SharedVehicleDataV1 sgVehicleDataV1 = {};

void sgPublishVehicleMainSnapshot() {
  sgVehicleDataV1.rpmRaw = sgGauge.rpmRaw;
  sgVehicleDataV1.throttlePositionPct = sgGauge.tps;
  sgVehicleDataV1.coolantC = sgGauge.tempC;
  sgVehicleDataV1.batteryV = sgGauge.batteryV;
  sgVehicleDataV1.injectorMs = sgGauge.injectorMs;
  sgVehicleDataV1.ignitionDeg = sgGauge.ignitionDeg;
  sgVehicleDataV1.lastMainUpdateMs = millis();
  sgVehicleDataV1.mainPublished = true;
}

#if SG_VEHICLE_DATA_VERIFY
bool sgVehicleDataMainSnapshotEqualsGauge() {
  const float tolerance = 0.0001f;
  return sgVehicleDataV1.rpmRaw == sgGauge.rpmRaw &&
         (sgVehicleDataV1.throttlePositionPct - sgGauge.tps) <= tolerance &&
         (sgGauge.tps - sgVehicleDataV1.throttlePositionPct) <= tolerance &&
         sgVehicleDataV1.coolantC == sgGauge.tempC &&
         (sgVehicleDataV1.batteryV - sgGauge.batteryV) <= tolerance &&
         (sgGauge.batteryV - sgVehicleDataV1.batteryV) <= tolerance &&
         (sgVehicleDataV1.injectorMs - sgGauge.injectorMs) <= tolerance &&
         (sgGauge.injectorMs - sgVehicleDataV1.injectorMs) <= tolerance &&
         (sgVehicleDataV1.ignitionDeg - sgGauge.ignitionDeg) <= tolerance &&
         (sgGauge.ignitionDeg - sgVehicleDataV1.ignitionDeg) <= tolerance;
}
#endif
