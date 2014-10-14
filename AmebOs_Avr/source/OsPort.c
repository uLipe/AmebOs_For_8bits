/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */

#include "OsBase.h"

/*
 Constantes
 */
#define OS_TIMER_LOAD_VAL  (uint16_t)(OS_CPU_RATE/256/OS_TICK)

/*
 Implementatacao deste modulo:
 */

/*
 *	OsInitTickTimer()
 */
void OsInitTickTimer(void)
{
	//Acerta o timer2 para gerar o tick do sistema:
	ASSR  &= ~(1 << AS2);
	GTCCR = 0x00;
	TCCR2A = 0x02;
	
	//timer 2 por enquanto desativado:
	TCCR2B = 0x00;
	
	//Acerta periodo de comparação do timer2:
	TCNT2 = 0x00;
	OCR2A = OS_TIMER_LOAD_VAL;
	
	//Habilita interrupcao do timer2:
	TIFR2  = 0x00;
	TIMSK2 = (1 << OCIE2A);
	
	//TImer configurado, dispara contagem:
	TCCR2B |= 0x06;

}

/*
 *	 OsStackInit()
 */
OsStack_t *OsStackInit(OsStack_t *taskStack, void (*task)(void), void *args)
{
	AvrRegListPtr_t ptr = NULL;
	uint16_t taskAdr = 0;
	
	
	taskAdr = (uint16_t)task;
	
	
	//Pega o proximo valor livre na pilha:
	ptr = (AvrRegListPtr_t) (taskStack - 1);
	ptr--;
	
	//Preenche o contexto:
	ptr->pch   = (uint16_t)taskAdr >> 8;
	ptr->pcl  =  (uint16_t)taskAdr & 0xFF;
	ptr->zReg = 0x3030;
	ptr->sReg = 0x80;
	ptr->xReg = 0x2626;
	ptr->yReg = 0x2828;
	ptr->r24  = (uint8_t)((uint16_t)args & 0xFF);
	ptr->r25  = (uint8_t)((uint16_t)args >> 8);
	ptr->r22  = 0x00;
	ptr->r23  = 0x00;
	ptr->r20  = 0x00;
	ptr->r21  = 0x00;
	ptr->r18  = 0x00;
	ptr->r19  = 0x00;
	ptr->r16  = 0x00;
	ptr->r17  = 0x00;
	ptr->r14  = 0x00;
	ptr->r15  = 0x00;
	ptr->r12  = 0x00;
	ptr->r13  = 0x00;
	ptr->r10  = 0x00;
	ptr->r11  = 0x00;	
	ptr->r8  = 0x00;
	ptr->r9  = 0x00;
	ptr->r6  = 0x00;
	ptr->r7  = 0x00;
	ptr->r4  = 0x00;
	ptr->r5  = 0x00;			
	ptr->r2  = 0x00;
	ptr->r3  = 0x00;	
	ptr->r0   = 0x00;
	ptr->r1   = 0x00;
	
	
	return (((OsStack_t *) ptr) - 1 );
}

/*
 *	ISR tick do sistema
 */
ISR(TIMER2_COMPA_vect, ISR_NAKED) 
{
	
		
	OsTick();
	
#ifdef DEBUG
	PORTC ^= 0x10;
#endif	
		
	asm volatile( "	reti\n");	
}