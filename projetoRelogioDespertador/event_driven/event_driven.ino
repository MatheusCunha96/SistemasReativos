#include "event_driven.h"
#include "projetoRelogioDespertador.h"
#include "pindefs.h"

/*************************************************************/
/*********************VARIÁVEIS*******************************/
/*************************************************************/

//VETOR PARA SABER QUAIS CHAVES TEMOS INTERESSE EM MONITORAR
static bool keys_interest[3] = {false, false, false};

//VETOR PARA ACESSAR AS CHAVES
static int keys[3] = {KEY1, KEY2, KEY3};

//VETOR CONTENDO O STATUS ATUAL DAS CHAVES
static int keys_status[3] = {0,0,0};

//VETOR CONTENDO O ESTADO DAS CHAVES NA ÚLTIMA ITERAÇÃO
static int old_keys_status[3] = {0,0,0};

//VETOR PARA ACESSAR OS LEDS
int leds[4] = {LED1, LED2, LED3, LED4};

//VARIAVEL QUE GUARDA O TEMPO DO TIMER
static int timer = 0;

//VARIAVEL PARA GUARDAR O DELTAT ENTRE AS VARIAVEIS tempo E tempo_antigo ABAIXO
static int delta = 0;
static unsigned long tempo = 0;
static unsigned long tempo_antigo = 0;


/*************************************************************/
/*********************FUNÇÕES********************************/
/*************************************************************/

void timer_set(int ms)
{//FUNÇÃO PARA SETAR O TEMPO DA VARIÁVEL TIMER
  timer = ms;
}

void button_listen(int pin)
{//FUNÇÃO PARA INDICAR SE TEM INTERESSE NA CHAVE QUE ESTA NO PINO pin
  for(int i=0; i<3; i++)
  {
    if(pin == keys[i])
      keys_interest[i] = true;
  }
}

/*************************************************************/
/*********************SETUP***********************************/
/*************************************************************/

void setup() {

  Serial.begin(9600);

  //LOOP DE INICIALIZAÇÃO DOS LEDS E CHAVES
  for(int p=0; p<3; p++)
    pinMode(keys[p], INPUT_PULLUP);

  for(int p=0; p<4; p++)
  {
    pinMode(leds[p], OUTPUT);
    digitalWrite(leds[p], HIGH);
  }
  
  pinMode(LATCH_DIO, OUTPUT);
  pinMode(CLK_DIO, OUTPUT);
  pinMode(DATA_DIO, OUTPUT);

  pinMode(BUZZ, OUTPUT);
  digitalWrite(BUZZ, HIGH);

  //CHAMADA DA FUNÇÃO QUE INDICA O QUE QUEREMOS MONITORAR
  appinit();

}

/*************************************************************/
/*********************LOOP PRINCIPAL**************************/
/*************************************************************/

void loop() {
  
  tempo = millis();                                 //TEMPO EM MILISSEGUNDOS
  delta = tempo - tempo_antigo;                     //VARIAÇÃO DE TEMPO
  
  if (delta >= timer)                               //CASO PASSE O VALOR DETERMINADO NA VARIÁVEL timer CHAMA A FUNÇÃO changeLed()
  {
    timer_expired();                                //CHAMA A FUNÇÃO QUE ACUSA QUE O TIMER EXPIROU
    tempo_antigo = tempo;                           //ATUALIZA O TEMPO ANTIGO RESPONSÁVEL PELO BLINK LED
  }
  
  for(int i=0; i<3; i++)                            //LOOP DE CHECAGEM DAS CHAVES
  {
    if (keys_interest[i] == true)                   //CHECA APENAS AS CHAVES QUE TEMOS INTERESSE
    {
      keys_status[i] = digitalRead(keys[i]);        //LE O ESTADO ATUAL DA CHAVE
      if (keys_status[i] != old_keys_status[i]){    //CASO O ESTADO TENHA MUDADO CHAMA A FUNÇÃO button_changed()
        button_changed(keys[i], keys_status[i]);    //CHAMA A FUNÇÃO QUE REALIZA O QUE O USUÁRIO DEFINIU
      }
      old_keys_status[i] = keys_status[i];          //ATUALIZA O VALOR ANTIGO DA CHAVE
    }
  }

}
