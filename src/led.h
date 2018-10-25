#pragma once

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

class Led
{
public:
    static void init()
    {
        // 配置 PC13 为推挽输出
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        GPIO_InitTypeDef gpio_init{
            gpio_pin,
            GPIO_Speed_50MHz,
            GPIO_Mode_Out_PP
        };
        GPIO_Init(gpiox, &gpio_init);
    }
    /* LED 亮 */
    static void on()
    {
        GPIO_SetBits(gpiox, gpio_pin);
    }
    /* LED 灭 */
    static void off()
    {
        GPIO_ResetBits(gpiox, gpio_pin);
    }
private:
    static constexpr GPIO_TypeDef *gpiox = GPIOC;
    static constexpr uint16_t gpio_pin = GPIO_Pin_13;
};
