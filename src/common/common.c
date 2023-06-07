#include "common.h"
#include <FreeRTOS.h>

static int ms_to_ticks(int ms) {
    return (ms * configTICK_RATE_HZ) / 1000;
}

void task_delay_ms(int ms) {
    vTaskDelay(ms_to_ticks(ms));
}