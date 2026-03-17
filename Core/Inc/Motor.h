#pragma once

#include <cstdint>

extern "C" {
#include "main.h"
#include "tim.h"
}

struct MotorPins {
    GPIO_TypeDef *DirPort1;
    uint16_t DirPin1;
    GPIO_TypeDef *DirPort2;
    uint16_t DirPin2;
    TIM_HandleTypeDef *PwmTimer;
    uint32_t PwmChannel;
};

class Motor {
public:
    Motor(const MotorPins &pins, uint32_t maxDuty);

    void Start();
    void SetSpeed(int32_t speed);

private:
    MotorPins m_Pins;
    uint32_t m_MaxDuty;
};
