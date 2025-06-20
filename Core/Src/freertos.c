#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"
#include "portmacro.h"
#include "queue.h"
#include "spi.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_spi.h"
#include "task.h"
#include <stdint.h>
#include <sys/reent.h>

static QueueHandle_t btnQueue;
static UBaseType_t btnQueueLength = 10;
static UBaseType_t bthQueueItemSize = sizeof(BTN_L_Pin);


osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void);

void MX_FREERTOS_Init(void) {
  defaultTaskHandle =
      osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  btnQueue = xQueueCreate(btnQueueLength, bthQueueItemSize);
}

void StartDefaultTask(void *argument) {
  UNUSED(argument);
  BaseType_t res;
  uint16_t idata;
  for (;;) {
    if (pdPASS == xQueueReceive(btnQueue, &idata, portMAX_DELAY)) {
      HAL_SPI_Transmit(&hspi2, (uint8_t*)&idata, 2, HAL_MAX_DELAY);
    }
    osDelay(1);
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    xQueueSendToBackFromISR(btnQueue, &GPIO_Pin, NULL);
}
