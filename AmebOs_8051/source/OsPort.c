/*
 *		AmebOs - Um simples kernel multitarefa de tempo real para 8051
 *
 *
 *		Autor: Felipe Neves
 */

#include "OsBase.h"

/*
 Constantes
 */
#define OS_TIMER_LOAD_VAL  (uint16_t)((OS_CPU_RATE/12)/OS_TICK)

/*
 Implementatacao deste modulo:
 */

/*
 *	OsInitTickTimer()
 */
void OsInitTickTimer(void)
{
	uint8_t currPage;

	currPage = SFRPAGE;

	//Muda pagina dos SFRs:
	SFRPAGE = ACTIVE_PAGE;

	//Configura o timer2 como contador e auto-reload:
	TMR2CN = 0x00;
	TMR2CN |= (1 << 5);

	//Faz reset do timer2:
	TMR2 = 0x0000;

	//Passar o valor de reload:
	TMR2RL = OS_TIMER_LOAD_VAL;

	//Dispara a contagem:
	TMR2CN |= (1 << 2);

	SFRPAGE = currPage;
}

/*
 *	 OsStackInit()
 */
OsStack_t *OsStackInit(OsStack_t *taskStack, void (*task)(void), void *args)
{
	c51RegListPtr_t ptr;
	uint16_t taskArgs;
	uint16_t taskAdr;

	taskAdr = (uint16_t) task;

	//Toma o conteuod dos parametros:
	taskArgs = (uint16_t) args;

	//Pega o proximo valor livre na pilha:
	ptr = (c51RegListPtr_t) (taskStack);

	//Preenche o contexto:
	ptr->r0 = 0x00;
	ptr->r1 = (uint8_t) (taskArgs >> 8);
	ptr->r2 = (uint8_t) (taskArgs & 0xFF);
	ptr->r3 = 0x03;
	ptr->r4 = 0x04;
	ptr->r5 = 0x05;
	ptr->r6 = 0x06;
	ptr->r7 = 0x07;
	ptr->Sp = 0x0F;
	ptr->Dph = 0x00;
	ptr->Dpl = 0x00;
	ptr->a = 0x0A;
	ptr->b = 0x0B;
	ptr->psw = 0x00;
	ptr->pch = taskAdr >> 8;
	ptr->pcl = taskAdr & 0xFF;

	return ((OsStack_t *) ptr);

}

/*
 *	ISR tick timer:
 */
INTERRUPT(T2ISR,INTERRUPT_TIMER2)
{
	OsTick();
}
