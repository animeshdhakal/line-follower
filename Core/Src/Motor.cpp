#include "Motor.h"

Motor::Motor(const MotorPins &pins, uint32_t maxDuty)
    : m_Pins(pins), m_MaxDuty(maxDuty)
{
}

void Motor::Start()
{
    HAL_TIM_PWM_Start(m_Pins.PwmTimer, m_Pins.PwmChannel);
}

void Motor::SetSpeed(int32_t speed)
{
    if (speed >= 0) {
        HAL_GPIO_WritePin(m_Pins.DirPort1, m_Pins.DirPin1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(m_Pins.DirPort2, m_Pins.DirPin2, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(m_Pins.DirPort1, m_Pins.DirPin1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(m_Pins.DirPort2, m_Pins.DirPin2, GPIO_PIN_SET);
        speed = -speed;
    }

    uint32_t duty = static_cast<uint32_t>(speed);
    if (duty > m_MaxDuty)
        duty = m_MaxDuty;

    __HAL_TIM_SET_COMPARE(m_Pins.PwmTimer, m_Pins.PwmChannel, duty);
}
