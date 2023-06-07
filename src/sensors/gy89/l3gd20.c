#include "l3gd20.h"
#include "hardware/i2c.h"
#include "imu.h"


#define   L3GD20_ID 0b11010100
static const uint8_t L3GD20_ADDR = 0b1101010;

static const uint8_t WHO_AM_I  = 0x0F;

static const uint8_t CTRL_REG1 = 0x20;
static const uint8_t CTRL_REG2 = 0x21;
static const uint8_t CTRL_REG3 = 0x22;
static const uint8_t CTRL_REG4 = 0x23;
static const uint8_t CTRL_REG5 = 0x24;
static const uint8_t REFERENCE = 0x25;
static const uint8_t OUT_TEMP = 0x26;
static const uint8_t STATUS_REG = 0x27;
static const uint8_t OUT_X_L = 0x28;
static const uint8_t OUT_X_H = 0x29;
static const uint8_t OUT_Y_L = 0x2A;
static const uint8_t OUT_Y_H = 0x2B;
static const uint8_t OUT_Z_L = 0x2C;
static const uint8_t OUT_Z_H = 0x2D;
static const uint8_t FIFO_CTRL_REG = 0x2E;
static const uint8_t FIFO_SRC_REG = 0x2F;
static const uint8_t INT1_CFG = 0x30;
static const uint8_t INT1_SRC = 0x31;
static const uint8_t INT1_TSH_XH = 0x32;
static const uint8_t INT1_TSH_XL = 0x33;
static const uint8_t INT1_TSH_YH = 0x34;
static const uint8_t INT1_TSH_YL = 0x35;
static const uint8_t INT1_TSH_ZH = 0x36;
static const uint8_t INT1_TSH_ZL = 0x37;
static const uint8_t INT1_DURATION = 0x38;



static void write_l3gdq20(uint8_t reg, uint8_t data) {
    uint8_t buf[2] = {reg, data};
    i2c_write_blocking(I2C_PORT, L3GD20_ADDR, buf, 2, false);
}


static uint8_t read_l3gdq20(uint8_t reg) {
    uint8_t data[1];
    i2c_write_blocking(I2C_PORT, L3GD20_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, L3GD20_ADDR, &data, 1, false);
    return data[0];
}


static uint8_t multi_read_l3gdq20(uint8_t reg, uint8_t *data, uint8_t len) {
    reg = reg | 0b10000000;
    i2c_write_blocking(I2C_PORT, L3GD20_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, L3GD20_ADDR, data, len, false);
}


int init_l3gd20() {
    // Check the who am i register
    uint8_t id = read_l3gdq20(WHO_AM_I);
    if (id != L3GD20_ID) {
        return 0;
    }

    // Configure the gyroscope
    write_l3gdq20(CTRL_REG1, 0b00001111); // 95 Hz, 12.5 Hz cut-off, normal mode, XYZ enabled
    write_l3gdq20(CTRL_REG2, 0b00000000); // HPF disabled, 250 dps
    write_l3gdq20(CTRL_REG4, 0b00000000); // 250 dps
}


static float raw_to_dps(int16_t raw) {
    return raw * 0.00875;
}


Gyroscope read_gyroscope() {
    int16_t raw_x, raw_y, raw_z;
    uint8_t data[6];

    multi_read_l3gdq20(OUT_X_L, data, 6);
    raw_x = (data[1] << 8) | data[0];
    raw_y = (data[3] << 8) | data[2];
    raw_z = (data[5] << 8) | data[4];

    Gyroscope gyro = {
        raw_to_dps(raw_x),
        raw_to_dps(raw_y),
        raw_to_dps(raw_z)
    };

    return gyro;
}