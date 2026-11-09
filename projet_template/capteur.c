#include "stm32f7xx.h"
#include "capteur.h"

// TIM2 sert de chronometre : il compte les microsecondes.
// TIM2 est sur APB1 : horloge timer = 2 x 50 MHz = 100 MHz (overclock du template).
// PSC = 99 -> fCNT = 100 MHz / (99 + 1) = 1 MHz, 1 tick = 1 us
// TIM2 est un compteur 32 bits : ARR = 0xFFFFFFFF, il ne repasse a 0 qu'apres 71 min.
#define TIMEOUT_US  30000   // sans echo au bout de 30 ms : TIMEOUT
#define TRIG_US     10      // impulsion de declenchement de 10 us

static int derniere_mesure = -1;   // en dixiemes de cm

static void attendre_us(unsigned int us)
{
  unsigned int debut = TIM2->CNT;
  while ((TIM2->CNT - debut) < us);
}

void capteur_init(void)
{
  RCC->AHB1ENR |= (1 << 1);     //activates GPIOB
  RCC->AHB1ENR |= (1 << 6);     //activates GPIOG
  RCC->APB1ENR |= (1 << 0);     //activates TIM2

  GPIOB->MODER &= ~(3 << 28);   //PB14 : on efface les 2 bits du mode
  GPIOB->MODER |= (1 << 28);    //PB14 in output mode (TRIG)
  GPIOB->BSRR = (1 << 30);      //TRIG = 0 (bit 14 + 16 = reset)

  GPIOG->MODER &= ~(3 << 12);   //PG6 in input mode (ECHO)

  TIM2->PSC = 99;               //Prescaler : 1 MHz
  TIM2->ARR = 0xFFFFFFFF;       //compte jusqu'au maximum
  TIM2->EGR |= (1 << 0);        //charge PSC tout de suite (UG)
  TIM2->CR1 |= (1 << 0);        //starts counter
}

float distance_cm(void)
{
  unsigned int debut;
  unsigned int duree;

  // 1) impulsion de 10 us sur TRIG
  GPIOB->BSRR = (1 << 14);      //TRIG = 1
  attendre_us(TRIG_US);
  GPIOB->BSRR = (1 << 30);      //TRIG = 0

  // 2) attente du front montant de ECHO
  debut = TIM2->CNT;
  while ((GPIOG->IDR & (1 << 6)) == 0) {
    if ((TIM2->CNT - debut) > TIMEOUT_US) {
      derniere_mesure = -1;
      return -1;                //pas d'echo : TIMEOUT
    }
  }

  // 3) mesure de la duree de ECHO a l'etat haut
  debut = TIM2->CNT;
  while ((GPIOG->IDR & (1 << 6)) != 0) {
    if ((TIM2->CNT - debut) > TIMEOUT_US) {
      derniere_mesure = -1;
      return -1;                //echo trop long : TIMEOUT
    }
  }
  duree = TIM2->CNT - debut;    //aller-retour du son, en us

  // 4) conversion : distance (cm) = duree (us) / 58
  derniere_mesure = (int)((duree * 10) / 58);
  return duree / 58.0f;
}

int capteur_get_dixiemes(void)
{
  return derniere_mesure;
}
