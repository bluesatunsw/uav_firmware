#ifndef LSM303D_H
#define LSM303D_H


// Acceleration, Measured in m/s2
typedef struct accelerometer {
    float x;
    float y;
    float z;
} Accelerometer;


// Magnetometer, Measured in Gauss
typedef struct magnetometer {
    float x;
    float y;
    float z;
} Magnetometer;


int init_lsm303d();
Accelerometer read_acceleration();
Magnetometer read_magnetometer();


#endif