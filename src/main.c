// Includes
#include <FreeRTOS.h>
#include <stdio.h>
#include "hello_there.h"
#include "pico/stdlib.h"
#include "sensors/imu.h"

/*
 * Main function
 */
int main() {
    stdio_init_all();
    
    // Create Tasks
    xTaskCreate(led_task, "LED Task", 128, NULL, 1, NULL);
    xTaskCreate(imu_logger_task, "IMU Task", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    // Infinite loop - Program will never get to here in execution
    // as FreeRTOS is running from `vTaskStartScheduler()`
    while (true) {};
}
