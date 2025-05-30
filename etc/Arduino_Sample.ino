#include <Arduino.h>
#include <IRremote.hpp>

// SET THIS TO THE DATA PIN USED FOR THE IR TRANSMITTER
const uint16_t kIrLed = 4;

// Encoding table definition (placed before use)
const uint8_t encode_table[] = {
  0x21, 0x32, 0x54, 0x65, 0xa9, 0x9a, 0x6d, 0x29,  // 0x00 - 0x07
  0x56, 0x92, 0xa1, 0xb4, 0xb2, 0x84, 0x66, 0x2a,  // 0x08 - 0x0F
  0x4c, 0x6a, 0xa6, 0x95, 0x62, 0x51, 0x42, 0x24,  // 0x10 - 0x17
  0x35, 0x46, 0x8a, 0xac, 0x8c, 0x6c, 0x2c, 0x4a,  // 0x18 - 0x1F
  0x59, 0x86, 0xa4, 0xa2, 0x91, 0x64, 0x55, 0x44,  // 0x20 - 0x27
  0x22, 0x31, 0xb1, 0x52, 0x85, 0x96, 0xa5, 0x69,  // 0x28 - 0x2F
  0x5a, 0x2d, 0x4d, 0x89, 0x45, 0x34, 0x61, 0x25,  // 0x30 - 0x37
  0x36, 0xad, 0x94, 0xaa, 0x8d, 0x49, 0x99, 0x26,  // 0x38 - 0x3F
};

void setup() {
  // Initialize IR sender with specified pin
  IrSender.begin(kIrLed);
  // Enable IR output at 38kHz carrier frequency
  IrSender.enableIROut(38);
}

void loop() {
  // Data array - first byte is checksum (initially 0x00)
  uint8_t newCode[] = {0x00, 0x00, 0x3F, 0x3F, 0x3F};
  
  // Checksum calculation and encoding process
  for(int i = 1; i < sizeof(newCode); i++){
    newCode[i] = encode_table[newCode[i]]; // Apply encoding transformation
    newCode[0] += newCode[i]; // Add to checksum
  }
  newCode[0] >>= 2; // Shift to 6-bit value
  newCode[0] = encode_table[newCode[0]]; // Encode checksum as well
  
  // Define maximum size for RAW data array with buffer
  uint8_t MAX_RAW_LENGTH = sizeof(newCode) * 8 + 10;
  uint16_t newRawData[MAX_RAW_LENGTH];
  uint8_t RAW_LENGTH = 0; // Actual length counter
  uint8_t lastBit = 1; // Start with HIGH state
  uint16_t bitCount = 1; // Count of consecutive identical bits
  
  // Process each byte with LSB-first bit transmission
  for (uint8_t i = 0; i < sizeof(newCode); i++) {
    for (uint8_t k = 0; k < 8; k++) {
      // Extract bit k from byte i (LSB first)
      uint8_t newBit = (newCode[i] >> k) & 1;
      
      if(lastBit == newBit){
        // Same bit as previous, increment count
        bitCount += 1;
      } else {
        // Bit changed, store duration and reset
        newRawData[RAW_LENGTH] = bitCount * 700; // Convert to microseconds
        RAW_LENGTH++;
        bitCount = 1; // Reset counter for new bit sequence
      }
      lastBit = newBit; // Update last bit state
    }
  }
    
  // Send RAW IR data at 38kHz carrier frequency
  IrSender.sendRaw(newRawData, RAW_LENGTH, 38);
  
  // Wait 10 seconds before next transmission
  delay(10000);
}
