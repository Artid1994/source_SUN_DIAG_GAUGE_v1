#pragma once
/*
  SUN DIAG GAUGE v1.0.0.1 — Stable Model Name Only

  กติกาใหม่:
  - ใช้ ECU ID + Part Number เป็นตัวอ้างอิงร่วมกัน
  - Runtime display แสดงเฉพาะชื่อรุ่น เช่น LEAD125
  - ห้ามใช้ค่าปีแบบช่วงในข้อมูลแสดงผล
  - เก็บ year/yearStatus เพื่ออ้างอิงภายในเท่านั้น ห้ามนำปีไปแสดงบนจอหรือแอป
  - ไม่กระทบ K-Line, Command, Offset, Formula หรือ Decode Profile
*/

struct SgConfirmedDisplayIdentity {
  const char* ecuId;
  const char* partNumber;
  const char* modelName;
  const char* year;
  const char* displayName;
  bool modelConfirmed;
  bool yearConfirmed;
  const char* yearStatus;
  const char* source;
};

static const SgConfirmedDisplayIdentity SG_CONFIRMED_DISPLAY_DB[] = {
  {"0104780D01","30400-K2TA-T02","LEAD125","2022","LEAD125",true,true,"CONFIRMED","Part+ID/Web dataset"},
  {"0105950D01","30400-K2TG-T21","LEAD125","2025","LEAD125",true,true,"CONFIRMED","Part+ID listing"},
  {"01052A0D01","30400-K3MF-T01","GIORNO+125","2024","GIORNO+125",true,true,"CONFIRMED","Part+ID listing"},

  {"0101990D01","30400-K36F-T01","PCX150","","PCX150",true,false,"YEAR UNCONFIRMED","Part confirmed; one exact year not proven"},
  {"0102180D01","30400-K36S-T31","PCX150","","PCX150",true,false,"YEAR UNCONFIRMED","Part confirmed; one exact year not proven"},
  {"0102990D01","30400-K97F-T01","PCX150","2019","PCX150",true,true,"CONFIRMED","User image: PCX150 Thailand Smartkey 2019, K97F-T01, ECU ID 0102990D01"},
  {"0903CF0D01","30400-K1ZF-T61","PCX160 e:HEV","2021","PCX160 e:HEV",true,true,"CONFIRMED","Part+ID dataset"},
  {"0103CF0D01","30400-K1ZF-T11","PCX160","2021","PCX160",true,true,"CONFIRMED","Part+ID/FormMain"},
  {"0103D00D01","30400-K1ZF-T01","PCX160","2021","PCX160",true,true,"CONFIRMED","Part+ID/FormMain"},
  {"0105040D01","30400-K1ZP-T71","PCX160","2023","PCX160",true,true,"CONFIRMED","Part+ID/Web dataset"},
  {"0105050D01","30400-K1ZP-T51","PCX160","2023","PCX160",true,true,"CONFIRMED","Part+ID dataset"},
  {"0105B40D01","30400-K1ZT-TF1","PCX160","2025","PCX160",true,true,"CONFIRMED","Exact Part+ID listing"},
  {"0105B30D01","30400-K1ZT-TC1","PCX160","2025","PCX160",true,true,"CONFIRMED","Exact Part+ID listing"},
  {"01046D0D01","30400-K2SF-T01","CLICK160","2022","CLICK160",true,true,"CONFIRMED","Part+ID/vehicle database"},

  {"0100B20D01","38770-KWW-601","WAVE110i","","WAVE110i",true,false,"YEAR UNCONFIRMED","Part confirmed; exact year pending"},
  {"0100CE0D02","38770-KWW-643","WAVE110i","","WAVE110i",true,false,"YEAR UNCONFIRMED","Part confirmed; exact year pending"},
  {"0101610D01","38770-K03-T61","WAVE110i","2013","WAVE110i",true,true,"CONFIRMED","Part+ID dataset"},
  {"0102B90D01","38770-K58-T81","WAVE110i","","WAVE110i",true,false,"YEAR UNCONFIRMED","User image confirms only 2014-2015 range; single year not proven"},
  {"01034C0D01","38770-K58-TC1","WAVE110i","2019","WAVE110i",true,true,"CONFIRMED","Part+ID dataset"},
  {"01040F0D01","38770-K2J-T01","WAVE110i","2021","WAVE110i",true,true,"CONFIRMED","Part+ID/Thailand dataset"},
  {"0105550D01","38770-K2J-TH1","WAVE110i","2024","WAVE110i",true,true,"CONFIRMED","Part+ID dataset"},

  {"01033E0D01","38770-K73-T61","WAVE125i","2019","WAVE125i",true,true,"CONFIRMED","Part+ID dataset"},
  {"01034F0D01","38770-K73-T62","WAVE125i","2019","WAVE125i",true,true,"CONFIRMED","Part+ID/FormMain"},
  {"0102580D01","38770-K73-T32","WAVE125i","2016","WAVE125i",true,true,"CONFIRMED","Part+ID dataset"},
  {"0101B30D01","38770-KYZ-T41","WAVE125i","2016","WAVE125i",true,true,"CONFIRMED","Part+ID/FormMain"},
  {"01047B0D01","38770-K3F-T01","WAVE125i","2023","WAVE125i",true,true,"CONFIRMED","Part+ID dataset"},

  {"0101BA0D01","38770-KZV-A61","DREAM110","2014","DREAM110",true,true,"CONFIRMED","Part+ID/FormMain"},
  {"0102B50D01","38770-K76-T62","DREAM CUB110","2018","DREAM CUB110",true,true,"CONFIRMED","Part+ID/FormMain"},
  {"0103FA0D01","38770-K1M-T01","SUPER CUB110","2022","SUPER CUB110",true,true,"CONFIRMED","Part+ID dataset"},
  {"0105260D01","38770-K1M-T31","SUPER CUB110","2024","SUPER CUB110",true,true,"CONFIRMED","Part+ID dataset"},
};

static const size_t SG_CONFIRMED_DISPLAY_DB_COUNT =
  sizeof(SG_CONFIRMED_DISPLAY_DB) / sizeof(SG_CONFIRMED_DISPLAY_DB[0]);

static const SgConfirmedDisplayIdentity* sgFindConfirmedDisplayIdentity(const char* ecuId) {
  if (!ecuId || !ecuId[0]) return nullptr;
  for (size_t i = 0; i < SG_CONFIRMED_DISPLAY_DB_COUNT; ++i) {
    if (strcmp(SG_CONFIRMED_DISPLAY_DB[i].ecuId, ecuId) == 0)
      return &SG_CONFIRMED_DISPLAY_DB[i];
  }
  return nullptr;
}
