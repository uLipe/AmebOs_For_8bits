;
;		AmebOS - um simples kernel multitarefa de Tempo Real para 8051
;
;
;		Autor: Felipe Neves
;
				;#include "C8051F580.inc"
			
				;exporta simbolos desejados:
				EXTRN XDATA(currentTask)
				EXTRN XDATA(highPrioTask)

				;Deixa as seguintes funcoes publicas:					
				PUBLIC __OsCriticalIn,__OsCriticalExit, OsFirstSwt, OsCtxSwt
							
				PORT_A SEGMENT CODE
				RSEG PORT_A


;
;	OsCriticalIn()
;

__OsCriticalIn:
			mov  r7, Ie			;salva contexto das interrupcoes
			clr  Ea				;derruba interrupts globais
			clr  Ea				;
			ret						;


;
;	OsCriticalOut()
;
__OsCriticalExit:
;			mov  a,r3			;restaura contexto das interrupcoes globais
;			mov  Ie, a
			setb Ea				;	 
			ret					;
			

;
;	OsFirstSwit
;

OsFirstSwt:
		
			mov  Dptr, #highPrioTask + 1	;pega a primeira tarefa pronta pra rodar:
			movx a, @Dptr							;	
			mov  r6,a								;
			inc  Dptr								;
			movx a, @Dptr							;
			mov  r7, a								;
			mov  Dpl, r7							;
			mov  Dph, r6							;
			inc  Dptr								;
			movx a, @Dptr							;
			mov  B,a								;
			inc  Dptr								;
			movx a, @Dptr							;
			mov  Dpl, a								;
			mov  Dph, B								;acessa a primeira posicao do TCB, que eh o stack corrente 										
;			inc  Dptr								;
			
			mov  Sp, #08h							;
			
			movx a, @dptr							;toma o tamanho do stack frame inciial:
			mov  r7, a								;
		    inc  Dptr								;

LoadCtxLoop:										;carrega todos os elementos no stack:
			movx a, @dptr							;
			push Acc										;
			inc  dptr									;
			djnz r7, LoadCtxLoop			;

			mov  dptr, #highPrioTask + 1		;task mais prioritario e o task corrente
			movx  a, @dptr							;
			mov   r6, a									;
			inc   dptr									;
			movx  a, @dptr					    		;
			mov   r7, a									;
			mov  dptr, #currentTask          			;
			mov  a, #01h
			movx @dptr, a	
			inc dptr
			mov  a, r6									;
			movx @dptr, a								;
			inc  dptr										;
			mov  a, r7									;
			movx @dptr, a								; 
				
			pop  Psw									; Restaurou todo o primeiro contexto		;
			pop  7										;
			pop	 6										;
			pop	 5										;
			pop	 4										;
			pop  3										;
			pop  2										;
			pop	 1										;
			pop  0										;
			pop  Dph									;
			pop  Dpl									;
			pop  B										;
			pop  Acc										;
			setb Ea										;
		
																; deveria retornar do stack da task corrente
			ret
			
;
;OsCtxSwt
;													;								

OsCtxSwt:
			
														;salva todo o contexto necessario:
			push	Acc									;
			push    B									;
			push    Dpl									;
			push    Dph									;
			push    0									;
			push	1									;
			push	2									;
			push	3									;
			push	4									;
			push	5									;
			push	6									;
			push	7									;
			push    Psw									;
			mov  dptr, #currentTask + 1		            ;
			movx a, @dptr							    ;
			mov  r6,a			 						;
			inc  dptr									;
			movx a, @dptr							;
			mov  r7,a									; acessa o tcb da task corrente.
			mov  dpl, r7							;
			mov  dph, r6							;
			inc  dptr
			movx a, @dptr							;
			mov  r6,a								;
			inc  dptr								;
			movx a, @dptr							; acessa o stack do tcb corrente								;
			mov  r7,a								;			
			mov  Dph, r6							; 
			mov  Dpl, r7							;
			mov  a, Sp								;
			subb a, #08h							; 
			mov  r0, a								;
			movx @dptr, a							; Guarda o stack usado corrente no fundo da pilha
			add  a, r7								; da task
			mov  r7, a								;
			clr  a									;
			addc a,r6								;
			mov  r6, a								;
			mov  Dph, r6							;
			mov  Dpl, r7							;
;			inc	 Dptr								;
			mov  a, r0								;
			movx @dptr, a							;
			inc  dptr								;
			
			mov  r6, Dph							;Aponta para o topo da top of stack
			mov  r7, Dpl							;
													;decremento do dptr somente na força bruta
			clr  a									;usando uma subtração de 16bits
			mov  a, r7								;
			subb a, #01h							;
			mov  r7, a								;
			mov  a, r6								;
			subb a, #00h							;
			mov  r6, a								;
			mov  Dph, r6							;
			mov  Dpl, r7							;
			
SaveLoop:											; salva todo o contexto
			pop  Acc								; retirando todo o conteudo do stack
			movx @dptr, a							;
			clr  a
			mov  a, r7								;
			subb a, #01h							;
			mov  r7, a								;
			mov  a, r6								;
			subb a, #00h							; e guardando no sw stack from top to bottom
			mov  r6, a								;
			mov  Dph, r6							; 
			mov  Dpl, r7
			djnz r0, SaveLoop					    ; checa se esvaziou toda a pilha:
				
			mov  dptr, #highPrioTask + 1	        ;carrega o contexto da nova task pronta pra rodar:									
			movx a, @dptr							;
			mov  r6, a								;
			inc  dptr								;
			movx a, @dptr							;
			mov  r7, a								;
			mov  dpl, r7							;
			mov  dph, r6							;
			inc  dptr								;
			movx a, @dptr							;
			mov  B,a								;
			inc  dptr								;
			movx a, @dptr							;
			mov  dpl, a								;
			mov  dph, B								; acessa o stack da task de alta prioridade:
			movx a,@dptr 							; toma a quantidade de bytes que deve carregar
			mov  r7, a								; uma vez que o ponteiro da pilha sempre
			inc  dptr								; aponta pro bottom to sw stack
LoadLoop:
			movx a, @dptr							; Copia do sw stack para o hardware
			push Acc								; stack em modo from bottom to top
			inc  dptr								;
			djnz r7, LoadLoop						; Nesse momento o Sp aponta para contexto da task
			
			mov dptr, #highPrioTask + 1				; copiou todomundo move o endereco da task mais prioritaria
			movx a,@dptr							; para a task corrente
			mov  r6, a								;
			inc  dptr								;
			movx a,@dptr							;
			mov  r7, a								;
			
			mov dptr, #currentTask + 1  			;
			mov a, r6								;
			movx @dptr,a							;
			inc  dptr								;
			mov a,r7								;
			movx @dptr, a							; o task corrente agora é o mesmo task high prio
													; finalmente restaura o contexto:
														
			pop  Psw								;														
			pop	 7									;	
		    pop	 6									;	
			pop	 5									;
			pop  4									;
			pop	 3									;
			pop  2									;
			pop  1									;
			pop  0									;
			pop  Dph								;
			pop  Dpl								;
			pop  B									;
			pop  Acc								;
			
			ret												;		
			
			
end															 