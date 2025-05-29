#include "TinyPixMobIR.h"

TinyPixMobIR pixmob(PIN_PA6);

void setup() {
  pixmob.begin();
}

void loop() {
  // 赤色を送信 - TIME定数がオレンジ色で表示される
  pixmob.sendGRB(0, 63, 0, TIME_32_MS, TIME_96_MS, TIME_192_MS);
  delay(1000);
  
  // 緑色を送信
  pixmob.sendGRB(63, 0, 0, TIME_480_MS, TIME_960_MS, TIME_2400_MS);
  delay(3000);
  
  // 青色を送信
  pixmob.sendGRB(0, 0, 63, TIME_960_MS, TIME_0_MS, TIME_32_MS);
  delay(3000);
  
  // 白色を送信（時間パラメータなし）
  pixmob.sendGRB(63, 63, 63);
  delay(500);
  
  // モーションモード
  pixmob.sendMotion(TIME_32_MS, TIME_3840_MS, TIME_2400_MS);
  delay(30000);
}