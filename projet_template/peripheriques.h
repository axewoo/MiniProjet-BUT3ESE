/*----------------------------------------------------------------------------
 * peripheriques.h
 * Seance 1, etudiant B : peripheriques simples de la carte STM32F746G-DISCO
 * pilotes depuis l'interface graphique (exercices d'echauffement).
 *
 * Acces direct aux registres (stm32f7xx.h) : aucun module HAL TIM/ADC/DAC
 * n'est necessaire dans le RTE.
 *---------------------------------------------------------------------------*/
#ifndef PERIPHERIQUES_H
#define PERIPHERIQUES_H

#include <stdint.h>
#include "stm32f7xx.h"

/* Horloge des timers du bus APB1 (TIM2..TIM7, TIM12..TIM14) :
 * configuration_overclock() regle HCLK = 200 MHz et APB1 = HCLK / 4 = 50 MHz ;
 * le prescaler APB1 etant different de 1, les timers recoivent 2 x 50 = 100 MHz. */
#define TIM_APB1_CLK_HZ    100000000U

/* Ramene une valeur dans l intervalle [min, max]. */
int borner(int valeur, int min, int max);

/* Configure une broche en fonction alternative (AF0..AF15). */
void gpio_config_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af);

/* Echauffement 1 : LED utilisateur LD1 (verte), broche PI1 */
void led_init(void);
void led_set(int allumee);           /* 0 = eteinte, autre = allumee */

/* Echauffement 2 : DAC1 voie 1, sortie PA4 (0 a 3,3 V) */
#define DAC_POURCENT_MAX   100
void dac_init(void);
void dac_set_percent(int pourcent);  /* 0..100 % de la pleine echelle */

/* Echauffement 3 : PWM 1 kHz sur TIM3 voie 1, sortie PB4 (Arduino D3) */
#define PWM_POURCENT_MAX   100
void pwm_init(void);
void pwm_set_duty(int pourcent);     /* rapport cyclique 0..100 % */

/* Echauffement 4 : potentiometre sur ADC3 voie 0, entree PA0 (Arduino A0) */
#define ADC_VALEUR_MAX     4095U             /* ADC 12 bits : 4095 = 3,3 V */
void adc_init(void);
uint16_t adc_read(void);             /* 0..4095 (derniere valeur si delai depasse) */

#endif /* PERIPHERIQUES_H */
