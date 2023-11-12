/*
 * inputs.c
 *
 *  Created on: Nov 9, 2023
 *      Author: Javier Cambiasso
 */

#include <inputs.h>

bool readButton(inputDebounce_t* input){
	GPIO_PinState pin =  HAL_GPIO_ReadPin(input->port,input->pin);
	//El XOR (^) es para admitir que la entrada tenga lógica negativa, si tenés por ejemplo un pull-up/open drain.
	if(pin == GPIO_PIN_RESET) return false^input->negativeLogic;
	else return true^input->negativeLogic;
}

void debounceFSM_init(inputDebounce_t* input, uint32_t debounceTime, bool negativeLogic){
	if(input == NULL) return;
	//eventualmente inicializaria la entrada.
	input->state = BUTTON_LOW;
	input->negativeLogic = negativeLogic;
	delayInit( &(input->delay), debounceTime, true );
}

bool debounceFSM_update(inputDebounce_t* input){
	if(input == NULL) return false;
	input->currentLevel = readButton(input);
	bool result;
	switch(input->state){
		case BUTTON_HIGH:{
			if(!input->currentLevel) input->state = BUTTON_FALLING;
			result = true;
		}
		break;
		case BUTTON_LOW:{
			if(input->currentLevel) input->state = BUTTON_RISING;
			result = false;
		}
		break;
		case BUTTON_FALLING:{
			if(delayRead(&(input->delay))){
				if(!input->currentLevel){
					if(input->onButtonReleased!= NULL) input->onButtonReleased();
					input->state = BUTTON_LOW;
					result = false;
				}
				else{
					delayRestart(&(input->delay));
					input->state = BUTTON_HIGH;
					result = true;
				}
			}
			else result = true;
		}
		break;
		case BUTTON_RISING:{
			if(delayRead(&(input->delay))){
				if(input->currentLevel){
					if(input->onButtonPressed!= NULL) input->onButtonPressed();
					input->state = BUTTON_HIGH;
					result = true;
				}
				else{
					delayRestart(&(input->delay));
					input->state = BUTTON_LOW;
					result = false;
				}
			}
			else result = false;
		}
		break;
		default:{
			if(input->currentLevel) input->state = BUTTON_HIGH;
			else input->state = BUTTON_LOW;
			result = input->currentLevel;
		}
		break;
	}
	return result;
}
