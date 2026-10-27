void sgUiDtcLoadingPump();
#pragma once
static const uint8_t SG_HONDAECU_DTC_FIRST_PAGE=0x01, SG_HONDAECU_DTC_LAST_PAGE=0x0B, SG_HONDAECU_DTC_CURRENT=0x74, SG_HONDAECU_DTC_PAST=0x73;
bool sgDtcEngineRunning(){return sgGauge.rpm>0;}
void sgDtcClearLines(){sgDtc.count=sgDtc.currentCount=sgDtc.pastCount=0;for(uint8_t i=0;i<12;i++)sgDtc.lines[i][0]='\0';}
void sgDtcSetEngineRunningLock(const char*a){sgDtc.readOk=sgDtc.clearOk=sgDtc.busy=false;sgDtcClearLines();snprintf(sgDtc.status,sizeof(sgDtc.status),"%s LOCKED: ENGINE RUNNING",a);sgDtc.lastActionMs=millis();}
void sgDtcBuildCommand(uint8_t service,uint8_t arg,uint8_t out[5]){out[0]=0x72;out[1]=0x05;out[2]=service;out[3]=arg;out[4]=sgChecksum(out,4);}
bool sgDtcIsEmptyPair(uint8_t m,uint8_t s){return(m==0&&s==0)||(m==0xFF&&s==0xFF);}
bool sgDtcCodeAlreadyAdded(uint8_t m,uint8_t s,bool cur){char k[14];snprintf(k,sizeof(k),"%c %02u-%02u",cur?'C':'P',m,s);for(uint8_t i=0;i<sgDtc.count;i++)if(strncmp(sgDtc.lines[i],k,7)==0)return true;return false;}
bool sgDtcAddV330Code(uint8_t dtcMain, uint8_t dtcSub, bool isCurrent) {
  if (sgDtcIsEmptyPair(dtcMain, dtcSub) || sgDtcCodeAlreadyAdded(dtcMain, dtcSub, isCurrent)) return false;
  if (isCurrent) sgDtc.currentCount++;
  else sgDtc.pastCount++;
  if (sgDtc.count >= 12) return true;

  const char statusCode = isCurrent ? 'C' : 'P';
  const SgDtcDefinition *d = sgDtcLookup(dtcMain, dtcSub, sgProfile ? sgProfile->id : SG_PROFILE_UNKNOWN);
  if (d) {
    const bool useTH = sgLanguageThai();
    const char *sensorName = useTH ? d->sensorTH : d->sensorEN;
    const char *faultDesc  = useTH ? d->faultTH  : d->faultEN;
    // Fallback ไป EN ถ้าข้อความไทยขาดหายใน DB
    if (!sensorName && useTH) sensorName = d->sensorEN;
    if (!faultDesc  && useTH) faultDesc  = d->faultEN;
    if (!sensorName) sensorName = "Unknown";
    if (!faultDesc)  faultDesc  = useTH ? "ไม่มีคำอธิบาย" : "UNKNOWN DTC";
    snprintf(sgDtc.lines[sgDtc.count], sizeof(sgDtc.lines[sgDtc.count]),
             "%c %02u-%02u %s %s", statusCode, dtcMain, dtcSub, sensorName, faultDesc);
  } else if (sgLanguageThai()) {
    snprintf(sgDtc.lines[sgDtc.count], sizeof(sgDtc.lines[sgDtc.count]),
             "%c %02u-%02u ไม่มีคำอธิบาย", statusCode, dtcMain, dtcSub);
  } else {
    snprintf(sgDtc.lines[sgDtc.count], sizeof(sgDtc.lines[sgDtc.count]),
             "%c %02u-%02u UNKNOWN DTC", statusCode, dtcMain, dtcSub);
  }
  sgDtc.count++;
  return true;
}
uint8_t sgDtcParseV330Page(const uint8_t*p,uint8_t n,bool cur){if(!p||n<3)return 0;uint8_t a=0;for(uint8_t pos=3;pos<=7;pos+=2)if(pos+1<n&&sgDtcAddV330Code(p[pos],p[pos+1],cur))a++;return a;}
void sgDtcScheduleRealtimeReconnect(){sgV330SetSession(SG_V330_SESSION_RECOVERING);sgGauge.ecuConnected=false;sgGauge.t17Ok=sgGauge.t20Ok=false;sgRuntimeProfileEnabled=false;sgEcuIdReadDone=false;sgPollStep=sgConsecutiveFail=0;sgReconnectPending=true;uint32_t backoff=SG_RECONNECT_PAUSE_MS<<min((uint8_t)4,sgReconnectBackoffLevel);if(backoff>SG_RECONNECT_BACKOFF_MAX_MS)backoff=SG_RECONNECT_BACKOFF_MAX_MS;sgReconnectNotBeforeMs=millis()+backoff;if(sgReconnectBackoffLevel<4)sgReconnectBackoffLevel++;sgReconnectCount++;sgLastConnectTryMs=millis();}
bool sgDtcPrepareDiagnosticSession(const char*t){snprintf(sgDtc.status,sizeof(sgDtc.status),"%s",t);sgGauge.ecuConnected=false;sgRuntimeProfileEnabled=false;sgKLineFlushRx();sgV330SetSession(SG_V330_SESSION_INITIALIZING);if(!sgKLineWakeup())return false;sgV330SetSession(SG_V330_SESSION_DIAGNOSTIC);sgKLineFlushRx();return true;}
bool sgDtcReadV330Group(uint8_t service,bool cur){bool got=false;for(uint8_t pg=1;pg<=0x0B;pg++){sgUiDtcLoadingPump();uint8_t cmd[5];sgDtcBuildCommand(service,pg,cmd);SgV330Reply r;auto x=sgV330SendCommand(cmd,5,r,SG_DTC_TIMEOUT_MS,cur?"[DTC CURRENT]":"[DTC PAST]");if(x!=SG_V330_OK){if(x==SG_V330_NEGATIVE_RESPONSE)break;continue;}got=true;sgDtcParseV330Page(r.payload,r.payloadLen,cur);if(r.payloadLen>2&&r.payload[2]==0)break;delay(SG_KLINE_GAP_MS);}return got;}
bool sgDtcReadInternal(bool prep,bool reconnect){if(prep&&!sgDtcPrepareDiagnosticSession("DTC WAKEUP...")){if(reconnect)sgDtcScheduleRealtimeReconnect();return false;}bool a=sgDtcReadV330Group(0x74,true);delay(SG_KLINE_GAP_MS);bool b=sgDtcReadV330Group(0x73,false);if(reconnect)sgDtcScheduleRealtimeReconnect();return a||b;}
bool sgDtcRead(){if(sgDtcEngineRunning()){sgDtcSetEngineRunningLock("READ");return false;}sgDtc.readOk=sgDtc.clearOk=false;sgDtc.busy=true;sgDtcClearLines();sgUiDtcLoadingPump();bool ok=sgDtcReadInternal(true,true);sgDtc.readOk=ok;sgDtc.busy=false;sgDtc.lastActionMs=millis();if(!ok)snprintf(sgDtc.status,32,"DTC %s",sgV330ResultName(sgV330LastResult));else snprintf(sgDtc.status,32,sgDtc.count?"CURRENT:%u PAST:%u":"DTC OK / NO CODE",sgDtc.currentCount,sgDtc.pastCount);return ok;}
const char* sgDtcClearSafetyReason(){
  if(sgDtcEngineRunning()) return "ENGINE RUNNING";
  if(sgGauge.speedKmh>0) return "VEHICLE MOVING";
  if(sgGauge.batteryV>0 && sgGauge.batteryV<SG_DTC_CLEAR_MIN_BATTERY_V) return "BATTERY LOW";
  if(!sgDtc.readOk) return "READ DTC FIRST";
  if(sgDtc.count==0) return "NO CODE";
  // เมื่ออ่าน DTC สำเร็จแล้ว ถือว่า session วินิจฉัยผ่านจริง
  // ไม่ใช้ค่า Quality สะสมจาก Realtime มาบล็อกการลบซ้ำอีกครั้ง
  if(SG_DTC_CLEAR_ONCE_PER_IGNITION && sgDtc.clearUsedThisIgnition) return "CLEAR LIMIT";
  return nullptr;
}
bool sgDtcVerifyClear(uint16_t waitMs){delay(waitMs);sgDtcClearLines();sgKLineFlushRx();sgV330SetSession(SG_V330_SESSION_DIAGNOSTIC);return sgDtcReadInternal(false,false)&&sgDtc.currentCount==0&&sgDtc.pastCount==0;}
bool sgDtcClear() {
#if !SG_ENABLE_DTC_CLEAR
  snprintf(sgDtc.status, sizeof(sgDtc.status), "CLEAR DISABLED");
  return false;
#else
  const char *reason = sgDtcClearSafetyReason();
  if (reason) { snprintf(sgDtc.status, sizeof(sgDtc.status), "CLEAR LOCK: %s", reason); return false; }
  sgDtc.busy = true;
  if (!sgDtcPrepareDiagnosticSession("CLEAR WAKEUP...")) { sgDtc.busy=false; sgDtcScheduleRealtimeReconnect(); return false; }
  uint8_t cmd[5]; sgDtcBuildCommand(0x60, 0x03, cmd); SgV330Reply r;
  const SgV330Result x = sgV330SendCommand(cmd, 5, r, SG_DTC_TIMEOUT_MS, "[CLEAR DTC]");
  const bool ack = x == SG_V330_OK && r.payloadLen > 1 && r.payload[1] == 0;
  const bool v1 = ack && sgDtcVerifyClear(SG_DTC_CLEAR_VERIFY_DELAY1_MS);
  const bool v2 = v1 && sgDtcVerifyClear(SG_DTC_CLEAR_VERIFY_DELAY2_MS);
  sgDtc.clearOk = v1 && v2; sgDtc.clearUsedThisIgnition = ack;
  snprintf(sgDtc.status, sizeof(sgDtc.status), sgDtc.clearOk ? "CLEAR VERIFIED 2X" : "CLEAR VERIFY FAILED");
  sgDtc.busy=false; sgDtc.lastActionMs=millis(); sgDtcScheduleRealtimeReconnect(); return sgDtc.clearOk;
#endif
}
