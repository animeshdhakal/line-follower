#pragma once

#include <cstdint>

class PID {
public:
    PID(float kp, float ki, float kd);

    void SetTunings(float kp, float ki, float kd);
    void Reset();
    int32_t Compute(int32_t error);

private:
    float m_Kp;
    float m_Ki;
    float m_Kd;
    float m_Integral;
    int32_t m_LastError;
};
