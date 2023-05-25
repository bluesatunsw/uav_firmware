#include "imu.h"
#include <FreeRTOS.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "common.h"
#include "gy89/lsm303d.h"


void imu_logger_task() {
    // Init i2c Communication
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Initialise + Config the LSM303D
    init_lsm303d();

    Accelerometer acc;
    Magnetometer mag;

    // Main Loop
    while (true) {
        acc = read_acceleration();
        mag = read_magnetometer();

        // Display Acc and Mag Data
        printf("Acc: (x: %2.2f, y: %2.2f, z: %2.2f)\n", acc.x, acc.y, acc.z);
        printf("Mag: (x: %2.2f, y: %2.2f, z: %2.2f)\n", mag.x, mag.y, mag.z);
        printf("--------------------\n");

        task_delay_ms(200);
    }
}

