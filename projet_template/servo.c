/*----------------------------------------------------------------------------
 * servo.c
 * Seance 1, etudiant B : servomoteur DM996, signal PWM 50 Hz sur PF8.
 *
 * PF8 = TIM13_CH1 (AF9). TIM13 est sur APB1 : horloge timer 100 MHz.
 * Le compteur tourne a 1 MHz, donc 1 tick = 1 us : CCR1 = largeur
 * d impulsion en microsecondes, ARR = 20000 us = 20 ms (50 Hz).
 *---------------------------------------------------------------------------*/
#include "servo.h"
#include "peripheriques.h"

#define SERVO_PORT          GPIOF
#define SERVO_PIN           8U
#define SERVO_AF_TIM13      9U                /* PF8 : AF9 = TIM13_CH1   */
#define SERVO_TICK_HZ       1000000U          /* 1 tick = 1 us           */
#define SERVO_FREQ_HZ       50U               /* periode de 20 ms        */
#define TIM_OCM_PWM1        6U                /* OCxM = 110 : mode PWM 1 */
#define SERVO_ANGLE_DEPART  90

/* Bornes d impulsion A CALIBRER sur votre servo :
 * augmenter/diminuer jusqu a ce que 0 et 180 degres correspondent a la
 * position reelle, sans que le servo force (bourdonnement) en butee.
 * Valeurs de depart du sujet : 1 ms a 2 ms. Le DM996 accepte en general
 * une plage plus large (environ 0,5 ms a 2,5 ms) pour couvrir 180 degres. */
#define SERVO_IMPULSION_MIN_US  1000U         /* impulsion a 0 degre    */
#define SERVO_IMPULSION_MAX_US  2000U         /* impulsion a 180 degres */

static volatile int angle_courant = SERVO_ANGLE_DEPART;

void servo_init(void)
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;
  (void)RCC->APB1ENR;

  gpio_config_af(SERVO_PORT, SERVO_PIN, SERVO_AF_TIM13);

  TIM13->PSC  = (TIM_APB1_CLK_HZ / SERVO_TICK_HZ) - 1U;   /* 100 MHz -> 1 MHz */
  TIM13->ARR  = (SERVO_TICK_HZ / SERVO_FREQ_HZ) - 1U;     /* 20000 us = 20 ms */
  TIM13->CCMR1 = (TIM13->CCMR1 & ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M))
               | (TIM_OCM_PWM1 << TIM_CCMR1_OC1M_Pos)
               | TIM_CCMR1_OC1PE;
  TIM13->CCER |= TIM_CCER_CC1E;
  TIM13->CR1  |= TIM_CR1_ARPE;

  servo_set_angle(SERVO_ANGLE_DEPART);
  TIM13->EGR   = TIM_EGR_UG;                  /* charge PSC, ARR et CCR1 */
  TIM13->CR1  |= TIM_CR1_CEN;
}

int servo_set_angle(int deg)
{
  uint32_t impulsion_us;

  if ((deg < SERVO_ANGLE_MIN) || (deg > SERVO_ANGLE_MAX)) {
    return -1;
  }

  /* Interpolation lineaire entre les deux bornes calibrees */
  impulsion_us = SERVO_IMPULSION_MIN_US
               + ((SERVO_IMPULSION_MAX_US - SERVO_IMPULSION_MIN_US) * (uint32_t)deg)
                 / (uint32_t)SERVO_ANGLE_MAX;

  TIM13->CCR1 = impulsion_us;
  angle_courant = deg;
  return 0;
}

int servo_get_angle(void)
{
  return angle_courant;
}
