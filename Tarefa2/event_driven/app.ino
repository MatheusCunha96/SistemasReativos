#include "event_driven.h"
#include "app.h"
#include "pindefs.h"

void appinit(void)
{   
  button_listen(KEY1);
  button_listen(KEY2);
  timer_set(1000);
  
}

void button_changed(int p)
{
  if (p == 0)
  {//KEY1
    time_key_1 = millis();
    timer_set(timer/2);
  }
  else if (p == 1)
  {//KEY2
    time_key_2 = millis();
    timer_set(timer*2);
  }
  if abs(time_key_1 - time_key_2 < 500)
  {
    flag_timer = 1; 
  }
}

void timer_expired(void)
{
  led_status = !led_status;
  digitalWrite(led, led_status);
}
