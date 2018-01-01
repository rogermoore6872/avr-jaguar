#include <Arduino.h>

#define DEBUG

#define SIZEOF_ARRAY(a) (sizeof(a) / sizeof(a[0]))

const unsigned BUTTON_DEBOUCE=20;


class DigitalInput
{
public:
    DigitalInput(const char *name, byte pin, bool latch=false) :
        m_name(name), m_pin(pin), m_latch(latch)
    {
    }

    void setup(void)
    {
        m_state = false;
        m_read = false;
        m_read_t = 0;
        pinMode(m_pin, INPUT_PULLUP);
    }

    bool get_state(void) const { return m_state; }

    bool loop(bool debug=false)
    {
        bool last_state = m_state;
        bool s = digitalRead(m_pin) ? false : true;

        // TODO: debounce here

        if( m_latch )
        {
            if( s && !m_read )
                m_state = !m_state;
        }
        else
        {
            m_state = s;
        }

        if( debug && m_state != last_state ) print();
    
        m_read = s;
        return m_state;
    }

    void print(void)
    {
        Serial.print(m_name);
        Serial.print(": ");
        Serial.print(m_state ? "ON" : "OFF");
        Serial.print(" ");
        Serial.println();
    }
private:
    const char* m_name; // Name of button    
    const byte m_pin;   // Arduino digital input pin
    const bool m_latch; // When true, button state if latched until next press
  
    bool m_state;       // Current logical state of button
    bool m_read;        // State of last read
    unsigned m_read_t;  // Time of last read in milliseconds
};


class AnalogInput
{
public:
    AnalogInput(const char* name, byte pin) :
        m_name(name), m_pin(pin)
    {}

    void setup(void)
    {
        m_state = 0;
    }

    unsigned loop(bool debug=false)
    {
        int s = analogRead(m_pin);

        if( debug && abs(m_state-s) > 5) print();
        m_state = s;
        return m_state;
    }

    void print(void)
    {
        Serial.print(m_name);
        Serial.print(": ");
        Serial.print(m_state);
        Serial.print(" ");
        Serial.println();
    }

private:
    const char* m_name;
    const byte m_pin;

    int m_state;
};



// Joystick Shield v2.4 Arduino buttons
class JoystickShield
{
public:
    JoystickShield(void) :
        m_a("A", 2), m_b("B", 3), m_c("C", 4), m_d("D", 5), m_e("E", 6), m_f("F", 7), m_z("Z", 8),
        m_x("X", 0), m_y("Y", 1)
    {
        m_buttons[0] = &m_a;
        m_buttons[1] = &m_b;
        m_buttons[2] = &m_c;
        m_buttons[3] = &m_d;
        m_buttons[4] = &m_e;
        m_buttons[5] = &m_f;
        m_buttons[6] = &m_z;
        m_analogs[0] = &m_x;
        m_analogs[1] = &m_y;
    }

    void setup(void)
    {
        for( unsigned i=0; i<SIZEOF_ARRAY(m_buttons); i++ )
            m_buttons[i]->setup();
        for( unsigned i=0; i<SIZEOF_ARRAY(m_analogs); i++ )
            m_analogs[i]->setup();
    }

    void loop(bool debug=false)
    {
        for( unsigned i=0; i<SIZEOF_ARRAY(m_buttons); i++)
            m_buttons[i]->loop(debug);
        for( unsigned i=0; i<SIZEOF_ARRAY(m_analogs); i++ )
            m_analogs[i]->loop(debug);
    }

    DigitalInput m_a, m_b, m_c, m_d, m_e, m_f, m_z;
    AnalogInput m_x, m_y;

    DigitalInput* m_buttons[7];
    AnalogInput* m_analogs[2];
};



// A class for creating a 'bit banged' Pulse Width Modulated output using an
// Arduino digital output. This class does *not* use the built in PWM
// capabilities of the Arduino but instead uses the microsecond clock to
// determine when the signal should be high or low. This PWM is not accurate
// because of the timing impact of the main loop and interrupts.
class PWM
{
public:
    PWM(const char* name, byte pin, unsigned long period=-1, unsigned long on=0) :
        m_name(name), m_pin(pin)
    {
        set_period(period, on);
    }

    // Set the period and duty cycle
    void set_period(unsigned long period, unsigned long on=0)
    {   m_period = period;
        m_on = on;
    }
    
    void setup(void)
    {
        pinMode(m_pin, OUTPUT);
        m_start = 0;
        m_state = false;
        digitalWrite(m_pin, LOW);
    }

    // Run the PWM in the main system loop
    void loop(bool debug=false)
    {
        unsigned long us = micros();
        unsigned long t = us - m_start;

        if( m_state )
        {
            if( t >= m_on )
            {
                m_state = false;
                digitalWrite(m_pin, LOW);
                if( debug ) print();
            }
        }
        else if( t >= m_period)
        {
            m_state = true;
            m_start += m_period;
            digitalWrite(m_pin, HIGH);
            if( debug ) print();
        }
    }

    void print(void)
    {
        Serial.print(m_name);
        Serial.print(": ");
        Serial.print(m_state ? "HIGH " : "LOW ");
        Serial.print(m_start);
        Serial.print(" ");
        Serial.print(m_period);
        Serial.print(" ");
        Serial.print(m_on);
        Serial.print(" ");
        Serial.println();
    }

private:
    const char* m_name;     // A name
    const byte m_pin;       // Arduino IO pin number
    unsigned long m_period; // Period in microseconds
    unsigned long m_on;     // Period when on
    unsigned long m_start;  // Time in microseconds when period started
    bool m_state;           // Current state of PWM
};


class JaguarPWM : public PWM
{
public:
    JaguarPWM(const char *name, byte pin, JoystickShield* joystick) :
        PWM(name, pin), m_joystick(joystick),
//        m_reverse(670), m_neutral(1500), m_forward(2330)    // TODO: figure out what values work? values in use are for LED troubleshooting
        m_reverse(67000), m_neutral(150000), m_forward(233000)
    {
        set_period(m_neutral*2, m_neutral);
    }

    void loop(bool debug=false)
    {
        unsigned long pulse = m_neutral;
        if( m_joystick->m_b.get_state() )
            pulse = m_forward;
        else if( m_joystick->m_d.get_state() )
            pulse = m_reverse;

        set_period(pulse*2, pulse);
        PWM::loop();
    }


private:
    JoystickShield* const m_joystick;

    const unsigned long m_reverse;   // Full speed reverse pulse width in microseonds
    const unsigned long m_neutral;   // Neutral
    const unsigned long m_forward;   // Full speed forward

};



JoystickShield Joystick;
PWM SystemLED("System", 13, 1000000, 500000);
JaguarPWM Jaguar("JaguarPWM", 12, &Joystick);


void setup()
{
    Serial.begin(9600);
  
    Joystick.setup();
    SystemLED.setup();
    Jaguar.setup();
}

void loop()
{
   
    Joystick.loop(true);
    SystemLED.loop();
    Jaguar.loop();
}
