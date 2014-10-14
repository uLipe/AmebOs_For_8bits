/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */

#include "OsBase.h"

/*
 *	Estado de aguarde dos flags
 */
#define OS_FLAGS_PENDING_ALL_CONSUME (uint8_t)0xFC
#define OS_FLAGS_PENDING_ANY_CONSUME (uint8_t)0xFB
#define OS_FLAGS_PENDING_NOT 		 (uint8_t)0xFA

/*
 *  Variaveis desse modulo:
 */

#if(OS_FLAGS_COUNT > 0)

OsFlags_t flagGrpTbl[OS_FLAGS_COUNT];
uint8_t flagsCount;

//Exporta a ready list e a task corrente:
extern OsPrioList_t osTaskList;
extern OsTCBPtr_t currentTask;
extern OsTCBPtr_t tcbPtrTbl[];

#endif

/*
 *	 Implementacao dos flags
 */

/*
 * OsFlagsPostDecoder()
 * Funcao interna do modulo de flags, decodifica o processamento do evento
 * se houver
 */
void OsFlagsPostDecoder(OsFlagsPtr_t flagGrp, uint8_t i)
{
#if OS_FLAGS_COUNT > 0	

	uint8_t ieReg;
	uint8_t consumed;
	uint16_t mask;

	consumed = FALSE;

	OS_CRITICAL_IN();

	switch (flagGrp->tasksPending[i])
	{
	case OS_FLAGS_PENDING_ALL_CONSUME :

		//checa se os flags foram atendidos:
		mask = flagGrp->flagsRegister & tcbPtrTbl[i]->taskEvents;
		if (mask == tcbPtrTbl[i]->taskEvents)
		{
			//Se atendeu coloca a tarefa como pronta:
			tcbPtrTbl[i]->taskState &= ~(1 << kOsFlagPendAll);
			tcbPtrTbl[i]->taskState |= (1 << kOsTaskRdy);
			OsPrioSet(i, &osTaskList);
			consumed = TRUE;
		}

		break;

	case OS_FLAGS_PENDING_ANY_CONSUME :
		//checa se os flags foram atendidos:
		mask = flagGrp->flagsRegister & tcbPtrTbl[i]->taskEvents;
		if (mask)
		{
			//Se atendeu coloca a tarefa como pronta:
			tcbPtrTbl[i]->taskState &= ~(1 << kOsFlagPendAny);
			tcbPtrTbl[i]->taskState |= (1 << kOsTaskRdy);
			OsPrioSet(i, &osTaskList);
			consumed = TRUE;
		}
		break;
	}

	if (consumed != FALSE)
	{
		//limpa os flags e delay da tcb, e consome os eventos
		tcbPtrTbl[i]->taskDelay = 0;
		tcbPtrTbl[i]->taskEvents = 0;
		flagGrp->flagsRegister &= ~mask;
		flagGrp->tasksPending[i] = OS_FLAGS_PENDING_NOT;
	}

	OS_CRITICAL_OUT();
#endif	 
}

/*
 *		OsFlagsInit()
 */
void OsFlagsInit(void)
{
#if OS_FLAGS_COUNT > 0	 
	uint8_t i, j;

	//coloca todas as variaveis em um estado inicial conhecido:
	flagsCount = 0;

	//Tambem inicializa todos os grupos de flags
	for (i = 0; i < OS_FLAGS_COUNT; i++)
	{
		flagGrpTbl[i].flagsRegister = 0;
		flagGrpTbl[i].flagsTaken = FALSE;
		for (j = 0; j < OS_NUMBER_OF_TASKS ; j++)
			flagGrpTbl[i].tasksPending[j] = OS_FLAGS_PENDING_NOT;

	}

#endif	 
}

/*
 *		OsFlagsCreate()
 */
OsFlagsPtr_t OsFlagsCreate(uint8_t *err)
{
#if OS_FLAGS_COUNT > 0
	uint8_t ieReg;
	uint8_t i;

	OS_CRITICAL_IN();

	//checa se temos blocos livres:
	if (flagsCount >= OS_FLAGS_COUNT)
	{
		OS_CRITICAL_OUT();
		*err = kOsOutOfFlags;
		return (NULL);
	}

	//temos blocos livres, entao tomamos mais um:
	flagsCount++;

	//procura pelo bloco que esta livre na tabela
	for (i = 0; i < OS_FLAGS_COUNT; i++)
	{

		if (flagGrpTbl[i].flagsTaken != TRUE)
		{
			//toma para si.
			flagGrpTbl[i].flagsTaken = TRUE;
			break;
		}
	}

	OS_CRITICAL_OUT();

	*err = kOsStatusOk;

	//esse bloco esta pronto pra ser usado:
	return ((OsFlagsPtr_t) &flagGrpTbl[i]);
#endif	 
}

/*
 *		OsFlagsPend()
 */
OsStatus_t OsFlagsPend(OsFlagsPtr_t flagGrp, uint16_t flags, uint8_t opt,
						uint8_t timeout)
{
#if OS_FLAGS_COUNT > 0
	uint8_t ieReg;

	//checa argumentos:
	if (flagGrp == NULL)
		return (kOsInvalidParam);
	if ((opt < OS_FLAGS_SET_ALL_CONSUME ) || (opt > OS_FLAGS_SET_ANY_CONSUME ))
		return (kOsInvalidParam);

	OS_CRITICAL_IN();

	//suspende a task conforme as opções:
	switch (opt)
	{
	case OS_FLAGS_SET_ALL_CONSUME :
		//Seta os flags no tcb da task
		currentTask->taskState |= (1 << kOsFlagPendAll);
		//Coloca essa task na lista de tasks pendentes:
		flagGrp->tasksPending[currentTask->prioVal] =
				OS_FLAGS_PENDING_ALL_CONSUME;

		break;

	case OS_FLAGS_SET_ANY_CONSUME :

		currentTask->taskEvents |= flags;
		//remove a task da lista de tasks prontas:
		OsPrioClr(currentTask->prioVal, &osTaskList);
		//task nao esta mais pronta, esta aguardando flag:
		currentTask->taskState |= (1 << kOsFlagPendAny);
		//Coloca essa task na lista de tasks pendentes:
		flagGrp->tasksPending[currentTask->prioVal] =
				OS_FLAGS_PENDING_ANY_CONSUME;
		break;
	}

	currentTask->taskEvents |= flags;
	//remove a task da lista de tasks prontas:
	OsPrioClr(currentTask->prioVal, &osTaskList);
	//task nao esta mais pronta, esta aguardando flag:
	currentTask->taskState &= ~(1 << kOsTaskRdy);
	//coloca o timeout:
	currentTask->taskDelay = timeout;

	OS_CRITICAL_OUT();

	//task pendente, pede troca de contexto:
	OsTaskYield();


#endif

	return (kOsStatusOk);

}

/*
 *		 OsFlagsPost()
 */
OsStatus_t OsFlagsPost(OsFlagsPtr_t flagGrp, uint16_t flags)
{
#if OS_FLAGS_COUNT > 0
	uint8_t i;

	//checa argumentos:
	if (flagGrp == NULL)
		return (kOsInvalidParam);
	//Pelo menos um evento deve ser passado
	if (flags == 0)
		return (kOsInvalidParam);

	flagGrp->flagsRegister |= flags;

	for (i = 0; i < OS_NUMBER_OF_TASKS ; i++)
	{
		if (flagGrp->tasksPending[i] != OS_FLAGS_PENDING_NOT)
		{
			//Se essa task esta pendente, então processa seus flags:
			OsFlagsPostDecoder(flagGrp, i);
		}
	}

	//Pede uma troca de contexto:
	OsTaskYield();

#endif

	return (kOsStatusOk);
}

/*
 *		 OsFlagsDelete()
 */
OsStatus_t OsFlagsDelete(OsFlagsPtr_t flagGrp)
{
#if OS_FLAGS_COUNT > 0
	uint8_t ieReg;
	uint8_t i;

	//checa argumentos:
	if (flagGrp == NULL)
		return (kOsInvalidParam);

	OS_CRITICAL_IN();

	//Retira de pending todas as tasks que aguardam por esse grupo de flags
	for (i = 0; i < OS_NUMBER_OF_TASKS ; i++)
	{
		flagGrp->tasksPending[i] = OS_FLAGS_PENDING_NOT;
		tcbPtrTbl[i]->taskEvents = 0;
		tcbPtrTbl[i]->taskDelay = 0;
		tcbPtrTbl[i]->taskState = (1 << kOsTaskRdy);
		OsPrioSet(i, &osTaskList);
	}

	flagGrp->flagsTaken = FALSE;

	flagsCount--;

	flagGrp = NULL;

	OS_CRITICAL_OUT();

	OsTaskYield();

#endif

	return (kOsStatusOk);	 
}

