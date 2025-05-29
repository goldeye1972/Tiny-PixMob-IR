#ifndef TINYPIXMOBIR_H
#define TINYPIXMOBIR_H

enum TimeCode{
  TIME_0_MS   = 0x00, 
  TIME_32_MS  = 0x01, 
  TIME_96_MS  = 0x02, 
  TIME_192_MS = 0x03, 
  TIME_480_MS = 0x04, 
  TIME_960_MS = 0x05, 
  TIME_2400_MS = 0x06,
  TIME_3840_MS = 0x07,
};

class TinyPixMobIR {
  public:
    TinyPixMobIR(uint8_t pwmPin);
    void begin();
    bool sendGRB(uint8_t g, uint8_t r, uint8_t b, uint8_t ata = 0, uint8_t sus = 0, uint8_t res = 0);
    bool sendMotion(uint8_t ata , uint8_t sus , uint8_t res );
    void sendBits(uint8_t *data, uint8_t leng);

  private:
    uint8_t _pwmPin;
    static const uint8_t encode_table[64];
};

#endif
