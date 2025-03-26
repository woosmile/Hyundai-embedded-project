/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "hw_vcom.h"
#include "command.h"
#include "at.h"
#include "stm32l0xx_nucleo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TX_ESP_DATA_SIZE 31
#define RX_ESP_DATA_SIZE 1   // Output Actuator Control
#define RX_SENSOR_DATA_SIZE 15
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim7;

/* USER CODE BEGIN PV */

//UART Flag
__IO ITStatus Uart1_Ready = RESET;
__IO ITStatus Uart4_Ready = RESET;
__IO ITStatus Uart5_Ready = RESET;
//Transmit to Sensor receive done flag
uint8_t receiveComplete = 1;
//Transmit to LCD car startup status
uint8_t carStartup = 0;
//Receive form LCD receive done flag
uint8_t checkReceive;
// Transmit data to ESP
uint8_t TxESP[TX_ESP_DATA_SIZE];
// Receive data from ESP
uint8_t RxESP;
// Receive data from Sensor
uint8_t RxSensor[RX_SENSOR_DATA_SIZE];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_USART4_UART_Init(void);
static void MX_USART5_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint32_t	it_1sec = 0;
volatile uint32_t direction = 0;
volatile uint32_t degree = 0;
volatile uint32_t mode_input = 0;
//volatile uint32_t op_time = 0;

volatile uint32_t temperature = 0;
volatile uint32_t humidity = 0;
volatile uint32_t light_lux = 0; //206~326
volatile uint32_t shock = 0;
volatile uint32_t potentiometer = 0; //0~326

volatile uint32_t is_open_door = 0;
volatile uint32_t next_open_door = 0;
volatile uint32_t is_start_fan = 0;
volatile uint32_t next_start_fan = 0;
volatile uint32_t is_open_sunroof = 0;
volatile uint32_t next_open_sunroof = 0;
volatile uint32_t is_driving = 0;
volatile uint32_t next_driving = 0;
volatile uint32_t emergency_stop = 0;
volatile uint32_t next_emergency_stop = 0;


//volatile uint32_t temp_user = 0;
volatile uint32_t bluetooth = 0;
volatile uint32_t user_open_sunroof = 0;
volatile uint16_t track = 1;

volatile int speed = 0;
// Shock
volatile int bVib = 0;
// DHT11
volatile int Temperature, Humidity;
//Rotation(Car Velocity)
volatile int rot;
//Light
volatile int light;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance== TIM7)
 {
	 it_1sec = 1;
	 //if(op_time!=0) op_time++;
 }
}

void open_door(){
	degree = 60;
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3,degree);
}
void close_door(){
	degree = 120;
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_3,degree);
}
void open_sunroof(){
	degree = 60;
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4,degree);
}
void close_sunroof(){
	degree = 120;
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4,degree);
}
/*
void control_wheel(uint32_t mode){
	uint32_t speed = 5;
	if(op_time==0){
		if(mode==0){
			// stop
			direction = 76;
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,direction);
		}
		else if(mode==1){
			// close
			op_time = 1;
			direction = 76+speed;
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,direction);
		}
		else if(mode==2){
			// open
			op_time = 1;
			direction = 75-speed;
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,direction);
		}
	}
	else if(op_time>3) op_time = 0;
}
*/
/*
void control_wheel(uint32_t mode, uint32_t speed){
	if(mode==0){
		// stop
		direction = 76;
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,direction);
	}
	else if(mode==1){
		// forward
		direction = 76+speed;
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,direction);
	}
	else if(mode==2){
		// backward
		direction = 75-speed;
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,direction);
	}
}
*/

void DFPlayer_Send_Command(uint8_t* command, uint8_t length)
{
    HAL_UART_Transmit(&hlpuart1, command, length, HAL_MAX_DELAY);
}

void DFPlayer_Play_Track(uint16_t track_number)
{
    uint8_t command[10];
    
    // 0x7E: Start byte, 0xFF: Version byte, 0x06: Command byte (Play)
    command[0] = 0x7E;
    command[1] = 0xFF;
    command[2] = 0x06;
    command[3] = 0x03;
    command[4] = 0x00;
    command[5] = (track_number >> 8) & 0xFF; // High byte of track number
    command[6] = track_number & 0xFF;        // Low byte of track number
		uint16_t checksum = 0xFFFF - (command[1]+command[2]+command[3]+command[4]+command[5]+command[6]) + 1;
    command[7] = (checksum >> 8) & 0xFF;
		command[8] = checksum & 0xFF; // Checksum (optional, can be computed)
    command[9] = 0xEF;                       // End byte
    
		DFPlayer_Send_Command(command, sizeof(command));
}
void DFPlayer_Stop(void)
{
    uint8_t command[10];
  
		command[0] = 0x7E;
    command[1] = 0xFF;
    command[2] = 0x06;
    command[3] = 0x0E;
    command[4] = 0x00;
    command[5] = 0x00; // High byte of track number
    command[6] = 0x00; // Low byte of track number
		uint16_t checksum = 0xFFFF - (command[1]+command[2]+command[3]+command[4]+command[5]+command[6]) + 1;
    command[7] = (checksum >> 8) & 0xFF;
		command[8] = checksum & 0xFF; // Checksum (optional, can be computed)
    command[9] = 0xEF;                       // End byte
	
    DFPlayer_Send_Command(command, sizeof(command));
}

void start_fan(){
	HAL_GPIO_WritePin(FAN_GPIO_Port,FAN_Pin,GPIO_PIN_SET);
}
void stop_fan(){
	HAL_GPIO_WritePin(FAN_GPIO_Port,FAN_Pin,GPIO_PIN_RESET);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
	if (UartHandle->Instance == USART1)
		Uart1_Ready = SET;
	if (UartHandle->Instance == USART5)
		Uart5_Ready = SET;
	if (UartHandle->Instance == USART4) {
		Uart4_Ready = SET;
		
		//Receive data from ESP
		HAL_UART_Receive_IT(&huart4, &RxESP, RX_ESP_DATA_SIZE);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete */
	if (UartHandle->Instance == USART1)
		Uart1_Ready = SET;
	if (UartHandle->Instance == USART5)
		Uart5_Ready = SET;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch(GPIO_Pin) {
		case USER_BUTTON_PIN:
			carStartup = !carStartup;
			BSP_LED_Toggle(LED2);
		break;
	}
}

void transmitData(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size){
	if(HAL_UART_Transmit(huart, pData, Size, 200) == HAL_OK){
		if(huart == &huart4){
			Printf("Transmit to ESP Success: ");
			TxESP[0] = 0;
		}
		for (volatile int i = 0; i <= 30; i++)
			Printf("%d", pData[i]);
		Printf("\r\n");
	}
	else{
		Printf("Timeout Transmit to ESP\r\n");
	}
}

void dataInit(void) {
	Temperature = 0;
	Humidity = 0;
	bVib = 0;
	rot = 0;
	light = 0;
}

void dataParsing(void) {
	for (volatile int i = 0; i < 5; i++) {
		volatile int digit = 100;
		for (volatile int j = i * 3; j < (i * 3) + 3; j++) {
			switch(i) {
				case 0:
					Temperature = Temperature + (RxSensor[j] * digit);
					break ;
				case 1:
					Humidity = Humidity + (RxSensor[j] * digit);
					break ;
				case 2:
					light = light + (RxSensor[j] * digit);
					break ;
				case 3:
					bVib = bVib + (RxSensor[j] * digit);
					break ;
				case 4: 
					rot = rot + (RxSensor[j] * digit);
					break ;
				default:
					break ;
			}
			digit = digit / 10;
		}
	}
}

void combineTxESP(void) {
	//Output Actuator Modify Flag
	//TxESP[0] = 0;
	
	for (volatile int i = 1; i <= 15; i++) {
		TxESP[i] = RxSensor[i - 1];
	}
	
	//Output Actuator Status
	for (volatile int i = 16; i <= 30; i++) {
		TxESP[i] = 0;
	}
	TxESP[16] = 0;
	TxESP[16] = 0;
	TxESP[16] = (uint8_t) is_open_door;
	
	TxESP[16] = 0;
	TxESP[16] = 0;
	TxESP[16] = (uint8_t) is_start_fan;
	
	TxESP[16] = 0;
	TxESP[16] = 0;
	TxESP[16] = (uint8_t) is_open_sunroof;
	
	TxESP[16] = 0;
	TxESP[16] = 0;
	TxESP[16] = (uint8_t) is_driving;
	
	TxESP[16] = 0;
	TxESP[16] = 0;
	TxESP[16] = (uint8_t) emergency_stop;
}

void controlActuator(void) {
	
	if (RxESP == '0')
		carStartup = 0;
	else if (RxESP == '1')
		carStartup = 1;
	else if (RxESP == '2')
		next_open_sunroof = 0;
	else if (RxESP == '3')
		next_open_sunroof = 1;
	else if (RxESP == '4')
		next_start_fan = 0;
	else if (RxESP == '5')
		next_start_fan = 1;
	else if (RxESP == '6')
		next_driving = 0;
	else if (RxESP == '7')
		next_driving = 1;
	else if (RxESP == '8')
		next_open_door = 0;
	else if (RxESP == '9')
		next_open_door = 1;
}
void control_wheel(){
		speed = ((int)rot)/10;
		direction = 76 + speed;
		__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2,direction);
}
void control_sunroof(){
	if(next_open_sunroof != is_open_sunroof){
		TxESP[0] = 1;
		is_open_sunroof = next_open_sunroof;
		if(is_open_sunroof==1) open_sunroof();
		else if(is_open_sunroof==0) close_sunroof();
		//else if(is_open_sunroof || light_lux < 160) open_sunroof();
		//else close_sunroof();
	}
}
void control_door(){
	if(speed > 0){
		next_open_door = 0;
	}
	if(next_open_door != is_open_door){
		TxESP[0] = 1;
		is_open_door = next_open_door;
		if(is_open_door){
			open_door();
			DFPlayer_Play_Track(1);
		}
		else{
			close_door();
			DFPlayer_Play_Track(2);
		}
	}
}
void control_fan(){
	if(next_start_fan != is_start_fan){
		TxESP[0] = 1;
		is_start_fan = next_start_fan;
		if(is_start_fan) start_fan();
		else stop_fan();
	}
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
  MX_TIM7_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_LPUART1_UART_Init();
  MX_USART4_UART_Init();
  MX_USART5_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	Printf("MASTER BOARD STARTUP\r\n");
	BSP_LED_Init(LED2);
	HAL_TIM_Base_Start_IT(&htim7); // for 1sec interupt
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2); // for Servo Motor
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3); // for Servo Motor
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4); // for Servo Motor
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	// from esp to master receive ready
	HAL_UART_Receive_IT(&huart4, &RxESP, RX_ESP_DATA_SIZE);
	
  while (1)
  {
		if(HAL_UART_Transmit_IT(&huart5, &carStartup, 1)!= HAL_OK)
		{
			Error_Handler();
		}
		
		while (Uart5_Ready != SET)
		{
		} 
		Uart5_Ready = RESET;
		
		if(HAL_UART_Receive_IT(&huart5, &checkReceive, 1) != HAL_OK)
		{
			Error_Handler();
		}
		
		/*##-5- Wait for the end of the transfer ###################################*/   
		while (Uart5_Ready != SET)
		{
		} 
		
		/* Reset transmission flag */
		Uart5_Ready = RESET;
		
		if(HAL_UART_Receive_IT(&huart1, (uint8_t *)RxSensor, RX_SENSOR_DATA_SIZE) != HAL_OK)
		{
			Error_Handler();
		}
		
		while (Uart1_Ready != SET)
		{
		} 
		Uart1_Ready = RESET;
		
		for (volatile int i = 0; i < 15; i++) {
			Printf("%d", RxSensor[i]);
		}
		Printf("\r\n");

		if(HAL_UART_Transmit_IT(&huart1, &receiveComplete, 1)!= HAL_OK)
		{
			Error_Handler();
		}
		
		while (Uart1_Ready != SET)
		{
		} 
		Uart1_Ready = RESET;
		

		
		
		
		// wookyung's code begin
		dataParsing();
		
		// Combine TxESP data to transmit ESP
		combineTxESP();
		
		control_wheel();
	
		control_sunroof();
		
		control_door();
		
		control_fan();
	
		if(it_1sec){
			it_1sec = 0;

			//Transmit Data to ESP
      transmitData(&huart4, TxESP, TX_ESP_DATA_SIZE);
			
			//DFPlayer_Play_Track(track++);
			//if(track==5) track = 1;
			
			//Dummy Data from system
			temperature = (temperature + 3) % 50;
			humidity = (humidity + 8) % 100;
			light_lux = (light_lux + 30) % 326;
			shock = (shock + 1) % 2;
			potentiometer = (potentiometer + 10) % 326;
			
			//Dummy Data from user
			is_open_door = (is_open_door + 1) % 2;
			//temp_user = (temp_user + 13) % 50;
			is_open_sunroof = (is_open_sunroof + 1) % 2;
			bluetooth = (bluetooth + 1) % 2;
		}
		
		if(Uart4_Ready == SET){
			Uart4_Ready = RESET;
			//data process with fromtop
			Printf("Receive from ESP Success: %c\r\n", RxESP);
			controlActuator();
    }
		
		dataInit();
    /* USER CODE END WHILE */

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_LPUART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 9600;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */


	HW_VCOM_Init(&huart2);
#if 0
  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
#endif
  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART4_UART_Init(void)
{

  /* USER CODE BEGIN USART4_Init 0 */

  /* USER CODE END USART4_Init 0 */

  /* USER CODE BEGIN USART4_Init 1 */

  /* USER CODE END USART4_Init 1 */
  huart4.Instance = USART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART4_Init 2 */

  /* USER CODE END USART4_Init 2 */

}

/**
  * @brief USART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART5_UART_Init(void)
{

  /* USER CODE BEGIN USART5_Init 0 */

  /* USER CODE END USART5_Init 0 */

  /* USER CODE BEGIN USART5_Init 1 */

  /* USER CODE END USART5_Init 1 */
  huart5.Instance = USART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART5_Init 2 */

  /* USER CODE END USART5_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 160-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 800-1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 10000-1;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

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

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_BUTTON_Pin */
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : FAN_Pin */
  GPIO_InitStruct.Pin = FAN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FAN_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
