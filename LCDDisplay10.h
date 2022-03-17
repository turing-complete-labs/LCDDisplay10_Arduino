/*
  LcdDisplay10.cpp - Library for interfacing with a 10 digit 7-segment display.
  Created by Enzo Lombardi, January 1, 2022.
  Released into public domain.
*/


#ifndef TEN_DIGIT_LCD_H
#define TEN_DIGIT_LCD_H


#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>

#define T_1 0b01000000
#define T_2 0b00100000
#define T_3 0b00010000
#define T_4 0b00001000
#define T_5 0b00000100
#define T_6 0b00000010
#define T_7 0b00000001

class LCDDisplay10
{
private:
     void fill(unsigned char ch);
     void fillDigits(unsigned char ch);
     uint8_t sendCommand(uint8_t command, uint8_t val);
     TwoWire *_wire = NULL;

public:

  enum blink_freq {
      NO_BLINK,
      BLINK_FAST,
      BLINK_NORMAL,
      BLINK_SLOW
    };

     void begin(TwoWire *theWire=&Wire);
     uint8_t writeMemory(uint8_t addr, uint8_t val);
     void clear();
     bool writeToBuffer(const char *characters);
     uint8_t sendBuffer();
     void reset();
     bool print(const char *characters);
     void setError(bool hasError);
     void setMemory(bool hasMemory);
     void setNegative(bool hasNegative);
     void setChar(uint8_t pos, char ch);
     void setPointPos(uint8_t pos);
     void setThousands(uint8_t num);
     void setBlink(blink_freq freq);
};

#endif
