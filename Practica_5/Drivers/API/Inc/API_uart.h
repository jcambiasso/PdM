/*
 * API_uart.h
 *
 *  Created on: Nov 22, 2023
 *      Author: Javier
 */

#ifndef API_INC_API_UART_H_
#define API_INC_API_UART_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"


typedef bool bool_t;

void USART3_IRQHandler(void);

/**
 * @brief
 */
HAL_UART_StateTypeDef uartGetState(void);

/**
 * @brief
 */
bool_t uartInit(uint32_t baudRate, uint32_t wordLength, uint32_t stopBits, uint32_t parity);

/**
 * @brief
 */
void uartSendString(uint8_t * pstring);

/**
 * @brief
 */
void uartSendStringSize(uint8_t * pstring, uint16_t size);

/**
 * @brief
 */
void uartReceiveStringSize(uint8_t * pstring, uint16_t size);

/**
 * @brief Registra un callback que se ejecuta cuando un error en el driver del UART, después de inicializado.
 * @param callback Un puntero a funcion con el prototipto void (*pUART_CallbackTypeDef)(UART_HandleTypeDef *huart)
 */
void uart_registerReceivedCallback(pUART_RxEventCallbackTypeDef callback);

/**
 * @brief Registra un callback a ejecutarse cuando se termine transmitir los bytes definidos por @ref uartSendStringSize
 * @param callback Un puntero a funcion con el prototipto void (*pUART_CallbackTypeDef)(UART_HandleTypeDef *huart)
 */
void uart_registerTransmitCompleteCallback(pUART_CallbackTypeDef callback);

/**
 * @brief Registra un callback a ejecutar cuando se termine de recibir la cantidad de bytes definidos en @ref uartReceiveStringSize
 * @note
 * @param callback Un puntero a funcion con el prototipto void (*pUART_RxEventCallbackTypeDef)(struct __UART_HandleTypeDef *huart, uint16_t Pos)
 */
void uart_registerErrorCallback(pUART_CallbackTypeDef callback);

#endif /* API_INC_API_UART_H_ */
