#include "stm32f7xx.h"
#include "servo.h"

// Horloge de TIM10 : configuration_overclock() met HCLK a 200 MHz,
// APB2 = 100 MHz, donc les timers d'APB2 recoivent 2 x 100 = 200 MHz
// (et non 16 MHz comme dans les exemples du cours).
//
// PSC = 99    -> fCNT = 200 MHz / (99 + 1) = 2 MHz, 1 tick = 0,5 us
// ARR = 39999 -> periode = 40000 x 0,5 us = 20 ms, soit 50 Hz
//
#define SERVO_CCR_MIN  1000   // impulsion a 0 deg
#define SERVO_CCR_MAX  5000   // impulsion a 180 deg

static int angle_courant = 90;

void servo_init(void)
{
  RCC->AHB1ENR |= (1 << 5);   //activates GPIOF
  RCC->APB2ENR |= (1 << 17);  //activates TIM10

  GPIOF->MODER |= (2 << 12);  //PF6 in AF mode
  GPIOF->AFR[0] |= (3 << 24); //PF6 in AF3 mode (TIM10_CH1)

  TIM10->CCMR1 |= (6 << 4);   //TIM10_CH1 in PWM mode 1
  TIM10->CCER |= (1 << 0);    //connects OC1 to TIM10_CH1

  TIM10->PSC = 99;            //Prescaler : 2 MHz
  TIM10->ARR = 39999;         //defines the PWM period : 20 ms
  servo_set_angle(90);        //defines the PWM duty cycle : 1,5 ms

  TIM10->CR1 |= (1 << 0);     //starts counter
}

void servo_set_angle(int deg)
{
  if (deg < 0)   deg = 0;
  if (deg > 180) deg = 180;

  TIM10->CCR1 = SERVO_CCR_MIN + ((SERVO_CCR_MAX - SERVO_CCR_MIN) * deg) / 180;
  angle_courant = deg;
}

int servo_get_angle(void)
{
  return angle_courant;
}
