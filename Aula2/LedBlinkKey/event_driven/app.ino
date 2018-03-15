#include "event_driven.h"
#include "app.h"
#include "pindefs.h"

/*************************************************************/
/*********************VARIÁVEIS********************************/
/*************************************************************/

//VARIÁVEL PARA GUARDAR O ESTADO ATUAL DOS LEDS
int leds_status[3] = {0,0,0};


/*************************************************************/
/*********************FUNÇÕES********************************/
/*************************************************************/

void appinit(void)
{ //FUNÇÃO RESPONSÁVEL POR INDICAR QUAIS CHAVES QUEREM SER MONITORADAS E SETAR O TIMER  
  button_listen(KEY1);
  button_listen(KEY2);
  button_listen(KEY3);
  
}

void button_changed(int p, int v)
{//FUNÇÃO RESPONSÁVEL PELA AÇÃO CASO ALGUMA CHAVE SEJA ACIONADA
  //LOOP QUE VERIFICA QUAL CHAVE FOI ALTERADA E PARA QUAL ESTADO, ALTERANDO O ESTADO DO LED CORRESPONDENTE
  for(int i=0; i<3; i++)
  {
    if(p == keys[i])
    {
      leds_status[i] = !leds_status[i];                               //TROCA O ESTADO DO LED
      digitalWrite(leds[i], leds_status[i]);                          //ATUALIZA O ESTADO DO LED
    }
  }
}

void timer_expired(void)
{//FUNCAO RESPONSÁVEL POR REALIZAR A AÇÃO QUANDO O TIMER EXPIRAR
}
