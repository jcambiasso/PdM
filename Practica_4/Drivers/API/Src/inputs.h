/*
 * inputs.h
 *
 *  Created on: Nov 9, 2023
 *      Author: Javier Cambiasso
 */

#ifndef API_SRC_INPUTS_H_
#define API_SRC_INPUTS_H_

#include "stm32f4xx_hal.h"
#include <stddef.h>
#include <stdint.h>
#include <API_delay.h>

/**
 * @brief Estados posibles de la entrada
 */
typedef enum{
	BUTTON_HIGH, 		/*!<Indica que la entrada está en un nivel lógica alto   */
	BUTTON_FALLING,		/*!<<Indica que la entrada está transicionando a un nivel bajo   */
	BUTTON_LOW,			/*!<Indica que la entrada está en un nivel lógica bajo    */
	BUTTON_RISING,		/*!<Indica que la entrada está transicionando a un nivel alto    */
} debounceState_t;

/**
 * @brief Prototipo de callback para las entradas
 */
typedef void (*inputCallback)(void);

/**
 * @brief Estrucutra para el controlador de debounce para una entrada
 */
typedef struct{
	debounceState_t state;					/*!<Estado actual de la entrada   */
	GPIO_TypeDef* port;						/*!<Puerto definido por STM32 HAL para la entrada   */
	uint16_t pin;							/*!<Pin correspondiente al puerto   */
	delay_t delay;							/*!<Estructura que controla el delay para el debounce   */
	bool currentLevel;						/*!<Nivel lógica actual   */
	bool negativeLogic;						/*!<Determina si la entrada funciona con lógica negativa   */
	inputCallback onButtonPressed;			/*!<Callback para cuando se aserta el evento de entrada en nivel lógico alto   */
	inputCallback onButtonReleased;			/*!<Callback para cuando se aserta el evento de entrada en nivel lógico bajo    */
} inputDebounce_t;

/**
 *	@brief Inicializa el controlador de debounce para el puerto y pin definido en @ref input
 *	@note Esta función no inicializa la entrada per se.
 *	@param input Puntero a estrucutra de controlador de debounce
 *	@param debounceTime Tiempo de debounce
 *	@param negativeLogic Define si la entrada funciona en lógica negativa
 *	@reval None
 */
void debounceFSM_init(inputDebounce_t* input, uint32_t debounceTime, bool negativeLogic);
/**
 *	@brief Función para ser llamada en un loop que actualiza el estado del debounce de una entrada y ejecuta los callbacks según corresponde.
 *	@param input Puntero a estrucutra de controlador de debounce
 *	@reval false La entrada esta en nivel bajo despues del debounce.
 */
bool debounceFSM_update(inputDebounce_t*input);
/**
 *	@brief Función que devuelve el estado del pin indicado por @ref input
 *	@param input Puntero a estrucutra de controlador de debounce
 *	@reval true La entrada esta en nivel alto.
 *	@reval false La entrada esta en nivel bajo.
 */
bool readButton(inputDebounce_t* input);

#endif /* API_SRC_INPUTS_H_ */
