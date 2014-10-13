/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */

#include "OsBase.h"

/*
 *
 */

extern OsTCBPtr_t currentTask;
extern OsPrioList_t osTaskList;

/*
 * Implementacao das funcoes:
 */

/*
 *  OsDelayTicks()
 */
OsStatus_t OsDelayTicks(uint8_t ticks)
{
	uint8_t ieReg;

	OS_CRITICAL_IN();

	//retira a task corrente da lista de tarefas prontas:
	OsPrioClr(currentTask->prioVal, &osTaskList);

	//Limpa o flag de tarefa pronta:
	currentTask->taskState &= ~(1 << kOsTaskRdy);

	//Aciona o flag de pending delay:
	currentTask->taskState |= (1 << kOsTaskPendDelay);

	//Passa o tempo que a task vai ficar em espera:
	currentTask->taskDelay = ticks;

	OS_CRITICAL_OUT();

	//Pede troca de contexto:
	OsTaskYield();

	return (kOsStatusOk);
}

/*
 *  OsDelayMs()
 */
OsStatus_t OsDelayMs(uint8_t milis)
{
	uint8_t ticks;

	ticks = 0;

	//Faz uma regra de tres pra ver quantos ticks sao necessarios para 
	//gerar o delay desejado:
	ticks = milis / OS_MIN_BASE_DELAY;

	//pede o delay desejado:
	return (OsDelayTicks(ticks));
}
