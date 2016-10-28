/*
 * fileC.c
 *
 *  Created on: 28. 10. 2016
 *      Author: Lenovo
 */

#include <stddef.h>
#include "stm32l1xx.h"
#include "fileH.h"
#include <stdio.h>
#include <string.h>

void initUSART2(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;


  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  //choosing peripherals for selected pins
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);  // GPIO_PinSource10, GPIO_AF_USART1
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);   // GPIO_PinSource9, GPIO_AF_USART1

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;   //  GPIO_Pin_10 | GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //usart configuration
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);

  //configuring interrupts
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  //choosing which event should cause interrupt
  USART_ITConfig(USART2, USART_IT_TXE, ENABLE);   // usart 1 / pre poslednu ulohu uprava pre posielanie hodnot

  // Enable USART
  USART_Cmd(USART2, ENABLE);
}

// zapis hodnoty do buffera
int dlzka = 0;
char znak[];
void posliDoFunkcie()
{
	int value1 = 0;
    value1 = 1234; // pre testovanie hodnota na tvrdo

	sprintf(znak,"%d",value1); // zapis do pola

    dlzka=strlen(znak);
}

int i = 0;
void USART2_IRQHandler(void)
{
	if(i < dlzka)
	{
		if (USART_GetFlagStatus(USART2, USART_FLAG_TC) != RESET)
		{
			if (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != RESET)
			{
				USART_SendData(USART2,znak[i]); //posielanie dat
				USART_ClearITPendingBit(USART2, USART_FLAG_TXE);

				i++;

				USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET;
			}
		}
	}
}
