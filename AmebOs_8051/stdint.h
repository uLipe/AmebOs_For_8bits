/*
 *		AmebOs - Um simples kernel multitarefa de tempo real para 8051
 *
 *
 *		Autor: Felipe Neves
 */

/* Para o C8051 nao existe o header padrao stdint, entao vamos
 * defini-lo de forma manual so que reduzida:
 *
 */

#ifndef _STDINT_H
#define _STDINT_H

typedef unsigned char uint8_t;
typedef signed char int8_t;

typedef unsigned short uint16_t;
typedef signed short int16_t;

typedef unsigned long uint32_t;
typedef signed long int32_t;

#endif

