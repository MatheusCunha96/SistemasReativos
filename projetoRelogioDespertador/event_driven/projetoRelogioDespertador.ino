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
  byte tipo_funcao;                               //VARIÁVEL PARA INDICAR QUAL FUNÇÃO ATUAL ((0)MUDAR RELOGIO - (1)RELOGIO - (2)SET ALARME)
};typedef struct relogio Relogio;

Relogio relogio_principal;
Relogio relogio_modificacao;

boolean flag_modificacao = true;                             //(false) MUDAR HORA - (true) MUDAR MINUTO
unsigned int tempo_key2 = 0;                                 //VARIAVEL PARA GUARDAR O TEMPO QUE A CHAVE 2 FOI PRESSIONADA

const byte SEGMENT_MAP[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0X80,0X90};   //BYTE MAP PARA NUMEROS DE 0 A 9
const byte SEGMENT_SELECT[] = {0xF1,0xF2,0xF4,0xF8};                              //BYTE MAP PARA SELECIONAR O DISPLAY

volatile int count_timer = 0;
int max_toque_beep = 20; // ESSA VARIAVEL DEFINE O TOTAL DE TEMPO QUE O DESPERTADOR IRA FICAR ATIVO
int qtd_toque_beep = 20; // ESSA EH UMA VARIAVEL QUE AUXILIA O TOTAL DE TEMPO QUE O DESPERTADOR FICA ATIVADO

boolean alarme_ativado = false;

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
  //ESSE IF ALTERA O ALARME DO RELOGIO
  if(relogio_principal.tipo_funcao == 2)
  {    
    if(tipo_modificacao == true)
    {//MUDA MINUTO
      r.minutos_alarme++;
      if(r.minutos_alarme == 60)
        r.minutos_alarme = 0;
    }
    else
    {//MUDA HORA
       r.hora_alarme++;
       if(r.hora_alarme == 24)
         r.hora_alarme = 0;
    }
  }
  else
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
  }
  return r;
}

void mostraRelogioDisplay(struct relogio r, boolean alarme)
{
  if(alarme == false)
  {
    EscreveNumeroNoDisplay(3, r.minutos%10);
    EscreveNumeroNoDisplay(2, r.minutos/10);
    EscreveNumeroNoDisplay(1, r.hora%10);
    EscreveNumeroNoDisplay(0, r.hora/10);
  }
  else
  {
    EscreveNumeroNoDisplay(3, r.minutos_alarme%10);
    EscreveNumeroNoDisplay(2, r.minutos_alarme/10);
    EscreveNumeroNoDisplay(1, r.hora_alarme%10);
    EscreveNumeroNoDisplay(0, r.hora_alarme/10);
  }
}


struct relogio criarRelogio()
{//FUNÇÃO PARA CRIAR UM RELOGIO INICIALIZANDO ELE COM OS VALORES DEFAULT
  Relogio novo_relogio;

  novo_relogio.hora = 0;                     //INICIA RELOGIO EM 0 HORAS              
  novo_relogio.minutos = 0;                  //INICIA RELOGIO EM 0 MINUTOS
  novo_relogio.segundos = 1;                 //INICIA RELOGIO EM 0 SEGUNDOS
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
  timer_set(10);

  relogio_principal = criarRelogio();  
}

//BOTA O VALOR DO ALARME NO RELOGIO PRINCIPAL
void set_alarme()
{
  relogio_principal.hora_alarme = relogio_modificacao.hora_alarme;
  relogio_principal.minutos_alarme = relogio_modificacao.minutos_alarme;
}

void button_changed(int p, int v)
{//FUNÇÃO RESPONSÁVEL PELA AÇÃO CASO ALGUMA CHAVE SEJA ACIONADA
  
  if(p == KEY1 and v == HIGH)
  {//SE A CHAVE 1 É APERTADA, AO SOLTAR OCORRE ESSE IF
    
    relogio_principal.tipo_funcao++;                                                          //ANDA A FUNÇÃO QUE ESTA OCORRENDO
    
    if(relogio_principal.tipo_funcao == 3)                                                    //IF PARA VOLTAR PARA O INICIO DAS FUNÇÕES
      relogio_principal.tipo_funcao = 0;
      
    if(relogio_principal.tipo_funcao == 2)                                                    //AO SAIR DA FUNÇÃO RELÓGIO, O RELOGIO DE MODIFICACAO RECEBE O PRINCIPAL
      relogio_modificacao = relogio_principal;
  }
  
  if(p == KEY2 and v == HIGH) 
  {//SE A CHAVE 2 É APERTADA, AO SOLTAR OCORRE ESSE IF
    
    if(relogio_principal.tipo_funcao == 0)                                                    
    {//SE A FUNÇÃO ESTIVER NA ZERO (MODIFICAR O RELOGIO)
      
      flag_modificacao = !flag_modificacao;                                                   //INVERTE A FLAG DE MODIFICACAO
      
      if(millis() - tempo_key2 >= 2000)        
      {//SE OCORRER UM DELTA DE 2S ENTRE APERTAR E SOLTAR, ENTAO OCORRE ESSE IF
        buzzAviso();                                                                          //AVISO DO BUZZ
        relogio_modificacao.tipo_funcao = 1;                                                  //VOLTA PARA A FUNÇÃO UM (RELÓGIO)
        relogio_principal = relogio_modificacao;                                              //RELOGIO PRINCIPAL RECEBE O QUE FOI MODIFICADO
      }
      
    }

    else if(relogio_principal.tipo_funcao == 1)
    {//SE A FUNÇÃO ESTIVER EM UM (RELOGIO)
      relogio_principal.alarme_status = !relogio_principal.alarme_status;
      digitalWrite(LED2, !relogio_principal.alarme_status);      
    }
        
    else if(relogio_principal.tipo_funcao == 2)                                                    
    {//SE A FUNÇÃO ESTIVER NA DOIS (MODIFICA ALARME)
      
      flag_modificacao = !flag_modificacao;                                                   //INVERTE A FLAG DE MODIFICACAO
      
      if(millis() - tempo_key2 >= 2000)                                                       
      {//SE OCORRER UM DELTA DE 2S ENTRE APERTAR E SOLTAR, ENTAO OCORRE ESSE IF
        buzzAviso();                                                                          //AVISO DO BUZZ
        relogio_principal.tipo_funcao = 1;                                                    //VOLTA PARA A FUNÇÃO UM (RELÓGIO)
        set_alarme();                                                                         //CHAMA A FUNÇÃO DE SETAR ALARME
      }
      
    }
  }
  
  if(p == KEY2 and v == LOW)
  {//SE A CHAVE 2 ESTA SOLTA E É APERTADA
    
    if(relogio_principal.tipo_funcao == 0 || relogio_principal.tipo_funcao == 2)
    {//SE A FUNÇÃO FOR ZERO OU DOIS (MODIFICAR RELOGIO OU ALARME)
      tempo_key2 = millis();                                                                  //PEGA O TEMPO QUE FOI APERTADA
    }
    
  }

  if(p == KEY3 and v == HIGH)
  {//SE A CHAVE 3 É APERTADA, AO SOLTAR OCORRE ESSE IF
    if(relogio_principal.tipo_funcao == 0)
    {//SE A FUNÇÃO FOR ZERO (MODIFICAR RELOGIO)
      relogio_modificacao = logicaModificacao(relogio_modificacao, flag_modificacao);         //RELOGIO MODIFICACAO RECEBE O QUE ESTA SENDO ALTERADO
    }
    else if(relogio_principal.tipo_funcao == 1)
    {//SE ESTIVER NO MODO RELÓGIO E O ALARME ESTIVER ATIVADO, DESLIGA O ALARME
      if(alarme_ativado == true)
      {
        alarme_ativado = false;
      }
    }
    else if(relogio_principal.tipo_funcao == 2)
    {//SE A FUNÇÃO FOR DOIS (MODIFICAR ALARME)
      relogio_modificacao = logicaModificacao(relogio_modificacao, flag_modificacao);         //RELOGIO MODIFICACAO RECEBE O QUE ESTA SENDO ALTERADO
    }
  }   
}

void toque_do_alame() //FUNCAO QUE LIGA O BEEP DO ALARME
{
  if(qtd_toque_beep < max_toque_beep)
  {
    buzzAviso();
    qtd_toque_beep++;  
  }
}

void despertar_alarme() //FUNCAO QUE DESPERTA O ALARME
{
  if((relogio_principal.hora == relogio_principal.hora_alarme && relogio_principal.minutos == relogio_principal.minutos_alarme && relogio_principal.segundos == relogio_principal.segundos_alarme))
    alarme_ativado = true;
  
  if(alarme_ativado == true && relogio_principal.alarme_status == true)
    digitalWrite(BUZZ, LOW);
  else
    digitalWrite(BUZZ, HIGH);
  //toque_do_alame();
}

void timer_expired(void)
{//FUNCAO RESPONSÁVEL POR REALIZAR A AÇÃO QUANDO O TIMER EXPIRAR
  count_timer++;

  despertar_alarme();

  //SE O LED3 ESTA LIGADO SIGNIFICA QUE ESTA MODIFICANDO O ALARME
  //Serial.print(relogio_principal.tipo_funcao);
 
  if(relogio_principal.tipo_funcao == 2)
    digitalWrite(LED3, LOW);  
  else
     digitalWrite(LED3, HIGH);

  if(count_timer >= 1)
  {
    relogio_principal = logicaRelogioSoma(relogio_principal); 
    count_timer = 0;
  }
   
  if(relogio_principal.tipo_funcao == 0)
    mostraRelogioDisplay(relogio_modificacao, false);
  else if(relogio_principal.tipo_funcao == 2)
    mostraRelogioDisplay(relogio_modificacao, true);
  else
    mostraRelogioDisplay(relogio_principal, false);  

  
  
}
