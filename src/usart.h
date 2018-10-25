#pragma once

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

class Usart
{
public:
	static void init()
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
        // 配置 PA9 为复用推挽输出，串口发送端口
		GPIO_InitTypeDef gpio_init{
			GPIO_Pin_9,
			GPIO_Speed_50MHz,
			GPIO_Mode_AF_PP
		};
		GPIO_Init(GPIOA, &gpio_init);
        // 配置 PA10 为上拉输入，串口接收端口
		gpio_init = {
			GPIO_Pin_10,
			GPIO_Speed_50MHz,
			GPIO_Mode_IPU
		};
		GPIO_Init(GPIOA, &gpio_init);
        // 配置串口中断
		NVIC_InitTypeDef nvic_init{USART1_IRQn, 0, 0, ENABLE};
		NVIC_Init(&nvic_init);
        // 配置串口为 1Mbps，发送和接收
		USART_InitTypeDef usart_init{
			1000000,
			USART_WordLength_8b,
			USART_StopBits_1,
			USART_Parity_No,
			USART_Mode_Rx | USART_Mode_Tx,
			USART_HardwareFlowControl_None
		};
		USART_Init(USART1, &usart_init);
        // 打开串口接收中断
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		USART_Cmd(USART1, ENABLE);
	}
    /* 发送一个字节 */
	static void send(uint8_t data)
	{
        // 等待发送寄存器空
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, data);
	}
};
