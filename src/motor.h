#pragma once

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

class Motor
{
public:
    static void init()
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        // 配置 PB13 PB14 为推挽输出，控制电机方向
        GPIO_InitTypeDef gpio_init
        {
            GPIO_Pin_13 | GPIO_Pin_14,
            GPIO_Speed_50MHz,
            GPIO_Mode_Out_PP
        };
        GPIO_Init(GPIOB, &gpio_init);
        stop();
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
        GPIO_PinRemapConfig(AFIO_MAPR_TIM3_REMAP_PARTIALREMAP, ENABLE);
        // 配置 PB4 PB5 为复用推挽输出，产生 PWM 脉冲
        gpio_init = {
            GPIO_Pin_4 | GPIO_Pin_5,
            GPIO_Speed_50MHz,
            GPIO_Mode_AF_PP
        };
        GPIO_Init(GPIOB, &gpio_init);
        // 配置定时器3为20KHz，输出PWM信号
        TIM_TimeBaseInitTypeDef tim_init
        { 0, TIM_CounterMode_Up, 3599, TIM_CKD_DIV1, 0 };
        TIM_TimeBaseInit(TIM3, &tim_init);
        // 配置输出比较模式
        TIM_OCInitTypeDef oc_init
        {
            TIM_OCMode_PWM1,
            TIM_OutputState_Enable,
            TIM_OutputNState_Disable,
            0,
            TIM_OCPolarity_High,
            TIM_OCNPolarity_High,
            TIM_OCIdleState_Reset,
            TIM_OCNIdleState_Reset
        };
        TIM_OC1Init(TIM3, &oc_init);
        TIM_OC2Init(TIM3, &oc_init);
        TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
        TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
        TIM_ARRPreloadConfig(TIM3, ENABLE);
        TIM_Cmd(TIM3, ENABLE);
    }
    /* 设置比较输出值，即占空比 */
    static void period(uint16_t t)
    {
        TIM_SetCompare2(TIM3, t);
    }
    /* 电机停止 */
    static void stop()
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14);
    }
    /* 电机正转 */
    static void cw()
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_13);
        GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    }
    /* 电机反转 */
    static void ccw()
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_14);
        GPIO_ResetBits(GPIOB, GPIO_Pin_13);
    }
};