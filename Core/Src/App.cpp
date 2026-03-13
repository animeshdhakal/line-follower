#include "App.h"
#include "stm32f1xx_hal.h"

void AppMain() {
  volatile int count = 0;

  while (1) {
    count++;
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(1000);
  }
}
