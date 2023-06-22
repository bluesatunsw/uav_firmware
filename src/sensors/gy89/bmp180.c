#include "bmp180.h"
#include "hardware/i2c.h"
#include "imu.h"
#include <math.h>
#include <stdio.h>

#define       BMP180_ID       0x55
static const uint8_t BMP180_ADDR   = 0x77;

static const uint8_t CALIB       = 0xAA; // Starting address of calib data
static const uint8_t CTRL_MEAS   = 0xF4; // Measurement Control
static const uint8_t MSB         = 0xF6; // MSB of data
static const uint8_t LSB         = 0xF7; // LSB of data
static const uint8_t TEMP_ADDR   = 0x2E; // Temperature control register value
static const uint8_t PRES_ADDR   = 0x34; // Pressure, OSS = 0, ultra-low power mode, 
                                  // set to 0x74 for standard mode
static const uint8_t ID_REG      = 0xD0; // Contains device id
static const uint8_t CHIP_ID     = 0x55; // Chip ID in ID_REG

static const uint8_t OSS              = 0; // Oversampling ratio for pressure measurement
static const uint8_t CONVERSION_DELAY = 4.5; // ms

// Calibration Coefficients from EEPROM
typedef struct {
    int16_t ac1;
    int16_t ac2;
    int16_t ac3;
    uint16_t ac4;
    uint16_t ac5;
    uint16_t ac6;
    int16_t b1;
    int16_t b2;
    int16_t mb;
    int16_t mc;
    int16_t md;
    int32_t b5;
} bmp180_calib_coeffs_t;

/*
    Combine 2 8-bit ints to a 16-bit int
*/
static int16_t eight_to_sixteen(int8_t val1, int8_t val2) {
    return ((val1 << 8) + val2);
}

/*
    Set the calibration constants in the struct
*/
static void set_calibration_constants(bmp180_calib_coeffs_t *calib_coeffs, uint8_t *reg_vals) {

    // Ensure data communication is good
    for (int i=0, j=1; i < 11; i++, j+=2) {
        int16_t data = eight_to_sixteen(reg_vals[i*2], reg_vals[j]);
        if ((data == 0xFFFF) | (data == 0)) {
            while (1) {
                printf("Invalid calibration coefficient - data communication error\n");
            }
        }
    }

    // Set the data calibration coefficient in the struct
    calib_coeffs->ac1 = eight_to_sixteen(reg_vals[0], reg_vals[1]);
    calib_coeffs->ac2 = eight_to_sixteen(reg_vals[2], reg_vals[3]);
    calib_coeffs->ac3 = eight_to_sixteen(reg_vals[4], reg_vals[5]);
    calib_coeffs->ac4 = eight_to_sixteen(reg_vals[6], reg_vals[7]);
    calib_coeffs->ac5 = eight_to_sixteen(reg_vals[8], reg_vals[9]);
    calib_coeffs->ac6 = eight_to_sixteen(reg_vals[10], reg_vals[11]);
    calib_coeffs->b1 = eight_to_sixteen(reg_vals[12], reg_vals[13]);
    calib_coeffs->b2 = eight_to_sixteen(reg_vals[14], reg_vals[15]);
    calib_coeffs->mb = eight_to_sixteen(reg_vals[16], reg_vals[17]);
    calib_coeffs->mc = eight_to_sixteen(reg_vals[18], reg_vals[19]);
    calib_coeffs->md = eight_to_sixteen(reg_vals[20], reg_vals[21]);
}

/*
    Get the calibration constants from the EEPROM
*/
static void bmp180_get_cal_param(bmp180_calib_coeffs_t *calib_coeffs) {
    uint8_t calib_data[22]; // 22, because there are 11 words of length 2 bytes

    // Read in calibration constants
    i2c_write_blocking(I2C_PORT, BMP180_ADDR, &CALIB, 1, true);
    i2c_read_blocking(I2C_PORT, BMP180_ADDR, calib_data, 22, false);

    task_delay_ms(CONVERSION_DELAY);

    // Create struct to hold calibration data
    set_calibration_constants(calib_coeffs, calib_data);
}

/*
 * Read unrefined temperature data (i.e. raw temperature data) 
*/
static int32_t bmp180_get_raw_temp() {
    uint8_t temp_data[2];

    // Write TEMP_ADDR into CTRL_MEAS
    uint8_t temp_reg[] = {
        CTRL_MEAS,
        TEMP_ADDR
    };
    i2c_write_blocking(I2C_PORT, BMP180_ADDR, temp_reg, 2, false);

    // Wait for CONVERSION_DELAYms
    task_delay_ms(CONVERSION_DELAY);

    // Read the data from the register
    // Read 2 bytes (i.e. MSB and LSB) starting from register 0xF6
    i2c_write_blocking(I2C_PORT, BMP180_ADDR, &MSB, 1, true);
    i2c_read_blocking(I2C_PORT, BMP180_ADDR, temp_data, 2, false); 

    // Combine MSB and LSB
    return (temp_data[0] << 8) + temp_data[1];
}

/*
 * Read unrefined pressure data (i.e. raw pressure data) 
*/
static int32_t bmp180_get_raw_pressure() {
    uint8_t pressure_data[3];

    // Write the data to the register
    uint8_t pressure_reg[] = {
        CTRL_MEAS,
        PRES_ADDR + (OSS << 6)
    };
    i2c_write_blocking(I2C_PORT, BMP180_ADDR, pressure_reg, 2, false);

    // Wait for CONVERSION_DELAYms
    task_delay_ms(CONVERSION_DELAY);

    // Read the data from the register
    // Read 3 bytes (i.e. MSB, LSB and XLSB) starting from register 0xF6
    i2c_write_blocking(I2C_PORT, BMP180_ADDR, &MSB, 1, true);
    i2c_read_blocking(I2C_PORT, BMP180_ADDR, pressure_data, 3, false);

    // Math found in datasheet
    return ((pressure_data[0] << 16) + (pressure_data[1] << 8) + pressure_data[2]) >> (8 - OSS);
}

/*
 * Get refined temperature value in degrees C
*/
static float bmp180_get_temp(bmp180_calib_coeffs_t *calib_coeffs) {
    int32_t raw_temp = bmp180_get_raw_temp();

    // Magical conversion maths found in datasheet
    int32_t X1 = ((raw_temp - calib_coeffs->ac6) * calib_coeffs->ac5) >> 15;
    int32_t X2 = (calib_coeffs->mc << 11) / (X1 + calib_coeffs->md);
    int32_t B5 = X1 + X2;
    calib_coeffs->b5 = B5;
    return ((B5 + 8) >> 4) / 10;
}

/*
 * Get refined pressure value in Pa
*/
static int32_t bmp180_get_pressure(bmp180_calib_coeffs_t *calib_coeffs) {
    int32_t raw_pressure = bmp180_get_raw_pressure();

    // Magical conversion maths found in datasheet
    int32_t B6 = calib_coeffs->b5 - 4000;
    int32_t X1 = (calib_coeffs->b2 * ((B6 * B6) >> 12)) >> 11;
    int32_t X2 = (calib_coeffs->ac2 * B6) >> 11;
    int32_t X3 = X1 + X2;
    int32_t B3 = (((calib_coeffs->ac1 * 4 + X3) << OSS) + 2) / 4;
    X1 = (calib_coeffs->ac3 * B6) >> 13;
    X2 = (calib_coeffs->b1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    uint32_t B4 = (calib_coeffs->ac4 * (uint32_t)(X3 + 32768)) >> 15;
    uint32_t B7 = ((uint32_t)(raw_pressure) - B3) * (50000 >> OSS);
    int32_t pressure = 0;
    if (B7 < 0x80000000) {
        pressure = (B7 * 2) / B4;
    } else {
        pressure = (B7 / B4) * 2;
    }

    X1 = (pressure >> 8) * (pressure >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * pressure) >> 16;

    return pressure + ((X1 + X2 + 3791) >> 4);
}

/*
    Get the altitude in metres
*/
static float bmp180_get_altitude(bmp180_calib_coeffs_t *calib_coeffs) {
    float pressure = bmp180_get_pressure(calib_coeffs) / 100; // hPa
    const float P0 = 1013.25; // Pressure are sea-level in hPa

    // Maths found in datasheet
    return 44330 * (1 - pow((pressure)/P0, 1/5.255));

} 

/*
    Initialise and get data from the BMP180 peripheral
*/
int bmp180_init() {
    
    // Write to and read from chip id register
    // Used to test communication is functioning
    uint8_t chipID[1];
    i2c_write_blocking(I2C_PORT, BMP180_ADDR, &ID_REG, 1, true);
    i2c_read_blocking(I2C_PORT, BMP180_ADDR, chipID, 1, false);
    if (chipID[0] != CHIP_ID) {
        return 0;
    } else {
        return 1;
    }
}

/*
    Return all data from BMP180 sensor
*/
Barometer read_barometer() {
    
    // Get calibration coefficients
    bmp180_calib_coeffs_t bmp180_coeffs;
    bmp180_get_cal_param(&bmp180_coeffs);

    Barometer baro = {
        // Get temperature degrees C
        (float)bmp180_get_temp(&bmp180_coeffs),
  
        // Get pressure in hPa
        (float)(bmp180_get_pressure(&bmp180_coeffs) / 100),

        // Get Altitude in m
        (float)bmp180_get_altitude(&bmp180_coeffs)
    };

    return baro;
}