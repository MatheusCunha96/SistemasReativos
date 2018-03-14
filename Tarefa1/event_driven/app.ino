#include "event_driven.h"
#include "app.h"
#include "pindefs.h"

void appinit(void)
{ //Quais chaves tero interesse no inicio
  
  button_listen(KEY1);
  button_listen(KEY2);
  button_listen(KEY3);
  
}

void button_changed(int p, int v)
{
   digitalWrite(p, v);
}

void timer_expire(void)
{
}
