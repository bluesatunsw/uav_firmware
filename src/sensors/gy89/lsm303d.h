#ifndef LSM303D_H
#define LSM303D_H


typedef struct acceleration {
    float x;
    float y;
    float z;
} Acceleration;


int init_lsm303d();
Acceleration read_acceleration();


#endif