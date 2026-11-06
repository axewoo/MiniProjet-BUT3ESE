/*----------------------------------------------------------------------------
 * peripheriques.c
 * Seance 1, etudiant B : exercices d'echauffement (LED, DAC, PWM, ADC).
 *
 * Toutes les initialisations sont appelees dans main(), entre les
 * commentaires "Debut/Fin Initialisation peripherique".
 *---------------------------------------------------------------------------*/
#include "peripheriques.h"

/*------------------------------- GPIO --------------------------------------*/
#define GPIO_MODE_BITS      2U      /* 2 bits par broche dans MODER        */
#define GPIO_MODE_MASK      3U
#define GPIO_MODE_OUTPUT    1U
#define GPIO_MODE_AF        2U
#define GPIO_MODE_ANALOG    3U
#define GPIO_AF_BITS        4U      /* 4 bits par broche dans AFR[0]/AFR[1] */
#define GPIO_AF_MASK        0xFU
#define GPIO_BSRR_RESET_POS 16U     /* bits 16..31 de BSRR : remise a 0    */

static void gpio_config_mode(GPIO_TypeDef *port, uint32_t pin, uint32_t mode)
{
  port->MODER = (port->MODER & ~(GPIO_MODE_MASK << (GPIO_MODE_BITS * pin)))
              | (mode << (GPIO_MODE_BITS * pin));
}

void gpio_config_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af)
{
  uint32_t reg   = pin / 8U;                  /* AFR[0] : 0..7, AFR[1] : 8..15 */
  uint32_t shift = GPIO_AF_BITS * (pin % 8U);

  port->AFR[reg] = (port->AFR[reg] & ~(GPIO_AF_MASK << shift)) | (af << shift);
  gpio_config_mode(port, pin, GPIO_MODE_AF);
}

int borner(int valeur, int min, int max)
{
  if (valeur < min) {
    return min;
  }
  if (valeur > max) {
    return max;
  }
  return valeur;
}

/*---------------------- Echauffement 1 : LED LD1 (PI1) ---------------------*/
#define LED_PORT   GPIOI
#define LED_PIN    1U

void led_init(void)
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;
  (void)RCC->AHB1ENR;                         /* attente activation horloge */

  gpio_config_mode(LED_PORT, LED_PIN, GPIO_MODE_OUTPUT);
  LED_PORT->OTYPER &= ~(1U << LED_PIN);       /* sortie push-pull */
  led_set(0);
}

void led_set(int allumee)
{
  if (allumee) {
    LED_PORT->BSRR = (1U << LED_PIN);
  } else {
    LED_PORT->BSRR = (1U << (LED_PIN + GPIO_BSRR_RESET_POS));
  }
}

/*-------------------- Echauffement 2 : DAC1 voie 1 (PA4) -------------------*/
#define DAC_PORT       GPIOA
#define DAC_PIN        4U
#define DAC_VALEUR_MAX 4095U                  /* DAC 12 bits : 4095 = 3,3 V */

void dac_init(void)
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;
  (void)RCC->APB1ENR;

  gpio_config_mode(DAC_PORT, DAC_PIN, GPIO_MODE_ANALOG);  /* evite tout conflit */
  DAC->CR |= DAC_CR_EN1;                      /* voie 1, buffer de sortie actif */
  dac_set_percent(0);
}

void dac_set_percent(int pourcent)
{
  uint32_t p = (uint32_t)borner(pourcent, 0, DAC_POURCENT_MAX);

  DAC->DHR12R1 = (p * DAC_VALEUR_MAX) / DAC_POURCENT_MAX;  /* 12 bits aligne a droite */
}

/*------------- Echauffement 3 : PWM TIM3 voie 1 (PB4, Arduino D3) ----------*/
#define PWM_PORT          GPIOB
#define PWM_PIN           4U
#define PWM_AF_TIM3       2U                  /* PB4 : AF2 = TIM3_CH1      */
#define PWM_TICK_HZ       1000000U            /* compteur a 1 MHz (1 us)  */
#define PWM_FREQ_HZ       1000U               /* periode de 1 ms          */
#define PWM_PERIODE_TICKS (PWM_TICK_HZ / PWM_FREQ_HZ)
#define TIM_OCM_PWM1      6U                  /* OCxM = 110 : mode PWM 1  */

void pwm_init(void)
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  (void)RCC->APB1ENR;

  gpio_config_af(PWM_PORT, PWM_PIN, PWM_AF_TIM3);

  TIM3->PSC  = (TIM_APB1_CLK_HZ / PWM_TICK_HZ) - 1U;   /* 100 MHz / 100 = 1 MHz */
  TIM3->ARR  = PWM_PERIODE_TICKS - 1U;                 /* 1000 ticks = 1 ms     */
  TIM3->CCR1 = 0U;
  TIM3->CCMR1 = (TIM3->CCMR1 & ~(TIM_CCMR1_CC1S | TIM_CCMR1_OC1M))
              | (TIM_OCM_PWM1 << TIM_CCMR1_OC1M_Pos)
              | TIM_CCMR1_OC1PE;                       /* CCR1 pris en compte a chaque periode */
  TIM3->CCER |= TIM_CCER_CC1E;                         /* sortie voie 1 active */
  TIM3->CR1  |= TIM_CR1_ARPE;
  TIM3->EGR   = TIM_EGR_UG;                            /* charge PSC et ARR */
  TIM3->CR1  |= TIM_CR1_CEN;
}

void pwm_set_duty(int pourcent)
{
  uint32_t p = (uint32_t)borner(pourcent, 0, PWM_POURCENT_MAX);

  TIM3->CCR1 = (p * PWM_PERIODE_TICKS) / PWM_POURCENT_MAX;
}
