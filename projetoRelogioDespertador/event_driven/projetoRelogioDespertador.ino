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
  boolean alarme_ativado;                         //VARIAVEL QUE INDICA SE O ALARME ESTA ATIVADO OU NAO
};typedef struct relogio Relogio;

//VARIAVEL QUE CONTEM O RELOGIO PRINCIPAL DO PROGRAMA
Relogio relogio_principal;

//VARIAVEL QUE CONTEM O RELOGIO TEMPORARIO ONDE SÃO REALIZADAS AS MODIFICACOES
Relogio relogio_modificacao;

boolean flag_modificacao = true;                                                           //(false) MUDAR HORA - (true) MUDAR MINUTO
unsigned int tempo_key1 = 0;                                                               //VARIÁVEL PARA PEGAR O TEMPO DE ACIONAMENTO DA CHAVE 1
unsigned int tempo_key2 = 1000;                                                            //VARIÁVEL PARA PEGAR O TEMPO DE ACIONAMENTO DA CHAVE 2 (É INICIADA DESSA FORMA PARA O ALARME NÃO COMEÇAR LIGADO)
unsigned int tempo_key3 = 0;                                                               //VARIÁVEL PARA PEGAR O TEMPO DE ACIONAMENTO DA CHAVE 3

const byte SEGMENT_MAP[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0X80,0X90};            //BYTE MAP PARA NUMEROS DE 0 A 9
const byte SEGMENT_SELECT[] = {0xF1,0xF2,0xF4,0xF8};                                       //BYTE MAP PARA SELECIONAR O DISPLAY

volatile int count_timer = 0;


/*************************************************************/
/*********************FUNÇÕES********************************/
/*************************************************************/

void EscreveNumeroNoDisplay(byte Segment, byte Value)
{
  /*FUNCAO RESPONSAVEL PELA MULTIPLEXAÇÃO DOS DISPLAYS*/
  digitalWrite(LATCH_DIO,LOW);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_MAP[Value]);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_SELECT[Segment] );
  digitalWrite(LATCH_DIO,HIGH);
}

void buzzAviso()
{
  /*FUNÇÃO RESPONSAVEL PELO SINAL SONORO AO CONFIRMAR UMA MUDANCA DE HORARIO OU DE ALARME*/
  digitalWrite(BUZZ, LOW);
  delay(40);
  digitalWrite(BUZZ, HIGH);
}


struct relogio logicaRelogioSoma(struct relogio r)
{
  /*FUNÇÃO ONDE OCORRE A LÓGICA DA SOMA DE UM RELÓGIO
    RECEBE COMO PARÂMETRO UM RELÓGIO
    RETORNA O RELÓGIO PASSADO COMO PARÂMETRO ATUALIZADO*/
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
  /*FUNÇÃO RESPONSÁVEL PELA MODIFICAÇÃO NA HORA DE ALTERAR O RELÓGIO OU O ALARME
    RECEBE COMO PARÂMETRO UM RELÓGIO E O TIPO DE MODIFICAÇÃO QUE SERÁ FEITO MINUTO/HORA
    RETORNA O RELÓGIO RECEBIDO COMO PARÂMETRO ALTERADO*/
    
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
  /*FUNÇÃO RESPONSÁVEL POR MOSTRAR NO DISPLAY
    RECEBE COMO PARÂMETRO O RELÓGIO QUE QUER SER MOSTRADO
    E UMA VARIÁVEL QUE INDICA SE SERÁ A HORA OU ALARME A SER MOSTRADO
  */
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
{/*FUNÇÃO PARA CRIAR UM RELOGIO INICIALIZANDO ELE COM OS VALORES DEFAULT
    RETORNA O NOVO RELÓGIO CRIADO*/
  Relogio novo_relogio;

  novo_relogio.hora = 0;                     //INICIA RELOGIO EM 0 HORAS              
  novo_relogio.minutos = 0;                  //INICIA RELOGIO EM 0 MINUTOS
  novo_relogio.segundos = 1;                 //INICIA RELOGIO EM 0 SEGUNDOS
  novo_relogio.hora_alarme = 0;              //INICIA ALARME EM 0 HORAS              
  novo_relogio.minutos_alarme = 0;           //INICIA ALARME EM 0 MINUTOS
  novo_relogio.segundos_alarme = 0;          //INICIA ALARME EM 0 SEGUNDOS
  novo_relogio.alarme_status = false;        //INICIA RELOGIO COM ALARME DESLIGADO
  novo_relogio.tipo_funcao = 0;              //INICIA RELOGIO NA FUNÇÃO RELOGIO
  novo_relogio.alarme_ativado = false;       //INICIA O ALARME DO RELOGIO DESATIVADO

  return novo_relogio;
}

void appinit(void)
{/*FUNÇÃO RESPONSÁVEL POR INDICAR QUAIS CHAVES QUEREM SER MONITORADAS E SETAR O TIMER*/
  button_listen(KEY1);
  button_listen(KEY2);
  button_listen(KEY3);
  timer_set(2);

  relogio_principal = criarRelogio();  
}


void set_alarme()
{/*FUNÇÃO RESPONSÁVEL POR SETAR A HORA E OS MINUTOS DO ALARME NO RELÓGIO PRINCIPAL*/
  relogio_principal.hora_alarme = relogio_modificacao.hora_alarme;
  relogio_principal.minutos_alarme = relogio_modificacao.minutos_alarme;
}

void set_soneca()
{
 relogio_principal.minutos_alarme = relogio_principal.minutos_alarme + 20;
}

void button_changed(int p, int v)
{/*FUNÇÃO RESPONSÁVEL PELA AÇÃO CASO ALGUMA CHAVE SEJA ACIONADA
   RECEBE UM PINO E O ESTADO DELE*/
  
  if(p == KEY1 and v == HIGH)
  {//SE A CHAVE 1 É APERTADA, AO SOLTAR OCORRE ESSE IF

    if(relogio_principal.tipo_funcao == 2)                                                    //AO SAIR DA FUNÇÃO RELÓGIO, O RELOGIO DE MODIFICACAO RECEBE O PRINCIPAL
    {
      tempo_key1 = millis();
                                     
      if (((tempo_key2 - tempo_key1 < 500) || (tempo_key1 - tempo_key2 < 500)))
      {//SE OCORRER UM DELTA MENOR QUE 500MS ENTRE SOLTAR A CHAVE 2 E 1, ENTAO CONFIRMA O NOVO ALARME
        buzzAviso();                                                                          //AVISO DO BUZZ
        relogio_principal.tipo_funcao = 1;                                                    //VOLTA PARA A FUNÇÃO UM (RELÓGIO)
        set_alarme();                                                                         //CHAMA A FUNÇÃO DE SETAR ALARME
        return;
      }    
      
      relogio_modificacao = relogio_principal;
    }

    if(relogio_principal.tipo_funcao == 0)
    {
      tempo_key1 = millis();

      if (((tempo_key2 - tempo_key1 < 500) || (tempo_key1 - tempo_key2 < 500)))
      {//SE OCORRER UM DELTA MENOR QUE 500MS ENTRE SOLTAR A CHAVE 2 E 1, ENTAO CONFIRMA O NOVO HORÁRIO
        buzzAviso();                                                                          //AVISO DO BUZZ
        relogio_modificacao.tipo_funcao = 1;                                                  //VOLTA PARA A FUNÇÃO UM (RELÓGIO)
        relogio_principal = relogio_modificacao;                                              //RELOGIO PRINCIPAL RECEBE O QUE FOI MODIFICADO       
        return;
      }  
      
    }
    relogio_principal.tipo_funcao++;                                                          //ANDA A FUNÇÃO DO RELÓGIO QUE ESTA OCORRENDO
    
    if(relogio_principal.tipo_funcao == 3)                                                    //IF PARA VOLTAR PARA O INICIO DAS FUNÇÕES
      relogio_principal.tipo_funcao = 0;   
      
  }
  
  if(p == KEY2 and v == HIGH) 
  {//SE A CHAVE 2 É APERTADA, AO SOLTAR OCORRE ESSE IF
     
    if(relogio_principal.tipo_funcao == 0)                                                    
    {//SE A FUNÇÃO ESTIVER NA ZERO (MODIFICAR O RELOGIO)

      tempo_key2 = millis();      
      flag_modificacao = !flag_modificacao;                                                   //INVERTE A FLAG DE MODIFICACAO
        
    }

    else if(relogio_principal.tipo_funcao == 1)
    {//SE A FUNÇÃO ESTIVER EM UM (RELOGIO)
      
      relogio_principal.alarme_status = !relogio_principal.alarme_status;                     //MUDA O STATUS DO ALARME
      digitalWrite(LED4, !relogio_principal.alarme_status);                                   //ACENDE/APAGA O LED2 DE ACORDO COM O STATUS DO ALARME  
    }
        
    else if(relogio_principal.tipo_funcao == 2)                                                    
    {//SE A FUNÇÃO ESTIVER NA DOIS (MODIFICA ALARME)

      tempo_key2 = millis();
      
      flag_modificacao = !flag_modificacao;                                                   //INVERTE A FLAG DE MODIFICACAO
      
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
      
      if(relogio_principal.alarme_ativado == true)                                            //CHECA SE O ALARME ESTA ATIVADO
        relogio_principal.alarme_ativado = false;                                             //DESATIVA O ALARME

      if(millis() - tempo_key3 >= 2000)                                                       
      {//SE OCORRER UM DELTA DE 2S ENTRE APERTAR E SOLTAR, ENTAO OCORRE ESSE IF
        buzzAviso();                                                                          //AVISO DO BUZZ
        //relogio_principal.tipo_funcao = 1;                                                    //VOLTA PARA A FUNÇÃO UM (RELÓGIO)
        set_soneca();                                                                         //CHAMA A FUNÇÃO DE SETAR ALARME
      }

    }
    
    else if(relogio_principal.tipo_funcao == 2)
    {//SE A FUNÇÃO FOR DOIS (MODIFICAR ALARME)    
      relogio_modificacao = logicaModificacao(relogio_modificacao, flag_modificacao);         //RELOGIO MODIFICACAO RECEBE O QUE ESTA SENDO ALTERADO
    }  
  }
  else if(p == KEY3 and v == LOW)
  {
     if(relogio_principal.tipo_funcao == 1)
      {
        tempo_key3 = millis();                                                                  //PEGA O TEMPO QUE FOI APERTADA
      } 
  }
}

void despertar_alarme() //FUNCAO QUE DESPERTA O ALARME
{/*FUNÇÃO RESPONSÁVEL POR ATIVAR O ALARME*/
  
  if(relogio_principal.hora == relogio_principal.hora_alarme && relogio_principal.minutos == relogio_principal.minutos_alarme && relogio_principal.segundos == relogio_principal.segundos_alarme && relogio_principal.alarme_status == true && relogio_principal.tipo_funcao == 1)
  {//SE O RELOGIO PRINCIPAL ATINGIR O HORARIO SETADO NO ALARME, ESTIVER NO ESTADO RELOGIO E O STATUS DO ALARME ESTIVER ATIVADO, ENTÃO ATIVA A FLAG DE ALARME ATIVADO
    relogio_principal.alarme_ativado = true;
  }
  
  if(relogio_principal.alarme_ativado == true && relogio_principal.alarme_status == true )
  {//SE O ALARME ESTIVER ATIVADO E SEU STATUS FOR TRUE, ALARME TOCA
    digitalWrite(BUZZ, LOW);
  }
  else
  {//CASO CONTRÁRIO ALARME NÃO TOCA
    digitalWrite(BUZZ, HIGH);
  }
}

void timer_expired(void)
{/*FUNCAO RESPONSÁVEL POR REALIZAR A AÇÃO QUANDO O TIMER EXPIRAR*/
  
  count_timer++;                                                                                  //ACRESCENTA A VARIÁVEL RESPONSÁVEL POR CONTAR OS SEGUNDOS

  //CHAMA A FUNÇÃO PARA DESPERTAR O ALARME QUANDO NECESSÁRIO
  despertar_alarme();

  if(relogio_principal.tipo_funcao == 2)
  {//SE O LED2 ESTA LIGADO SIGNIFICA QUE ESTA MODIFICANDO O ALARME 
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);  
  }
  else if(relogio_principal.tipo_funcao == 0)
  {//SE O LED1 ESTA LIGADO SIGNIFICA QUE ESTA MODIFICANDO A HORA
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
  }
  else
  {//SE O LED1 E O LED2 ESTAO DESLIGADOS SIGNIFICA QUE ESTA NO ESTADO RELOGIO
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);    
  }

  if(count_timer >= 500)
  {//SE O COUNT_TIMER CHEGAR A UM VALOR DETERMINADO ACRESCENTA UM SEGUNDO NO RELOGIO
    relogio_principal = logicaRelogioSoma(relogio_principal); 
    count_timer = 0;                                                                      //ZERA O COUNT TIMER
  }
   
  if(relogio_principal.tipo_funcao == 0)                                                  //QUANDO A FUNÇÃO É ZERO (MODIFICA HORA) MOSTRA O RELOGIO QUE ESTA SENDO MODIFICADO
    mostraRelogioDisplay(relogio_modificacao, false); 
  else if(relogio_principal.tipo_funcao == 2)                                             //QUANDO A FUNÇÃO É DOIS (MODIFICA ALARME) MOSTRA A HORA QUE O ALARME ESTA SENDO SETADO
    mostraRelogioDisplay(relogio_modificacao, true);
  else
    mostraRelogioDisplay(relogio_principal, false);                                       //NAS OUTRAS FUNÇÕES MOSTRA A HORA NORMAL

  
  
}
