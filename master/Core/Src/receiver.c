#include "receiver.h"
#include "main.h"
#include "hw_vcom.h"
#include "command.h"
#include "at.h"
#include "stm32l0xx_nucleo.h"

//RX callback function
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//UART4: FromEsp
	if (huart->Instance == USART4)
	{
		Printf("Received Data: %s\r\n", FromEsp);
		//ready to receive data one more time
		HAL_UART_Receive_IT(&huart4, (uint8_t*)FromEsp, FROMESPSIZE);
	}
	
	//UART5: FromSensor
	if (huart->Instance == USART5)  // USART4?? ??? ??
	{
			Printf("Received Data: %s\r\n", FromSensor);
		//ready to receive data one more time
		HAL_UART_Receive_IT(&huart5, (uint8_t*)FromSensor, FROMSENSORSIZE);
	}
}