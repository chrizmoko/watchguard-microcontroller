#ifndef LED_H
#define LED_H


class Led
{
public:
    /**
     * @brief Constructs a new LED at the pin number.
     * 
     * @param pin The digital pin number.
     */
    Led(int pin);

    /**
     * @brief Sets the LED to be powered (on) or unpowered (off).
     * 
     * @param power The LED will be powered on true, and unpowered on false.
     */
    void setPowered(bool power);

    /**
     * @brief Determines if the LED is powered (on) or not (off).
     * 
     * @return The state of the LED.
     */
    bool isPowered();

    
    /**
     * @brief Flips the powered state of the LED. If the LED is powered, then it
     * will become unpowered. If the LED is unpowered, then it will become
     * powered.
     * 
     */
    void flip();
    
private:
    int _pin;
    bool _is_powered;
};


#endif
