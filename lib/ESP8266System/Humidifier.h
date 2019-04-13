#ifndef Humidifier_h
#define Humidifier_h

#include <Arduino.h>

class Humidifier {
  public:
    Humidifier(u8 pin, u8 statePin, String metricPrefix);
    void setup(u8 level);
    u8 getLevel();
    String metrics();
  private:
    u8 pin;
    u8 statePin;
    String metricPrefix;
    void click();
};

#endif