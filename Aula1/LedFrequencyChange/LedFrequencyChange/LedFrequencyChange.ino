#include "pindefs.h"


/*************************************************************/
/*********************VARIÁVEIS********************************/
/*************************************************************/

//VETOR PARA ACESSAR AS CHAVES
int keys[2] = {KEY1, KEY2};

//VETOR CONTENDO O STATUS ATUAL DAS CHAVES
int keys_status[2] = {};

//VETOR CONTENDO O ESTADO DAS CHAVES NA ÚLTIMA ITERAÇÃO
int old_keys_status[2] = {};

//TEMPO QUE AS CHAVES FORAM ACIONADAS
unsigned long time_key_1 = 0;
unsigned long time_key_2 = 0;

//VARIÁVEL QUE GUARDA O LED QUE ESTA SENDO USADO
int led = LED1;

//ESTADO ATUAL DO LED
int led_status = 0;

//VARIAVEL QUE GUARDA O TEMPO DE TROCA DO ESTADO DO LED
unsigned int timer = 1000;

//VARIAVEIS PARA CALCULAR O DELTAT QUE FAZ O LED PISCAR
unsigned int tempo;
unsigned int tempo_antigo = 1;

/*************************************************************/
/*********************FUNÇÕES********************************/
/*************************************************************/

void changeLed()
{//FUNCAO RESPONSÁVEL POR TROCAR O ESTADO DO LED E ATIVAR O ESTADO ATUAL
  led_status = !led_status;
  digitalWrite(led, led_status);
}

void changeFrequency(int key)
{//FUNCAO RESPONSAVEL PELA TROCA DE FREQUENCIA DO BLINK COM BASE NO BOTAO QUE FOI APERTADO
 //CASO OS BOTOES SEJAM ACIONADOS COM UM INTERVALO MENOR QUE 500ms O LED APAGA
  if(key == 0){
    //CASO SEJA A CHAVE 1 SALVA O TMEPO QUE ELA FOI APERTADA E AUMENTA A FREQUÊNCIA DO LED
    time_key_1 = millis();
    timer = timer/2;
  }
  else{
    //CASO SEJA A CHAVE 2 SALVA O TMEPO QUE ELA FOI APERTADA E DIMINUI A FREQUÊNCIA DO LED
    time_key_2 = millis();
    timer = timer*2;
  }
  if (((time_key_2 - time_key_1 < 500) || (time_key_1 - time_key_2 < 500)) && (time_key_1 != 0))
    {
      //CASO O INTERVALO ENTRE O ACIONAMENTO DOS DOIS BOTOES SEJA MENOR QUE 500ms DESLIGA O LED
      digitalWrite(LED1, HIGH);
      while(1){};
    }
}

/*************************************************************/
/*********************SETUP***********************************/
/*************************************************************/

void setup() {
  //INICIALIZA AS CHAVES COMO INPUT
  for(int i=0; i<2; i++)
    pinMode(keys[i], INPUT_PULLUP);

  //INICIALIZA O LED COMO OUTPUT
  pinMode(led, OUTPUT);
}

/*************************************************************/
/*********************LOOP PRINCIPAL**************************/
/*************************************************************/

void loop() {

  //LOOP PARA CHECAR SE AS CHAVES TROCARAM DE ESTADO E CASO POSITIVO REALIZAR A FUNCAO changeFrequency()
  for(int i=0; i<2; i++)
  {
    keys_status[i] = digitalRead(keys[i]);                                          //LE O ESTADO DA CHAVE i
    
    if (keys_status[i] != old_keys_status[i] && keys_status[i] == HIGH)             //CASO OCORRA UMA TROCA DE ESTADO E O ESTADO ATUAL SEJA HIGH CHAMA A FUNCAO changeFrequency()
      changeFrequency(i);
      
    old_keys_status[i] = keys_status[i];                                            //ATUALIZA O VALOR ANTIGO DA CHAVE
  }

  tempo = millis();                                                                 //TEMPO EM MILISSEGUNDOS


  if(tempo - tempo_antigo >= timer)                                                 //CASO PASSE O VALOR DETERMINADO NA VARIÁVEL timer CHAMA A FUNÇÃO changeLed()
  {
    changeLed();
    tempo_antigo = tempo;                                                           //ATUALIZA O TEMPO ANTIGO RESPONSÁVEL PELO BLINK LED
  }

  
  
  

}
