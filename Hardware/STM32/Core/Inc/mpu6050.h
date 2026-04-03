#pragma once
#include "stm32f4xx_hal.h"   // hoặc stm32f7xx_hal.h tùy chip
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MPU6050_I2C_ADDR			(0x68 << 1)  // 7-bit addr = 0x68
#define MPU6050_REG_PWR_MGMT1		0x6B
#define MPU6050_REG_SMPLRT_DIV 		0x19
#define MPU6050_REG_CONFIG     		0x1A
#define MPU6050_REG_GYRO_CFG   		0x1B
#define MPU6050_REG_ACCEL_CFG  		0x1C
#define MPU6050_REG_WHO_AM_I   		0x75
#define MPU6050_REG_ACCEL_XOUT 		0x3B

typedef struct {
  float ax, ay, az;   // m/s^2
  float gx, gy, gz;   // rad/s
  float temp_c;       // degree C
} mpu6050_data_t;

HAL_StatusTypeDef mpu6050_init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef mpu6050_read(I2C_HandleTypeDef *hi2c, mpu6050_data_t *out);

#ifdef __cplusplus
}
#endif
