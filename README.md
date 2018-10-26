# 基于STM32的直流电机调速系统

## 下位机软件
使用C++开发，基于 IAR Embedded Workbench for Arm
* 采用 STM32F103V8 微控制器
* 采用 M 法测速，测速周期 5ms
* PI 调节器速度闭环控制
* 速度阶跃响应超调量小于 5%
* 运行时调节 Kp, Ki 参数

## 上位机软件
使用 C# 和 WPF 开发，基于 Visual Studio 2017
![Screenshot](https://raw.githubusercontent.com/dingzimin/DC-Motor-Controller-System-Based-on-STM32/master/images/Screenshot0.png)
* 调整电机的速度
* 显示电机转速变化曲线
* 实时显示电机转速
* 调节Kp, Ki参数
