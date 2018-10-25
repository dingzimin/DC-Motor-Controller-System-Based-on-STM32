#include "timer.h"
#include "led.h"
#include "motor.h"
#include "usart.h"
#include "encoder.h"
#include "controller.h"

int main()
{
    Timer::init();
    Led::init();
    Motor::init();
    Usart::init();
    Encoder::init();
    Timer::initTimer1();
    // 每隔 20ms 向上位机发送速度数据
    Timer::setOnTick(20, []()
    {
        static uint16_t lastval = 0;
        auto v = Encoder::getValue();
        auto s = v - lastval;
        lastval = v;
        Usart::send(s >> 8);
        Usart::send(s & 0xFF);
    });
    // 每隔 500ms 改变 LED 亮灭
    Timer::setOnTick(500, []()
    {
        static bool s = false;
        s = !s;
        s ? Led::on() : Led::off();
    });
    while (true);
}

extern "C"
void TIM1_UP_IRQHandler()
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        // 执行一次 PI 控制
        Controller::execMotor();
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

extern "C"
void USART1_IRQHandler()
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        int8_t s = USART_ReceiveData(USART1);
        static uint8_t kpi = 0xFF;
        static uint8_t d[8];
        if (kpi != 0xFF)
        {
            // 接收新的 Kp 和 Ki
            d[kpi] = s;
            kpi++;
            if (kpi == 8)
            {
                // 更新 Kp 和 Ki
                kpi = 0xFF;
                Controller::kp = *(float *)d;
                Controller::ki = *(float *)(d + 4);
            }
        }
        else if (s != 0x7F) // 设置给定速度
            Controller::setSpeed(s * 52);
        else
            kpi = 0;
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}