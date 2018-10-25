#pragma once

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

class Encoder
{
public:
    static void init()
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        // 配置 PA0 PA1 为浮空输入
        GPIO_InitTypeDef gpio_init{
            GPIO_Pin_0 | GPIO_Pin_1,
            GPIO_Speed_50MHz,
            GPIO_Mode_IN_FLOATING
        };
        GPIO_Init(GPIOA, &gpio_init);
        TIM_TimeBaseInitTypeDef tim_init{
            0, TIM_CounterMode_Up, 0xFFFF, TIM_CKD_DIV1, 0
        };
        TIM_TimeBaseInit(TIM2, &tim_init);
        // 配置编码器接口
        TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12,
            TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
        TIM_Cmd(TIM2, ENABLE);
    }
    /* 读取定时器计数值 */
    static uint16_t getValue()
    {
        auto val = TIM_GetCounter(TIM2);
        return val;
    }
};
