/*
 * API_uart.c
 *
 *  Created on: Nov 22, 2023
 *      Author: Javier Cambiasso
 */

#include <API_uart.h>
#include <string.h>

#if USE_HAL_UART_REGISTER_CALLBACKS == 0U
#error "API_UART for STM32F4 requires USE_HAL_UART_REGISTER_CALLBACKS set in hal_conf.h"
#endif

#ifndef HAL_UART_MODULE_ENABLED
#error "API_UART for STM32F4 requires HAL_UART_MODULE_ENABLED defined in hal_conf.h"
#endif

#define UART_RX_Pin GPIO_PIN_8
#define UART_TX_Pin GPIO_PIN_9
#define UART_GPIO_Port GPIOD
#define UART_INSTANCE USART3

static pUART_CallbackTypeDef transmitCompletedCallback = NULL;
static pUART_CallbackTypeDef errorCallback = NULL;
static pUART_RxEventCallbackTypeDef receiveCallback = NULL;
static UART_HandleTypeDef huart;

void uart_registerReceivedCallback(pUART_RxEventCallbackTypeDef callback){
	if(callback != NULL) receiveCallback = callback;
}

void uart_registerTransmitCompleteCallback(pUART_CallbackTypeDef callback){
	if(callback != NULL) transmitCompletedCallback = callback;
}

void uart_registerErrorCallback(pUART_CallbackTypeDef callback){
	if(callback != NULL) errorCallback = callback;
}

static void uartMspInit(UART_HandleTypeDef* huart){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(huart->Instance==USART3){
		__HAL_RCC_USART3_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
	}
	GPIO_InitStruct.Pin = UART_RX_Pin|UART_TX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
	HAL_GPIO_Init(UART_GPIO_Port, &GPIO_InitStruct);
}

static void uartMspDeInit(UART_HandleTypeDef* huart){
	if(huart->Instance==USART3){
		__HAL_RCC_USART3_CLK_DISABLE();
	}
	HAL_GPIO_DeInit(UART_GPIO_Port, UART_RX_Pin|UART_TX_Pin);
}


bool_t uartInit(uint32_t baudRate, uint32_t wordLength, uint32_t stopBits, uint32_t parity){
	bool result = false;
	huart.Instance = UART_INSTANCE;
	if(baudRate != 0 ) huart.Init.BaudRate = baudRate;
	else huart.Init.BaudRate = 115200;
	if(wordLength!= 0 ) huart.Init.WordLength = wordLength;
	else huart.Init.WordLength = UART_WORDLENGTH_8B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_NONE;
	huart.Init.Mode = UART_MODE_TX_RX;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_RegisterCallback(&huart, HAL_UART_MSPINIT_CB_ID, uartMspInit);
	HAL_UART_RegisterCallback(&huart, HAL_UART_MSPDEINIT_CB_ID, uartMspDeInit);
	if (HAL_UART_Init(&huart) == HAL_OK)
	{
		HAL_UART_RegisterCallback(&huart, HAL_UART_TX_COMPLETE_CB_ID, transmitCompletedCallback);
		HAL_UART_RegisterCallback(&huart, HAL_UART_ERROR_CB_ID, errorCallback);
		HAL_UART_RegisterRxEventCallback(&huart, receiveCallback);
		result = true;
	}
	return result;
}


void uartSendString(uint8_t * pstring){
	if(pstring == NULL) return;
	size_t size = strlen((char*)pstring);
	HAL_UART_Transmit_IT(&huart, (const uint8_t*)pstring , (uint16_t)size);
}

void uartSendStringSize(uint8_t * pstring, uint16_t size){
	if(pstring == NULL) return;
	HAL_UART_Transmit_IT(&huart, (const uint8_t*)pstring , size);
}

void uartReceiveStringSize(uint8_t * pstring, uint16_t size){
	HAL_UART_Receive_IT(&huart,pstring, size);
}
HAL_UART_StateTypeDef uartGetState(){
	return HAL_UART_GetState(&huart);
}

void USART3_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart);
}
