#include "controller.h"

volatile int Controller::eSpeed = 0;
float Controller::kp = 5; // 10
float Controller::ki = 1.5; // 0.1