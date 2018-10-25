#pragma once

#include  <functional>
#include <vector>
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

using std::function;
using std::vector;

class Timer;

struct TickHandler
{
    function<void()> h;
    uint16_t tick_t, tick_c;
};

class Timer
{
public:
    static void init()
    {
        SysTick_Config(72000);
    }
    /* 延时 t 毫秒 */
    static void delayMs(uint16_t t)
    {
        time = t;
        while (time);
    }
    static void initTimer1()
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        // 配置定时器1为200Hz，定时执行测速、PID 控制
        TIM_TimeBaseInitTypeDef tim_init
        {
            359,
            TIM_CounterMode_Up,
            999,
            TIM_CKD_DIV1,
            0
        };
        TIM_TimeBaseInit(TIM1, &tim_init);
        // 打开定时器更新中断
        NVIC_InitTypeDef nvic_init
            {TIM1_UP_IRQn, 0, 0, ENABLE};
        NVIC_Init(&nvic_init);
        TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
        TIM_Cmd(TIM1, ENABLE);
    }
    /* 设置每隔 time 毫秒定时执行一次 func 函数 */
    static void setOnTick(uint16_t time, const function<void()>& func)
    {
        tickHandler.push_back(TickHandler{
            std::move(func), time, time
        });
    }
    /* 设置每隔 1 毫秒定时执行一次 func 函数 */
    static void setOnTick(const function<void()>& func)
    {
        setOnTick(1, std::move(func));
    }
    volatile static uint16_t time;
    static vector<TickHandler> tickHandler;
};
