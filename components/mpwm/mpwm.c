/**
 * @brief PWM引脚
 * @author 小鱼 (fishros@foxmail.com)
 * @version V1.0.0
 * @date 2022-10-15
 * @copyright 版权所有：FishBot Open Source Organization
 */
#include "mpwm.h"

// #define FISHBOT_MODLUE "PWM"
// #define PWM_PIN 5

#define PWM_PERIOD (1000)

// pwm pin number
const uint32_t pin_num[4] = {
    PWM_0_OUT_IO_NUM,
    PWM_1_OUT_IO_NUM,
};

// duties table, real_duty = duties[x]/PERIOD
uint32_t duties[2] = {
    1000, 1000};

// phase table, delay = (phase[x]/360)*PERIOD
float phase[2] = {
    0, 0};

bool mpwm_init(void)
{
  pwm_init(PWM_PERIOD, duties, 2, pin_num);
  pwm_set_phases(phase);
  pwm_start();
  pwm_set_percent(0,600);
  pwm_set_percent(1,600);
  return true;
}


void pwm_set_percent(uint8_t chanel,uint16_t percent)
{
  pwm_stop(0x0);
  pwm_set_duty(chanel, percent);
  pwm_start();
}