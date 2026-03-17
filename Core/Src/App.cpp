#include "App.h"
#include "Motor.h"
#include "PID.h"
#include "QTR8RC.h"

extern "C" {
#include "usart.h"
}

static constexpr int32_t BASE_SPEED = 400;
static constexpr int32_t MAX_SPEED = 999;
static constexpr int32_t LINE_CENTER = 3500;

static const uint16_t SensorPins[QTR8RC::NUM_SENSORS] = {
    GPIO_PIN_0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
};

static QTR8RC Sensor(GPIOA, SensorPins, &htim2);
static PID LinePid(0.15f, 0.0f, 2.0f);

static Motor LeftMotor(
    { GPIOA, GPIO_PIN_9, GPIOA, GPIO_PIN_8, &htim1, TIM_CHANNEL_4 },
    MAX_SPEED);
static Motor RightMotor(
    { GPIOB, GPIO_PIN_15, GPIOB, GPIO_PIN_14, &htim1, TIM_CHANNEL_3 },
    MAX_SPEED);

static int AppendInt(char* buf, int32_t val, int width)
{
    char tmp[12];
    int neg = 0;
    int len = 0;

    if (val < 0) {
        neg = 1;
        val = -val;
    }
    if (val == 0) {
        tmp[len++] = '0';
    }
    while (val > 0) {
        tmp[len++] = '0' + (val % 10);
        val /= 10;
    }
    if (neg)
        tmp[len++] = '-';

    int pad = (width > len) ? width - len : 0;
    int pos = 0;
    for (int i = 0; i < pad; i++)
        buf[pos++] = ' ';
    for (int i = len - 1; i >= 0; i--)
        buf[pos++] = tmp[i];
    return pos;
}

static void SendDebug()
{
    char buf[128];
    int pos = 0;

    for (int i = 0; i < QTR8RC::NUM_SENSORS; i++)
        pos += AppendInt(buf + pos, Sensor.GetValue(i), 5);

    buf[pos++] = ' ';
    buf[pos++] = '|';
    pos += AppendInt(buf + pos, Sensor.GetPosition(), 6);
    buf[pos++] = '\r';
    buf[pos++] = '\n';

    HAL_UART_Transmit(&huart3, reinterpret_cast<uint8_t*>(buf), pos, HAL_MAX_DELAY);
}

static int32_t Clamp(int32_t val, int32_t lo, int32_t hi)
{
    if (val < lo)
        return lo;
    if (val > hi)
        return hi;
    return val;
}

void App_Init()
{
    HAL_TIM_Base_Start(&htim2);
    LeftMotor.Start();
    RightMotor.Start();
    LinePid.Reset();
}

void App_Loop()
{
    Sensor.Read();

    int32_t error = Sensor.GetPosition() - LINE_CENTER;
    int32_t correction = LinePid.Compute(error);

    LeftMotor.SetSpeed(Clamp(BASE_SPEED + correction, -MAX_SPEED, MAX_SPEED));
    RightMotor.SetSpeed(Clamp(BASE_SPEED - correction, -MAX_SPEED, MAX_SPEED));

    SendDebug();
    HAL_Delay(10);
}
