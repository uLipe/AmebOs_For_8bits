/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */

#include "OsBase.h"

/*
 * 	Constantes uteis:
 */
#define OS_Q_PENDING_FULL   0x01
#define OS_Q_PENDING_EMPTY  0x02
#define OS_Q_PENDING_NOT    0x03

/*
 *	Variaveis desse modulo:
 */

#if OS_QUEUE_COUNT > 0

OsQueue_t queueBlockTbl[OS_QUEUE_COUNT];
uint8_t queueCount;

//Exporta task corrente e lista de prioridades global
extern OsTCBPtr_t currentTask;
extern OsTCBPtr_t tcbPtrTbl[OS_NUMBER_OF_TASKS ];
extern OsPrioList_t osTaskList;

#endif

/*
 *	Implementacao do modulo Queue:
 */

/*
 * OsQueueEmptyLoop()
 *
 * funcao interna do modulo queue, para notificar as tasks que a queue nao esta mais cheia
 */
void OsQueueEmptyLoop(OsQueuePtr_t queue)
{
#if OS_QUEUE_COUNT > 0	
	uint8_t i;

	for (i = 0; i < OS_NUMBER_OF_TASKS ; i++)
	{
		if (queue->tasksPending[i] == OS_Q_PENDING_FULL)
		{
			//i - esima task nao esta mais aguardando queue:
			queue->tasksPending[i] = OS_Q_PENDING_NOT;

			//colca essa task em pronta:
			tcbPtrTbl[i]->taskState &= ~(1 << kOsQueuePend);
			tcbPtrTbl[i]->taskState |= (1 << kOsTaskRdy);
			OsPrioSet(i, &osTaskList);
		}
	}
#endif
}

/*
 * OsQueueFullLoop()
 *
 * funcao interna do modulo queue, para notificar as tasks que a queue nao esta mais Vazia
 */
void OsQueueFullLoop(OsQueuePtr_t queue)
{
#if OS_QUEUE_COUNT > 0	
	uint8_t i;

	for (i = 0; i < OS_NUMBER_OF_TASKS ; i++)
	{
		if (queue->tasksPending[i] == OS_Q_PENDING_EMPTY)
		{
			//i - esima task nao esta mais aguardando queue:
			queue->tasksPending[i] = OS_Q_PENDING_NOT;

			//colca essa task em pronta:
			tcbPtrTbl[i]->taskState &= ~(1 << kOsQueuePend);
			tcbPtrTbl[i]->taskState |= (1 << kOsTaskRdy);
			OsPrioSet(i, &osTaskList);
		}
	}
#endif	 
}

/*
 * OsQueueDeleteLoop()
 *
 * funcao interna do modulo queue, para notificar as tasks que a queue Sera deletada
 * assim limpando qualquer pendencia em relacao a esse queue
 */
void OsQueueDeleteLoop(OsQueuePtr_t queue)
{
#if OS_QUEUE_COUNT > 0	
	uint8_t i;

	for (i = 0; i < OS_NUMBER_OF_TASKS ; i++)
	{
		if (queue->tasksPending[i] != OS_Q_PENDING_NOT)
		{
			//i - esima task nao esta mais aguardando queue:
			queue->tasksPending[i] = OS_Q_PENDING_NOT;

			//colca essa task em pronta:
			tcbPtrTbl[i]->taskState &= ~(1 << kOsQueuePend);
			tcbPtrTbl[i]->taskState |= (1 << kOsTaskRdy);
			OsPrioSet(i, &osTaskList);
		}
	}
#endif	 
}

/*
 *	OsQueueInit()
 */
void OsQueueInit(void)
{
#if OS_QUEUE_COUNT > 0	 

	uint8_t i, j;

	//Inicializa numero de queues usadas:
	queueCount = 0;

	//iniciliza queue kernel objects:
	for (i = 0; i < OS_QUEUE_COUNT; i++)
	{
		//Zera todos os dados e libera o i-esimo bloco
		queueBlockTbl[i].queueData = NULL;
		queueBlockTbl[i].queueDataSize = 0;
		queueBlockTbl[i].queueEntries = 0;
		queueBlockTbl[i].queueCurrentFree = 0;
		queueBlockTbl[i].queueCurrentFront = 0;
		queueBlockTbl[i].queueCurrentEntries = 0;
		queueBlockTbl[i].queueTaken = FALSE;

		for (j = 0; j < OS_NUMBER_OF_TASKS ; j++)
		{
			//Essa queue nao tem tasks pendentes.
			queueBlockTbl[i].tasksPending[j] = OS_Q_PENDING_NOT;
		}
	}
#endif		  
}

/*
 *	OsQueueCreate()
 */
OsQueuePtr_t OsQueueCreate(OsQueueData_t *dataPtr, uint8_t dataSize,
							uint8_t queueSize, uint8_t *err)
{
#if OS_QUEUE_COUNT > 0

	uint8_t ieReg;
	uint8_t i;

	//checa argumentos:
	if (dataPtr == NULL)
	{
		*err = kOsInvalidParam;
		return (NULL );
	}
	if (dataSize == 0)
	{
		*err = kOsInvalidParam;
		return (NULL );
	}
	if (queueSize == 0)
	{
		*err = kOsInvalidParam;
		return (NULL );
	}

	//checa se temos blocos livres:
	OS_CRITICAL_IN();

	if (queueCount >= OS_QUEUE_COUNT)
	{
		OS_CRITICAL_OUT();
		
		*err = kOsOutOfQueue;
		return (NULL);
	}

	//procura pelo primeiro bloco de queue livre:
	for (i = 0; i < OS_QUEUE_COUNT; i++)
	{
		if (queueBlockTbl[i].queueTaken != TRUE)
		{
			//achou livre
			queueBlockTbl[i].queueTaken = TRUE;
			queueCount++;
			break;
		}
	}

	OS_CRITICAL_OUT();

	//Preenche restante do bloco de controle:
	queueBlockTbl[i].queueData = dataPtr;
	queueBlockTbl[i].queueDataSize = dataSize;
	queueBlockTbl[i].queueEntries = queueSize;
	//Primeiro bloco livre da queue é sempre o indice 0
	queueBlockTbl[i].queueCurrentFree = 0;

	return ((OsQueuePtr_t) &queueBlockTbl[i]);

#endif	 
}

/*
 *	OsQueuePend()
 */
OsStatus_t OsQueuePend(OsQueuePtr_t queue, uint8_t *dataPtr,
						uint8_t *sizeInBytes, uint8_t opt)
{
#if OS_QUEUE_COUNT > 0	

	uint8_t i,tmp;
	uint8_t ieReg;
	uint16_t queueBaseIndex;

	//checa argumentos:
	if (queue == NULL)
		return (kOsInvalidParam);
	if (dataPtr == NULL)
		return (kOsInvalidParam);
	if ((opt != OS_Q_PEND_BLOCK_EMPTY) && (opt != OS_Q_NOT_BLOCK))
		return (kOsInvalidParam);

	OS_CRITICAL_IN();

	//checa se a fifo da queue esta vazia:
	if (queue->queueCurrentEntries == 0)
	{
		//esta vazio, checa se deve bloquear:
		if (opt != OS_Q_NOT_BLOCK)
		{
			//sinaliza qual prio vai aguardar daddos na queue:
			queue->tasksPending[currentTask->prioVal] = OS_Q_PENDING_EMPTY;

			//Suspende execucao da task corrente:
			OsPrioClr(currentTask->prioVal, &osTaskList);
			currentTask->taskState &= ~(1 << kOsTaskRdy);
			currentTask->taskState |= (1 << kOsQueuePend);

			OS_CRITICAL_OUT();

			//pede uma troca de contexto:
			OsTaskYield();

			return (kOsStatusOk);
		}
		//Senao, apenas notifica que a queue esta vazia:
		else
		{
			OS_CRITICAL_OUT();
			return (kOsQueueEmpty);
		}
	}


	//Se tem dado na queue, vamos retirar e copiar
	//para o endereço apontado por dataPtr:

	//calcula o deslocamento no vetor de dados:
	queueBaseIndex = ((queue->queueCurrentFront) * (queue->queueDataSize + 1));
	queue->queueCurrentFront++;
	if(queue->queueCurrentFront > queue->queueEntries)
	{
		//A queue se comporta de forma circular desde que tenhamos slots livres
		queue->queueCurrentFront = 0;
	}
	queue->queueCurrentEntries--;

	OS_CRITICAL_OUT();

	//Toma a quantidade de dados:
	tmp = queue->queueData[queueBaseIndex];
	queueBaseIndex++;
	for (i = 0; i < tmp; i++)
	{
		//copia
		*dataPtr++ = queue->queueData[queueBaseIndex++];
	}

	if(sizeInBytes != NULL)
	{
		*sizeInBytes = tmp;		
	}
	
	OS_CRITICAL_IN();
	OsQueueEmptyLoop(queue);
	OS_CRITICAL_OUT();

	OsTaskYield();	

#endif	
		
	return (kOsStatusOk); 
}

/*
 *	OsQueuePost()
 */
OsStatus_t OsQueuePost(OsQueuePtr_t queue, uint8_t *dataPtr,
						uint8_t sizeInBytes, uint8_t opt)
{
#if OS_QUEUE_COUNT > 0	 

	uint8_t i;
	uint8_t ieReg;
	uint16_t queueBaseIndex;

	//checa argumentos:
	if (queue == NULL)
		return (kOsInvalidParam);
	if (dataPtr == NULL)
		return (kOsInvalidParam);
	if ((opt != OS_Q_POST_BLOCK_FULL) && (opt != OS_Q_NOT_BLOCK))
		return (kOsInvalidParam);
	if (sizeInBytes > queue->queueDataSize)
		return (kOsTooManyData);

	OS_CRITICAL_IN();

	//checa se ha espaço:
	if (queue->queueCurrentEntries == queue->queueEntries)
	{
		//checa se deve bloquear a task:
		if (opt != OS_Q_NOT_BLOCK)
		{
			//se for bloquear, entao retira a task corrente da lista de prontas:
			OsPrioClr(currentTask->prioVal, &osTaskList);
			currentTask->taskState &= ~(1 << kOsTaskRdy);
			currentTask->taskState |= (1 << kOsQueuePend);
			queue->tasksPending[currentTask->prioVal] = OS_Q_PENDING_FULL;

			OS_CRITICAL_OUT();

			OsTaskYield();

			return (kOsStatusOk);
		}
		else
		{
			OS_CRITICAL_OUT();
			//se nao for bloquear entao notifioca que a queue esta cheia:
			return (kOsQueueFull);
		}
	}

	//Se temos espaço entao calcula o deslocamento dentro da area de dados:
	queueBaseIndex = (queue->queueCurrentFree * (queue->queueDataSize + 1));
	queue->queueCurrentFree++;
	if(queue->queueCurrentFree == queue->queueEntries)
	{
		//Se houver espaço livre, faz wrap around:
		if(queue->queueCurrentEntries < queue->queueEntries)
		{
			queue->queueCurrentFree = 0;
		}
	}
	queue->queueCurrentEntries++;

	OS_CRITICAL_OUT();

	//Deposita os dados:
	queue->queueData[queueBaseIndex] = sizeInBytes;
	queueBaseIndex++;

	for (i = 0; i < sizeInBytes; i++)
	{
		queue->queueData[queueBaseIndex++] = *dataPtr++;
	}

	//Depositou, notifica que a queue nao esta mais vaizia (se estiver)
	OS_CRITICAL_IN();
	OsQueueFullLoop(queue);
	OS_CRITICAL_OUT();


	OsTaskYield();	


#endif	

	return (kOsStatusOk); 
}

/*
 *	OsQueueGetStatus()
 */
OsStatus_t OsQueueGetStatus(OsQueuePtr_t queue, uint8_t *freeEntries)
{
#if OS_QUEUE_COUNT > 0	 	
	uint8_t ieReg;
	
	//checa argumentos:
	if(queue == NULL)return(kOsInvalidParam);
	
	//checa se a queue nao esta cheia:
	OS_CRITICAL_IN();
	if(queue->queueCurrentEntries == queue->queueEntries)
	{
		
		OS_CRITICAL_OUT();
		
		*freeEntries = 0;
		return(kOsQueueFull);
	}
	
	//se esta vazia:
	if(queue->queueCurrentEntries == 0)
	{
		
		OS_CRITICAL_OUT();
		
		*freeEntries = queue->queueEntries;
		return(kOsQueueEmpty);
	}
	
	//entao pode esta com n-Entries livres:
	
	*freeEntries = (queue->queueEntries - queue->queueCurrentEntries);
	OS_CRITICAL_OUT();

#endif	
	return(kOsStatusOk);	
}


/*
 *	OsQueueDelete()
 */
OsStatus_t OsQueueDelete(OsQueuePtr_t queue)
{
#if OS_QUEUE_COUNT > 0	
	uint8_t ieReg;

	//checa argumentos:
	if (queue == NULL)
		return (kOsInvalidParam);

	//Libera qualquer evento associado a esse queue:
	OS_CRITICAL_IN();
	OsQueueDeleteLoop(queue);

	//Devolve esse bloco:
	queue->queueTaken = FALSE;
	queueCount--;

	queue->queueData = NULL;

	//Destroi referencia desse queue.
	queue = NULL;
	OS_CRITICAL_OUT();

	OsTaskYield();

#endif	

	return (kOsStatusOk); 	 
}
