/*
 * oee_eventController.c
 *
 *  Created on: Dec 6, 2023
 *      Author: Javier
 */

#include <oee_eventController.h>
#include <ringBuffer.h>

static HardwareEvent_t eventsArray[EVENT_BUFFER_SIZE] = {0};
static RingBuffer_t events = {0};
static bool bufferInit = false;

void initEvents(){
	if(bufferInit) return;
	rb_init(&events, eventsArray, sizeof(HardwareEvent_t), EVENT_BUFFER_SIZE);
	events.asList = false;
}

void events_init(HardwareEvent_t* event, char * name, EventCallback function){
	if(!bufferInit) initEvents();
	if(event==NULL) return;
	event->valid = false;
	if(name!=NULL) event->name = name;
	if(function!=NULL) event->function = function;
}

void events_setPayload(HardwareEvent_t* event, char * payload){
	if(event==NULL) return;
	if(payload!=NULL) event->payload = payload;
}

void events_register(HardwareEvent_t* event){
	if(!bufferInit) initEvents();
	if(event==NULL) return;
	rb_push(&events, (void*) event);
}

bool events_peek(HardwareEvent_t* event){
	if(!bufferInit) initEvents();
	if(event==NULL) return false;
	return rb_peek(&events, (void*)event);
}

bool events_read(HardwareEvent_t* event){
	if(!bufferInit) initEvents();
	if(event==NULL) return false;
	return rb_pop(&events, (void*)event);
}

void events_execute(HardwareEvent_t* event){
	if(event==NULL) return;
	if(event->function != NULL) event->function();
}

void events_setFunction(HardwareEvent_t* event, EventCallback function){
	if(event==NULL) return;
	if(function != NULL) event->function = function;
}
