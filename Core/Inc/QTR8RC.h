#pragma once

#include <cstdint>

extern "C" {
#include "main.h"
#include "tim.h"
}

class QTR8RC {
public:
    static constexpr int NUM_SENSORS = 8;
    static constexpr uint16_t TIMEOUT_US = 2500;

    QTR8RC(GPIO_TypeDef *port, const uint16_t pins[NUM_SENSORS],
           TIM_HandleTypeDef *timer);

    void Read();
    int32_t GetPosition() const;
    uint16_t GetValue(int index) const { return m_Values[index]; }

private:
    GPIO_TypeDef *m_Port;
    uint16_t m_Pins[NUM_SENSORS];
    uint16_t m_AllPinsMask;
    TIM_HandleTypeDef *m_Timer;
    uint16_t m_Values[NUM_SENSORS];

    void DelayUs(uint16_t us);
};
