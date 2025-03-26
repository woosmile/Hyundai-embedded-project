/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: MCU UART

License: Revised BSD License, see LICENSE.TXT file include in the project
*/
 /*******************************************************************************
  * @file    hw_uart.c
  * @author  MCD Application Team
  * @version V1.1.2
  * @date    08-September-2017
  * @brief   driver for UART
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hw_vcom.h"
#include "main.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* Exported variables ---------------------------------------------------------*/
/* UART handler declaration */
UART_HandleTypeDef *huart;

/* Private typedef -----------------------------------------------------------*/
static struct {
  char buffRx[256];                   /**< Circular buffer of received chars */
  int rx_idx_free;                    /**< 1st free index in BuffRx */
  int rx_idx_toread;                  /**< next char to read in buffRx, when not rx_idx_free */
  int status;                         /**< Status for UART RX Occurred */
} uart_context;
/* Private define ------------------------------------------------------------*/
#define BUFSIZE 256
/* Private macro -------------------------------------------------------------*/
/**
  * @brief macro used to enter the critical section
  */
#define ENTER_CRITICAL_SECTION() uint32_t primask_bit= __get_PRIMASK();\
  __disable_irq()

/**
  * @brief macro used to exit the critical section
  */
#define EXIT_CRITICAL_SECTION()  __set_PRIMASK(primask_bit)

/* Private variables ---------------------------------------------------------*/
__IO ITStatus UartReady = RESET;
/* buffer */
static char buff[BUFSIZE];
/* buffer write index*/
__IO uint16_t iw=0;
/* buffer read index*/
static uint16_t ir=0;

unsigned char uart2_rcvbuf;

/* Private function prototypes -----------------------------------------------*/
static void receive(unsigned char rx);

/* Exported functions ---------------------------------------------------------*/

void HW_VCOM_Init( UART_HandleTypeDef *h_uart )
{
	huart = h_uart;
	if(huart == NULL)
		return;
	
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
  huart->Instance = USART2;
  huart->Init.BaudRate = 9600;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;	
	
  if(HAL_UART_Init(huart) != HAL_OK)
  {
    Error_Handler();
  }
	
	HAL_UART_Receive_IT(h_uart,&uart2_rcvbuf, 1);
}

void DBGPrintf( const char *format, ... )
{
#ifdef DBG	
  va_list args;
  va_start(args, format);
  uint8_t len;
  uint8_t lenTop;
  char tempBuff[128];
  
  BACKUP_PRIMASK();
  DISABLE_IRQ();
  
  /*convert into string at buff[0] of length iw*/
  len = vsprintf(&tempBuff[0], format, args);
  
  if (iw+len<BUFSIZE)
  {
    memcpy( &buff[iw], &tempBuff[0], len);
    iw+=len;
  }
  else
  {
    lenTop=BUFSIZE-iw;
    memcpy( &buff[iw], &tempBuff[0], lenTop);
    len-=lenTop;
    memcpy( &buff[0], &tempBuff[lenTop], len);
    iw = len;
  }
  RESTORE_PRIMASK();
  
//  HAL_NVIC_SetPendingIRQ(USARTX_IRQn);
	HW_VCOM_TxData();
  va_end(args);
#endif	
}


void Printf( const char *format, ... )
{
  va_list args;
  va_start(args, format);
  uint8_t len;
  uint8_t lenTop;
  char tempBuff[128];
  
  ENTER_CRITICAL_SECTION();
  
  /*convert into string at buff[0] of length iw*/
  len = vsprintf(&tempBuff[0], format, args);
  
  if (iw+len<BUFSIZE)
  {
    memcpy( &buff[iw], &tempBuff[0], len);
    iw+=len;
  }
  else
  {
    lenTop=BUFSIZE-iw;
    memcpy( &buff[iw], &tempBuff[0], lenTop);
    len-=lenTop;
    memcpy( &buff[0], &tempBuff[lenTop], len);
    iw = len;
  }
  EXIT_CRITICAL_SECTION();
  
	HW_VCOM_TxData();
  va_end(args);
}

void HW_VCOM_TxData(void)
{
	char* CurChar;
	while( ( (iw+BUFSIZE-ir)%BUFSIZE) >0 )
  {
    ENTER_CRITICAL_SECTION();
    
    CurChar = &buff[ir];
    ir= (ir+1) %BUFSIZE;
    
    EXIT_CRITICAL_SECTION();
	  HAL_UART_Transmit(huart,(uint8_t *) CurChar, 1, 300);
  }
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HW_VCOM_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	receive(uart2_rcvbuf);
	HAL_UART_Receive_IT(UartHandle,&uart2_rcvbuf, 1);
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle

  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
    Error_Handler();
}

static void receive(unsigned char rx)
{
  int next_free;

  /** no need to clear the RXNE flag because it is auto cleared by reading the data*/
  uart_context.buffRx[uart_context.rx_idx_free] = rx;
  next_free = (uart_context.rx_idx_free + 1) % sizeof(uart_context.buffRx);
  if (next_free != uart_context.rx_idx_toread)
  {
    /* this is ok to read as there is no buffer overflow in input */
    uart_context.rx_idx_free = next_free;
  }
  else
  {
    /* force the end of a command in case of overflow so that we can process it */
    uart_context.buffRx[uart_context.rx_idx_free] = '\r';
    PRINTF("uart_context.buffRx buffer overflow %d\r\n");
  }
	
	uart_context.status = 1;
}


FlagStatus IsNewCharReceived(void)
{
  FlagStatus status;
  ENTER_CRITICAL_SECTION();
  
  status = ((uart_context.rx_idx_toread == uart_context.rx_idx_free) ? RESET : SET);
  
  EXIT_CRITICAL_SECTION();
  
  return status;
}

uint8_t GetNewChar(void)
{
  uint8_t NewChar;

  ENTER_CRITICAL_SECTION();

  NewChar = uart_context.buffRx[uart_context.rx_idx_toread];
  uart_context.rx_idx_toread = (uart_context.rx_idx_toread + 1) % sizeof(uart_context.buffRx);

  EXIT_CRITICAL_SECTION();
  return NewChar;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

