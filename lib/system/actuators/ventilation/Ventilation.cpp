#include "Ventilation.h"

Lighting::Lighting(u8 pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
}

void Lighting::setup(u8 level) {

  this->level = level;

  if (level <= 0)
    digitalWrite(pin, LOW);
  else
    digitalWrite(pin, HIGH);
}

String Lighting::metrics() {
  return String("actuators/ventilation level=") + level;
}