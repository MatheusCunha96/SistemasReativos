#include "event_driven.h"
#include "app.h"
#include "pindefs.h"


static bool keys_interest[3] = {false, false, false};
static int keys[3] = {KEY1, KEY2, KEY3};
static int keys_status[3] = {0,0,0};
static int old_keys_status[3] = {0,0,0};
static int leds[3] = {LED1, LED2, LED3};

void timer_set(int ms)
{
  
}

void button_listen(int pin)
{
  if (pin == KEY1)
    keys_interest[0] = true;
  else if (pin == KEY2)
    keys_interest[1] = true;
  else
    keys_interest[2] = true;
}

void setup() {
  
  Serial.begin(9600);
  
  for(int p=0; p<3; p++)
  {
    pinMode(leds[p], OUTPUT);
    pinMode(keys[p], INPUT_PULLUP);
  }
    
  appinit();
  
}

void loop() {
  
  for(int i=0; i<3; i++)
  {
    if (keys_interest[i] == true)
    {
      keys_status[i] = digitalRead(keys[i]);
      if (keys_status[i] != old_keys_status[i])
        button_changed(leds[i], keys_status[i]);
      old_keys_status[i] = keys_status[i];
    }
  }

}
