/*
 *		AmebOs - Um simples kernel multitarefa de tempo real para 8051
 *
 *
 *		Autor: Felipe Neves
 */

#ifndef OS_PORT_H_
#define OS_PORT_H_

/*
 Macros para critical section:
 */
#define OS_CRITICAL_IN()   ieReg = __OsCriticalIn()
#define OS_CRITICAL_OUT()  __OsCriticalExit()

/*
 Estrutura de contexto para 8051
 */

struct ctx_
{
	uint8_t Sp;
	uint8_t pcl;
	uint8_t pch;
	uint8_t a;
	uint8_t b;
	uint8_t Dpl;
	uint8_t Dph;
	uint8_t r0;
	uint8_t r1;
	uint8_t r2;
	uint8_t r3;
	uint8_t r4;
	uint8_t r5;
	uint8_t r6;
	uint8_t r7;
	uint8_t psw;
};

typedef struct ctx_ c51RegList_t;
typedef struct ctx_* c51RegListPtr_t;

/*!
 *	OsInitTickTimer()
 * \brief inicializa timer2 que é usado como base de tempo.
 *
 * \param
 * \return
 */
void OsInitTickTimer(void);

/*!
 *	 OsStackInit()
 * \brief inicializa pilha de uma tarefa
 *
 * \param
 * \return
 */
OsStack_t *OsStackInit(OsStack_t *taskStack, void (*task)(void), void *args);

//As funcoes abaixo podem ser apreciadas em OsPort_a.a51

/*!
 *	 OsCriticalIn()
 * \brief Cria uma secao de codigo critica.
 *
 * \param
 * \return
 */
extern uint8_t __OsCriticalIn(void);

/*!
 *	OsCriticalOut()
 * \brief Finaliza secao de codigo critico
 *
 * \param
 * \return
 */
extern void __OsCriticalExit(void);

/*!
 *	OsCtxSwt()
 * \brief efetua troca de contexto e entre duas tarefas, usado na primeira troca
 *
 * \param
 * \return
 */
extern void OsFirstSwt(void);

/*!
 *	OsCtxSwt()
 * \brief efetua troca de contexto e entre duas tarefas
 *
 * \param
 * \return
 */
extern void OsCtxSwt(void);

#endif
