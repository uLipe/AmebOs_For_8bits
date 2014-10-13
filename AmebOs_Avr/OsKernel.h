/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */

#ifndef OS_KERNEL_H_
#define OS_KERNEL_H_

/*
 Constantes
 */

#define IDLE_TASK_STACK_SIZE (uint8_t)64 //tamanho em bytes da pilha da tarefa idle.
/*!
 *	OsInit()
 *	\brief inicializa todos os kernel objects do rtos
 *	\param 
 *	\return
 */
void OsInit(void);

/*!
 *	OsStart()
 *	\brief entrega o controle da aplicacao ao RTOS
 *	\param 
 *	\return
 */
OsStatus_t OsStart(void);

/*!
 *	 OsIrqIn()
 *	\brief notifica ao rtos que a aplicacao esta em uma IRQ
 *	\param 
 *	\return
 */
void OsIrqIn(void);

/*!
 *	 OsIrqExit()
 *	\brief notifica ao rtos que a aplicacao esta saindo de uma IRQ e escolhe uma nova task para rodar
 *	\param 
 *	\return
 */
void OsIrqExit(void);

/*!
 *	 OsFindHighPrioRdy()
 *	\brief Decodifica uma priolist e retorna a maior prioridade pronta (maior o valor, mais alta a prio)
 *	\param 
 *	\return
 */
uint8_t OsFindHighPrioRdy(OsPrioListPtr_t prioList);

/*!
 *	 OsTaskYield()
 *	\brief seleciona uma nova task para rodar e pede troca de contexto
 *	\param 
 *	\return
 */
void OsTaskYield(void);


/*!
 *	 OsTaskYield()
 *	\brief seleciona uma nova task para rodar e pede troca de contexto
 *	\param 
 *	\return
 */
void OsTaskYieldFromIsr(void);

/*!
 *	 OsPrioSet()
 *	\brief coloca uma prioridade em ready em determinada prioList
 *	\param 
 *	\return
 */
OsStatus_t OsPrioSet(uint8_t prio, OsPrioListPtr_t prioList);

/*!
 *	 OsPrioClr()
 *	\brief retira de uma priolist um valor de prioridade
 *	\param 
 *	\return
 */
OsStatus_t OsPrioClr(uint8_t prio, OsPrioListPtr_t prioList);

/*!
 *	 OsTick()
 *	\brief  Trata o tick do sistema.
 *	\param 
 *	\return
 */
void OsTick(void);

#endif

