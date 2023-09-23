#ifndef MAX7456_H_
#define MAX7456_H_

#include <Arduino.h>
#include <SPI.h>
#include "max7456_registers.h"

class MAX7456 {
public:
  MAX7456(byte pinCS) {
    _pinCS = pinCS;
    _isActivatedOsd = false;

    _regVm1.whole = 0b01000111;
    pinMode(_pinCS, OUTPUT);
    digitalWrite(_pinCS, HIGH);  //disable device
    delay(100);                  //power up time

    digitalWrite(_pinCS, LOW);
    SPI.transfer(VM0_ADDRESS_WRITE);

    _regVm0.whole = 0x00;
    _regVm0.bits.videoSelect = 1;  //PAL
    _regVm0.bits.softwareResetBit = 1;
    SPI.transfer(_regVm0.whole);
    digitalWrite(_pinCS, HIGH);
    delay(500);


    digitalWrite(_pinCS, LOW);
    for (int x = 0; x < 16; x++) {
      _regRb[x].whole = 0x00;
      _regRb[x].bits.characterWhiteLevel = 2;
      SPI.transfer(x + RB0_ADDRESS_WRITE);
      SPI.transfer(_regRb[x].whole);
    }

    _regVm0.whole = 0x00;
    _regVm0.bits.verticalSynch = 1;
    SPI.transfer(VM0_ADDRESS_WRITE);
    SPI.transfer(_regVm0.whole);
    digitalWrite(_pinCS, HIGH);
  }

  void set_blink_params(byte blinkBase, byte blinkDC) {
    _regVm1.bits.blinkingTime = blinkBase;
    _regVm1.bits.blinkingDutyCycle = blinkDC;
    digitalWrite(_pinCS, LOW);
    SPI.transfer(VM1_ADDRESS_WRITE);
    SPI.transfer(_regVm1.whole);
    digitalWrite(_pinCS, HIGH);
  }

  void set_display_offsets(byte horizontal, byte vertical) {

    _regHos.whole = 0;
    _regVos.whole = 0;

    _regHos.bits.horizontalPositionOffset = horizontal;
    _regVos.bits.verticalPositionOffset = vertical;

    digitalWrite(_pinCS, LOW);
    SPI.transfer(HOS_ADDRESS_WRITE);
    SPI.transfer(_regHos.whole);

    SPI.transfer(VOS_ADDRESS_WRITE);
    SPI.transfer(_regVos.whole);

    digitalWrite(_pinCS, HIGH);
  }

  void activate_OSD(bool act) {
    if (_isActivatedOsd != act) {

      _regVm0.bits.videoSelect = 1;
      if (act)
        _regVm0.bits.enableOSD = 1;
      else
        _regVm0.bits.enableOSD = 0;

      digitalWrite(_pinCS, LOW);
      SPI.transfer(VM0_ADDRESS_WRITE);
      SPI.transfer(_regVm0.whole);
      digitalWrite(_pinCS, HIGH);
      _isActivatedOsd = act;
    }
  }

  void activate_external_video(bool activExtVid) {
    if (!activExtVid)
      _regVm0.bits.synchSelect = 3;
    else
      _regVm0.bits.synchSelect = 0;

    digitalWrite(_pinCS, LOW);
    SPI.transfer(VM0_ADDRESS_WRITE);
    SPI.transfer(_regVm0.whole);
    digitalWrite(_pinCS, HIGH);
  }

  void write_character(const charact chara, byte address) {
    activate_OSD(false);
    digitalWrite(_pinCS, LOW);
    SPI.transfer(CMAH_ADDRESS_WRITE);
    SPI.transfer(address);

    for (byte i = 0; i < 64; i++) {
      SPI.transfer(CMAL_ADDRESS_WRITE);
      SPI.transfer(i);
      SPI.transfer(CMDI_ADDRESS_WRITE);
      SPI.transfer(chara[i]);
    }

    _regCmm = 0xA0;
    SPI.transfer(CMM_ADDRESS_WRITE);
    SPI.transfer(_regCmm);
    _regStat.bits.characterMemoryStatus = 1;
    while (_regStat.bits.characterMemoryStatus == 1) {
      SPI.transfer(STAT_ADDRESS_READ);
      _regStat.whole = SPI.transfer(0x00);
    }

    digitalWrite(_pinCS, HIGH);
  }

  void printMax7456Char(const byte address, byte x, byte y, byte blink = 0, byte inv = 0) {
    byte ad = address;
    printMax7456Chars(&ad, 1, x, y, blink, inv);
  }

  void printMax7456Chars(byte chars[], byte size, byte x, byte y, byte blink, byte inv) {
    byte currentCharMax7456;
    byte posAddressLO;
    byte posAddressHI;
    unsigned int posAddress;

    posAddress = 30 * y + x;

    posAddressHI = posAddress >> 8;
    posAddressLO = posAddress;

    _regDmm.whole = 0x01;

    _regDmm.bits.INV = inv;
    _regDmm.bits.BLK = blink;

    digitalWrite(_pinCS, LOW);
    SPI.transfer(DMM_ADDRESS_WRITE);



    SPI.transfer(_regDmm.whole);


    SPI.transfer(DMAH_ADDRESS_WRITE);  // set start address high
    SPI.transfer(posAddressHI);

    SPI.transfer(DMAL_ADDRESS_WRITE);  // set start address low
    SPI.transfer(posAddressLO);


    for (int i = 0; i < size; i++) {
      currentCharMax7456 = chars[i];
      SPI.transfer(DMDI_ADDRESS_WRITE);
      SPI.transfer(currentCharMax7456);
    }

    //end character (we're done).
    SPI.transfer(DMDI_ADDRESS_WRITE);
    SPI.transfer(0xff);

    digitalWrite(_pinCS, HIGH);
  }

  void print(const char string[], byte x, byte y, byte blink, byte inv) {
    char currentChar;
    byte size;
    byte *chars = NULL;

    if (!string) return;

    size = 0;
    currentChar = string[0];

    while (currentChar != '\0') {
      currentChar = string[++size];
    }

    chars = (byte *)malloc(size * sizeof(byte));

    for (byte i = 0; i < size; i++) {
      chars[i] = string[i];
    }

    printMax7456Chars(chars, size, x, y, blink, inv);
    free(chars);
  }

  void clear() {
    _regDmm.bits.clearDisplayMemory = 1;

    digitalWrite(_pinCS, LOW);

    SPI.transfer(DMM_ADDRESS_WRITE);
    SPI.transfer(_regDmm.whole);

    while (_regDmm.bits.clearDisplayMemory == 1) {
      SPI.transfer(DMM_ADDRESS_READ);
      _regDmm.whole = SPI.transfer(0x00);
    }
    digitalWrite(_pinCS, HIGH);
  }

private:
  byte _pinCS;
  bool _isActivatedOsd;
  REG_VM0 _regVm0;
  REG_VM1 _regVm1;
  REG_HOS _regHos;
  REG_VOS _regVos;
  REG_DMM _regDmm;
  REG_DMAH _regDmah;  // not used yet
  REG_DMAL _regDmal;  // not used yet
  REG_DMDI _regDmdi;  // not used yet
  REG_CMM _regCmm;
  REG_CMAH _regCmah;    // not used yet
  REG_CMAL _regCmal;    // not used yet
  REG_CMDI _regCmdi;    // not used yet
  REG_OSDM _regOsdm;    // not used yet
  REG_RBN _regRb[16];   // not used yet
  REG_OSDBL _regOsdbl;  // not used yet
  REG_STAT _regStat;    // not used yet
  DMDO _regDmdo;        // not used yet
  REG_CMDO _regCmdo;    // not used yet
};

void get_character(const char *table, byte i, char *car) {
  unsigned long index;
  byte read;
  index = i * 64;
  for (unsigned long j = 0; j < 64; j++) {
    read = pgm_read_byte_near(table + index + j);
    car[j] = read;
  }
}

#endif