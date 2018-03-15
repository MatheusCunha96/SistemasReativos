#include "pindefs.h"

/*************************************************************/
/*********************VARIÁVEIS********************************/
/*************************************************************/

//VARIÁVEL PARA GUARDAR O ESTADO DA CHAVE
int key_status;

/*************************************************************/
/*********************SETUP***********************************/
/*************************************************************/

void setup() {
  //INICIALIZAÇÃO DA CHAVE E DO LED
  pinMode(KEY1, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);

}

/*************************************************************/
/*********************LOOP PRINCIPAL**************************/
/*************************************************************/

void loop() {
  key_status = digitalRead(KEY1);                   //LE O ESTADO ATUAL DA CHAVE
  
  if(key_status == LOW)                             //CASO O ESTADO SEJA LOW
    digitalWrite(LED1, LOW);                        //ATUALIZA O ESTADO DO LED PARA LOW
  else                                              //CASO O ESTADO SEJA HIGH
    digitalWrite(LED1, HIGH);                       //ATUALIZA O ESTADO DO LED PARA HIGH

}
