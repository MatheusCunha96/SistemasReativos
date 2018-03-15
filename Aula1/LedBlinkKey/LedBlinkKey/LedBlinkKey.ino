#include "pindefs.h"

/*************************************************************/
/*********************VARIÁVEIS********************************/
/*************************************************************/

//VARIÁVEL PARA GUARDAR AS CHAVES
int keys[3] = {KEY1, KEY2, KEY3};

//VARIÁVEL PARA GUARDAR O ESTADO DA CHAVE
int key_status[3];

//VARIÁVEL QUE GUARDA OS LEDS
int leds[3] = {LED1, LED2, LED3};

/*************************************************************/
/*********************SETUP***********************************/
/*************************************************************/

void setup() {
  //INICIALIZAÇÃO DA CHAVE E DO LED
  pinMode(KEY1, INPUT_PULLUP);
  pinMode(KEY1, INPUT_PULLUP);
  pinMode(KEY1, INPUT_PULLUP);

  for(int i=0; i<3; i++){
    pinMode(leds[i], OUTPUT);
    pinMode(keys[i], INPUT_PULLUP);  
  }

}

/*************************************************************/
/*********************LOOP PRINCIPAL**************************/
/*************************************************************/

void loop() {

  for(int i=0; i<3; i++){
    key_status[i] = digitalRead(keys[i]);                   //LE O ESTADO ATUAL DA CHAVE
    
    if(key_status[i] == LOW)                             //CASO O ESTADO SEJA LOW
      digitalWrite(leds[i], LOW);                        //ATUALIZA O ESTADO DO LED PARA LOW
    else                                              //CASO O ESTADO SEJA HIGH
      digitalWrite(leds[i], HIGH);                       //ATUALIZA O ESTADO DO LED PARA HIGH
  }

}
