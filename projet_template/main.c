#include "stm32f7xx.h"                  // Device header
#include "servo.h"
#include "capteur.h"
extern void configuration_overclock(void);
extern int Init_GUIThread (void);
extern void Init_Ethernet (void);

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

/*----------------------------------------------------------------------------
 *      Tâche de test (vide) : à compléter par les étudiants
 *---------------------------------------------------------------------------*/
osThreadId_t tid_Test;

 static void Thread_Test (void *argument)
 {
	 (void)argument;
	 while (1)
	 {
		 osDelay(1000);
	 }
 }

static int Init_TestThread (void)
{
  tid_Test = osThreadNew(Thread_Test, NULL, NULL);
  if (tid_Test == NULL) {
    return -1;
  }
  return 0;
}


int main(void)
{
  configuration_overclock(); //pour pouvoir utiliser l'écran LCD

	/**********************************
	Début Initialisation périphérique
	**********************************/
	//Initialiser vos périphériques ici
	
	RCC->APB1ENR |= (1<<29); //activation DAC
	RCC->AHB1ENR |= (1<<8) | (1<<0) | (1<<5); // Activation de GPIOI et de GPIOA et de GPIOF
	RCC->APB2ENR |= (1<<14); // Activation de SYSCFG (interruptions)
	GPIOI->MODER |= (1<<6); // PI3 en mode Sortie (Output)
	
	GPIOA->MODER |= (3<<8); //PA4 en mode  analog
	DAC->CR |= (1<<0); // Activation du canal sortie 1 PA4
	//----------------PWM-----------------------------------
	RCC->APB2ENR |=	(1<<17); // Activation TIM10
	GPIOF->AFR[0] |= (3<<24); // PF6 en mode AF3 (TIM10_CH1)
	
	TIM10->CCMR1 |= (6<<4); //TIM10_CH1 en PWM mode 1
	TIM10->CCER |= (1<<0); //connecte OC1 à TIM10_CH1
	
	servo_init();
	capteur_init();
	//----------------------------------------------------------------
	RCC->APB2ENR |=(1<<10); // Activation ADC3
	GPIOF->MODER |= (3<<20) | (2<<12); // PF10 Analog mode |PF6 en AF mode 
	ADC3->SQR3 |= (8<<0); // Utilisation de ADC3_IN8
	/**********************************
	Fin Initialisation périphérique
	**********************************/
	
	/**********************************
	Utilisation RTOS (Keil RTX5)
	**********************************/
	#ifdef RTE_CMSIS_RTOS2
	  /* Initialize CMSIS-RTOS2 */
	  osKernelInitialize ();

	  /* Create thread functions that start executing */
		Init_Ethernet();
		Init_GUIThread();
		Init_TestThread();   // tâche de test (boucle avec osDelay)

	  /* Start thread execution */
	  osKernelStart();
	#endif

  /* Infinite loop  (ne s'exécute pas avec le RTOS!!!)*/
  while (1)
  {
  }
}