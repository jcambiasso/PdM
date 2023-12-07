/*
 * oee_applicationProtocol.h
 *
 *  Created on: Dec 7, 2023
 *      Author: Javier
 *
 *      Codes and decodes (parses) raw string data to pass to link protocol.
 */

#ifndef API_INC_OEE_APPLICATIONPROTOCOL_H_
#define API_INC_OEE_APPLICATIONPROTOCOL_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <oee_eventController.h>

typedef struct{
	//placeholder.
} AppData_t;

/**
 *@brief Initializes the object
 */
void app_init(void);

/**
 *@brief Converts payload to AppData_t type.
 *@param payload string input.
 *@param data output data from payload.
 *@retval None.
 */
void app_parse(const char * payload, AppData_t* data);

/**
 *@brief Converts AppData_t type to a string payload
 *@param payload char array input
 *@param size size of output array
 *@param data input data object.
 *@retval True Success
 *@retval False (void pointers, payload size too small)

 */
bool app_stringyfy(char * payload, size_t size, const AppData_t* data);

/**
 *@brief Converts event to string payload.
 *@param payload char array input
 *@param size size of output array
 *@param event Event type input to stringify.
 *@param ID string with machine name
 *@retval None.
 *@retval False Error (void pointers, payload size too small)
 */
bool app_stringifyEvent(char * payload, size_t size, const HardwareEvent_t* event, const char * ID);

#endif /* API_INC_OEE_APPLICATIONPROTOCOL_H_ */

