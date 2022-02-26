#define USE_ARDUINO_INTERRUPTS true


#include <ULP.h>                        // For carbon monoxide sensor
#include <PulseSensorPlayground.h>      // For heart rate sensor
#include <LiquidCrystal_I2C.h>          // For LCD
#include <AceRoutine.h>                 // Coroutine management


// LCD variables
const uint8_t LCD_ID = 0x27;
const uint8_t LCD_ROWS = 2;
const uint8_t LCD_COLUMNS = 16;
LiquidCrystal_I2C lcd(LCD_ID, LCD_COLUMNS, LCD_ROWS);


// Pulse sensor variables
const int PULSE_SENSOR_ANALOG_PIN = A4;
const int PULSE_SENSOR_LED_PIN = 5;
PulseSensorPlayground pulseSensor;


// Methane sensor
const int GAS_SENSOR_ANALOG_PIN = A0;


// Carbon monoxide sensor (possibly not necessary)
const int CO_SENSOR_CARBON_ANALOG_PIN = A5;
const int CO_SENSOR_TEMPERATURE_ANALOG_PIN = A3;   // This pin is not used
const float CO_SENSOR_SENSITIVITY = 4.94f;
CO coSensor(
    CO_SENSOR_CARBON_ANALOG_PIN,
    CO_SENSOR_TEMPERATURE_ANALOG_PIN,
    CO_SENSOR_SENSITIVITY
);


// Printing to lcd display
void write_to_lcd(int line, String message);
void write_bpm_to_lcd();
void write_methane_to_lcd();
void write_carbon_to_lcd();
void write_gasses_to_lcd();


// Global variables for reading data
int global_bpm = 0;
int global_methane = 0;
float global_co = 0.0f;


/**
 * Collects the current data from the sensors and sends it through the serial
 * port and to the radio frequency (RF) module. The data sent to the RF module
 * will be formatted according to protocol described below:
 * 
 * <XXX[payload]>
 * 
 *     - Header "<"
 *     - Footer ">"
 *     - 3-digit length of entire message including headers and footer "XXX"
 *     - Payload of the message "[payload]"
 */
COROUTINE(writeToSerial1) {
    const unsigned int MILLISECOND_DELAY = 2000;

    static char messageBuffer[256];
    static char payloadBuffer[128];

    const char* BPM_KEY = "\"beats_per_minute\": ";
    const char* METHANE_KEY = "\"methane_level\": ";
    const char* CO_KEY = "\"co_level\": ";

    const int messageLengthIndex = 1;
    const int messageLengthLength = 3;

    COROUTINE_LOOP() {
        // JSON formatting
        sprintf(payloadBuffer, "{%s%d, %s%d, %s%d.%02d}",
            BPM_KEY, global_bpm,
            METHANE_KEY, global_methane,
            CO_KEY, (int)global_co, (int)(global_co * 100) % 100
        );

        // Protocol formatting
        int messageLength = 5 + strlen(payloadBuffer);
        sprintf(messageBuffer, "<%03d%s>",
            messageLength,
            payloadBuffer
        );

        // Send it!
        Serial1.write(messageBuffer);
        
        COROUTINE_DELAY(MILLISECOND_DELAY);
    }
}


/**
 * Displays to the top line of the lcd.
 */
COROUTINE(writeToLcdTop) {
    const unsigned int MILLISECOND_DELAY = 500;
    
    COROUTINE_LOOP() {
        write_bpm_to_lcd();
        COROUTINE_DELAY(MILLISECOND_DELAY);
    }
}


/**
 * Displays to the bottom line of the lcd.
 */
COROUTINE(writeToLcdBottom) {
    const unsigned int MILLISECOND_DELAY = 2000;

    const unsigned int DISPLAY_FUNCTION_LENGTH = 2;
    void(*displayFunctions[])() = {
        write_methane_to_lcd,
        write_carbon_to_lcd
    };
    
    static int index = 0;

    COROUTINE_LOOP() {
        displayFunctions[index]();

        index++;
        if (index == DISPLAY_FUNCTION_LENGTH) {
            index = 0;
        }

        COROUTINE_DELAY(MILLISECOND_DELAY);
    }
}


/**
 * Calls the appropriate API methods of sensors to get the appropriate data
 * from them. This coroutine is meant for sensors whose methods do not hold, or
 * execute in a fast manner.
 */
COROUTINE(collectFastSensorData) {
    const unsigned int MILLISECOND_DELAY = 50;

    COROUTINE_LOOP() {
        // Heart rate sensor
        global_bpm = pulseSensor.getBeatsPerMinute();

        // Methane sensor
        global_methane = analogRead(GAS_SENSOR_ANALOG_PIN);

        COROUTINE_DELAY(MILLISECOND_DELAY);
    }
}


/**
 * Calls the appropriate API methods of sensors to get the appropriate data
 * from them. This coroutine is meant for sensors whose methods may hold or
 * execute very slowly.
 */
COROUTINE(collectSlowSensorData) {
    const unsigned int MILLISECOND_DELAY = 50;

    COROUTINE_LOOP() {

        // Carbon sensor
//        coSensor.getIgas(1);    // Average over 1 s
//        global_co = coSensor.convertX('M'); // in ppm

        // I think this is in ppm already, can't tell with the datasheet
        // All I can collect is that it outputs a percentage out of 100ppm
        
        int temp = analogRead(CO_SENSOR_CARBON_ANALOG_PIN);
        global_co = (double)temp / 100.0;

        COROUTINE_DELAY(MILLISECOND_DELAY);
    }
}


void setup() {
    const int BAUD_RATE = 9600;
    
    Serial1.begin(BAUD_RATE);
    while (!Serial1);

    // Setup pulse sensor
    pulseSensor.analogInput(PULSE_SENSOR_ANALOG_PIN);
    pulseSensor.blinkOnPulse(PULSE_SENSOR_LED_PIN);   // blink LED
    pulseSensor.setThreshold(550); // prevents noise
    pulseSensor.begin();
    
    // Setup lcd
    lcd.begin();
    lcd.backlight();

    // Setup co (carbon monoxide) sensor
    coSensor.zero();
}


void loop() {
    // Sensor coroutines
    collectFastSensorData.runCoroutine();
    collectSlowSensorData.runCoroutine();

    // Output coroutines
    writeToSerial1.runCoroutine();
    writeToLcdTop.runCoroutine();
    writeToLcdBottom.runCoroutine();
}


void write_to_lcd(int line, String message) {
    if (line < 0 || line >= LCD_ROWS ||
        message.length() > LCD_COLUMNS) {
        return;
    }

    lcd.setCursor(0, line);
    for (int i = 0; i < LCD_COLUMNS; i++) {
        lcd.print(" ");
    }

    lcd.setCursor(0, line);
    lcd.print(message);
}


void write_bpm_to_lcd() {
    static int prevBpm = -1;

    // Prevent the very fast updates to the lcd
    if (global_bpm == prevBpm) {
        return;
    }
    
    String message = "bpm: " + String(global_bpm);
    write_to_lcd(0, message);
    prevBpm = global_bpm;
}


void write_methane_to_lcd() {
    String message = "methane: " + String(global_methane);
    write_to_lcd(1, message);
}


void write_carbon_to_lcd() {
    String message = "carbon: " + String(global_co);
    write_to_lcd(1, message);
}
