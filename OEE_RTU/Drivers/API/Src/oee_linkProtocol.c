/*
 * oee_linkProtocol.c
 *
 *  Created on: Dec 7, 2023
 *      Author: Javier
 */

#include <oee_linkProtocol.h>
#include <API_uart.h>

static LinkState_t uartStateToState(HAL_UART_StateTypeDef uartState){
	LinkState_t result;
	switch(uartState){
	case HAL_UART_STATE_RESET: result = LINK_RESET;
	break;
	case HAL_UART_STATE_READY: result = LINK_STANDBY;
	break;
	case HAL_UART_STATE_BUSY: result = LINK_BUSY;
	break;
	case HAL_UART_STATE_BUSY_TX: result = LINK_TRANSMITTING;
	case HAL_UART_STATE_BUSY_TX_RX:
	case HAL_UART_STATE_BUSY_RX: result = LINK_LISTENING;
	break;
	case HAL_UART_STATE_TIMEOUT: result = LINK_TIMEOUT;
	break;
	case HAL_UART_STATE_ERROR: result = LINK_ERROR;
	break;
	default: result = ERROR;
	break;
	}
	return result;
}

static bool doneTransmit = false;
static void setDoneTransmit(UART_HandleTypeDef* uart){
	if(uart->Instance == USART3) doneTransmit = true;
}

bool link_init(){
	uart_registerTransmitCompleteCallback(setDoneTransmit);
	return uartInit(115200, UART_WORDLENGTH_8B, UART_STOPBITS_1, UART_PARITY_NONE);
}

bool link_connect(){
	//placeholder
	return true;
}

bool link_ping(){
	//placeholder
	return true;
}

LinkState_t link_send(char * payload){
	HAL_UART_StateTypeDef state;
	state = uartGetState();
	if(state == HAL_UART_STATE_READY || state == HAL_UART_STATE_BUSY_RX){
		state = uartSendString((uint8_t* )payload);
	}
	if(doneTransmit){
		doneTransmit = false;
		return LINK_DONE;
	}
	return uartStateToState(state);
}

LinkState_t link_listen(char * payload, uint16_t size){
	//placeholder
	return LINK_RESET;
}
