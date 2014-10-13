/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */
#ifndef OS_TINY_QUEUE_H
#define OS_TINY_QUEUE_H

/*
 *	 Constantes uteis
 */
#define OS_Q_PEND_BLOCK_EMPTY 		0x01
#define OS_Q_POST_BLOCK_FULL  		0x02
#define OS_Q_NOT_BLOCK				0x03

/*
 *		Macro para reservar espaço para o queue data:
 */

#define OS_QUEUE_RESERVE(name, DATASIZE, NUMENTRIES) OsQueueData_t name[(DATASIZE + 1) * NUMENTRIES]

/*!
 *	OsQueueInit()
 * \brief Inicializa kernel objects para uso com queues
 * \param
 * \return
 */
void OsQueueInit(void);

/*!
 *	OsQueueCreate()
 * \brief Aloca um bloco de controle de queue para uso
 * \param
 * \return
 */
OsQueuePtr_t OsQueueCreate(OsQueueData_t *dataPtr, uint8_t dataSize,
							uint8_t queueSize, uint8_t *err);

/*!
 *	OsQueuePend()
 * \brief Retira uma entry da queue, opcionalmente a task pode ser bloqueada se ela estiver vazia
 * \param
 * \return
 */
OsStatus_t OsQueuePend(OsQueuePtr_t queue, uint8_t *dataPtr,
						uint8_t *sizeInBytes, uint8_t opt);

/*!
 *	OsQueuePost()
 * \brief Enfila uma stream de dados na queue, opcionalmente bloqueia a task se a queue estiver cheia
 * \param
 * \return
 */
OsStatus_t OsQueuePost(OsQueuePtr_t queue, uint8_t *dataPtr,
						uint8_t sizeInBytes, uint8_t opt);
/*!
 *	OsQueueGetStatus()
 * \brief Pede o status da queue, se esta cheio, vazio ou ok, nesse ultimo retorna o numero de bytes livres 
 * \param
 * \return
 */
OsStatus_t OsQueueGetStatus(OsQueuePtr_t queue, uint8_t *freeEntries);

						
						

/*!
 *	OsQueueDelete()
 * \brief Libera um kernel object para queue
 * \param
 * \return
 */
OsStatus_t OsQueueDelete(OsQueuePtr_t queue);

#endif
