/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */

#ifndef OS_BASE_H_
#define OS_BASE_H_

/*
 Dependencias
 */

#include "OsConfig.h"

/*
 8051 base type:
 */
typedef uint8_t OsStack_t;

/*
 constantes
 */

#define TRUE			    (uint8_t)0xFF
#define FALSE			  	(uint8_t)0x00
#define NULL         		(void *)0x000000

#define OS_TASKGROUPS 	 	(uint8_t)8
#define OS_MAX_TASKS    	(uint8_t)64
#define OS_LEAST_PRIO   	(uint8_t)0

/*
 timers & delays
 */
#define OS_MAX_DELAY	 	(uint8_t)255
#define OS_MIN_BASE_DELAY   (uint8_t)OS_TIME_BASE

/*
 rtos status flags
 */
typedef enum {
	kOsStatusOk = 0,
	kOsInvalidParam,
	kOsOutOfTCB,
	kOsPrioInUse,
	kOsOutOfFlags,
	kOsOutOfQueue,
	kOsTooManyData,
	kOsQueueFull,
	kOsQueueEmpty,
	kOsFailedStart,

} OsStatus_t;

/*
 rtos task states:
 */
typedef enum {
	kOsTaskRdy = 0,
	kOsTaskPendDelay,
	kOsFlagPendAll,
	kOsFlagPendAny,
	kOsQueuePend,
	kOsTaskSuspend,
} OsTaskStates_t;

/*
 Rtos kernel objects
 */

//Estrutura de lista de prioridades para acesso do RTOS
struct OsPrioList_ {
	uint8_t osPrioGrp;
	uint8_t osPrioTbl[OS_TASKGROUPS ];
};

typedef struct OsPrioList_ OsPrioList_t;
typedef struct OsPrioList_ * OsPrioListPtr_t;

//Estrutura de controle das tasks:
struct ostcb_ {
	OsStack_t *topOfStack;
	uint8_t prioVal;
	uint8_t taskDelay;
	uint8_t taskState;
	uint16_t taskEvents;
	uint8_t tcbTaken;
	struct ostcb_ *nextTCB;
};

typedef struct ostcb_ OsTCB_t;
typedef struct ostcb_ *OsTCBPtr_t;

//estrutura de controle dos events flags;
struct flags_ {
	uint16_t flagsRegister;
	uint8_t flagsTaken;
	uint8_t tasksPending[OS_NUMBER_OF_TASKS ];
};

typedef struct flags_ OsFlags_t;
typedef struct flags_* OsFlagsPtr_t;

//Estrutura de controle para queue:

typedef uint8_t OsQueueData_t;

struct tinyqueue_ {
	OsQueueData_t *queueData;
	uint8_t queueDataSize;
	uint8_t queueEntries;
	uint8_t queueCurrentEntries;
	uint8_t queueCurrentFree;
	uint8_t queueCurrentFront;
	uint8_t queueTaken;
	uint8_t tasksPending[OS_NUMBER_OF_TASKS ];
};

typedef struct tinyqueue_ OsQueue_t;
typedef struct tinyqueue_ * OsQueuePtr_t;

/*
 Inclusao dos arquivos do rtos aqui:
 */

#include "OsPort.h"

/*
 OsTask.h
 
 Com o modulo gerenciador de tasks o usuario pode criar quantos
 blocos de tarefas achar necessarios a sua aplicação, o limite
 pode ser ajustado em OsConfig.h e o AmebOs limita internamenta
 a um maximo de 64 Tasks, mais podem ser criadas, com pequenas
 modificações no kernel.
 
 Uma task é basicamente constituida de:
 - Funcao da task;
 - Pilha da task;
 - Prioridade da task;

 Cada task possui uma unica prioridade no kernel, a funcçao
 de criação ira retornar erro se duas tasks de mesma prioridade
 forem instaladas. O AmebOs suporta a instalação e desinstalação
 dinâmica, tornando desencessario o preenchimento manual do tcb.
 
 exemplo de uso:
 
 OsStack_t taskStack[32];
 
 void Task(void *args)
 {
 //toda task é um loop infinito
 for(;;)
 {
 //seu codigo aqui
 }
 }

 //em algum lugar em tua aplicação instale a sua task:
 uint8_t err;

 err = OsTaskCreate(&Task, &taskStack, PRIORIDADE_TASK, parametros_task);
 if( err != kOsStatusOk) while(1);

 //Se nao quiser mais apague e libere o tcb:
 err = OsTaskDelete(PRIORIDADE_TASK);

 */
#include "OsTask.h"

/*
 OsKernel.h

 Esse modulo é o modulo raiz do Rtos, o usuario não deveria utilizar
 as funcoes aqui descritas, apenas as funcoes de inicialização e disparo
 do kernel, portanto se for usar alguma funcao aqui, tenha certeza que sabe
 o que esta fazendo.

 Exemplo de uso:

 uint8_t err;

 OsInit(); //inicialize os kernel objects do rtos

 //Seu codigo, instale tasks inicialize variaveis, hw crie
 //algum grupo de event flags

 err = OsStart(); //a partir desse ponto o Rtos toma o controle
 // de sua aplicação, portanto essa função
 // nao deveria retornar, se isso ocorrer
 // foi algo que falhou internamente no kernel


 
 */
#include "OsKernel.h"

/*
 OsDelays.h

 Esse modulo contem rotinas para suspensao de execução da task
 por periodo de ticks ou milissegundos(dependendo da frequencia
 de tick utilizada em OsConfig.h)o usuario pode utilizar alguma
 das duas funções passando o tempo que aguarde que achar necessario


 Exemplo:

 void Task(void *argss)
 {
 //Executa o codigo da task, e aguarda pelo menos 10 ticks antes de ficar
 //novamente pronta pra execução.
 for(;; OsDelayTicks(10))
 {
 //Seu codigo
 }
 }

 */
#include "OsDelays.h"

/*			OsFlags.h

 Esse modulo contem rotinas para gerenciamento de grupo de flags,
 flags são muito uteis para monitorar eventos, suspender ou resumir
 a execução de uma task que aguarda por alguma ação, oferencem
 rápida resposta, onde sinalizar a ocorrencia de um evento pode
 ser feita por exemplo dentro de uma interrupção.


 Exemplo:


 #define FLAG_CHEGOU_DADO 	 0x01
 #define FLAG_TRANSMITIU_DADO 0x02

 //cria um bloco de controle para grupos de flags
 OsFlagsPtr_t flagGrp;


 void Task(void *args)
 {

 uint8_t err;

 //Primeiro criamos um grupo de flags:
 flagGrp = OsFlagsCreate(&err);
 if(err != kOsStatusOk){} //trate os erros nesse if

 for(;;)
 {
 //aguarda a chegada de um pacote da porta serial:
 err = OsFlagsPend( flagGrp, FLAG_CHEGOU_DADO, FLAG_SET_ALL_CONSUME, 20);

 //flags podem aguardar eternamente ou esperar um numero de ticks maximos especificado no 4 parametro
 
 //seu codigo
 }
 }
 
 void InterrupcaoSerial(void)
 {
 //notifica pra todo mundo que esta aguardando que chegou dado.
 err = OsFlagsPost(flagGrp, FLAG_CHEGOU_DADO);
 }
 */
#include "OsFlags.h"

/*
 OsTinyQueue.h
 
 Esse modulo contem rotinas de queueing, enfileiramento de mensagens.
 Traz apenas as rotinas basicas de push e pop, porem com algumas opções
 como bloqueio da task em caso de inserçai dem queue cheia, ou retirada
 de queue vazia, alem disso uma macro é provida para alocar exatamente
 o espaço necessario para N entradas de B bytes.Os dados da queue sao
 organizados da seguinte forma:
 ________________________________________________________________
 |datasize -> 1byte| ----------------- N * B Bytes ---------------|
 |_________________|______________________________________________|


 Exemplo:

 OS_QUEUE_RESERVE(dados, 12, 32); //reserva uma queue de 32 entradas de 12bytes cada

 OsQueuePtr_t queue;


 void Task(void *args)
 {
 uint8_t err;
 uint8_t vector[12];
 uint8_t dataSize;

 //cria uma queue usando os mesmos parametros usados para alocar os dados
 queue = OsQueueCreate(&dados, 12, 32, &err);
 if(err != kOsStatusOk) while(1);

 for(;;)
 {
 //Insere alguns dados na queue:
 err = OsQueuePost(queue, (uint8_t *) "HELLO WORLD!",12, OS_Q_NOT_BLOCK);

 //retira uma entry da queue:
 err = OsQueuePend(queue, &vector, &dataSize, OS_Q_NOT_BLOCK);
 }
 }

 */
#include "OsTinyQueue.h"

#endif 			
