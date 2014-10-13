/*
 *		AmebOs - Um simples kernel multitarefa, preemptivo de tempo real para microcontroladores
 *				 de 8bits
 *
 *		Autor: Felipe Neves
 */

/*
 * Prototipos:
 */

/*!
 *  OsDelayTicks()
 *
 *	 \brief Suspende a execuçao da task corrente por um numero de ticks
 *  \param
 *  \return
 */
OsStatus_t OsDelayTicks(uint8_t ticks);

/*!
 *  OsDelayBaseTime()
 *
 *	 \brief Suspende a execuçao da task corrente por um multiplo inteiro da base de tempo
 *  \param
 *  \return
 */
OsStatus_t OsDelayMs(uint8_t milis);
