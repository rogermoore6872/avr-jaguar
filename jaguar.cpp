#include <Arduino.h>

#if 0
void setup()
{
    Serial.begin(9600);
    pinMode(13, OUTPUT);
    digitalWrite(13,HIGH);
}

void loop()
{
    Serial.print(".");
    delay(1000);
    digitalWrite(13,LOW);
    delay(1000);
    digitalWrite(13,HIGH);
}

#endif


#if 0
int main(void)
{
    setup();

    while(1)
        loop();

    return 0;
}
#endif


#if 1

const byte PIN_ANALOG_X = 0;
const byte PIN_ANALOG_Y = 1;


// Select button is triggered when joystick is pressed
const byte PIN_BUTTON_SELECT = 2;

const byte PIN_BUTTON_RIGHT = 3;
const byte PIN_BUTTON_UP = 4;
const byte PIN_BUTTON_DOWN = 5;
const byte PIN_BUTTON_LEFT = 6;

void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_BUTTON_RIGHT, INPUT_PULLUP);  
  pinMode(PIN_BUTTON_LEFT, INPUT_PULLUP);  
  pinMode(PIN_BUTTON_UP, INPUT_PULLUP);  
  pinMode(PIN_BUTTON_DOWN, INPUT_PULLUP);  
}

int x=0;
int y=0;

int br, bl, bu, bd;


void print_int( const char * label, int i, boolean lf=false)
{
    Serial.print(label);
    Serial.print(": ");
    Serial.print(i);
    Serial.print(" ");
    if(lf)
    {  
      Serial.println();
    }
}


int check_button(const char * label, int pin, int state)
{
    int s=digitalRead(pin);
  
    if(s != state)
    {
      Serial.print(label);
      Serial.print(": ");
      Serial.print(s ? "OFF" : "ON");
      Serial.print(" ");
      Serial.println();
    }
    
    return s;
}
 

void loop() {

  int xx = analogRead(PIN_ANALOG_X);
  int yy = analogRead(PIN_ANALOG_Y);

  if( abs(x-xx) > 5 || abs(y-yy) > 5 )
  {
    x = xx;
    y = yy;
    print_int("x", x);
    print_int("y", y, true);

    Serial.println();
  }

  br = check_button("Right",PIN_BUTTON_RIGHT, br);
  bl = check_button("Left ",PIN_BUTTON_LEFT,  bl);
  bu = check_button("Up   ",PIN_BUTTON_UP,    bu);
  bd = check_button("Down ",PIN_BUTTON_DOWN,  bd);
}
#endif
