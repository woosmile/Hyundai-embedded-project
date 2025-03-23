#include "transmitter.h"
#include "main.h"
#include "hw_vcom.h"
#include "command.h"
#include "at.h"
#include "stm32l0xx_nucleo.h"

void transmitData(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size){
	if(HAL_UART_Transmit(huart, pData, Size, 200) == HAL_OK){
		if(huart == &huart4){
			Printf("(To Top)");
		}
		else if(huart == &huart5){
			Printf("(To Bot)");
		}
		Printf("send data success: %s\r\n", pData);
	}
	else{
		Printf("timeout in sending data\r\n");
	}
}