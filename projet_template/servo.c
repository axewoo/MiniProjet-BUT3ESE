#include "stm32f7xx.h"
#include "servo.h"

// Servo sur PF8 = TIM13_CH1 (AF9), comme demande dans le sujet.
//
// Horloge de TIM13 : configuration_overclock() met HCLK a 200 MHz,
// APB1 = 200 / 4 = 50 MHz, donc les timers d'APB1 recoivent 2 x 50 = 100 MHz
// (et non 16 MHz comme dans les exemples du cours).
//
// PSC = 99    -> fCNT = 100 MHz / (99 + 1) = 1 MHz, 1 tick = 1 us
// ARR = 19999 -> periode = 20000 x 1 us = 20 ms, soit 50 Hz
//
// Largeur d'impulsion = CCR1 x 1 us. Bornes calibrees sur notre DM996
// (0,5 ms a 2,5 ms, memes valeurs qu'avant sur TIM10 : 1000..5000 x 0,5 us).
#define SERVO_CCR_MIN  500    // impulsion a 0 deg   (0,5 ms)
#define SERVO_CCR_MAX  2500   // impulsion a 180 deg (2,5 ms)

static int angle_courant = 90;

void servo_init(void)
{
  RCC->AHB1ENR |= (1 << 5);     //activates GPIOF
  RCC->APB1ENR |= (1 << 7);     //activates TIM13

  GPIOF->MODER &= ~(3 << 16);   //PF8 : on efface les 2 bits du mode
  GPIOF->MODER |= (2 << 16);    //PF8 in AF mode
  GPIOF->AFR[1] &= ~(15 << 0);  //PF8 = broche 8 -> AFR[1], bits 0 a 3
  GPIOF->AFR[1] |= (9 << 0);    //PF8 in AF9 mode (TIM13_CH1)

  TIM13->CCMR1 |= (6 << 4);     //TIM13_CH1 in PWM mode 1
  TIM13->CCER |= (1 << 0);      //connects OC1 to TIM13_CH1

  TIM13->PSC = 99;              //Prescaler : 1 MHz
  TIM13->ARR = 19999;           //defines the PWM period : 20 ms
  servo_set_angle(90);          //defines the PWM duty cycle : 1,5 ms

  TIM13->CR1 |= (1 << 0);       //starts counter
}

void servo_set_angle(int deg)
{
  if (deg < 0)   deg = 0;
  if (deg > 180) deg = 180;

  TIM13->CCR1 = SERVO_CCR_MIN + ((SERVO_CCR_MAX - SERVO_CCR_MIN) * deg) / 180;
  angle_courant = deg;
}

int servo_get_angle(void)
{
  return angle_courant;
}
