/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */

#include "OsBase.h"

/*
 *	Variaveis usadas nesse modulo
 */

OsTCBPtr_t currentTask; //task que roda nesse momento
OsTCBPtr_t highPrioTask; //task de maior prioridade pronta pra rodar
OsPrioList_t osTaskList;	//Lista de prioridades principal do rtos

uint8_t irqCounter;  //usado para fazer nesting de irqs
uint8_t osRunning;
uint16_t tickCounter = 0;
OsStack_t osIdleTaskStack[IDLE_TASK_STACK_SIZE ];

extern OsTCB_t tcbList[OS_NUMBER_OF_TASKS ];
extern OsTCBPtr_t tcbPtrTbl[OS_NUMBER_OF_TASKS ];//

/*
 *	OsKernel, implementacao:
 */

/*
 *	OsIdleTask()
 */
//Interno do Rtos, tarefa livre do sistema, roda na prioridade mais baixa
void OsIdleTask(void *pargs)
{
	volatile uint8_t execCounter;

	//Dummy read para evitar warning.
	execCounter =  (uint8_t)((uint16_t)pargs >> 8);

	for (;;)
	{
		//Fica etertnamente aqui
		execCounter++;
		(void)execCounter;

	}
}

/*
 *	OsTick
 */
//Interno do Rtos, essa funcao varre a lista de tarefas e atualiza seus contadores
void OsTick(void)
{
	uint8_t ieReg;
	OsTCBPtr_t ostcb;

	//So deve rodar esse codigo com o kernel rodando:
	if (osRunning != TRUE)
		return;

	OS_CRITICAL_IN();

	//Notifica que estamos numa IRQ:
	OsIrqIn();

	//comeca a busca sempre pela idle task:
	ostcb = tcbPtrTbl[OS_LEAST_PRIO];
	
	tickCounter++;

	do
	{
		//checa se ha delay para processar:
		if (ostcb->taskDelay != 0)
		{
			ostcb->taskDelay--;
			if (!ostcb->taskDelay)
			{
				//contador chegou a zero, podemos remover o pending delay;
				ostcb->taskState &= ~(1 << kOsTaskPendDelay);

				//Essa tarefa esta pronta:
				ostcb->taskState |= 1 << kOsTaskRdy;

				//Insere na lista de tasks prontas:
				OsPrioSet(ostcb->prioVal, &osTaskList);

			}
		}

		//Acessa o proximo tcb:
		ostcb = ostcb->nextTCB;
	} while (ostcb != NULL );

	OS_CRITICAL_OUT();

	//saindo da IRQ:
	OsIrqExit();

}

/*
 * OsClz()
 */
uint8_t OsClz(uint8_t reg)
{
	uint8_t clz = 0;
	uint8_t mask = 0x80;
	uint8_t ret = 0;

	//Inicializa busca pelo numero de leading zeroes
	ret = 0;
	clz = FALSE;

	for (mask = 0x80; mask != 0; mask >>= 1)
	{
		//encontrou o primeiro high bit.
		if (mask & reg)
		{
			//ok, ja terminou a busca.
			clz = TRUE;
		}
		else
		{
			if (clz != TRUE)
			{
				//se nao encontrou, significa que temos mais um zero a adicionar
				ret++;
			}
		}
	}
	return (ret);
}
/*
 *	OsInit()
 */
void OsInit(void)
{
	uint8_t i = 0;

	//inicializa todas as variaveis locais:
	currentTask = NULL;
	highPrioTask = NULL;
	irqCounter = 0;
	osRunning = FALSE; //kernel parado

	//Zera lista de prioridades:
	osTaskList.osPrioGrp = 0;

	for (i = 0; i < OS_TASKGROUPS ; i++)
		osTaskList.osPrioTbl[i] = 0;

	OsTaskInit(); //inicializa tasks.
	OsFlagsInit(); //inicializa grupos de flags
	OsQueueInit(); //inicializa grupo de queues.

	//Instala a Idle task:
	OsTaskCreate(&OsIdleTask,(OsStack_t *)&osIdleTaskStack,sizeof(osIdleTaskStack), OS_LEAST_PRIO, 0);

}

/*
 *	OsStart()
 */
OsStatus_t OsStart(void)
{

	//procura a tarefa mais prioritaria pronta:
	highPrioTask = tcbPtrTbl[OsFindHighPrioRdy((OsPrioListPtr_t )&osTaskList)];

	//rtos rodando:
	osRunning = TRUE;
	
	//inicializa tick timer
	OsInitTickTimer();

	OsFirstSwt();

	//nao deveria mais retornar, mas se o ocorrer
	// houve algum problema no OS:

	return (kOsFailedStart);

}

/*
 *	 OsIrqIn()
 */
void OsIrqIn(void)
{
	//So executa com o kernel rodando:
	if (osRunning != TRUE)
		return;

	//A notificacao ocorre quando temos o irq counter > 0
	if (irqCounter < 0xFF)
		irqCounter++;
}

/*
 *	 OsIrqExit()
 */
void OsIrqExit(void)
{
	//So executa com o kernel rodando:
	if (osRunning != TRUE)
		return;

	//A notificacao se da pelo decremento do irq counter a cada ISR atendida
	if (irqCounter != 0)
	{
		irqCounter--;

		//Se atendeu a todas as ISRs, pede pra trocar contexto:
		if (irqCounter == 0)
		{
			OsTaskYieldFromIsr();
		}
	}
}

/*
 *	 OsFindHighPrioRdy()
 */
uint8_t OsFindHighPrioRdy(OsPrioListPtr_t prioList)
{

	uint8_t osPrio = 0;
	uint8_t y = 0;
	uint8_t x = 0;

	//Procura o grupo mais prioritario pronto:
	x = (OS_TASKGROUPS - 1) - OsClz(prioList->osPrioGrp);

	//Procura qual a task mais prioritaria nesse grupo:
	y = (OS_TASKGROUPS - 1) - OsClz(prioList->osPrioTbl[x]);

	//forma prioridade:
	osPrio = (x << 3) | y;

	return (osPrio);
}

/*
 *	 OsTaskYield()
 */
void OsTaskYield(void)
{
	uint8_t ieReg = 0;

	//so deve executar com o kernel rodando:
	if (osRunning != TRUE)
		return;

	//So pede troca de contexto se estiver fora de um ISR ou saindo de um
	if(irqCounter != 0)
		return;

	OS_CRITICAL_IN();

	//toma tarefa pronta de maior prioridade:
	highPrioTask = tcbPtrTbl[OsFindHighPrioRdy((OsPrioListPtr_t )&osTaskList)];

	//checa se precisa mesmo trocar:
	if (highPrioTask != currentTask)
	{	
		//Pede troca de contexto:
		OsCtxSwt();
	}

	OS_CRITICAL_OUT();

}

/*
 *	 OsTaskYield()
 */
void OsTaskYieldFromIsr(void)
{
	uint8_t ieReg = 0;

	//so deve executar com o kernel rodando:
	if (osRunning != TRUE)
		return;

	//So pede troca de contexto se estiver fora de um ISR ou saindo de um
	if(irqCounter != 0)
		return;

	OS_CRITICAL_IN();

	//toma tarefa pronta de maior prioridade:
	highPrioTask = tcbPtrTbl[OsFindHighPrioRdy((OsPrioListPtr_t )&osTaskList)];

	//Pede troca de contexto:
	OsCtxSwt();


	OS_CRITICAL_OUT();

}
/*
 *	 OsPrioSet()
 */
OsStatus_t OsPrioSet(uint8_t prio, OsPrioListPtr_t prioList)
{
	uint8_t x = 0, y = 0;

	//Checa se a prio existe:
	if (prio > OS_NUMBER_OF_TASKS)
	{
		//nao executa:
		return (kOsInvalidParam);
	}

	//calcula prio base:
	x = prio >> 3;
	y = prio & 0x07;

	//Modificia o bitmap de tasks com base em x e y:
	prioList->osPrioGrp |= (1 << x);
	prioList->osPrioTbl[x] |= (1 << y);

	return (kOsStatusOk);

}

/*
 *	 OsPrioClr()
 */
OsStatus_t OsPrioClr(uint8_t prio, OsPrioListPtr_t prioList)
{
	uint8_t x = 0, y = 0;

	//Checa se a prio existe:
	if (prio > OS_NUMBER_OF_TASKS)
	{
		//nao executa:
		return (kOsInvalidParam);
	}

	//calcula prio base:
	x = prio >> 3;
	y = prio & 0x07;

	//Modificia o bitmap de tasks com base em x e y:
	prioList->osPrioTbl[x] &= ~(1 << y);
	if (prioList->osPrioTbl[x] == 0)
		prioList->osPrioGrp &= ~(1 << x);

	return (kOsStatusOk);
}


