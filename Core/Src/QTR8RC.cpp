#include "QTR8RC.h"

QTR8RC::QTR8RC(GPIO_TypeDef *port, const uint16_t pins[NUM_SENSORS],
               TIM_HandleTypeDef *timer)
    : m_Port(port), m_AllPinsMask(0), m_Timer(timer), m_Values{}
{
    for (int i = 0; i < NUM_SENSORS; i++) {
        m_Pins[i] = pins[i];
        m_AllPinsMask |= pins[i];
    }
}

void QTR8RC::DelayUs(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(m_Timer, 0);
    while (__HAL_TIM_GET_COUNTER(m_Timer) < us) {
    }
}

void QTR8RC::Read()
{
    GPIO_InitTypeDef gpio = {};

    // Charge capacitors
    gpio.Pin   = m_AllPinsMask;
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(m_Port, &gpio);
    HAL_GPIO_WritePin(m_Port, m_AllPinsMask, GPIO_PIN_SET);

    DelayUs(10);

    // Switch to input and time discharge
    gpio.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(m_Port, &gpio);

    for (int i = 0; i < NUM_SENSORS; i++)
        m_Values[i] = TIMEOUT_US;

    __HAL_TIM_SET_COUNTER(m_Timer, 0);
    uint8_t remaining = NUM_SENSORS;

    while (remaining > 0 && __HAL_TIM_GET_COUNTER(m_Timer) < TIMEOUT_US) {
        uint16_t now = __HAL_TIM_GET_COUNTER(m_Timer);
        for (int i = 0; i < NUM_SENSORS; i++) {
            if (m_Values[i] == TIMEOUT_US &&
                HAL_GPIO_ReadPin(m_Port, m_Pins[i]) == GPIO_PIN_RESET) {
                m_Values[i] = now;
                remaining--;
            }
        }
    }

    // Restore pins to output LOW
    gpio.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(m_Port, &gpio);
    HAL_GPIO_WritePin(m_Port, m_AllPinsMask, GPIO_PIN_RESET);
}

int32_t QTR8RC::GetPosition() const
{
    uint32_t weighted_sum = 0;
    uint32_t sum = 0;

    for (int i = 0; i < NUM_SENSORS; i++) {
        weighted_sum += (uint32_t)m_Values[i] * i * 1000;
        sum += m_Values[i];
    }

    if (sum == 0)
        return 3500;

    return static_cast<int32_t>(weighted_sum / sum);
}
