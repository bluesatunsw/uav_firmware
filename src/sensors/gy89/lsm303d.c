#include "lsm303d.h"
#include "hardware/i2c.h"
#include "imu.h"


#define       LSM303D_ID       0b01001001
static const uint8_t LSM303D_ADDR   = 0x1E;

static const uint8_t TEMP_OUT_L     = 0x05;
static const uint8_t TEMP_OUT_H     = 0x06;
static const uint8_t STATUS_M       = 0x07;
static const uint8_t INT_CTRL_M     = 0x12;
static const uint8_t INT_SRC_M      = 0x13;
static const uint8_t INT_THS_L_M    = 0x14;
static const uint8_t INT_THS_H_M    = 0x15;
static const uint8_t OFFSET_X_L_M   = 0x16;
static const uint8_t OFFSET_X_H_M   = 0x17;
static const uint8_t OFFSET_Y_L_M   = 0x18;
static const uint8_t OFFSET_Y_H_M   = 0x19;
static const uint8_t OFFSET_Z_L_M   = 0x1A;
static const uint8_t OFFSET_Z_H_M   = 0x1B;
static const uint8_t REFERENCE_X    = 0x1C;
static const uint8_t REFERENCE_Y    = 0x1D;
static const uint8_t REFERENCE_Z    = 0x1E;
static const uint8_t CTRL0          = 0x1F;
static const uint8_t CTRL1          = 0x20;
static const uint8_t CTRL2          = 0x21;
static const uint8_t CTRL3          = 0x22;
static const uint8_t CTRL4          = 0x23;
static const uint8_t CTRL5          = 0x24;
static const uint8_t CTRL6          = 0x25;
static const uint8_t CTRL7          = 0x26;
static const uint8_t STATUS_A       = 0x27;
static const uint8_t OUT_X_L_A      = 0x28;
static const uint8_t OUT_X_H_A      = 0x29;
static const uint8_t OUT_Y_L_A      = 0x2A;
static const uint8_t OUT_Y_H_A      = 0x2B;
static const uint8_t OUT_Z_L_A      = 0x2C;
static const uint8_t OUT_Z_H_A      = 0x2D;
static const uint8_t FIFO_CTRL      = 0x2E;
static const uint8_t FIFO_SRC       = 0x2F;
static const uint8_t IG_CFG1        = 0x30;
static const uint8_t IG_SRC1        = 0x31;
static const uint8_t IG_THS1        = 0x32;
static const uint8_t IG_DUR1        = 0x33;
static const uint8_t IG_CFG2        = 0x34;
static const uint8_t IG_SRC2        = 0x35;
static const uint8_t IG_THS2        = 0x36;
static const uint8_t IG_DUR2        = 0x37;
static const uint8_t CLICK_CFG      = 0x38;
static const uint8_t CLICK_SRC      = 0x39;
static const uint8_t CLICK_THS      = 0x3A;
static const uint8_t TIME_LIMIT     = 0x3B;
static const uint8_t TIME_LATENCY   = 0x3C;
static const uint8_t TIME_WINDOW    = 0x3D;
static const uint8_t Act_THS        = 0x3E;
static const uint8_t Act_DUR        = 0x3F;
static const uint8_t WHO_AM_I       = 0x0F;
static const uint8_t OUT_X_L_M      = 0x08;
static const uint8_t OUT_X_H_M      = 0x09;
static const uint8_t OUT_Y_L_M      = 0x0A;
static const uint8_t OUT_Y_H_M      = 0x0B;
static const uint8_t OUT_Z_L_M      = 0x0C;
static const uint8_t OUT_Z_H_M      = 0x0D;

// Continuous Reading
// Set leading bit to 1
static const uint8_t ACC_XYZ_START  = OUT_X_L_A | 0b10000000;
static const uint8_t MAG_XYZ_START  = OUT_X_L_M | 0b10000000;


static void write_lsm303d_reg(uint8_t reg, uint8_t data, bool nostop) {
    uint8_t buf[2] = {reg, data};
    i2c_write_blocking(I2C_PORT, LSM303D_ADDR, buf, 2, nostop);
}


int init_lsm303d() {
    // Check the who am I register
    uint8_t data[1];
    i2c_write_blocking(I2C_PORT, LSM303D_ADDR, &WHO_AM_I, 1, true);
    i2c_read_blocking(I2C_PORT, LSM303D_ADDR, &data, 1, false);

    if (data[0] != LSM303D_ID) {
        return 0;
    }

    // 50Hz, Continuous Data Reg Update, XYZ Enabled
    write_lsm303d_reg(CTRL1, _u(0b01010111), true);
    // xx001xxxx -> +- 4g res for accelerometer
    write_lsm303d_reg(CTRL2, _u(0b00001000), true);
    // Temp enabled, Mag High Res @ 50Hz, 
    write_lsm303d_reg(CTRL5, _u(0b11110000), true);
    // +-4 gauss res for magnetometer
    write_lsm303d_reg(CTRL6, _u(0b00100000), true);
    // High / Low pass filters -> not enabled
    write_lsm303d_reg(CTRL7, _u(0b00000000), false);

    return 1;
}


static float raw_to_ms2(int16_t raw) {
    // +- 4g range -> 0.122 mg/LSB
    return raw * 0.122 / 1000 * 9.81;
}


Accelerometer read_acceleration() {
    int16_t raw_x, raw_y, raw_z;
    uint8_t buff[6];

    i2c_write_blocking(I2C_PORT, LSM303D_ADDR, &ACC_XYZ_START, 1, true);
    i2c_read_blocking(I2C_PORT, LSM303D_ADDR, buff, 6, false);

    raw_x = (int16_t)(buff[1] << 8 | buff[0]);
    raw_y = (int16_t)(buff[3] << 8 | buff[2]);
    raw_z = (int16_t)(buff[5] << 8 | buff[4]);
    
    Accelerometer acc = {
        raw_to_ms2(raw_x),
        raw_to_ms2(raw_y),
        raw_to_ms2(raw_z)
    };

    return acc;
}


static float raw_to_gauss(int16_t raw) {
    // +- 4 gauss range -> 0.16 mg/LSB
    return raw * 0.16 / 1000;
}


Magnetometer read_magnetometer() {
    int16_t raw_x, raw_y, raw_z;
    uint8_t buff[6];

    i2c_write_blocking(I2C_PORT, LSM303D_ADDR, &MAG_XYZ_START, 1, true);
    i2c_read_blocking(I2C_PORT, LSM303D_ADDR, buff, 6, false);

    raw_x = (int16_t)(buff[1] << 8 | buff[0]);
    raw_y = (int16_t)(buff[3] << 8 | buff[2]);
    raw_z = (int16_t)(buff[5] << 8 | buff[4]);
    
    Magnetometer mag = {
        raw_to_gauss(raw_x),
        raw_to_gauss(raw_y),
        raw_to_gauss(raw_z)
    };

    return mag;
}
