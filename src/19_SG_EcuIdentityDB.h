#pragma once
/*
  19_SG_EcuIdentityDB.h
  ฐานข้อมูลอ้างอิง Part Number + ECU ID + รุ่น/ปีเดียว ตามนโยบาย v1.3.2
  ใช้เพื่อ Monitor/Validation เท่านั้น ไม่ใช้เปิด Decode อัตโนมัติ
  เพราะเกจอ่านได้เฉพาะ ECU ID และบาง ID ซ้ำหลาย Part Number/หลายรุ่น
*/

// ECU Identity Database (K-Line ISO9141 Honda decoding reference table).
// Map: Part Number -> ECU ID (4-byte hex) -> model -> year -> verify status -> confidence.
//  -ecuId: 4-byte ECU identity code (e.g. "0101610D01")
//  -year: production year string; "YEAR UNCONFIRMED" if not yet verified
//  -confidence: confidence level for ID matching: สูง/กลาง/ต่ำ
//  NOTE:
//   - This is REFERENCE data for Monitor/Validation only.
//   - Do NOT use this table for automatic decode; some IDs map to multiple
//     Part Numbers/models.

struct SgEcuIdentityRecord {
  const char *partNumber;
  const char *ecuId;
  const char *model;
  const char *year;
  const char *verifyStatus;
  const char *confidence;
};

const SgEcuIdentityRecord sgEcuIdentityDb[] = {
  { "30400-K1ZF-T01", "0103D00D01", "PCX160", "2021", "แก้ปี", "สูง" },
  { "30400-K1ZF-T11", "0103CF0D01", "PCX160", "2021", "แก้ปี", "สูง" },
  { "30400-K1ZF-T61", "0903CF0D01", "PCX160 e:HEV / Hybrid", "2021", "แก้ปี", "สูง" },
  { "30400-K1ZP-T51", "0105050D01", "PCX160", "2023", "ขยายปี", "กลาง" },
  { "30400-K1ZP-T71", "0105040D01", "PCX160", "2023", "ขยายปี", "สูง" },
  { "30400-K1ZT-TC1", "0105B30D01", "PCX160", "2025", "ตรง", "สูง" },
  { "30400-K1ZT-TF1", "0105B40D01", "PCX160", "2025", "ตรง", "สูง" },
  { "30400-K2SF-T01", "01046D0D01", "Click160", "2022", "ขยายปี", "กลาง" },
  { "30400-K2TA-T02", "0104780D01", "Lead125", "2022", "ขยายปี", "กลาง" },
  { "30400-K2TG-T21", "0105950D01", "Lead125", "2025", "ตรง", "กลาง" },
  { "30400-K2VH-T01", "0101D60D02", "Click125", "2023", "แก้ปี", "สูง" },
  { "30400-K36F-T01", "0101990D01", "PCX150", "YEAR UNCONFIRMED", "แก้ปี", "สูง" },
  { "30400-K36S-T31", "0102180D01", "PCX150", "YEAR UNCONFIRMED", "แก้ปี", "สูง" },
  { "30400-K3MF-T01", "01052A0D01", "Giorno+ 125", "2024", "ปรับชื่อ", "กลาง" },
  { "30400-K59-T11", "0103070D01", "Click150i", "YEAR UNCONFIRMED", "ตรง/ปรับปลายปี", "สูง" },
  { "30400-K60F-T01", "0101D11D01", "Click125i", "YEAR UNCONFIRMED", "แก้ปี", "สูง" },
  { "30400-K60S-T71", "01030A0D01", "Click125i", "YEAR UNCONFIRMED", "แก้ปี", "กลาง" },
  { "30400-K60-T71", "01030A0D01", "Click125i LED", "YEAR UNCONFIRMED", "แก้ปี", "สูง" },
  { "30400-K97F-T01", "0102990D01", "PCX150", "YEAR UNCONFIRMED", "ขยายปี", "กลาง" },
  { "32160-K76-T61", "0101880D01", "Super Cub C125 (ชิ้นส่วนชุดสายไฟ ไม่ใช่ ECU)", "YEAR UNCONFIRMED", "ผิดประเภท Part", "สูง" },
  { "38770-K03-H01", "0101880D01", "Wave110i", "2019-2021", "ตรง", "สูง" },
  { "38770-K03-T61", "0101610D01", "Wave110i", "YEAR UNCONFIRMED", "รุ่นตรง/ปีต้องยืนยัน", "กลาง" },
  { "38770-K0G-902", "01030E0D01", "CT125", "YEAR UNCONFIRMED", "แก้รุ่น", "สูง" },
  { "38770-K0G-T22", "0104220D01", "Super Cub C125", "YEAR UNCONFIRMED", "ตรง", "กลาง" },
  { "38770-K1M-T01", "0103FA0D01", "Super Cub 110", "YEAR UNCONFIRMED", "ขยายปี", "กลาง" },
  { "38770-K1M-T31", "0105260D01", "Super Cub 110", "2024", "ตรง", "กลาง" },
  { "38770-K2J-T01", "01040F0D01", "Wave110i", "2021", "แก้ปี", "สูง" },
  { "38770-K2J-TH1", "0105550D01", "Wave110i", "2024", "ตรง", "กลาง" },
  { "38770-K3F-T01", "01047B0D01", "Wave125i", "2023", "ตรง", "กลาง" },
  { "38770-K58-T81", "0102B90D01", "Wave110i", "2018", "ตรง", "สูง" },
  { "38770-K58-TC0", "0102B90D01", "Wave110i", "2018", "ตรง/Part ต้องยืนยัน", "กลาง" },
  { "38770-K58-TC1", "01034C0D01", "Wave110i", "2019", "ตรง", "สูง" },
  { "38770-K73-T32", "0102580D01", "Wave125i", "2016", "แก้ปี", "กลาง" },
  { "38770-K73-T61", "01033E0D01", "Wave125i", "2019", "แก้ปี", "กลาง" },
  { "38770-K73-T62", "01034F0D01", "Wave125i", "2019", "แก้ปี", "สูง" },
  { "38770-K76-T62", "0102B50D01", "Dream Super Cub 110", "2018", "ปรับชื่อ", "สูง" },
  { "38770-KWW-601", "0100B20D01", "Wave110i", "YEAR UNCONFIRMED", "ขยายปี", "ต่ำ" },
  { "38770-KWW-643", "0100CE0D02", "Wave110i", "YEAR UNCONFIRMED", "ตรง", "สูง" },
  { "38770-KYZ-T03", "01010E0D01", "Wave125i / Future125 family", "YEAR UNCONFIRMED", "ปรับชื่อกำกับ", "สูง" },
  { "38770-KYZ-T41", "0101B30D01", "Wave125i", "2016", "ตรง", "สูง" },
  { "38770-KZV-A61", "0101BA0D01", "Dream110", "2014", "แก้รุ่น", "สูง" },
  { "38770-KZV-T03", "01010E0D01", "Super Cub 110 / NBC110", "YEAR UNCONFIRMED", "ปรับชื่อ", "สูง" },
};
const uint8_t SG_ECU_IDENTITY_COUNT = sizeof(sgEcuIdentityDb) / sizeof(sgEcuIdentityDb[0]);

uint8_t sgIdentityCountByEcuId(const char *ecuId) {
  if (!ecuId || !ecuId[0]) return 0;
  uint8_t count = 0;
  for (uint8_t i = 0; i < SG_ECU_IDENTITY_COUNT; ++i) {
    if (strcmp(sgEcuIdentityDb[i].ecuId, ecuId) == 0) count++;
  }
  return count;
}

const SgEcuIdentityRecord *sgIdentityFirstByEcuId(const char *ecuId) {
  if (!ecuId || !ecuId[0]) return nullptr;
  for (uint8_t i = 0; i < SG_ECU_IDENTITY_COUNT; ++i) {
    if (strcmp(sgEcuIdentityDb[i].ecuId, ecuId) == 0) return &sgEcuIdentityDb[i];
  }
  return nullptr;
}
