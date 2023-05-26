#include "imu.h"
#include <FreeRTOS.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "common.h"
#include "gy89/lsm303d.h"
#include "gy89/l3gd20.h"


static void get_aggregated_data(
    Accelerometer *acc,
    Magnetometer *mag,
    Gyroscope *gyro,
    uint16_t display_rate,
    uint8_t aggregate_count
);


void imu_logger_task() {
    // Setup Data Gathering
    uint16_t display_rate = 250;  // ms
    uint8_t  aggregate_count = 5;

    // Init i2c Communication
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Initialise + Config the LSM303D
    while (!init_lsm303d()) {printf("LSM303D Init Failed\n"); task_delay_ms(100);}
    while (!init_l3gd20())  {printf("L3GD20 Init Failed\n");  task_delay_ms(100);}

    Accelerometer acc;
    Magnetometer mag;
    Gyroscope gyro;

    // Main Loop
    while (true) {
        get_aggregated_data(&acc, &mag, &gyro, display_rate, aggregate_count);

        // Display Acc and Mag Data
        printf("Acc:  (x: %2.2f, y: %2.2f, z: %2.2f)\n", acc.x, acc.y, acc.z);
        printf("Mag:  (x: %2.2f, y: %2.2f, z: %2.2f)\n", mag.x, mag.y, mag.z);
        printf("Gyro: (x: %2.2f, y: %2.2f, z: %2.2f)\n", gyro.x, gyro.y, gyro.z);
        printf("--------------------\n");
    }
}


static void get_aggregated_data(
    Accelerometer *acc,
    Magnetometer *mag,
    Gyroscope *gyro,
    uint16_t display_rate,
    uint8_t aggregate_count
) {
    // Store the data 
    float acc_sums[3]  = {0, 0, 0};
    float mag_sums[3]  = {0, 0, 0};
    float gyro_sums[3] = {0, 0, 0};

    uint8_t time_delay = display_rate / aggregate_count;

    // Current It Data
    Accelerometer curr_acc;
    Magnetometer  curr_mag;
    Gyroscope     curr_gyro;

    for (uint8_t i = 0; i < aggregate_count; i++) {
        curr_acc  = read_acceleration();
        acc_sums[0] += curr_acc.x;
        acc_sums[1] += curr_acc.y;
        acc_sums[2] += curr_acc.z;

        curr_mag  = read_magnetometer();
        mag_sums[0] += curr_mag.x;
        mag_sums[1] += curr_mag.y;
        mag_sums[2] += curr_mag.z;

        curr_gyro = read_gyroscope();
        gyro_sums[0] += curr_gyro.x;
        gyro_sums[1] += curr_gyro.y;
        gyro_sums[2] += curr_gyro.z;

        task_delay_ms(time_delay);
    }

    // Average the data and store in acc, mag, gyro
    acc->x  = acc_sums[0]  / aggregate_count;
    acc->y  = acc_sums[1]  / aggregate_count;
    acc->z  = acc_sums[2]  / aggregate_count;
    mag->x  = mag_sums[0]  / aggregate_count;
    mag->y  = mag_sums[1]  / aggregate_count;
    mag->z  = mag_sums[2]  / aggregate_count;
    gyro->x = gyro_sums[0] / aggregate_count;
    gyro->y = gyro_sums[1] / aggregate_count;
    gyro->z = gyro_sums[2] / aggregate_count;
}
