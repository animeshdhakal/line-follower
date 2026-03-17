#include "PID.h"

PID::PID(float kp, float ki, float kd)
    : m_Kp(kp), m_Ki(ki), m_Kd(kd), m_Integral(0.0f), m_LastError(0)
{
}

void PID::SetTunings(float kp, float ki, float kd)
{
    m_Kp = kp;
    m_Ki = ki;
    m_Kd = kd;
}

void PID::Reset()
{
    m_Integral  = 0.0f;
    m_LastError = 0;
}

int32_t PID::Compute(int32_t error)
{
    m_Integral += static_cast<float>(error);
    int32_t derivative = error - m_LastError;
    m_LastError = error;

    float output = m_Kp * static_cast<float>(error)
                 + m_Ki * m_Integral
                 + m_Kd * static_cast<float>(derivative);

    return static_cast<int32_t>(output);
}
