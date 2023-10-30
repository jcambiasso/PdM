/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
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
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

ETH_TxPacketConfig TxConfig;
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

ETH_HandleTypeDef heth;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
/**
 * Apunto el puntero a función getCurrentTick a la función que me interesa en este target.
 */
GetCurrentTick_t getCurrentTick = HAL_GetTick;
delay_t delayLed = {0};
TimerPattern_t testPattern1 = {0};
TimerPattern_t testPattern2 = {0};
TimerPattern_t testPattern3 = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
/* USER CODE BEGIN PFP */

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
  * @brief  Ejecuta una secuencia
  * @note	Era más fácil el código si tomaba un array de punteros a las estrucutras. Como para poder aceptar arrays de cualquier tamaño tengo que aceptar punteros
  * 		del mismo tipo (osea, puntero a TimerPattern_t), me termina quedando un puntero de puntero a TimerPattern_t.
  * 		Esta función _no_ es thread safe (uso una variable estática).
  * @param size Tamaño de la colección.
  * @retval true La secuencia finalizó
  * @retval false La secuencia no finalizó
  */
bool patternCollectionRun(TimerPattern_t ** collection, size_t size);


void patternRestart(TimerPattern_t* pattern);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void testPatternCallback(void){
	HAL_GPIO_TogglePin(GPIOB, LD1_Pin);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */

  delayInit(&delayLed, 100, false);
  patternInit(&testPattern1,&delayLed,5,1000,50,testPatternCallback,testPatternCallback);
  patternInit(&testPattern2,&delayLed,5,200,50,testPatternCallback,testPatternCallback);
  patternInit(&testPattern3,&delayLed,5,100,50,testPatternCallback,testPatternCallback);

  TimerPattern_t* patternArray[3] = {&testPattern1,&testPattern2,&testPattern3};

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  patternCollectionRun(patternArray,3);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void delayInit( delay_t * delay, tick_t duration, bool_t singleShot ){
	delay->startTime= 0;
	delay->running = false;
	delay->singleShot = singleShot;
	delay->duration = duration;
	if(delay->singleShot) delay->enabled=false;
	else delay->enabled = true;
	return;
}

bool_t delayRead( delay_t * delay ){
	if(delay->enabled == false) return false;
	if(delay->running == false){
		delay->startTime = getCurrentTick();
		delay->running = true;
		return false;
	}
	else{
		/**
		 * - Uso mayor o igual, porque con este mecanismo de "polling" al delay, es muy dificil pegarle justo al tiempo establecido.
		 * - getCurretTick() normalmente devolvería un número mayor a startTime, excepto justo después de un overflow del tipo uint32.
		 * 	Esto tal vez sugiera el uso de abs, pero no hace falta: la resta complemento a 1 con tipos unsigned da correctamente la distancias entre números.
		 * 	La excepción, y donde esto falla, es si delayRead se ejecuta después de una vuelta entera del uint32 (después de 1193 horas), más un poquito por encima
		 * 	de startTime, pero sin llegar a que la resta dé mayor a duration, con lo que como mucho este método puede tener un error de duration.
		 */
		if(getCurrentTick() - delay->startTime >= delay->duration){
			delay->running = false;
			if(delay->singleShot) delay->enabled=false;
			return true;
		}
		else return false;
	}
}

void delayWrite( delay_t * delay, tick_t duration, bool_t restart){
	delay->duration = duration;
	if(restart){
		delay->running = false;
		delayRead(delay);
	}
}

void delayStart(delay_t * delay){
	delay->enabled = true;
}

void delatStop(delay_t * delay){
	delay->enabled = false;
}

void patternInit(TimerPattern_t* pattern,delay_t* delay,uint16_t repeats,uint32_t interval, uint8_t dutyCicle, patternCallback callbackOff, patternCallback callbackOn){
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
	delayInit(delay,interval,false);
}

bool patternRun(TimerPattern_t* pattern){
	if(pattern->done) return true;
	if(pattern->currentCicle == pattern->repeats){
		pattern->done = true;
		return true;
	}
	if(!pattern->delay->running) {
		pattern->callbackOn();
		uint8_t dutycicle = pattern->dutyCicle;
		if(dutycicle > 99) dutycicle = 99;
		delayWrite(pattern->delay, (uint32_t)((pattern->interval*(dutycicle))/100), true );
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
	pattern->done = false;
}

bool patternCollectionRun(TimerPattern_t ** collection, size_t size) {
	static uint8_t index;
	if(patternRun(collection[index])){
		patternRestart(collection[index]);
		index++;
		if(index>size) {
			index = 0;
			return true;
		}
		else return false;
	}
	else return false;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
