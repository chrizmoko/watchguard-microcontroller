#include <PulseSensorPlayground.h>
#include <LiquidCrystal_I2C.h>

#include "led.h"

// LCD variables
const uint8_t LCD_ID = 0x27;
const uint8_t LCD_ROWS = 2;
const uint8_t LCD_COLUMNS = 16;
LiquidCrystal_I2C lcd(LCD_ID, LCD_COLUMNS, LCD_ROWS);

// Pulse sensor variables
const uint8_t PULSE_SENSOR_ANALOG_PIN = 4;
int sensor_data;
PulseSensorPlayground pulse_sensor;


const char* messages[] = {"Hello world!", "Today is nice.", "It's okay."};


void setup() {
    Serial.begin(9600);

    pulse_sensor.analogInput(PULSE_SENSOR_ANALOG_PIN);
    pulse_sensor.blinkOnPulse(4);
    pulse_sensor.begin();
    
    lcd.begin();
    
    // Turn on the backlight
    lcd.backlight();
    
    // Clear and print
    lcd.clear();
    lcd.print("Hello world!");
}


void loop() {
    int bpm = pulse_sensor.getBeatsPerMinute();

    if (pulse_sensor.sawStartOfBeat())
    {
        lcd.clear();
        lcd.print(String(bpm));
    }

    delay(20);
}
