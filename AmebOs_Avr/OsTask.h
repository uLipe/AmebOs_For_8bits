/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */
#ifndef OS_TASK_H_
#define OS_TASK_H_

/*
 *	Prototipos
 */

/*!
 *	 OsTaskInit()
 *	 \brief inicializa task kernel objects
 *  \param
 *  \return
 */
void OsTaskInit(void);

/*!
 *	  OsTaskCreate()
 *	 \brief Installa uma task
 *  \param
 *  \return
 */
OsStatus_t OsTaskCreate(void (*task)(), OsStack_t * stk, uint16_t stkSize, uint8_t osPrio,
						void *taskArgs);

/*!
 *	 OsTaskDelete()
 *	 \brief Desinstala uma task
 *  \param
 *  \return
 */
OsStatus_t OsTaskDelete(uint8_t osPrio);

/*!
 *	 OsTaskSuspend()
 *	 \brief suspende a execuç~~ao de uma task
 *  \param
 *  \return
 */
OsStatus_t OsTaskSuspend(uint8_t osPrio);

/*!
 *	 OsTaskResume()
 *	 \brief retoma a execucao de uma task
 *  \param
 *  \return
 */
OsStatus_t OsTaskResume(uint8_t osPrio);

#endif
