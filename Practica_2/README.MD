# Práctica 2
# Consigna

## Autor de la práctica

Javier Eduardo Cambiasso

## Objetivo:

Implementar un módulo de software para trabajar con retardos no bloqueantes. 

## Punto 1
Implementar las funciones auxiliares necesarias para usar retardos no bloqueantes en un archivo fuente main.c con su correspondiente archivo de cabecera main.h.

En main.h se deben ubicar los prototipos de las siguientes funciones y las declaraciones

```c
typedef uint32_t tick_t; // Qué biblioteca se debe incluir para que esto compile?
typedef bool bool_t;	  // Qué biblioteca se debe incluir para que esto compile?
typedef struct{
   tick_t startTime;
   tick_t duration;
   bool_t running;
} delay_t;

void delayInit( delay_t * delay, tick_t duration );
bool_t delayRead( delay_t * delay );
void delayWrite( delay_t * delay, tick_t duration );
```

En main.c se deben ubicar la implementación de todas las funciones:

Consideraciones para la implementación:

1. delayInit debe cargar el valor de duración del retardo en la estructura, en el campo correspondiente. No debe iniciar el conteo del retardo. Debe inicializar el flag running en `false´.
2. delayRead debe verificar el estado del flag running.
    - false, tomar marca de tiempo y cambiar running a ‘true’ 
    - true, hacer la cuenta para saber si el tiempo del retardo se cumplió o no ‘marca de tiempo actual - marca de tiempo inicial es mayor o igual a duración del retardo’? 
    - devolver un valor booleano que indique si el tiempo se cumplió o no.
    - Cuando el tiempo se cumple se debe cambiar el flag running a false.

3. delayWrite debe permitir cambiar el tiempo de duración de un delay existente

NOTA: para obtener una marca de tiempo se puede usar la función HAL_GetTick() que devuelve un valor que se incrementa cada 1 ms y que se puede usar como base de tiempo.

## Punto 2

Sobre el código desarrollado para el punto 1 y sobre el mismo proyecto, implementar un programa que utilice retardos no bloqueantes y  haga parpadear el leds de la placa de desarrollo: 100 ms encendido, 100 ms apagado, en forma periódica.

## Punto 3 [opcional]

Sobre el código desarrollado para el punto 2 y sobre el mismo proyecto, implementar un programa que haga parpadear el led de la placa de desarrollo en forma periódica con el siguiente patrón:

1. 5 veces con período 1 segundo y ciclo de trabajo 50%.
2. 5 veces con período 200 ms y ciclo de trabajo 50%.
3. 5 veces con período 100 ms y ciclo de trabajo 50%. 

Utilizar un vector o arreglo para definir el patrón y cambiar los tiempos de parpadeo.

## Para pensar luego de resolver el ejercicio:

- ¿Se pueden cambiar los tiempos de encendido de cada led fácilmente en un solo lugar del código o éstos están hardcodeados?
- ¿Qué bibliotecas estándar se debieron agregar para que el código compile? Si las funcionalidades crecieran, habría que pensar cuál sería el mejor lugar para incluir esas bibliotecas y algunos typedefs que se usan en el ejercicio.
- ¿Es adecuado el control de los parámetros pasados por el usuario que se hace en las funciones implementadas? ¿Se controla que sean valores válidos? ¿Se controla que estén dentro de los rangos correctos?
- ¿Cuán reutilizable es el código implementado? 
- ¿Cuán sencillo resulta en su implementación cambiar el patrón de tiempos de parpadeo?

# Resolución
## Punto 1
Se incluye, además de lo requerido, un boolean singleShot y un enable. Lo último permite pausar y reanudar el timer con métodos start y stop, y el permite automaticamente llama a stop cuando termina, efectivamente haciendo al timer de un solo dísparo (es útil en algunas circunstancias)

```c
/**
  * @brief Habilita el delay
  * @param delay Puntero a estrucutra delay_t
  * @retval None
  */
void delayStart(delay_t * delay);

/**
  * @brief Deshbilita el delay.
  * @param delay Puntero a estrucutra delay_t
  * @retval None
  */
void delatStop(delay_t * delay);

/**
 * @brief Struct para un delay no bloqueante.
 */
typedef struct{
   tick_t startTime;		/*!< Instante, en milisengundos del sistema, en que inició el delay  */
   tick_t duration;			/*!< Duración, en milisegundos del sistema, desde @ref startTime   */
   bool_t running;			/*!< Indica si el delay está activado */
   bool_t enabled;			/*!< Indica si el delay está habilitado */
   bool_t singleShot;		/*!< Delays en singleShot no vuelven a contar hasta ejecutarse un @ref delayStart  */
} delay_t;
```

Además, no invoco `HAL_GetTick()` directamente, si no que creo un prototipo a función, y declaro en main.h un puntero a función que el código de usuario en main.c (o donde sea) deberá definir:

```c
typedef uint32_t (*GetCurrentTick_t)(void);
extern GetCurrentTick_t getCurrentTick;
```

Para esto es válido también usar `__weak` en una función de nombre `void GetCurrentTick()` y es incluso más robusto. La única ventaja del puntero a función es que se puede cambiar en runtime, lo que mucho sentido, en este caso, no tiene. 

## Punto 2
Se implementan todas las funciones. Se define `getCurrentTick` global, inicializada en `HAL_GetTick`, después de los includes del HAL.
```c
GetCurrentTick_t getCurrentTick = HAL_GetTick;
```
En `delayRead` verifico enabled y uso la función apuntada por getCurrentTick. 
```c
bool_t delayRead( delay_t * delay ){
	if(delay->enabled == false) return false;
	if(delay->running == false){
		delay->startTime = getCurrentTick();
		delay->running = true;
		return false;
	}
	else{
		if(getCurrentTick() - delay->startTime >= delay->duration){
			delay->running = false;
			if(delay->singleShot) delay->enabled=false;
			return true;
		}
		else return false;
	}
}
```
`delayWrite` tiene un argumento nuevo, `restart`, el cual si está en `true`, no solo escribe el nuevo valor de `duration`, sino que pone en false `running` e invoca `delayRead`.

## Punto 3
Para hacer los patrones, defino varias cosas: una estructura para guarda el progreso, callbacks y un puntero al delay.
```c
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
/* USER CODE END PTD *
```

Un método de inicialización, y otro de ejecución. El último puede ser invocado entre interrupciones porque al tener todo en un `TimerPattern_t`, es thread safe. Usar un puntero a un delay, en vez de guardarlo en el struct, me permite reciclar delays.

Los dos punteros a función en este caso son necesarios (contrario al punto 2) para no hardcodear la función del patrón. Recordemos, esta estructura controla solamente la parte de tiempos de una cosa, no la _cosa_ perse. Defino TON al periodo entre el inicio del ciclo y `interval*dutyCicle`, y a TOFF al resto del tiempo, hasta interval. Al inicio de TON, se invoca `callbackOn`, y al final de TON y al inicio de TOFF, se invoca `callbackOff`. 

El ejecutarse `repeats` veces el periodo, se pone `done` en True. El método `patternRestart` limpia ambos `done` y `currentCicle`.

Se escribe `bool patternCollectionRun(TimerPattern_t ** collection, size_t size, bool continuous)` para ejecutar una colección de patrones `TimerPattern_t`. Acepta un array de punteros a `TimerPattern_t`, y los ejecuta en orden siempre que se invoque. Si `continuous`, vuelve a empezar cuando llega el último. 

Este esquema permite hacer un lindo generador de señales digitales arbitarias de 1ms de precisión.

# Notas
Todos estos métodos están pensandos para ser invocados en el bucle infinito principal, pero no está asegurada su ejecución (si el bucle contiene alguna función bloqueante, por ejemplo). Por supuesto, las condiciones de mayor igual aseguran que las cosas ocurran, pero no la precisión en tiempo. En un retraso no importa mucho, pero la ejecución de la colección de patrones se sentiría más en casa en una interrupción que ocurra la menos cada 1ms. Eso es fácil de hacer, de hecho, con el HAL de STM. Simplemente se agrega la llamada de la colección que quieras en `SysTick_Handler(void)`, en el archivo stm32f4xx_it.c.
