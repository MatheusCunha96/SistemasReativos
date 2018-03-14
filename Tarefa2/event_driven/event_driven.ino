#include "event_driven.h"
#include "app.h"
#include "pindefs.h"


static bool keys_interest[2] = {false, false};
static int keys[2] = {KEY1, KEY2};
static int keys_status[2] = {1,1};
static int old_keys_status[2] = {1,1};
static int led = LED1;
static int led_status = 0;

static int timer;
static int delta = 0;
static unsigned long tempo = 0;
static unsigned long tempo_antigo = 0;

static unsigned long time_key_1;
static unsigned long time_key_2;

static int flag_timer = 0;

void timer_set(int ms)
{
  timer = ms;
}

void button_listen(int pin)
{
  if (pin == KEY1)
    keys_interest[0] = true;
  else
    keys_interest[1] = true;
}

void setup() {
  
  Serial.begin(9600);
  
  for(int p=0; p<2; p++)
    pinMode(keys[p], INPUT_PULLUP);
  
  pinMode(led, OUTPUT);
   
  appinit();
  
}

void loop() {
  tempo = millis();
  delta = tempo - tempo_antigo;

  if (flag_timer == 1)
    digitalWrite(led, HIGH);
  
  if (delta >= timer && flag_timer == 0)
  {
    timer_expired();
    tempo_antigo = tempo;
  }

  for(int i=0; i<2; i++)
  {
    if (keys_interest[i] == true)
    {
      keys_status[i] = digitalRead(keys[i]);
      Serial.println(keys_status[i]);
      
      if (keys_status[i] == HIGH && old_keys_status[i] == LOW)
      {
        Serial.println(i);
        button_changed(i);
      } 
      old_keys_status[i] = keys_status[i];
    }
  }
  
  
  
  
  
}
