/*
 * oee_stateController.h
 *
 *  Created on: Dec 6, 2023
 *      Author: Javier
 */

#ifndef API_INC_OEE_STATECONTROLLER_H_
#define API_INC_OEE_STATECONTROLLER_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum{
	INITIALIZATION,
	LISTENING,
	PARSING_EVENT,
	PARSING_REMOTE,
	UPDATING_REMOTE,
	RETRYING,
	TOTAL_STATES
}oee_states_t;

typedef void(*fsm_Callback)(void);

/**
 * @brief Changes the behavious of the system by switching to another state.
 * @param state State to change to
 * @retval None
 */

void fsm_changeState(oee_states_t state);

/**
 * @brief Initializes the state machine
 * @param initialization Callback for the initialization state behaviour.
 * @param listening Callback for the listening state behaviour.
 * @param parsingEvent Callback for the parsingEvent state behaviour.
 * @param parsingRemote Callback for the parsingRemote state behaviour.
 * @param updatingRemote Callback for the updatingRemote state behaviour.
 * @param retrying Callback for the retrying state behaviour.
 * @retval None
 */
void fsm_init(
			fsm_Callback initialization,
			fsm_Callback listening,
			fsm_Callback parsingEvent,
			fsm_Callback parsingRemote,
			fsm_Callback updatingRemote,
			fsm_Callback retrying
			);
/**
 * @brief Run the finite state machine callbacks.
 * @retval None
 */
void fsm_run(void);



#endif /* API_INC_OEE_STATECONTROLLER_H_ */
