#include "timer.h"

volatile uint16_t Timer::time;
vector<TickHandler> Timer::tickHandler;

extern "C"
void SysTick_Handler()
{
    if (Timer::time)
        Timer::time--;
    for (auto &h : Timer::tickHandler)
    {
        if (!h.h)
            continue;
        if (h.tick_c)
            h.tick_c--;
        if (h.tick_c == 0)
        {
            h.tick_c = h.tick_t;
            h.h();
        }
    }
}
