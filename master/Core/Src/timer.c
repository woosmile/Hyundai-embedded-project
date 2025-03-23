#include "timer.h"
#include "main.h"
#include "hw_vcom.h"
#include "command.h"
#include "at.h"
#include "stm32l0xx_nucleo.h"

//interrupt timer funtion
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM6){
		it_1sec_uart = 1;
	}
}

void force_uart_rx_check() {
    static uint32_t last_check = 0;
    if (HAL_GetTick() - last_check > 1000) {
        last_check = HAL_GetTick();

        HAL_UART_AbortReceive(&huart4);
        HAL_UART_Receive_IT(&huart4, FromBot, FROMBOTSIZE);
    }
}