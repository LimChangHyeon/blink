#include "Blink.h"
#include "Arduino.h"

Blink::Blink(int pin)
    : mPin(pin)
{
    pinMode(mPin, OUTPUT);
}

Blink::~Blink()
{

}

void Blink::on(unsigned int milliseconds)
{
    digitalWrite(mPin, LOW);
    delay(milliseconds);
}

void Blink::off(unsigned int milliseconds)
{
    digitalWrite(mPin, HIGH);
    delay(milliseconds);
}

