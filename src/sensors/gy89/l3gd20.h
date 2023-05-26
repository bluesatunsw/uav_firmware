#ifndef L3GD20_H
#define L3GD20_H

// Gyroscope, Measured in degrees per second
typedef struct gyroscope {
    float x;
    float y;
    float z;
} Gyroscope;


int init_l3gd20();
Gyroscope read_gyroscope();

#endif
