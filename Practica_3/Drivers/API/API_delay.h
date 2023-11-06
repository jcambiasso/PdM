/*
 * API_delay.h
 *
 *  Created on: Nov 2, 2023
 *      Author: Javier Cambiasso
 */

#ifndef API_API_DELAY_H_
#define API_API_DELAY_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef uint32_t tick_t;
typedef bool bool_t;

/**
 * @brief Struct para un delay no bloqueante.
 */
typedef struct{
   tick_t startTime;		/*!< Instante, en milisengundos del sistema, en que inició el delay  */
   tick_t duration;			/*!< Duración, en milisegundos del sistema, desde @ref startTime   */
   tick_t elapsed;			/*!< Guarda el tiempo transcurrido desde el inicio del delay   */
   bool_t running;			/*!< Indica si el delay está activado */
   bool_t enabled;			/*!< Indica si el delay está habilitado */
   bool_t singleShot;		/*!< Si es falso, startTime se actualiza al finalizar el conteo y no con delayRead, excepto es el primer llamado del programa. */
} delay_t;

/**
 * @brief 	Prototipo de función que obtiene un tiempo en milisegundos.
 * @note 	Esto es modularizable, y es raro hardcodear una función muy target-specific (te estoy mirando HAL_GetTick())
 * 			a una estructura tan genérica. Declaro un prototipo de la función que será referenciada en estos métodos.
 * 			(es un puntero a una función de acepta void y devuelve uint32_t)
 */
typedef uint32_t (*GetCurrentTick_t)(void);

/**
 * @brief Prototipo a puntero de función para los callbacks del generador de patrones
 */

typedef void (*patternCallback)(void);

/**
 * @brief Struct que describe un patrón de ejecución de un timer.
 */
typedef struct{
	uint16_t repeats; 				/*!< Define cuantas veces se ejecuta el patrón hasta dar done  */
	uint16_t currentCicle;			/*!< Cuantas veces se ejecuto el ciclo */
	uint32_t interval;				/*!< Tiempo total que dura cada repetición  */
	uint8_t dutyCicle;				/*!< Porcentaje de interval hasta ejecutar callback_off. Maximo 99   */
	delay_t * delay;				/*!< controlador de tiempo para el generador de patrones  */
	bool done;						/*!< Indica que el patrón se ejecutó completo   */
	bool off_period;				/*!< Indica que el patron está ejecutando el ciclo off   */
	patternCallback callbackOff;	/*!< Función a ejecutar cuando trascurre un tiempo interval*dutyCicle   */
	patternCallback callbackOn;		/*!< Función a ejecutar apenas inicia un ciclo   */
} TimerPattern_t;

/**
 * Una pequeño macro para inicializar un delay
 */
#define NewDelay(in,ss)\
{\
	.duration = in, \
	.start = 0, \
	.enabled = 1, \
	.singleShot = ss, \
	.running = 0 \
}

/**
 * @brief Asignar un puntero a función a usar en la API delay.
 * @param function Una función que devuelve el tiempo del sistema como uint32_t, sin argumentos.
 * @retval None
 */
void setGetCurrentTickFunction(GetCurrentTick_t function);

/**
  * @brief  Inicializa un delay no bloqueante.
  * @param delay Puntero a estrucutra delay_t
  * @param duration Duration del delay
  * @param singleShot Determina si el delay es de disparo único
  * @retval None
  */
void delayInit( delay_t * delay, tick_t duration, bool_t singleShot );

/**
  * @brief  La primer llamada inicia el timer, y las subsiguientes verifica que Duration haya transcurrido.
  * @note	Si no es singleShot, en cambio, el delay es reiniciado al trascurrir Duration automaticamente.
  * @param delay Puntero a estrucutra delay_t
  * @retval true Transcurrió el tiempo establecido
  * @retval false No transcurrió el tiempo establecido
  */
bool_t delayRead( delay_t * delay );

/**
  * @brief  Permite reescribir la duración del delay.
  * @param delay Puntero a estrucutra delay_t
  * @param duration Nueva duración del delay
  * @param restart True para además reiniciar el delay con la nueva duración
  * @retval True si transcurrió el tiempo establecido, false si lo contrario.
  */
void delayWrite( delay_t * delay, tick_t duration, bool_t restart);

/**
  * @brief Habilita el delay
  * @param delay Puntero a estrucutra delay_t
  * @retval None
  */
void delayStart(delay_t * delay);

/**
  * @brief Deshbilita el delay y lo pone en pausa. Al reanudarlo con @ref delayStart, tiene en cuenta el tiempo ya transcurrido.
  * @param delay Puntero a estrucutra delay_t
  * @retval None
  */
void delayStop(delay_t * delay);

/**
  * @brief Obtiene el tiempo transcurrido desde el inicio del delay
  * @param delay Puntero a estrucutra delay_t
  * @retval Tiempo transcurrido del delay.
  */
tick_t delayElapsed(delay_t * delay);

/**
  * @brief  Inicializa una secuencia de timer
  * @param pattern Puntero a estrucutra TimePattern_t
  * @param delay Puntero a instancia de delay para usar en el patrón.
  * @param repeats La cantidad de ciclos que se repite la secuencia.
  * @param interval el tiempo de duración total de cada ciclo
  * @param Duty dicle del periodo On.
  * @param callbackOff Función llamaba cuando expire el periodo Off.
  * @param callbackOn Función llamaba cuando inica cada ciclo.
  * @retval None
  */
void patternInit(TimerPattern_t* pattern,delay_t* delay,uint16_t repeats,uint32_t interval, uint8_t dutyCicle, patternCallback callbackOff, patternCallback callbackOn);

/**
  * @brief  Ejecuta una secuencia.
  * @note 	Esta función es thread safe porque guardo todo en el TimerPattern_t.
  * @param pattern Puntero a estrucutra TimePattern_t
  * @retval true La secuencia finalizó
  * @retval false La secuencia no finalizó
  */
bool patternRun(TimerPattern_t* pattern);

/**
  * @brief  Ejecuta en orden las secuencias en un array.
  * @note	Era más fácil el código si tomaba un array de punteros a las estrucutras. Como para poder aceptar arrays de cualquier tamaño tengo que aceptar punteros
  * 		del mismo tipo (osea, puntero a TimerPattern_t), me termina quedando un puntero de puntero a TimerPattern_t.
  * 		Esta función _no_ es thread safe (uso una variable estática).
  * @param collection Puntero a array de patrones.
  * @param size Tamaño de la colección.
  * @retval true La secuencia finalizó
  * @retval false La secuencia no finalizó
  */
bool patternCollectionRun(TimerPattern_t ** collection, size_t size, bool continuous);

/**
  * @brief  Versión thread safe de patternCollectionRun.
  * @param collection Puntero a array de patrones.
  * @param size Tamaño de la colección.
  * @param index Puntero al índice del patrón en ejecución.
  * @retval true La secuencia finalizó
  * @retval false La secuencia no finalizó
  */
bool patternCollectionRun_s(TimerPattern_t ** collection, size_t size, bool continuous, uint8_t* index );

void patternRestart(TimerPattern_t* pattern);


#endif /* API_API_DELAY_H_ */
