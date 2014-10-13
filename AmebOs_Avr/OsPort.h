/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */

#ifndef OS_PORT_H_
#define OS_PORT_H_


/*
 Macros para critical section:
 */
#define OS_CRITICAL_IN()   ieReg = OsCriticalIn()
#define OS_CRITICAL_OUT()  OsCriticalExit(ieReg)




/*
 Estrutura de contexto para 8051
 */

struct ctx_
{
	uint8_t r0;
	uint8_t r1;
	uint8_t r2;
	uint8_t r3;
	uint8_t r4;
	uint8_t r5;
	uint8_t r6;
	uint8_t r7;
	uint8_t r8;
	uint8_t r9;
	uint8_t r10;
	uint8_t r11;
	uint8_t r12;
	uint8_t r13;
	uint8_t r14;
	uint8_t r15;
	uint8_t r16;
	uint8_t r17;	
	uint8_t r18;
	uint8_t r19;
	uint8_t r20;
	uint8_t r21;
	uint8_t r22;
	uint8_t r23;
	uint8_t r24;
	uint8_t r25;
	uint16_t xReg;
	uint16_t yReg;
	uint8_t  sReg;
	uint16_t zReg;	
	uint8_t pch;
	uint8_t pcl;
};

typedef struct ctx_ AvrRegList_t;
typedef struct ctx_* AvrRegListPtr_t;

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
extern uint8_t OsCriticalIn(void);

/*!
 *	OsCriticalOut()
 * \brief Finaliza secao de codigo critico
 *
 * \param
 * \return
 */
extern void OsCriticalExit(uint8_t ieReg);



/*!
 *	OsSaveCtx()
 * \brief 
 *
 * \param
 * \return
 */
extern void OsFastSwt(void);

/*!
 *	OsSaveCtx()
 * \brief 
 *
 * \param
 * \return
 */
extern void OsSaveCtx(void);

/*!
 *	OsRestoreCtx()
 * \brief 
 *
 * \param
 * \return
 */
extern void OsRestoreCtx(void);


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
