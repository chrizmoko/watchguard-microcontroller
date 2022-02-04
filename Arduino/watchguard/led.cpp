#include "Arduino.h"
#include "led.h"


Led::Led(int pin)
    : _pin(pin), _is_powered(false)
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}


void Led::setPowered(bool power)
{
    if (power == _is_powered)
    {
        return;
    }

    if (power)
    {
        digitalWrite(_pin, HIGH);
    }
    else
    {
        digitalWrite(_pin, LOW);
    }

    _is_powered = power;
}


bool Led::isPowered()
{
    return _is_powered;
}


void Led::flip()
{
    if (_is_powered)
    {
        digitalWrite(_pin, LOW);
    }
    else
    {
        digitalWrite(_pin, HIGH);
    }

    _is_powered = !_is_powered;
}