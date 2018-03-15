#include "event_driven.h"
#include "app.h"
#include "pindefs.h"

/*************************************************************/
/*********************VARIÁVEIS********************************/
/*************************************************************/

//ESTADO ATUAL DO LED
int led_status = 0;

//TEMPO QUE AS CHAVES FORAM ACIONADAS
unsigned long time_key_1 = 0;
unsigned long time_key_2 = 0;


/*************************************************************/
/*********************FUNÇÕES********************************/
/*************************************************************/

void appinit(void)
{//FUNÇÃO RESPONSÁVEL POR INDICAR QUAIS CHAVES QUEREM SER MONITORADAS E SETAR O TIMER
  button_listen(KEY1);
  button_listen(KEY2);
  timer_set(1000);
  
}

void button_changed(int p, int v)
{//FUNÇÃO RESPONSÁVEL PELA AÇÃO CASO ALGUMA CHAVE SEJA ACIONADA
  //CASO O ESTADO ATUAL DA CHAVE QUE FOI ALTERADA SEJA HIGH EXECUTA O CÓDIGO ABAIXO
  if (v == HIGH){    
    if (p == KEY1)
    {//CASO SEJA A CHAVE 1 SALVA O TMEPO QUE ELA FOI APERTADA E AUMENTA A FREQUÊNCIA DO LED
      time_key_1 = millis();
      timer_set(timer/2);
    }
    else if (p == KEY2)
    {//CASO SEJA A CHAVE 2 SALVA O TMEPO QUE ELA FOI APERTADA E DIMINUI A FREQUÊNCIA DO LED
      time_key_2 = millis();
      timer_set(timer*2);
    }
    if (((time_key_2 - time_key_1 < 500) || (time_key_1 - time_key_2 < 500)) && (time_key_1 != 0))
    {//CASO O INTERVALO ENTRE O ACIONAMENTO DOS DOIS BOTOES SEJA MENOR QUE 500ms DESLIGA O LED
      digitalWrite(LED1, HIGH);
      while(1) {}
    }
  }
}

void timer_expired(void)
{//FUNCAO RESPONSÁVEL POR REALIZAR A AÇÃO QUANDO O TIMER EXPIRAR
  led_status = !led_status;                               //TROCA O ESTADO DO LED
  digitalWrite(LED1, led_status);                         //ATUALIZA O ESTADO DO LED  
}
