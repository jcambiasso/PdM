#include "inputs.h"

#define INCREMENT 1
#define DECREMENT 2
#define STANDBY 0
#define DEBOUNCING 1
#define DEBOUNCED 2
#define CALLBACK 3

  #define __LOCK(__HANDLE__)                                           	\
                                do{                                     \
                                    if((__HANDLE__)->Lock == true)   	\
                                    {                                   \
                                       return false;                   	\
                                    }                                   \
                                    else                                \
                                    {                                   \
                                       (__HANDLE__)->Lock = true;    	\
                                    }                                   \
                                  }while (0U)

  #define __UNLOCK(__HANDLE__)                                          \
                                  do{                                   \
                                      (__HANDLE__)->Lock = false;    	\
                                    }while (0U)

#ifdef __cplusplus
extern "C"{
#endif

static uint8_t safeArrayOperation(uint8_t operation, uint8_t data, uint8_t floor, uint8_t top, size_t size){
    switch (operation)
    {
    case INCREMENT:
    {
        data+= size;
        if(data>=top) data = floor;
    }
    break;
    case DECREMENT:
    {
        data-= size;
        if(data<=floor) data = top;
    }
    break;
    default: return 0;
        break;
    }
    return data;       
}

bool timeoutGeneralCallback(void* input){
	if(input == NULL) return false;
    ((Input_t*)input)->stage = DEBOUNCED;
    input_eval((Input_t*)input);
    return true;
}

void input_init(Input_t* input, uint8_t pin, uint32_t interval, InputModes_t mode, InputCallback cb, InputTypes_t interruptType){
	if(input == NULL) return;
    delayInit(&input->debounceTimer, interval, true);
    if(interval == 0) input->debounce = false;
    else input->debounce = true;
    input->pin = pin;
    primeInput(input);
    input->mode = mode;
    input->callback = cb;
    input->stage = STANDBY;
    input->state = readPin(pin)^input->negate;
    input->lastState = input->state;
    input->type = interruptType;
}

uint8_t input_eval(Input_t* input){
	if(input == NULL) return false;
    if(input->stage == CALLBACK) return CALLBACK; //This is to avoid freezing if input_eval is called between the callback itself.
    if(input->stage == STANDBY){
        input_update(input);
        if(input->state == input->lastState && input->mode != HIGH && input->mode != LOW) return STANDBY;
        input->debounceTimer.enabled = 1;
        delayRead(&input->debounceTimer);
        timer_registerTimeout(&input->debounceTimer,timeoutGeneralCallback,(void*)input);
        input->stage = DEBOUNCING;
        return DEBOUNCING;
    }
    else if(input->stage == DEBOUNCING){
        return DEBOUNCING;
    }
    else if(input->stage == DEBOUNCED){
    	if(input->state != (readPin(input->pin)^input->negate)){
            input->stage = STANDBY;
            return STANDBY;
        }
    	__LOCK(input);
    	//The lock and this boolean assures that when reaching this state, there is no extra pin read and no extra callback call from
    	//other IRQs that mall call this function.
    	bool state = input->state;
        bool check = false;
        switch(input->mode)
        {
            case FALLING:
            {
                if(input->lastState == true && state == false) check = true;
            }
            break;
            case RISING:
            {
                if(input->lastState == false && state == true) check = true;
            }
            break;
            case LOW:
            {
                if(state == false) check = true;
            }
            break;
            case HIGH:
            {
                if(state == true) check = true;
            }
            break;
            default:
            {
                //defaults to change:
                if(state != input->lastState) check = true;
            }
            break;
        }
        if(check){
            input->stage = CALLBACK;    
            input->callback();
            input->engaged = true;
        }
        input->lastState = state;
        input->stage = STANDBY;
        __UNLOCK(input);
        return DEBOUNCED;
    }
    else return STANDBY;
}

bool input_update(Input_t* input){
    if(input == NULL) return false;
	input->state = readPin(input->pin)^input->negate;
    return input->state;
}

static Input_t* volatile inputs[MAX_PCINPUTS];
static bool vectorInitialized = false;
static uint8_t index;

static void emptyVector(){
    for(uint8_t i = 0; i<MAX_PCINPUTS; i++){ 
        inputs[i] = NULL;
    }
    vectorInitialized = true;
}

void input_addInterrupt(Input_t* input){
	if(input == NULL) return;
    //External interrupt
    if(input->type == EXTERNAL){
        attachExternalInterrupt(input->pin, input->callback, CHANGE);
    }
    else if(input->type == PINCHANGE){
        if(!vectorInitialized) emptyVector();
        inputs[index] = input;  
        index = safeArrayOperation(INCREMENT,index,0,MAX_PCINPUTS,1);
        enablePinInterrupt(input->pin);
    }
    else if(input->type == NONE){

    }
}

void input_attachCallback(Input_t* input, InputCallback callback){
	if(input!=NULL && callback!=NULL && input->stage!=CALLBACK){
		input->callback = callback;
	}
	return;
}

void input_irq(void){
    for(uint8_t i = 0; i<MAX_PCINPUTS; i++){
        if(inputs[i]!=NULL) {input_update((Input_t*)inputs[i]);}
    }
    for(uint8_t i = 0; i<MAX_PCINPUTS; i++){
        if(inputs[i]!=NULL) {input_eval((Input_t*)inputs[i]);}
    }
}
void(*inputs_irqHandler)() = input_irq;
 
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

void Input::init(uint8_t p, uint32_t interval,int t,InputCallback cb){
    input_init(this,p,interval,t,cb,NONE);
}

Input::Input(uint8_t pin){
    init(250,pin,FALLING,nullptr,NONE);
}

Input::Input(uint8_t pin,uint32_t interval,int mode,InputCallback cb){
    init(pin,interval,mode,cb,NONE);
}

void Input::attachCallback(InputCallback cb){
    callback = cb;
}

bool Input::updateState(){
    return input_update(this);
}

uint8_t Input::evaluate(bool autoUpdate){
   return input_eval(this);
}

bool Input::isSet(bool clear){
    bool a = engaged;
    if(clear) engaged = false;
    return a;
}

ExternalInterruptInput::ExternalInterruptInput(uint8_t pin) : Input(pin){
    addInterrupt();
}

ExternalInterruptInput::ExternalInterruptInput(uint8_t pin,uint32_t interval,int mode, InputCallback cb) : Input(pin, interval, mode, cb) {
    addInterrupt();
}

void ExternalInterruptInput::addInterrupt(){
    type = EXTERNAL;
    input_addInterrupt(this);
}

PinChangeInterruptInput::PinChangeInterruptInput(uint8_t pin) : ExternalInterruptInput(pin){
    addInterrupt();
}

PinChangeInterruptInput::PinChangeInterruptInput(uint8_t pin,uint32_t interval,int mode, InputCallback cb) : ExternalInterruptInput(pin, interval, mode, cb) {
    addInterrupt();
}

void PinChangeInterruptInput::allowInterrupts(){
    enableGlobalInputInterrupts();
}

void PinChangeInterruptInput::disableInterrupts(){
    disableGlobalInputInterrupts();
}

void PinChangeInterruptInput::addInterrupt(){
    type = PINCHANGE;
    input_addInterrupt(this);
}

#endif
