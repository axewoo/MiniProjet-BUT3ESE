#include "stm32f7xx.h"
#include "cmsis_os2.h"
#include "capteur.h"

// Mesure du HC-SR04 par interruptions (cours 3) :
//  - EXTI6 (PG6 = ECHO), front montant ET descendant :
//      front montant   -> on note TIM2->CNT (debut de l'echo)
//      front descendant -> duree = CNT - debut, calcul de la distance
//  - TIM2 canal 1 en comparaison : interruption 30 ms apres TRIG = TIMEOUT
//    (pas d'echo, ou echo trop long)
//
// TIM2 sert de chronometre : il compte les microsecondes.
// TIM2 est sur APB1 : horloge timer = 2 x 50 MHz = 100 MHz (overclock du template).
// PSC = 99 -> fCNT = 100 MHz / (99 + 1) = 1 MHz, 1 tick = 1 us
// TIM2 est un compteur 32 bits : ARR = 0xFFFFFFFF, il ne repasse a 0 qu'apres 71 min.
#define TIMEOUT_US  30000   // sans echo au bout de 30 ms : TIMEOUT
#define TRIG_US     10      // impulsion de declenchement de 10 us

static volatile int derniere_mesure = -1;   // en dixiemes de cm (-1 = TIMEOUT)
static volatile int mesure_finie = 1;       // 0 pendant une mesure, 1 quand elle est finie
static volatile int echo_commence = 0;      // 1 apres le front montant de ECHO
static volatile unsigned int debut_echo;    // TIM2->CNT au front montant de ECHO

void capteur_init(void)
{
  RCC->AHB1ENR |= (1 << 1);     //activates GPIOB
  RCC->AHB1ENR |= (1 << 6);     //activates GPIOG
  RCC->APB1ENR |= (1 << 0);     //activates TIM2
  RCC->APB2ENR |= (1 << 14);    //activates SYSCFG (pour EXTI)

  GPIOB->MODER &= ~(3 << 28);   //PB14 : on efface les 2 bits du mode
  GPIOB->MODER |= (1 << 28);    //PB14 in output mode (TRIG)
  GPIOB->BSRR = (1 << 30);      //TRIG = 0 (bit 14 + 16 = reset)

  GPIOG->MODER &= ~(3 << 12);   //PG6 in input mode (ECHO)

  // EXTI6 relie a PG6
  SYSCFG->EXTICR[1] &= ~(15 << 8);  //EXTI6[3:0] : on efface
  SYSCFG->EXTICR[1] |= (6 << 8);    //EXTI6 = PG6 (0110 = port G)
  EXTI->RTSR |= (1 << 6);           //front montant
  EXTI->FTSR |= (1 << 6);           //front descendant
  EXTI->IMR |= (1 << 6);            //demasque la ligne 6

  TIM2->PSC = 99;               //Prescaler : 1 MHz
  TIM2->ARR = 0xFFFFFFFF;       //compte jusqu'au maximum
  TIM2->EGR |= (1 << 0);        //charge PSC tout de suite (UG)
  TIM2->SR = 0;                 //efface les drapeaux (UG a leve UIF)
  TIM2->CR1 |= (1 << 0);        //starts counter

  NVIC_SetPriority(EXTI9_5_IRQn, 2);  //EXTI5 a EXTI9 partagent la meme IT
  NVIC_EnableIRQ(EXTI9_5_IRQn);
  NVIC_SetPriority(TIM2_IRQn, 3);
  NVIC_EnableIRQ(TIM2_IRQn);
}

// Envoie l'impulsion TRIG et rend la main tout de suite.
// Le resultat arrive par interruption (capteur_get_dixiemes).
void capteur_lancer_mesure(void)
{
  unsigned int t;

  echo_commence = 0;
  mesure_finie = 0;
  TIM2->CCR1 = TIM2->CNT + TIMEOUT_US + TRIG_US;  //date du TIMEOUT
  TIM2->SR = ~(1 << 1);         //efface CC1IF
  TIM2->DIER |= (1 << 1);       //CC1IE : IT de TIMEOUT

  GPIOB->BSRR = (1 << 14);      //TRIG = 1
  t = TIM2->CNT;
  while ((TIM2->CNT - t) < TRIG_US);   //10 us seulement
  GPIOB->BSRR = (1 << 30);      //TRIG = 0
}

// Version bloquante, gardee pour UDP_Socket.c (DIST) : lance la mesure
// et attend la fin (IT) en laissant tourner les autres taches.
float distance_cm(void)
{
  capteur_lancer_mesure();
  while (mesure_finie == 0) {
    osDelay(1);
  }
  if (derniere_mesure < 0) return -1;
  return derniere_mesure / 10.0f;
}

int capteur_get_dixiemes(void)
{
  return derniere_mesure;
}

// Le nom de la fonction d'interruption est impose (table d'interruption)
void EXTI9_5_IRQHandler(void)
{
  unsigned int duree;

  if (EXTI->PR & (1 << 6)) {
    EXTI->PR = (1 << 6);        //acquittement (ecrire 1)

    if (mesure_finie) return;   //front parasite hors mesure : ignore

    if (GPIOG->IDR & (1 << 6)) {
      debut_echo = TIM2->CNT;   //front montant : debut de l'echo
      echo_commence = 1;
    } else if (echo_commence) {
      duree = TIM2->CNT - debut_echo;            //aller-retour du son, en us
      derniere_mesure = (int)((duree * 10) / 58); //distance (cm) = duree / 58
      TIM2->DIER &= ~(1 << 1);  //plus besoin du TIMEOUT
      mesure_finie = 1;
    }
  }
}

void TIM2_IRQHandler(void)
{
  if (TIM2->SR & (1 << 1)) {
    TIM2->SR = ~(1 << 1);       //acquittement (ecrire 0 dans CC1IF)
    TIM2->DIER &= ~(1 << 1);    //une seule IT par mesure
    if (mesure_finie == 0) {
      derniere_mesure = -1;     //pas d'echo a temps : TIMEOUT
      mesure_finie = 1;
    }
  }
}