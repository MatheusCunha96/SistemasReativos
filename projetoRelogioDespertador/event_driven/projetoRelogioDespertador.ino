#include "event_driven.h"
#include "projetoRelogioDespertador.h"
#include "pindefs.h"

/*************************************************************/
/*********************VARIÁVEIS********************************/
/*************************************************************/


struct relogio{
  byte hora;                                      //VARIÁVEL PARA GUARDAR A HORA
  byte minutos;                                   //VARIÁVEL PARA GUARDAR OS MINUTOS
  byte segundos;                                  //VARIÁVEL PARA GUARDAR OS SEGUNDOS
  byte hora_alarme;                               //VARIÁVEL PARA GUARDAR A HORA DO ALARME
  byte minutos_alarme;                            //VARIÁVEL PARA GUARDAR OS MINUTOS DO ALARME
  byte segundos_alarme;                           //VARIÁVEL PARA GUARDAR OS SEGUNDOS DO ALARME
  boolean alarme_status;                          //VARIÁVEL PARA GUARDAR A SITUAÇÃO DO ALARME (ON OU OFF)
  byte tipo_funcao;                               //VARIÁVEL PARA INDICAR QUAL FUNÇÃO ATUAL ((0)MUDAR RELOGIO - (1)RELOGIO)
};typedef struct relogio Relogio;

Relogio relogio_principal;
Relogio relogio_modificacao;

boolean flag_modificacao = true;                             //(false) MUDAR HORA - (true) MUDAR MINUTO
unsigned int tempo_key2 = 0;                                 //VARIAVEL PARA GUARDAR O TEMPO QUE A CHAVE 2 FOI PRESSIONADA

const byte SEGMENT_MAP[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0X80,0X90};   //BYTE MAP PARA NUMEROS DE 0 A 9
const byte SEGMENT_SELECT[] = {0xF1,0xF2,0xF4,0xF8};                              //BYTE MAP PARA SELECIONAR O DISPLAY

volatile int count_timer = 0;
/*************************************************************/
/*********************FUNÇÕES********************************/
/*************************************************************/

void EscreveNumeroNoDisplay(byte Segment, byte Value)
{
  digitalWrite(LATCH_DIO,LOW);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_MAP[Value]);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_SELECT[Segment] );
  digitalWrite(LATCH_DIO,HIGH);
}

void buzzAviso()
{
  digitalWrite(BUZZ, LOW);
  delay(40);
  digitalWrite(BUZZ, HIGH);
}

struct relogio logicaRelogioSoma(struct relogio r)
{
  r.segundos++;
  if(r.segundos == 60)
  {
    r.segundos = 0;
    r.minutos++;
    if(r.minutos == 60)
    {
      r.minutos = 0;
      r.hora++;
      if(r.hora == 24)
        r.hora = 0;
    }
  }

  return r;
}

struct relogio logicaModificacao(struct relogio r, boolean tipo_modificacao)
{
  if(tipo_modificacao == true)
  {//MUDA MINUTO
    r.minutos++;
    if(r.minutos == 60)
      r.minutos = 0;
  }
  else
  {//MUDA HORA
     r.hora++;
     if(r.hora == 24)
       r.hora = 0;
  }

  return r;
}

void mostraRelogioDisplay(struct relogio r)
{
  EscreveNumeroNoDisplay(3, r.minutos%10);
  EscreveNumeroNoDisplay(2, r.minutos/10);
  EscreveNumeroNoDisplay(1, r.hora%10);
  EscreveNumeroNoDisplay(0, r.hora/10);
}

void printRelogio(struct relogio r)
{
  Serial.print(r.hora);
  Serial.print(":");
  Serial.print(r.minutos);
  Serial.print(":");
  Serial.print(r.segundos);
  Serial.print("\t");
  Serial.print(r.hora_alarme);
  Serial.print(":");
  Serial.print(r.minutos_alarme);
  Serial.print(":");
  Serial.print(r.segundos_alarme);
  Serial.print("\t");
  Serial.print("Alarme status:");
  Serial.print(r.alarme_status);
  Serial.print("\t");
  Serial.print("Tipo funcao:");
  Serial.print(r.tipo_funcao);
  Serial.print("\t");
  Serial.print("Flag modificacao: ");
  Serial.println(flag_modificacao);
}

struct relogio criarRelogio()
{//FUNÇÃO PARA CRIAR UM RELOGIO INICIALIZANDO ELE COM OS VALORES DEFAULT
  Relogio novo_relogio;

  novo_relogio.hora = 0;                     //INICIA RELOGIO EM 0 HORAS              
  novo_relogio.minutos = 0;                  //INICIA RELOGIO EM 0 MINUTOS
  novo_relogio.segundos = 0;                 //INICIA RELOGIO EM 0 SEGUNDOS
  novo_relogio.hora_alarme = 0;              //INICIA ALARME EM 0 HORAS              
  novo_relogio.minutos_alarme = 0;           //INICIA ALARME EM 0 MINUTOS
  novo_relogio.segundos_alarme = 0;          //INICIA ALARME EM 0 SEGUNDOS
  novo_relogio.alarme_status = false;        //INICIA RELOGIO COM ALARME DESLIGADO
  novo_relogio.tipo_funcao = 0;              //INICIA RELOGIO NA FUNÇÃO RELOGIO

  return novo_relogio;
}

void appinit(void)
{//FUNÇÃO RESPONSÁVEL POR INDICAR QUAIS CHAVES QUEREM SER MONITORADAS E SETAR O TIMER
  button_listen(KEY1);
  button_listen(KEY2);
  button_listen(KEY3);
  timer_set(1);

  relogio_principal = criarRelogio();  
}

void button_changed(int p, int v)
{//FUNÇÃO RESPONSÁVEL PELA AÇÃO CASO ALGUMA CHAVE SEJA ACIONADA
  if(p == KEY1 and v == HIGH)
  {
    buzzAviso();
    relogio_principal.tipo_funcao++;
    if(relogio_principal.tipo_funcao == 2)
      relogio_principal.tipo_funcao = 0;
    if(relogio_principal.tipo_funcao == 0) relogio_modificacao = relogio_principal;
  }
  if(p == KEY2 and v == HIGH)
  {
    if(relogio_principal.tipo_funcao == 0)
    {
      flag_modificacao = !flag_modificacao;
      
      if(millis() - tempo_key2 >= 2000)
      {
        buzzAviso();
        delay(50);
        buzzAviso();
        relogio_modificacao.tipo_funcao == 1;
        relogio_principal = relogio_modificacao;
      }
    }
  }
  if(p == KEY2 and v == LOW)
    if(relogio_principal.tipo_funcao == 0)
      tempo_key2 = millis();

  if(p == KEY3 and v == HIGH)
  {
    if(relogio_principal.tipo_funcao == 0)
    {
      relogio_modificacao = logicaModificacao(relogio_modificacao, flag_modificacao);
    }
  } 
}

/*void timer_expired_display(void)
{
  if(relogio_principal.tipo_funcao == 0)
    mostraRelogioDisplay(relogio_modificacao);
  else
    mostraRelogioDisplay(relogio_principal);
}*/

void timer_expired(void)
{//FUNCAO RESPONSÁVEL POR REALIZAR A AÇÃO QUANDO O TIMER EXPIRAR
  count_timer++;

  if(relogio_principal.tipo_funcao == 0)
    mostraRelogioDisplay(relogio_modificacao);
  else
  {
    if(count_timer >= 1000)
    {
      relogio_principal = logicaRelogioSoma(relogio_principal);
      count_timer = 0;
    }
    mostraRelogioDisplay(relogio_principal);
  }
    

  
  
}
