/*
  Basic example for LCDDisplay10 library, showcasing how to 
  display numbers and set the various segments.
*/ 

#include <LCDDisplay10.h>
#include <Wire.h>

LCDDisplay10 display;

void setup()
{
  Serial.begin(115200);
  display.begin();
  // TEST ALL SEGMENTS
  display.print("8.8.8.8.8.8.8.8.8.8.");
  display.setThousands(T_1 | T_3 | T_5 | T_7  |T_2 | T_4 | T_6);
  display.setMemory(true);
  display.setNegative(true);
  display.setError(true);
  display.sendBuffer();
  delay(4000);
  display.setBlink(display.BLINK_FAST);
  delay(4000);
  display.setBlink(display.BLINK_NORMAL);
  delay(4000);
  display.setBlink(display.BLINK_SLOW);
  delay(4000);
  display.setBlink(display.NO_BLINK);
}

void loop()
{
  static bool flag = true;
  display.clear();
  delay(1000);
  // Beware, print clears the flags.
  display.print("1234567890");
  display.setMemory(flag);
  display.setNegative(flag);
  display.setError(flag);
  display.setThousands(flag? (T_1 | T_3 | T_5 | T_7 ): (T_2 | T_4 | T_6));
  flag = !flag;
  for (int i = 0; i < 10; i++)
  {
    display.setPointPos(i);
    display.sendBuffer();
    delay(1000);
  }
}
