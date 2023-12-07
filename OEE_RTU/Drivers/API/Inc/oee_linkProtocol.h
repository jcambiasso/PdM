/*
 * oee_linkProtocol.h
 *
 *  Created on: Dec 7, 2023
 *      Author: Javier
 */

#ifndef API_INC_OEE_LINKPROTOCOL_H_
#define API_INC_OEE_LINKPROTOCOL_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef enum{
	LINK_RESET,
	LINK_STANDBY,
	LINK_BUSY,
	LINK_LISTENING,
	LINK_TRANSMITTING,
	LINK_DONE,
	LINK_TIMEOUT,
	LINK_ERROR
}LinkState_t;

/**
 * @brief Initializes the link protocol module
 * @retval True Initilization complete
 * @reval False Link internal error.
 */
bool link_init();

/**
 * @brief Connects to remote device
 * @retval True Connection stablished
 * @retval False Connection timeout
 */
bool link_connect();

/**
 * @brief Checks connection with remote
 * @retval True Connection stablished
 * @retval False Connection timeout
 */
bool link_ping();

/**
 * @brief Sends a payload to remote
 * @param payload c-string payload to send
 * @retval True packet sent
 * @retval False Link busy
 */
LinkState_t link_send(char * payload);

/**
 * @brief Receives a payload from remote
 * @param payload c-string pointer to read data into.
 * @param size size of data to receive.
 * @retval True packet sent
 * @retval False Connection timeout
 */
LinkState_t link_listen(char * payload, uint16_t size);


#endif /* API_INC_OEE_LINKPROTOCOL_H_ */
