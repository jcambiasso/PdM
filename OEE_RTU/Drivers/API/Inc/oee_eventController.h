/*
 * oee_eventController.h
 *
 *  Created on: Dec 6, 2023
 *      Author: Javier Cambiasso
 *
 *      Event buffer manager and controller.
 */


#ifndef API_INC_OEE_EVENTCONTROLLER_H_
#define API_INC_OEE_EVENTCONTROLLER_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <inputs.h>

//Sets the maximum number of distinct events
#define MAX_UNIQUE_EVENTS 16
//Events RAM buffer size
#define EVENT_BUFFER_SIZE 100

typedef void(*EventCallback)(void);

typedef struct{
	bool valid;
	char * name;
	char * payload;
	EventCallback function;
}HardwareEvent_t;


/**
 * @brief Initializes an event type.
 * @param event Pointer to event type.
 */
void events_init(HardwareEvent_t* event, char * name, EventCallback function);

/**
 * @brief Attaches a function callback for the event.
 * @param event Pointer to event type.
 * @param Funcition callback pointer
 */
void events_setFunction(HardwareEvent_t* event, EventCallback function);

/**
 * @brief Sets the payload to the event.
 * @param event Pointer to event type.
 */
void events_setPayload(HardwareEvent_t* event, char * payload);

/**
 * @brief Registers an event to the event buffer
 * @param event Pointer to event type.
 */
void events_register(HardwareEvent_t* event);

/**
 * @brief Gets the head event of the buffer, without removing it.
 * @param event Pointer to event type.
 */
bool events_peek(HardwareEvent_t* event);

/**
 * @brief Gets the head event of the buffer, removing it afterwards.
 * @param event Pointer to event type.
 */
bool events_read(HardwareEvent_t* event);

/**
 * @brief Executes the event callback function
 * @param event Pointer to event type.
 */
void events_execute(HardwareEvent_t* event);

#endif /* API_INC_OEE_EVENTCONTROLLER_H_ */
