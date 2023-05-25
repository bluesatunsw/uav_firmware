#include "lsm303d.h"
#include "hardware/i2c.h"
#include "common.h"
#include "imu.h"


#define       LSM303D_ID       0b01001001

const uint8_t LSM303D_ADDR   = 0x1E;

const uint8_t TEMP_OUT_L     = 0x05;
const uint8_t TEMP_OUT_H     = 0x06;
const uint8_t STATUS_M       = 0x07;
const uint8_t INT_CTRL_M     = 0x12;
const uint8_t INT_SRC_M      = 0x13;
const uint8_t INT_THS_L_M    = 0x14;
const uint8_t INT_THS_H_M    = 0x15;
const uint8_t OFFSET_X_L_M   = 0x16;
const uint8_t OFFSET_X_H_M   = 0x17;
const uint8_t OFFSET_Y_L_M   = 0x18;
const uint8_t OFFSET_Y_H_M   = 0x19;
const uint8_t OFFSET_Z_L_M   = 0x1A;
const uint8_t OFFSET_Z_H_M   = 0x1B;
const uint8_t REFERENCE_X    = 0x1C;
const uint8_t REFERENCE_Y    = 0x1D;
const uint8_t REFERENCE_Z    = 0x1E;
const uint8_t CTRL0          = 0x1F;
const uint8_t CTRL1          = 0x20;
const uint8_t CTRL2          = 0x21;
const uint8_t CTRL3          = 0x22;
const uint8_t CTRL4          = 0x23;
const uint8_t CTRL5          = 0x24;
const uint8_t CTRL6          = 0x25;
const uint8_t CTRL7          = 0x26;
const uint8_t STATUS_A       = 0x27;
const uint8_t OUT_X_L_A      = 0x28;
const uint8_t OUT_X_H_A      = 0x29;
const uint8_t OUT_Y_L_A      = 0x2A;
const uint8_t OUT_Y_H_A      = 0x2B;
const uint8_t OUT_Z_L_A      = 0x2C;
const uint8_t OUT_Z_H_A      = 0x2D;
const uint8_t FIFO_CTRL      = 0x2E;
const uint8_t FIFO_SRC       = 0x2F;
const uint8_t IG_CFG1        = 0x30;
const uint8_t IG_SRC1        = 0x31;
const uint8_t IG_THS1        = 0x32;
const uint8_t IG_DUR1        = 0x33;
const uint8_t IG_CFG2        = 0x34;
const uint8_t IG_SRC2        = 0x35;
const uint8_t IG_THS2        = 0x36;
const uint8_t IG_DUR2        = 0x37;
const uint8_t CLICK_CFG      = 0x38;
const uint8_t CLICK_SRC      = 0x39;
const uint8_t CLICK_THS      = 0x3A;
const uint8_t TIME_LIMIT     = 0x3B;
const uint8_t TIME_LATENCY   = 0x3C;
const uint8_t TIME_WINDOW    = 0x3D;
const uint8_t Act_THS        = 0x3E;
const uint8_t Act_DUR        = 0x3F;
const uint8_t WHO_AM_I       = 0x0F;
const uint8_t OUT_X_L_M      = 0x08;
const uint8_t OUT_X_H_M      = 0x09;
const uint8_t OUT_Y_L_M      = 0x0A;
const uint8_t OUT_Y_H_M      = 0x0B;
const uint8_t OUT_Z_L_M      = 0x0C;
const uint8_t OUT_Z_H_M      = 0x0D;

// Continuous Reading
// Set leading bit to 1
const uint8_t ACC_XYZ_START  = OUT_X_L_A | 0b10000000;


static void write_lsm303d_reg(uint8_t reg, uint8_t value, bool nostop) {
    uint8_t data[] = {reg, value};
    i2c_write_blocking(I2C_PORT, LSM303D_ADDR, data, 2, nostop);
}


int init_lsm303d() {
    uint8_t data[1];
    i2c_write_blocking(I2C_PORT, LSM303D_ADDR, &WHO_AM_I, 1, true);
    i2c_read_blocking(I2C_PORT, LSM303D_ADDR, &data, 1, false);

    if (data[0] != LSM303D_ID) {
        return 0;
    }

    write_lsm303d_reg(CTRL2, _u(0b00000000), true);
    write_lsm303d_reg(CTRL1, _u(0x57), true);
    write_lsm303d_reg(CTRL5, _u(0x64), true);
    write_lsm303d_reg(CTRL6, _u(0x20), true);
    write_lsm303d_reg(CTRL7, _u(0x00), false);

    return 1;
}


static float raw_to_ms2(int16_t raw) {
    return raw * 0.061 / 1000 * 9.81;
}


Acceleration read_acceleration() {
    int16_t raw_x, raw_y, raw_z;
    uint8_t buff[6];

    i2c_write_blocking(I2C_PORT, LSM303D_ADDR, &ACC_XYZ_START, 1, true);
    i2c_read_blocking(I2C_PORT, LSM303D_ADDR, buff, 6, false);

    raw_x = (int16_t)(buff[1] << 8 | buff[0]);
    raw_y = (int16_t)(buff[3] << 8 | buff[2]);
    raw_z = (int16_t)(buff[5] << 8 | buff[4]);
    
    Acceleration acc = {
        raw_to_ms2(raw_x),
        raw_to_ms2(raw_y),
        raw_to_ms2(raw_z)
    };

    return acc;
}
