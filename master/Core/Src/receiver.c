#include "receiver.h"
#include "main.h"
#include "hw_vcom.h"
#include "command.h"
#include "at.h"
#include "stm32l0xx_nucleo.h"

//RX callback function
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART4) {
		__IO ITStatus Uart4R_Ready = SET;
	}
	HAL_UART_Receive_IT(&huart4, (uint8_t*)FromTop, FROMTOPSIZE);
}


void receiveData(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size){
	if(HAL_UART_Receive(huart, pData, Size, 400) == HAL_OK){
		if(huart == &huart4){
			Printf("(From Top)");
		}
		if(huart == &huart5){
			Printf("(From Bot)");
		}
		Printf("receive data success: %s\r\n", pData);
	}
	else{
		Printf("timeout in receiving data\r\n");
	}
}