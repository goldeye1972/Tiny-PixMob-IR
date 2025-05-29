
#include <Arduino.h>
#include "TinyPixMobIR.h"
#define CYCL (F_CPU / 2 / 38000L)
#define DUTY (CYCL / 3)
const uint8_t TinyPixMobIR::encode_table[64] = {
  0x21, 0x32, 0x54, 0x65, 0xa9, 0x9a, 0x6d, 0x29,
  0x56, 0x92, 0xa1, 0xb4, 0xb2, 0x84, 0x66, 0x2a,
  0x4c, 0x6a, 0xa6, 0x95, 0x62, 0x51, 0x42, 0x24,
  0x35, 0x46, 0x8a, 0xac, 0x8c, 0x6c, 0x2c, 0x4a,
  0x59, 0x86, 0xa4, 0xa2, 0x91, 0x64, 0x55, 0x44,
  0x22, 0x31, 0xb1, 0x52, 0x85, 0x96, 0xa5, 0x69,
  0x5a, 0x2d, 0x4d, 0x89, 0x45, 0x34, 0x61, 0x25,
  0x36, 0xad, 0x94, 0xaa, 0x8d, 0x49, 0x99, 0x26,
};
TinyPixMobIR::TinyPixMobIR(uint8_t pwmPin) : _pwmPin(pwmPin) {}
void TinyPixMobIR::begin() {
  pinMode(_pwmPin, OUTPUT);
  TCB0.CTRLA = TCB_ENABLE_bm | TCB_CLKSEL_CLKDIV2_gc;
  TCB0.CTRLB = TCB_CCMPEN_bm | TCB_CNTMODE_PWM8_gc;
  TCB0.CCMPL = CYCL;
  TCB0.CCMPH = 0;
}
bool TinyPixMobIR::sendGRB(uint8_t g, uint8_t r, uint8_t b, uint8_t ata, uint8_t sus, uint8_t res) {
  if (~0x3F & (g | r | b)) return false; // 6bit超過チェック
  uint8_t buff[8] = {}; // 初期化しておく
  uint8_t bytesize = 5;
  buff[1] = encode_table[0]; // 送信モード用固定値
  buff[2] = encode_table[g];
  buff[3] = encode_table[r];
  buff[4] = encode_table[b];
  if (ata | sus | res) {
    bytesize = 8;
    buff[5] = encode_table[(ata << 3)];
    buff[6] = encode_table[(res << 3) | sus];
    buff[7] = encode_table[0]; 
  }
  // チェックサム計算をsendBitsに移譲
  sendBits(buff, bytesize);
  return true;
}
bool TinyPixMobIR::sendMotion(uint8_t ata, uint8_t sus, uint8_t res) {
  uint8_t buff[5] = {}; // 初期化しておく
  uint8_t bytesize = 5;
  buff[1] = encode_table[0b00100011]; // 送信モード用固定値
  buff[2] = encode_table[0b00110000];
  buff[3] = encode_table[0b00000111|(ata << 3)];
  buff[4] = encode_table[(res << 3) | sus];
  // チェックサム計算をsendBitsに移譲
  sendBits(buff, bytesize);
  return true;
}
void TinyPixMobIR::sendBits(uint8_t *data, uint8_t leng) {
  // sendBits内でチェックサム計算を実行
  uint8_t sum = 0;
  for (uint8_t i = 1; i < leng; i++) {
    sum += data[i];
  }
  data[0] = encode_table[sum >> 2];
  
  TCB0.CCMPH = DUTY;
  delayMicroseconds(700);
  for (uint8_t i = 0; i < leng; i++) {
    for (uint8_t bit = 0; bit < 8; bit++) {
      if (data[i] & (1 << bit)) {
        TCB0.CCMPH = DUTY;
      } else {
        TCB0.CCMPH = 0;
      }
      delayMicroseconds(700);
    }
  }
  TCB0.CCMPH = 0;
}