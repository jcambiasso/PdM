/*
 * oee_applicationProtocol.h
 *
 *  Created on: Dec 7, 2023
 *      Author: Javier
 */

#include <oee_applicationProtocol.h>

static const char newLine[] = "\n\r";

void app_init(void){
	//placeholder
	return;
}

void app_parse(const char * payload, AppData_t* data){
	//placeholder
	return;
}

bool app_stringyfy(char * payload, size_t size, const AppData_t* data){
	//placeholder
	return false;
}

bool app_stringifyEvent(char * payload, size_t size, const HardwareEvent_t* event, const char * ID){
	if(event == NULL) return false;
	if((strlen(event->payload) + strlen(newLine))>size) return false;
	strcpy(payload,event->payload);
	strcat(payload,newLine);
	return true;
}


