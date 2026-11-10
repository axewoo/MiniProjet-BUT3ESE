#ifndef CAPTEUR_H
#define CAPTEUR_H

void  capteur_init(void);          // TRIG = PB14, ECHO = PG6 (EXTI6), TIM2 a 1 MHz, IT activees
void  capteur_lancer_mesure(void); // envoie TRIG et rend la main : la mesure se fait en IT
float distance_cm(void);           // lance + attend la fin (30 ms max), -1 si TIMEOUT
int   capteur_get_dixiemes(void);  // derniere mesure en dixiemes de cm (-1 = TIMEOUT)

#endif