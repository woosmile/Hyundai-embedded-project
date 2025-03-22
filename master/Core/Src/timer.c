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