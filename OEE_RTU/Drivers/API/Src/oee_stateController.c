/*
 * oee_stateController.c
 *
 *  Created on: Dec 6, 2023
 *      Author: Javier
 */

#include <oee_stateController.h>

static oee_states_t currentState = INITIALIZATION;
static fsm_Callback initialization_cb = NULL;
static fsm_Callback listening_cb = NULL;
static fsm_Callback parsingEvent_cb = NULL;
static fsm_Callback parsingRemote_cb = NULL;
static fsm_Callback updatingRemote_cb = NULL;
static fsm_Callback retrying_cb = NULL;

void fsm_changeState(oee_states_t state){
	if(state >= TOTAL_STATES) return;
	currentState = state;
	return;
}

void fsm_init(
			fsm_Callback initialization,
			fsm_Callback listening,
			fsm_Callback parsingEvent,
			fsm_Callback parsingRemote,
			fsm_Callback updatingRemote,
			fsm_Callback retrying
			){
	if(initialization!=NULL) initialization_cb = initialization;
	if(listening!=NULL) listening_cb = listening;
	if(parsingEvent!=NULL) parsingEvent_cb = parsingEvent;
	if(parsingRemote!=NULL) parsingRemote_cb = parsingRemote;
	if(updatingRemote!=NULL) updatingRemote_cb = updatingRemote;
	if(retrying!=NULL) retrying_cb = retrying;
	currentState = INITIALIZATION;
	return;
}

void fsm_run(void){
	switch(currentState){
	case INITIALIZATION:{
		if(initialization_cb!=NULL) initialization_cb();
		}
	break;
	case LISTENING:{
		if(listening_cb!=NULL) listening_cb();
		}
	break;
	case PARSING_EVENT:{
		if(parsingEvent_cb!=NULL) parsingEvent_cb();
		}
	break;
	case PARSING_REMOTE:{
		if(parsingRemote_cb!=NULL) parsingRemote_cb();
		}
	break;
	case UPDATING_REMOTE:{
		if(updatingRemote_cb!=NULL) updatingRemote_cb();
		}
	break;
	case RETRYING:{
		if(retrying_cb!=NULL) retrying_cb();
		}
	break;
	default: fsm_changeState(INITIALIZATION);
		break;
	}
}
