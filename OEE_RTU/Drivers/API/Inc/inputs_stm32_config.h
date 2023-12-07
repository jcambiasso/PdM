/**
 * @file inputs_stm32_config.h
 * @author Javier Cambiasso
 * @brief STM32 config file for inputs.h
 * @version 0.1
 * @date 2023-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef API_INC_INPUTS_STM32_CONFIG_H_
#define API_INC_INPUTS_STM32_CONFIG_H_

#include <inputs.h>
#include "stm32f4xx_hal.h"

/**
 * @brief Estructura para definir un pin específico según HAL de STM32F4XX
 */
typedef struct{
	GPIO_TypeDef* port;						/*!<Puerto definido por STM32 HAL para la entrada   */
	uint16_t pin;							/*!<Pin correspondiente al puerto   */
} input_stm32_t;

/**
 * Este array define los puertos y pins que oficiaran de entradas en la solución. El resto de la libreria los referencia por índice.
 * Así, la entrada 0 es inputs[0], la 1 inputs[1], etc.
 */

const input_stm32_t inputs[] = {
		{.port = GPIOC, .pin = GPIO_PIN_13},
		{0},
		{0},
		{0},
		{0}
};

void EXTI15_10_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);


#endif /* API_INC_INPUTS_STM32_CONFIG_H_ */
