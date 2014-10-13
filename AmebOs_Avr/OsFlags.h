/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */
 
 #ifndef OS_FLAGS_H
 #define OS_FLAGS_H
 
/*
 *	Opcoes para a funcao OsFlagsPend
 */
 #define OS_FLAGS_SET_ALL_CONSUME  (uint8_t)0x03
 #define OS_FLAGS_SET_ANY_CONSUME  (uint8_t)0x04
 
 /*
  *	 Flags proto 
	*/
 
 /*!
  *		OsFlagsInit() 
	*		\brief inicializa kernel objects dos flags de eventos
	*	  \param
	*   \return
	*/
 void OsFlagsInit(void);

 /*!
  *		OsFlagsCreate() 
	*		\brief instancia um objeto grupo de flags
	*	  \param
	*   \return
	*/
 OsFlagsPtr_t OsFlagsCreate(uint8_t *err);
 
/*!
  *		OsFlagsPend() 
	*		\brief Suspende a task em execução ate que um grupo de flags esteja em on
	*	  \param
	*   \return
	*/
 OsStatus_t OsFlagsPend(OsFlagsPtr_t flagGrp, uint16_t flags, uint8_t opt, uint8_t timeout);

 /*!
  *		 OsFlagsPost() 
	*		\brief atualiza grupo de flags e coloca na ready list a task mais prioritaria que tiver o grupo atendido
	*	  \param
	*   \return
	*/
 OsStatus_t OsFlagsPost(OsFlagsPtr_t flagGrp, uint16_t flags);
 
 /*!
  *		 OsFlagsDelete() 
	*		\brief Desinstala um grupo de flags.
	*	  \param
	*   \return
	*/
 OsStatus_t OsFlagsDelete(OsFlagsPtr_t flagGrp);
 
 
 
 
 
 
 
 
 
 
 
 #endif