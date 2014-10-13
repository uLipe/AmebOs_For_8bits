;
;		AmebOS - um simples kernel multitarefa preemptivo de Tempo Real para microcontroladores
;				 de 8 bits
;
;		Autor: Felipe Neves
;

				#include <avr/io.h>
			
				;exporta simbolos desejados:
				.extern currentTask
				.extern highPrioTask

				;Deixa as seguintes funcoes publicas:					
				.global OsCriticalIn
				.global OsCriticalExit
				.global OsFastSwt
				.global OsSaveCtx
				.global OsRestoreCtx 
				.global OsFirstSwt
				.global OsCtxSwt
							
				.section .text

;
;	OsCriticalIn()
;
OsCriticalIn:
				lds r24, SREG	;toma o sreg corrente
				cli				;derruba interrupts	
				ret				;
				

;
;	OsCriticalOut()
;
OsCriticalExit:
				sts SREG, r24		;Retoma estado anterior dos interrupts
				ret					;
			  	
			

;
;	OsFirstSwit
;

OsFirstSwt:		

				ldi r30,  lo8(highPrioTask)	;Toma o endereço da primeira task pronta:
				ldi r31,  hi8(highPrioTask); 
				
				ldi r28, lo8(currentTask)	;Toma o endereço de onde vai depositar
				ldi r29, hi8(currentTask)	;o endereço da task corrente.

				ldd	r16, Z+0				;
				ldd r17, Z+1				;
				movw r30, r16				;Esta com o endereço do tcb
				ldd r16, Z+0				;
				ldd r17, Z+1				;toma o stack corrente
				sts SPL, r16				;
				sts SPH, r17				;

				ldi r30,  lo8(highPrioTask)	;Toma o endereço da primeira task pronta
				ldi r31,  hi8(highPrioTask)	;novamente
				ldd r16,  Z+0				;
				ldd r17,  Z+1				;
				std Y+0,  r16				;
				std Y+1,  r17				;O task corrente tbm é o de mais alta prioridade

											;toma o estado inicial de todos os registradores:
				pop r0						;
				pop r1						;
				pop r2						;
				pop r3						;
				pop r4						;
				pop r5						;
				pop r6						;
				pop r7						;
				pop r8						;
				pop r9						;
				pop r10						;
				pop r11						;
				pop r12						;
				pop r13						;
				pop r14						;
				pop r15						;
				pop r16						;
				pop r17						;
				pop r18						;
				pop r19						;
				pop r20						;
				pop r21						;
				pop r22						;
				pop r23						;
				pop r24						;
				pop r25						;
				pop r26						;
				pop r27						;
				pop r28						;
				pop r29						;
				pop r30						;
				clr	r1						;
				sts SREG, r30				;						
				pop r30						;
				pop r31						;
								
				ret							; o retorno devera estar no pc incial que esta ainda na pilha.	
				 


;
;OsSaveCtx
;
OsFastSwt:

				ldi r18,  lo8(highPrioTask)	;Toma o endereço da primeira task pronta:
				ldi r19,  hi8(highPrioTask)	; 
				
				ldi r24, lo8(currentTask)	;Toma o endereço de onde vai depositar
				ldi r25, hi8(currentTask)	;o endereço da task corrente.			
				
				movw r30, r18				;
				movw r28, r24				;

				ldd r16, Y+0				;
				ldd r17, Y+1				;
				movw r28, r16				;
				lds  r16, SPL				;
				lds  r17, SPH				;Toma o stack corrente
				std  Y+0, r16				;e salva no tcb 
				std  Y+1, r17				;
			
				ldd  r16, Z+0				;
				ldd  r17, Z+1				;
				movw r30, r16				;
				ldd  r16, Z+0				;
				ldd  r17, Z+1				;Toma o endereço do stack corrente;
				sts  SPL, r16				;Carrega o novo stackpointer
				sts  SPH, r17				;	
			
				movw r30, r18				;
				ldd  r16, Z+0				;
				ldd  r17, Z+1				;
				movw r28, r24				;Toma o endereço do task corrente.
				std  Y+0, r16				;
				std  Y+1, r17				;Assim o task mais prioritario tbm é o task corrente


				ret							;
											

;
;OsSaveCtx
;
OsSaveCtx:
				ret							;

;
;OsRestoreCtx
;
OsRestoreCtx:
				
				ret							; o retorno devera estar no pc que esta ainda na pilha.	

;
;OsCtxSwt
;																					

OsCtxSwt:
											;Salva o contexo atual
				push	r31					;
				push	r30					;
				lds     r30, SREG			;
				cli							;
				push    r30
				push	r29					;
				push	r28					;
				push    r27					;
				push    r26					;
				push    r25					;
				push    r24					;
				push	r23					;
				push    r22					;
				push    r21					;
				push	r20					;
				push    r19					;
				push    r18					;
				push	r17					;
				push	r16					;
				push    r15					;
				push    r14					;
				push    r13					;
				push    r12					;
				push	r11					;
				push    r10					;
				push    r9					;
				push	r8					;
				push    r7					;
				push    r6					;
				push    r5					;
				push	r4					;
				push    r3					;
				push    r2					;					
				push	r1					;
				push    r0					;


				
				ldi r18,  lo8(highPrioTask)	;Toma o endereço da primeira task pronta:
				ldi r19,  hi8(highPrioTask)	; 
				
				ldi r24, lo8(currentTask)	;Toma o endereço de onde vai depositar
				ldi r25, hi8(currentTask)	;o endereço da task corrente.			
				
				movw r30, r18				;
				movw r28, r24				;

				ldd r16, Y+0				;
				ldd r17, Y+1				;
				movw r28, r16				;
				lds  r16, SPL				;
				lds  r17, SPH				;Toma o stack corrente
				std  Y+0, r16				;e salva no tcb 
				std  Y+1, r17				;
			
				ldd  r16, Z+0				;
				ldd  r17, Z+1				;
				movw r30, r16				;
				ldd  r16, Z+0				;
				ldd  r17, Z+1				;Toma o endereço do stack corrente;
				sts  SPL, r16				;Carrega o novo stackpointer
				sts  SPH, r17				;	
			
				movw r30, r18				;
				ldd  r16, Z+0				;
				ldd  r17, Z+1				;
				movw r28, r24				;Toma o endereço do task corrente.
				std  Y+0, r16				;
				std  Y+1, r17				;Assim o task mais prioritario tbm é o task corrente


				pop r0						;
				pop r1						;
				pop r2						;
				pop r3						;
				pop r4						;
				pop r5						;
				pop r6						;
				pop r7						;
				pop r8						;
				pop r9						;
				pop r10						;
				pop r11						;
				pop r12						;
				pop r13						;
				pop r14						;
				pop r15						;
				pop r16						;
				pop r17						;
				pop r18						;
				pop r19						;
				pop r20						;
				pop r21						;
				pop r22						;
				pop r23						;
				pop r24						;
				pop r25						;
				pop r26						;
				pop r27						;
				pop r28						;
				pop r29						;
				pop r30						;
				clr	r1						;
				sts SREG, r30				;						
				pop r30						;
				pop r31						;

											;restaura o contexto da nova task:
											;toma o estado inicial de todos os registradores:
				ret
			
.end															 