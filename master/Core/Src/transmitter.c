#include "transmitter.h"
#include "main.h"
#include "hw_vcom.h"
#include "command.h"
#include "at.h"
#include "stm32l0xx_nucleo.h"

//function to transmit data to ESP every second 
void transmitdata(void){
	Printf("every 1 sec\r\n");
	HAL_UART_Transmit_IT(&huart4, (uint8_t*)ToEsp, TOESPSIZE);
	it_1sec_uart = 0;
}

//TX callback function
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if (huart->Instance == USART4)  // USART2 ????? ???? ?? ??
	{
		//Printf("Data to ESP<%s> is sent successfully.\r\n", ToEsp);
	}
}