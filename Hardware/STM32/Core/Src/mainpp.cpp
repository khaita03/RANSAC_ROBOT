/*
 * mainpp.cpp (STM32: raw encoder + IMU, odom xử lý trên ROS, nhận lệnh từ /cmd_vel)
 */
#include "main.h"
#include "mainpp.h"

#include <ros.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int32.h>
#include <math.h>

extern "C" {
  #include "mpu6050.h"
}

/* ========= CubeMX handles ========= */
extern I2C_HandleTypeDef hi2c3;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef  htim8;   // Motor PWM: CH1 forward, CH2 reverse
extern TIM_HandleTypeDef  htim11;  // Servo PWM: CH1 (50 Hz, 1 tick = 1us)

/* ========= SIGN & HW CONFIG =========
 * Nếu sau này bạn đảo dây phần cứng, chỉ cần đổi macro dưới.
 */
#define DRIVE_SIGN       (+1.0f)  // +1 giữ nguyên, -1 đảo tiến↔lùi
#define SERVO_SIGN       (-1.0f)  // +1 giữ nguyên, -1 đảo chiều rẽ
#define SERVO_TRIM_DEG   (0.0f)   // Trim (độ) để bánh thẳng khi lệnh 0°
#define ENCODER_SIGN_RAW (-1)     // đi tới mà enc_pos đang âm -> đặt -1 cho dương

/* Motor EN pin */
#define MOTOR_EN_PORT   GPIOC
#define MOTOR_EN_PIN    GPIO_PIN_8   // PC8

/* ========= ROS ========= */
static ros::NodeHandle nh;

/* IMU */
static sensor_msgs::Imu imu_msg;
static ros::Publisher  imu_pub("imu/data", &imu_msg);

/* Encoder raw ticks (cho ROS xử lý odom) */
static std_msgs::Int32 encoder_msg;
static ros::Publisher  encoder_pub("encoder_ticks", &encoder_msg);

/* ========= UART callbacks cho rosserial (HAL non-blocking) ========= */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  if (huart == &huart1) nh.getHardware()->flush();
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  if (huart == &huart1) nh.getHardware()->reset_rbuf();
}

/* ========= Utils ========= */
static inline float clampf(float x, float lo, float hi){
  return (x < lo) ? lo : (x > hi) ? hi : x;
}

/* =================== MOTOR & SERVO =================== */
static inline void motor_set_percent(float p){
  // p in [-1..1]; >0 = tiến (CH1), <0 = lùi (CH2)
  p = DRIVE_SIGN * p;
  const float deadband = 0.02f;   // <2% thì coi như 0
  p = clampf(p, -1.0f, 1.0f);
  if (fabsf(p) < deadband) p = 0.0f;

  const uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim8);
  const uint32_t ccr = (uint32_t)(fabsf(p) * (float)arr);

  if (p >= 0.0f){
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, ccr); // forward
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);   // reverse off
  }else{
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);   // forward off
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, ccr); // reverse
  }
}

/* Servo steering: TIM11 CH1, 1 tick = 1us, 50Hz */
#define SERVO_US_MIN     1000
#define SERVO_US_MAX     2000
#define SERVO_US_CENTER  1500
#define SERVO_MAX_DEG    30.0f

static inline uint16_t servo_pulse_from_deg(float deg){
  // Đảo chiều + trim
  deg = SERVO_SIGN * (deg + SERVO_TRIM_DEG);
  deg = clampf(deg, -SERVO_MAX_DEG, SERVO_MAX_DEG);
  const float k = (float)(SERVO_US_MAX - SERVO_US_MIN) / (2.0f * SERVO_MAX_DEG);
  const float us = SERVO_US_CENTER + k * deg;
  return (uint16_t)us; // TIM11 PSC=83, ARR=19999 ⇒ 1 tick = 1us
}
static inline void servo_write_deg(float deg){
  __HAL_TIM_SET_COMPARE(&htim11, TIM_CHANNEL_1, servo_pulse_from_deg(deg));
}

/* =================== ENCODER PB5(A) / PB6(B) qua EXTI (X4) =================== */
/* Tham số cần chỉnh theo encoder của bạn */
#define ENC_PORT_A   GPIOB
#define ENC_PIN_A    GPIO_PIN_5
#define ENC_PORT_B   GPIOB
#define ENC_PIN_B    GPIO_PIN_6

static volatile int32_t enc_pos   = 0; // tổng xung (X4) từ lúc bật nguồn
static volatile uint8_t enc_state = 0; // (A<<1 | B)

static inline uint8_t read_AB(){
  const uint8_t a = (HAL_GPIO_ReadPin(ENC_PORT_A, ENC_PIN_A) != 0);
  const uint8_t b = (HAL_GPIO_ReadPin(ENC_PORT_B, ENC_PIN_B) != 0);
  return (uint8_t)((a<<1) | b);
}

static inline int8_t qdelta(uint8_t prev, uint8_t cur){
  static const int8_t table[16] = {
    /* 00->00,01,10,11 */  0, +1, -1,  0,
    /* 01->00,01,10,11 */ -1,  0,  0, +1,
    /* 10->00,01,10,11 */ +1,  0,  0, -1,
    /* 11->00,01,10,11 */  0, -1, +1,  0
  };
  return table[(prev<<2) | cur];
}

static void encoder_soft_init(){
  enc_state = read_AB();
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if (GPIO_Pin == ENC_PIN_A || GPIO_Pin == ENC_PIN_B){
    const uint8_t cur = read_AB();
    enc_pos += qdelta(enc_state, cur);
    enc_state = cur;
  }
}

/* =================== ROS Subscriber: /cmd_vel =================== */
/*
 * /cmd_vel:
 *   - linear.x ∈ [-1..1]  → %PWM (tiến/lùi) cho motor
 *   - angular.z [rad]     → góc lái [radian] cho servo
 */
static void cmdvel_cb(const geometry_msgs::Twist& msg){
  /* 1) Motor: đi theo linear.x */
  float motor = (float)msg.linear.x;      // vd: 0.15 → 15% PWM tiến
  motor = clampf(motor, -1.0f, 1.0f);

  const float MOTOR_DEADBAND = 0.02f;
  if (fabsf(motor) < MOTOR_DEADBAND) motor = 0.0f;

  /* 2) Servo: nhận góc [rad] từ angular.z, đổi sang độ rồi sang PWM */
  const float z_rad = (float)msg.angular.z;
  float steerDeg = z_rad * 57.2957795f;   // rad -> deg
  steerDeg = clampf(steerDeg, -SERVO_MAX_DEG, SERVO_MAX_DEG);

  /* 3) Gửi lệnh xuống phần cứng */
  motor_set_percent(motor);
  servo_write_deg(steerDeg);
}

static ros::Subscriber<geometry_msgs::Twist> sub_cmdvel("cmd_vel", &cmdvel_cb);

/* =================== IMU helpers =================== */
static mpu6050_data_t mpu;

static void imu_msg_init(){
  imu_msg.orientation_covariance[0] = -1.0; // không cung cấp orientation
  imu_msg.angular_velocity_covariance[0]  = 0.02;
  imu_msg.angular_velocity_covariance[4]  = 0.02;
  imu_msg.angular_velocity_covariance[8]  = 0.02;
  imu_msg.linear_acceleration_covariance[0] = 0.04;
  imu_msg.linear_acceleration_covariance[4] = 0.04;
  imu_msg.linear_acceleration_covariance[8] = 0.04;
}

static inline void publish_imu_now(){
  if (mpu6050_read(&hi2c3, &mpu) == HAL_OK){
      imu_msg.header.stamp = nh.now();
      imu_msg.header.frame_id = (char*)"base_link";  // hoặc "imu_link" nếu bạn có tf riêng
      imu_msg.angular_velocity.x     = -mpu.gy;
      imu_msg.angular_velocity.y     =  mpu.gx;
      imu_msg.angular_velocity.z     =  mpu.gz;
      imu_msg.linear_acceleration.x  = -mpu.ay;
      imu_msg.linear_acceleration.y  =  mpu.ax;
      imu_msg.linear_acceleration.z  =  mpu.az;
      imu_pub.publish(&imu_msg);
    }
}

/* =================== Arduino-style API =================== */
void setup(void)
{
  nh.initNode();

  imu_msg_init();
  nh.advertise(imu_pub);
  nh.advertise(encoder_pub);

  nh.subscribe(sub_cmdvel);

  (void)mpu6050_init(&hi2c3);
  mpu6050_calibrate_gyro(&hi2c3, 200, 20);

  HAL_TIM_PWM_Start(&htim8,  TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim8,  TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);

  HAL_GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_SET);

  motor_set_percent(0.0f);
  servo_write_deg(0.0f);

  encoder_soft_init();
}

void loop(void)
{
  const uint32_t now = HAL_GetTick();

  /* IMU @30 Hz */
  static uint32_t last_imu_ms = 0;
  if ((uint32_t)(now - last_imu_ms) >= 33){
    last_imu_ms = now;
    publish_imu_now();
  }

  /* Encoder ticks @30 Hz */
  static uint32_t last_enc_ms = 0;
  if ((uint32_t)(now - last_enc_ms) >= 33){
    last_enc_ms = now;

    // Gửi tổng số xung enc_pos (X4) từ lúc bật nguồn
    encoder_msg.data = ENCODER_SIGN_RAW * enc_pos;  // đảm bảo đi tới là dương
    encoder_pub.publish(&encoder_msg);
  }

  nh.spinOnce();
}