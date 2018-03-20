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
  boolean alarme_status;                          //VARIÁVEL PARA GUARDAR A SITUAÇÃO DO ALARME (ON OU OFF)
  byte tipo_funcao;                               //VARIÁVEL PARA INDICAR QUAL FUNÇÃO ATUAL ((0)MUDAR RELOGIO - (1)RELOGIO - (2)ATIVAR/DESATIVAR ALARME - (3)MUDAR ALARME)
};typedef struct relogio Relogio;

Relogio relogio_principal;
Relogio relogio_modificacao;

boolean flag_modificacao = true;                             //(false) MUDAR HORA - (true) MUDAR MINUTO
unsigned int tempo_key2 = 0;                                 //VARIAVEL PARA GUARDAR O TEMPO QUE A CHAVE 2 FOI PRESSIONADA


/*************************************************************/
/*********************FUNÇÕES********************************/
/*************************************************************/

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

void printRelogio(struct relogio r)
{
  Serial.print(r.hora);
  Serial.print(":");
  Serial.print(r.minutos);
  Serial.print(":");
  Serial.print(r.segundos);
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
  novo_relogio.alarme_status = false;        //INICIA RELOGIO COM ALARME DESLIGADO
  novo_relogio.tipo_funcao = 0;              //INICIA RELOGIO NA FUNÇÃO RELOGIO

  return novo_relogio;
}

void appinit(void)
{//FUNÇÃO RESPONSÁVEL POR INDICAR QUAIS CHAVES QUEREM SER MONITORADAS E SETAR O TIMER
  button_listen(KEY1);
  button_listen(KEY2);
  button_listen(KEY3);
  timer_set(1000);

  relogio_principal = criarRelogio();  
}

void button_changed(int p, int v)
{//FUNÇÃO RESPONSÁVEL PELA AÇÃO CASO ALGUMA CHAVE SEJA ACIONADA
  if(p == KEY1 and v == HIGH)
  {
    relogio_principal.tipo_funcao++;
    if(relogio_principal.tipo_funcao == 4)
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
        relogio_principal = relogio_modificacao;
        relogio_principal.tipo_funcao == 1;
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

void timer_expired(void)
{//FUNCAO RESPONSÁVEL POR REALIZAR A AÇÃO QUANDO O TIMER EXPIRAR
  if(relogio_principal.tipo_funcao == 1)
  {
    relogio_principal = logicaRelogioSoma(relogio_principal);
    Serial.print("Principal:\t");
    printRelogio(relogio_principal);
  }
  else if(relogio_principal.tipo_funcao == 0)
  {
    Serial.print("Modificacao:\t");
    printRelogio(relogio_modificacao);
  }
  
}
