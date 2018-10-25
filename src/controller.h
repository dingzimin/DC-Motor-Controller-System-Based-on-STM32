#pragma once

#include "motor.h"
#include "encoder.h"

class Controller
{
public:
    /* 执行一次 PI 控制 */
    static void execMotor()
    {
        int16_t speed = getEncoder() * 200; // 测速转换
        speed = speed >= 0 ? speed : -speed;
        float v = pid((eSpeed - speed) / 625.0f); // PI 控制
        int p = (int)(v * 3600 / 1200); // 转换
        Motor::period(p < 0 ? 0 : p > 3600 ? 3600 : p); //设置占空比
    }
	/* 设置给定速度 */
    static void setSpeed(int s)
    {
        if (s > 0) // 正转
            Motor::cw();
        else if (s < 0)
            Motor::ccw(); // 反转
        else
            Motor::stop(); // 停止
        eSpeed = s >= 0 ? s : -s;
    }
    static float kp;
    static float ki;
private:
    static constexpr float dt = 5;
    static volatile int eSpeed; // 给定速度
	/* 获取编码器计数个数 */
    static uint16_t getEncoder()
    {
        static int16_t lastv = 0;
        uint16_t v = Encoder::getValue();
        uint16_t r = v - lastv;
        lastv = v;
        return r;
    }
	/* PI 控制器 */
    static float pid(float e)
    {
        static float is = 0;
        is += dt * ki * e;
        float v = kp * e + is;
        return v;
    }
};
