/*
 * API_delay.c
 *
 *  Created on: Nov 2, 2023
 *      Author: Javier Cambiasso
 */
#include <API_timer.h>
#include <ringBuffer.h>

typedef struct timeout_s{
	bool valid;
	Timer_t* timer;
	void * argument;
	timerCallback_t callback;
} Timeout_t;

static Timeout_t timeouts_array[TIMEOUTS_COUNT] = {0};

static RingBuffer_t timeouts = {0};

void initTimers(){
	rb_init(&timeouts, timeouts_array, sizeof(Timeout_t), TIMEOUTS_COUNT);
	timeouts.asList = true;
}

void timer_irqHandler(){
	for(uint8_t i = 0; i<TIMEOUTS_COUNT; i++){
		Timer_t* timer = timeouts_array[i].timer;
		if(timeouts_array[i].valid){
			if(delayRead(timer)){
				timeouts_array[i].callback(timeouts_array[i].argument);
				if(timer->singleShot) timeouts_array[i].valid = 0;
			}
		}
	}
}

void timer_registerTimeout(Timer_t* timer,timerCallback_t callback, void* argument){
	Timeout_t to = {1,timer,argument,callback};
	list_push(&timeouts,(void*)&to);
	return;
}

/**
 * @brief Función vacía para inicializar @ref getCurrentTick.
 */
uint32_t dummyGetTick(){
	return 0;
}

/**
 * Variable globlal funcional para la función que obtiene el tiempo de sistema en milisegundos.
 */
static GetCurrentTick_t getCurrentTick = dummyGetTick;

/**
 * @brief Función para escribir @ref getCurrentTick.
 * @param function Puntero a función que obtiene el tiempo de sistema en milisegundos.
 * @retval None
 */
void setGetCurrentTickFunction(GetCurrentTick_t function){
	if(function!=NULL) getCurrentTick = function;
}

void delayInit( Timer_t * delay, tick_t duration, bool_t singleShot ){
	if(delay == NULL) return;
	delay->startTime= 0;
	delay->running = false;
	delay->singleShot = singleShot;
	delay->duration = duration;
	delay->enabled = true;
	return;
}

bool_t delayRead( Timer_t * delay ){
	if(delay == NULL) return false;
	if(delay->enabled == false) return false;
	if(delay->running == false){
		delay->startTime = getCurrentTick() - delay->elapsed;
		delay->running = true;
	}
	//ver en el .list, al no ser volatil, si es lo mismo repetir delay->elapsed en la comparación que declarar la variable intermedia,
	//o si realmente estoy evitando el doble fetch.
	tick_t elapsed = getCurrentTick() - delay->startTime;
	delay->elapsed = elapsed;
	if(elapsed >= delay->duration){
		if(delay->singleShot) delay->running=false;
		else delay->startTime = getCurrentTick();
		delay->elapsed = 0;
		return true;
	}
	else return false;
}

void delayWrite( Timer_t * delay, tick_t duration, bool_t restart){
	if(delay == NULL) return;
	delay->duration = duration;
	if(restart){
		delayRestart(delay);
	}
}

void delayRestart(Timer_t * delay){
	delay->elapsed = 0;
	delay->running = false;
	if(!delay->singleShot) delayRead(delay);
}

void delayStart(Timer_t * delay){
	if(delay == NULL) return;
	delay->enabled = true;
}

void delayStop(Timer_t * delay){
	if(delay == NULL) return;
	delay->enabled = false;
	delay->running = false;
}

tick_t delayElapsed(Timer_t * delay){
	if(delay == NULL) return 0;
	return delay->elapsed;
}

void patternInit(TimerPattern_t* pattern,Timer_t* delay,uint16_t repeats,uint32_t interval, uint8_t dutyCicle, patternCallback callbackOff, patternCallback callbackOn){
	if(pattern == NULL)  return;
	if(dutyCicle > 99) dutyCicle = 99;
	pattern->callbackOff = callbackOff;
	pattern->callbackOn = callbackOn;
	pattern->interval = interval;
	pattern->repeats = repeats;
	pattern->delay = delay;
	pattern->done = false;
	pattern->dutyCicle = dutyCicle;
	pattern->currentCicle = 0;
	pattern->off_period = false;
	interval = (uint32_t)((interval * dutyCicle)/100);
	/* Reinicio el delay como no singleShot para poder llamar a patternRun de a periodos mayores o iguales a 1ms*/
	delayInit(delay,interval,false);
}

bool patternRun(TimerPattern_t* pattern){
	if(pattern == NULL)  return false;
	if(pattern->done) return true;
	if(pattern->currentCicle == pattern->repeats){
		pattern->done = true;
		return true;
	}

	if(delayRead(pattern->delay)){
		if(!pattern->off_period){
			pattern->callbackOff();
			pattern->off_period = true;
			uint8_t dutycicle = pattern->dutyCicle;
			if(dutycicle > 99) dutycicle = 99;
			delayWrite(pattern->delay, (uint32_t)((pattern->interval*(100-dutycicle))/100), true );
			return false;
		}
		else{
			//Con esto estoy funcionalmente empezando en TOFF y terminando en TON, lo que es poco intuitivo.
			pattern->callbackOn();
			uint8_t dutycicle = pattern->dutyCicle;
			if(dutycicle > 99) dutycicle = 99;
			delayWrite(pattern->delay, (uint32_t)((pattern->interval*(dutycicle))/100), true );
			pattern->off_period = false;
			pattern->currentCicle++;
			return false;
		}
	}
	else{
		return false;
	}
}

void patternRestart(TimerPattern_t* pattern){
	if(pattern == NULL)  return;
	pattern->done = false;
	pattern->currentCicle = 0;
}

bool patternCollectionRun_s(TimerPattern_t ** collection, size_t size, bool continuous, uint8_t* index) {
	if(collection == NULL) return false;
	if(*index >= size) return true;
	if(patternRun(collection[*index])){
		patternRestart(collection[*index]);
		*index = *index + 1;
		if(*index >= size) {
			if(continuous) *index = 0;
			return true;
		}
		else return false;
	}
	else return false;
}

bool patternCollectionRun(TimerPattern_t ** collection, size_t size, bool continuous) {
	if(collection == NULL) return false;
	static uint8_t index;
	return patternCollectionRun_s(collection,size,continuous,&index);

}
