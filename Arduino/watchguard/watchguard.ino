#define USE_ARDUINO_INTERRUPTS true

#include <SoftwareSerial.h>
#include <PulseSensorPlayground.h>
#include <LiquidCrystal_I2C.h>

// LCD variables
const uint8_t LCD_ID = 0x27;
const uint8_t LCD_ROWS = 2;
const uint8_t LCD_COLUMNS = 16;
LiquidCrystal_I2C lcd(LCD_ID, LCD_COLUMNS, LCD_ROWS);

// Pulse sensor variables
const int PULSE_SENSOR_ANALOG_PIN = A4;
const int PULSE_SENSOR_LED_PIN = 5;
int sensor_data;
PulseSensorPlayground pulse_sensor;
int print_bpm;

// Methane sensor
const int GAS_SENSOR_ANALOG_PIN = A0;
int print_gas;

// RF Module
SoftwareSerial mySerial(20, 21);

// String
void lcd_print(int bpm, int gas);
int slow_update = 0;

void setup() {
    const int BAUD_RATE = 9600;
    
    Serial.begin(BAUD_RATE);

    mySerial.begin(BAUD_RATE);

    // Setup pulse sensor
    pulse_sensor.analogInput(PULSE_SENSOR_ANALOG_PIN);
    pulse_sensor.blinkOnPulse(PULSE_SENSOR_LED_PIN);   // blink LED
    pulse_sensor.setThreshold(550); // prevents noise
    pulse_sensor.begin();
    
    // Setup lcd
    lcd.begin();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("bpm: ?");
    lcd.setCursor(0, 1);
    lcd.print("gas level: ?");
}


void loop() {
    int bpm = pulse_sensor.getBeatsPerMinute();
    int gas = analogRead(GAS_SENSOR_ANALOG_PIN);
    
    if (pulse_sensor.sawStartOfBeat())
    {
        print_bpm = bpm;
        lcd_print(print_bpm, print_gas);
    }

    print_gas = gas;
    if (slow_update == 200) {
        lcd_print(print_bpm, print_gas);
        slow_update = 0;
    }

    if (mySerial.available()) {
        lcd.setCursor(0, 1);
        lcd.print("received: " + String(mySerial.read()));
    }

    slow_update++;
    
    // Loop pause
    delay(20);
}


void lcd_print(int bpm, int gas)
{   
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("bpm: " + String(bpm) + "    ");
    lcd.setCursor(0, 1);
    if (mySerial.available()) {
        lcd.print(char(mySerial.read()));
    } else {
        lcd.print("gas level: " + String(gas) + "   ");
    }

    mySerial.write(bpm);
}
