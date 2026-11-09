#ifndef CAPTEUR_H
#define CAPTEUR_H

void  capteur_init(void);         // TRIG = PB14 (sortie), ECHO = PG6 (entree), TIM2 a 1 MHz
float distance_cm(void);          // distance en cm, ou -1 si pas d'echo (TIMEOUT)
int   capteur_get_dixiemes(void); // derniere mesure en dixiemes de cm (-1 = TIMEOUT)

#endif
