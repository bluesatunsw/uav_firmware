#ifndef BMP180_H
#define BMP180_H

// Temp, Pressure and Altitude
typedef struct barometer {
    float temp;
    float pressure;
    float altitude;
} Barometer;

int bmp180_init();
Barometer read_barometer();

#endif
