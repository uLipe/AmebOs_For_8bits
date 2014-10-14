/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */

#include "OsBase.h"

/*
 *  Variaveis desse mpdulo: 
 */

OsTCB_t tcbList[OS_NUMBER_OF_TASKS ];
OsTCBPtr_t tcbPtrTbl[OS_NUMBER_OF_TASKS ];
uint8_t taskCount;

//Exporta lista de prioridades principal:
extern OsPrioList_t osTaskList;

/*
 *	Implementacao:
 */

/*
 *	 OsTaskInit()
 */
void OsTaskInit(void)
{
	uint8_t i;

	//limpa todos os blocos de tcb:
	for (i = 0; i < OS_NUMBER_OF_TASKS ; i++)
	{
		tcbList[i].prioVal  = 0xFF;
		tcbList[i].tcbTaken = FALSE;
		tcbPtrTbl[i] = 0;
	}

	//zera contagem de tasks:
	taskCount = 0;

}

/*
 *	  OsTaskCreate()
 */
OsStatus_t OsTaskCreate(void (*task)(), OsStack_t *stk, uint16_t stkSize, uint8_t osPrio,
						void *taskArgs)
{
	uint8_t ieReg = 0;
	OsTCBPtr_t tcb_a = NULL;

	//checa argumentos:
	if (taskCount >= OS_NUMBER_OF_TASKS)
		return (kOsOutOfTCB);
	if (osPrio > OS_NUMBER_OF_TASKS)
		return (kOsInvalidParam);
	if (task == NULL)
		return (kOsInvalidParam);
	if (stk == NULL)
		return (kOsInvalidParam);

	OS_CRITICAL_IN();

	//checa se a prioridade desejada nao esta em uso;
	if (tcbList[osPrio].tcbTaken != FALSE)
	{
		OS_CRITICAL_OUT();
		return (kOsPrioInUse);
	}

	//se nao estiver toma esse tcb para si:
	tcbList[osPrio].tcbTaken = TRUE;

	taskCount++;

	//Preenche o TCB:
	tcbList[osPrio].prioVal = osPrio;
	tcbList[osPrio].taskDelay = 0;
	tcbList[osPrio].taskState = (1 << kOsTaskRdy); //sempre começa com o taskredy

	//Inicializa restante do TCB:
	tcbList[osPrio].topOfStack = OsStackInit((stk + stkSize), task, taskArgs);

	//
	tcbPtrTbl[osPrio] = &tcbList[osPrio];

	//Liga TCB na linkedList:
	tcb_a = tcbPtrTbl[OS_LEAST_PRIO ];

	//checa se estamos instalando a idletask.
	if (osPrio == OS_LEAST_PRIO)
	{
		tcb_a->nextTCB = NULL;
	}
	else
	{
		//procura pelo fim da linked list
		while (tcb_a->nextTCB != NULL )
		{
			tcb_a = tcb_a->nextTCB;
		}

		//quando encontrar, liga nosso tcb:
		tcb_a->nextTCB = tcbPtrTbl[osPrio];

		//marca novo fim da linkedlist:
		tcbPtrTbl[osPrio]->nextTCB = NULL;
	}

	//por ultimo insere esse TCB na lista de prioridades:
	OsPrioSet(osPrio, &osTaskList);

	OS_CRITICAL_OUT();

	//pede um context switch:
	OsTaskYield();

	//correu tudo bem:
	return (kOsStatusOk);

}

/*
 *	 OsTaskDelete()
 */
OsStatus_t OsTaskDelete(uint8_t osPrio)
{
	uint8_t ieReg;
	OsTCBPtr_t tcb_a;

	//checa argumentos:
	if (osPrio > OS_NUMBER_OF_TASKS)
		return (kOsInvalidParam);
	//nao pode deletar a idle task:
	if (osPrio == OS_LEAST_PRIO)
		return (kOsInvalidParam);

	//checa se a task esta instalada;
	if (tcbList[osPrio].tcbTaken != TRUE)
		return (kOsInvalidParam);

	OS_CRITICAL_IN();

	//retira task da lista de tarefas prontas pra rodar:
	OsPrioClr(osPrio, &osTaskList);

	//pega o tcb desejado e remove ele da linkedlist:
	tcb_a = tcbPtrTbl[OS_LEAST_PRIO ];

	//procura por qual TCB aponta para ele:
	while (tcb_a->nextTCB->prioVal != osPrio)
	{
		tcb_a = tcb_a->nextTCB;
	}

	//checa se é o ultimo item da lista:
	if (tcb_a->nextTCB->nextTCB == NULL)
	{
		//retira o tcbdesejado da lista:
		tcb_a->nextTCB = NULL;
	}
	else
	{
		//se estiver entre dois tcbs, entao o corrente deve apontar ao
		// proximo do proximo elemento:
		tcb_a->nextTCB = tcbPtrTbl[osPrio]->nextTCB;
	}

	//libera tcb:
	tcbPtrTbl[osPrio]->tcbTaken = FALSE;

	taskCount--;

	//removido pede uma troca de contexto:
	OS_CRITICAL_OUT();

	OsTaskYield();

	return (kOsStatusOk);

}

/*
 *	 OsTaskSuspend()
 */
OsStatus_t OsTaskSuspend(uint8_t osPrio) {
	uint8_t ieReg;

	//checa argumentos:
	if (osPrio > OS_NUMBER_OF_TASKS)
		return (kOsInvalidParam);
	//nao pode suspender a idle task:
	if (osPrio == OS_LEAST_PRIO)
		return (kOsInvalidParam);
	//checa se a task esta instalada;
	if (tcbList[osPrio].tcbTaken != TRUE)
		return (kOsInvalidParam);

	OS_CRITICAL_IN();

	//remove a task desejada da lista de tarefas prontas:
	OsPrioClr(osPrio, &osTaskList);

	//limpa o flag de task ready se houver:
	tcbPtrTbl[osPrio]->taskState &= ~(1 << kOsTaskRdy);

	//aciona o flag de task suspensa	
	tcbPtrTbl[osPrio]->taskState |= (1 << kOsTaskSuspend);

	OS_CRITICAL_OUT();

	//pede uma troca de contexto:
	OsTaskYield();

	return (kOsStatusOk);
}

/*
 *	 OsTaskResume()
 */
OsStatus_t OsTaskResume(uint8_t osPrio)
{
	uint8_t ieReg;

	//checa argumentos:
	if (osPrio > OS_NUMBER_OF_TASKS)
		return (kOsInvalidParam);
	// a idle task nunca e suspensa manualmente:
	if (osPrio == OS_LEAST_PRIO)
		return (kOsInvalidParam);
	//checa se a task esta instalada;
	if (tcbList[osPrio].tcbTaken != TRUE)
		return (kOsInvalidParam);

	OS_CRITICAL_IN();

	//coloca a task desejada na lista de tarefas prontas:
	OsPrioSet(osPrio, &osTaskList);

	//limpa o flag de task suspensa:
	tcbPtrTbl[osPrio]->taskState &= ~(1 << kOsTaskSuspend);

	//aciona o flag de task pronta	
	tcbPtrTbl[osPrio]->taskState |= (1 << kOsTaskRdy);

	OS_CRITICAL_OUT();

	//pede uma troca de contexto:
	OsTaskYield();

	return (kOsStatusOk);

}

