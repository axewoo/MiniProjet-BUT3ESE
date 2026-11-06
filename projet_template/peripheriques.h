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

/* Configure une broche en fonction alternative (AF0..AF15). */
void gpio_config_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af);

/* Echauffement 1 : LED utilisateur LD1 (verte), broche PI1 */
void led_init(void);
void led_set(int allumee);           /* 0 = eteinte, autre = allumee */

/* Echauffement 2 : DAC1 voie 1, sortie PA4 (0 a 3,3 V) */
#define DAC_POURCENT_MAX   100
void dac_init(void);
void dac_set_percent(int pourcent);  /* 0..100 % de la pleine echelle */

#endif /* PERIPHERIQUES_H */
