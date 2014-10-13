/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */
#ifndef OS_CONFIG_H_
#define OS_CONFIG_H_

/*
 Especificos da maquina:
 */
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/*
 Constantes de configuracao do rtos:
 */

#define OS_TICK					 100		 //Tick do rtos em Hz
#define OS_CPU_RATE 		(uint32_t)4000000 //clock da CPU em Hz
/*
 tasks
 */
#define  OS_NUMBER_OF_TASKS		  (uint8_t)3//Numero de tasks que o rtos aceita (tem que ser obrigatoriamente > 1)
/*
 timers & delays
 */
#define  OS_TIME_BASE		     (uint8_t)(1000/(OS_TICK)) //base de tempo do sistema em ms
/*
 flags
 */
#define  OS_FLAGS_COUNT			 0		//Numero de objetos para flags de eventos
/*
 queues
 */
#define OS_QUEUE_COUNT      	 1 	//Numero de blocos de controle de queueing
#endif
