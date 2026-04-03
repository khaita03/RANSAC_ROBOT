#include "mpu6050.h"
#include <math.h>

static HAL_StatusTypeDef mpu_write(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t val){
  return HAL_I2C_Mem_Write(hi2c, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 100);
}
static HAL_StatusTypeDef mpu_read_bytes(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *buf, uint16_t len){
  return HAL_I2C_Mem_Read(hi2c, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 100);
}

HAL_StatusTypeDef mpu6050_init(I2C_HandleTypeDef *hi2c){
  uint8_t who=0;
  if (mpu_read_bytes(hi2c, MPU6050_REG_WHO_AM_I, &who, 1) != HAL_OK) return HAL_ERROR;
  if ((who & 0x7E) != 0x68) return HAL_ERROR; // kỳ vọng 0x68

  // Wake up
  if (mpu_write(hi2c, MPU6050_REG_PWR_MGMT1, 0x00) != HAL_OK) return HAL_ERROR;
  HAL_Delay(100);

  // LPF ~42Hz (CONFIG=3), Gyro ±250 dps, Accel ±2g, Sample 1kHz/(1+7)=125Hz
  if (mpu_write(hi2c, MPU6050_REG_SMPLRT_DIV, 7) != HAL_OK) return HAL_ERROR;
  if (mpu_write(hi2c, MPU6050_REG_CONFIG,     3) != HAL_OK) return HAL_ERROR;
  if (mpu_write(hi2c, MPU6050_REG_GYRO_CFG,   0) != HAL_OK) return HAL_ERROR;
  if (mpu_write(hi2c, MPU6050_REG_ACCEL_CFG,  0) != HAL_OK) return HAL_ERROR;

  return HAL_OK;
}

HAL_StatusTypeDef mpu6050_read(I2C_HandleTypeDef *hi2c, mpu6050_data_t *out){
  uint8_t raw[14];
  if (mpu_read_bytes(hi2c, MPU6050_REG_ACCEL_XOUT, raw, 14) != HAL_OK) return HAL_ERROR;

  int16_t ax = (raw[0]<<8) | raw[1];
  int16_t ay = (raw[2]<<8) | raw[3];
  int16_t az = (raw[4]<<8) | raw[5];
  int16_t t  = (raw[6]<<8) | raw[7];
  int16_t gx = (raw[8]<<8) | raw[9];
  int16_t gy = (raw[10]<<8)| raw[11];
  int16_t gz = (raw[12]<<8)| raw[13];

  // Scale: Accel 2g => 16384 LSB/g ; Gyro 250 dps => 131 LSB/(deg/s)
  const float g = 9.80665f;
  out->ax = -((float)ax / 16384.0f) * g;
  out->ay = -((float)ay / 16384.0f) * g;
  out->az = ((float)az / 16384.0f) * g;

  const float deg2rad = (float)M_PI/180.0f;
  out->gx = -((float)gx / 131.0f) * deg2rad;
  out->gy = -((float)gy / 131.0f) * deg2rad;
  out->gz = ((float)gz / 131.0f) * deg2rad;

  out->temp_c = ((float)t)/340.0f + 36.53f;
  return HAL_OK;
}
