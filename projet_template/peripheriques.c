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
