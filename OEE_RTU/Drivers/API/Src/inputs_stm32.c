#include <inputs.h>
#include <inputs_stm32_config.h>


#ifdef __cplusplus
extern "C"{
#endif

static void(*exti0callback)(void) = NULL;
static void(*exti1callback)(void) = NULL;
static void(*exti2callback)(void) = NULL;
static void(*exti3callback)(void) = NULL;
static void(*exti4callback)(void) = NULL;



bool readPin(uint8_t pin){
	if(pin > (sizeof(inputs)/sizeof(input_stm32_t))) return false;
	if(inputs[pin].port == NULL) return false;
	GPIO_PinState result = HAL_GPIO_ReadPin(inputs[pin].port, inputs[pin].pin);
	if(result == GPIO_PIN_SET) return true;
	else return false;
}

void enablePinInterrupt(uint8_t pin){
	if(pin > (sizeof(inputs)/sizeof(input_stm32_t))) return;
	if(inputs[pin].port == NULL) return;
	switch(inputs[pin].pin){
	case GPIO_PIN_10: case GPIO_PIN_11: case GPIO_PIN_12: case GPIO_PIN_13: case GPIO_PIN_14: case GPIO_PIN_15:{
		HAL_NVIC_SetPriority(EXTI15_10_IRQn, 7, 0);
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
		} break;
	case GPIO_PIN_9: case GPIO_PIN_8: case GPIO_PIN_7: case GPIO_PIN_6: case GPIO_PIN_5:{
		HAL_NVIC_SetPriority(EXTI9_5_IRQn, 6, 0);
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
		} break;
	default:
		break;
	}
}
void enableGlobalInputInterrupts(){

}

void disableGlobalInputInterrupts(){

}

void primeInput(Input_t* input){
	if(input->pin > (sizeof(inputs)/sizeof(input_stm32_t))) return;
	if(inputs[input->pin].port == NULL) return;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = inputs[input->pin].pin;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	if(input->type != NONE){
		if((input->mode == FALLING) || (input->mode == LOW) ) GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		else if((input->mode == RISING) || (input->mode == HIGH)) GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		else GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	}
	else GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(inputs[input->pin].port, &GPIO_InitStruct);
}

void attachExternalInterrupt(uint8_t pin, void(*callback)(), InputModes_t mode){
	if(pin > (sizeof(inputs)/sizeof(input_stm32_t))) return;
	if(inputs[pin].port == NULL) return;
	switch(inputs[pin].pin){
		case GPIO_PIN_0:{
			HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(EXTI0_IRQn);
			exti0callback = callback;
		} break;
		case GPIO_PIN_1:{
			HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
			HAL_NVIC_EnableIRQ(EXTI1_IRQn);
			exti1callback = callback;
		} break;
		case GPIO_PIN_2:{
			HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
			HAL_NVIC_EnableIRQ(EXTI2_IRQn);
			exti2callback = callback;
		} break;
		case GPIO_PIN_3:{
			HAL_NVIC_SetPriority(EXTI3_IRQn, 3, 0);
			HAL_NVIC_EnableIRQ(EXTI3_IRQn);
			exti3callback = callback;
		} break;
		case GPIO_PIN_4:{
			HAL_NVIC_SetPriority(EXTI4_IRQn, 4, 0);
			HAL_NVIC_EnableIRQ(EXTI4_IRQn);
			exti4callback = callback;
		} break;
		default:
		break;
	}
}

//I have two interrupts routines potentially running on the same array of objets.

void EXTI15_10_IRQHandler(void){
	input_irq();
	for(uint8_t i = 0; i<(sizeof(inputs)/sizeof(input_stm32_t)); i++){
		uint16_t pin = inputs[i].pin;
		if(pin == GPIO_PIN_10 || pin == GPIO_PIN_11 || pin == GPIO_PIN_12 || pin == GPIO_PIN_13 || pin == GPIO_PIN_14 || pin == GPIO_PIN_15){
			HAL_GPIO_EXTI_IRQHandler(pin);
		}
	}
}
void EXTI9_5_IRQHandler(void){
	input_irq();
	for(uint8_t i = 0; i<(sizeof(inputs)/sizeof(input_stm32_t)); i++){
		uint16_t pin = inputs[i].pin;
		if(pin == GPIO_PIN_5 || pin == GPIO_PIN_6 || pin == GPIO_PIN_7 || pin == GPIO_PIN_8 || pin == GPIO_PIN_9){
			HAL_GPIO_EXTI_IRQHandler(pin);
		}
	}
}
void EXTI0_IRQHandler(void){
	if(exti0callback!=NULL) exti0callback();
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
void EXTI1_IRQHandler(void){
	if(exti1callback!=NULL) exti1callback();
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
void EXTI2_IRQHandler(void){
	if(exti2callback!=NULL) exti2callback();
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}
void EXTI3_IRQHandler(void){
	if(exti3callback!=NULL) exti3callback();
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}
void EXTI4_IRQHandler(void){
	if(exti4callback!=NULL) exti4callback();
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

#ifdef __cplusplus
}
#endif
