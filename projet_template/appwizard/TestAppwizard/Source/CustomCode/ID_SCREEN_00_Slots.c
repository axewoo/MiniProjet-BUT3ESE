/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2025  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : ID_SCREEN_00_Slots.c
Purpose     : AppWizard managed file, function content could be changed
---------------------------END-OF-HEADER------------------------------
*/

#include "Application.h"
#include "../Generated/Resource.h"
#include "../Generated/ID_SCREEN_00.h"

/*** Begin of user code area ***/
#include "stm32f7xx.h"                  // Device header
#include "../../../../peripheriques.h"   // LED, DAC, PWM, ADC (racine du projet Keil)

#include "../../../../servo.h"           // servo_get_angle()
#include <stdio.h>

#define ROTARY_DEMI_COURSE 900            // la mollette donne -900..+900
#define TAILLE_TEXTE_ANGLE 24

/*** End of user code area ***/

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       cbID_SCREEN_00
*/
void cbID_SCREEN_00(WM_MESSAGE * pMsg) {
  GUI_USE_PARA(pMsg);
}

/*********************************************************************
*
*       ID_SCREEN_00__ID_SWITCH_00__WM_NOTIFICATION_VALUE_CHANGED
*/
void ID_SCREEN_00__ID_SWITCH_00__WM_NOTIFICATION_VALUE_CHANGED(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  int Val;
  Val = APPW_GetValue(ID_SCREEN_00, ID_SWITCH_00, NULL);
  //Val vaut 0 ou 1
  led_set(Val);   // 1 : LED LD1 allumee, 0 : eteinte
  //code à exécuter à mettre ici
}

/*********************************************************************
*
*       ID_SCREEN_00__ID_SLIDER_00__WM_NOTIFICATION_VALUE_CHANGED
*/
void ID_SCREEN_00__ID_SLIDER_00__WM_NOTIFICATION_VALUE_CHANGED(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  int Val;
  Val = APPW_GetValue(ID_SCREEN_00, ID_SLIDER_00, NULL);
  //Val vaut une valeur entre 0 et 100
  dac_set_percent(Val);   // 0 % -> 0 V, 100 % -> 3,3 V sur PA4
  //code à exécuter à mettre ici
}

/*********************************************************************
*
*       ID_SCREEN_00__ID_ROTARY_00__WM_NOTIFICATION_VALUE_CHANGED
*/
void ID_SCREEN_00__ID_ROTARY_00__WM_NOTIFICATION_VALUE_CHANGED(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  int Val;
  Val = APPW_GetValue(ID_SCREEN_00, ID_ROTARY_00, NULL);
  //Val vaut une valeur entre -900 et 900
  // -900 -> 0 %, 0 -> 50 %, +900 -> 100 % de rapport cyclique
  pwm_set_duty(((Val + ROTARY_DEMI_COURSE) * PWM_POURCENT_MAX) / (2 * ROTARY_DEMI_COURSE));
  //code à exécuter à mettre ici
}

/*********************************************************************
*
*       ID_SCREEN_00__APPW_NOTIFICATION_INITDIALOG__ID_TIMER_00__APPW_JOB_START
*/
void ID_SCREEN_00__APPW_NOTIFICATION_INITDIALOG__ID_TIMER_00__APPW_JOB_START(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);
}

/*********************************************************************
*
*       ID_SCREEN_00__ID_TIMER_00__APPW_NOTIFICATION_TIMER
*/
void ID_SCREEN_00__ID_TIMER_00__APPW_NOTIFICATION_TIMER(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  GUI_USE_PARA(pAction);
  GUI_USE_PARA(hScreen);
  GUI_USE_PARA(pMsg);
  GUI_USE_PARA(pResult);
  // La jauge est reglee sur 0..4095 : on lui donne directement la mesure 12 bits
  APPW_SetValue(ID_SCREEN_00, ID_GAUGE_00, adc_read());

  // Affiche l angle du servo dans le texte du haut (ID_TEXT_00). Le texte
  // ID_TEXT_01 est reserve a l adresse IP (mise a jour par Check_IP).
  // On ecrit ici, dans la tache de l ecran, et non dans Thread_Test :
  // emWin n est pas protege contre les appels depuis plusieurs taches.
  {
    static char texte_angle[TAILLE_TEXTE_ANGLE];
    static int  angle_affiche = -1;
    int         angle = servo_get_angle();

    if (angle != angle_affiche) {           // ne redessine que si l angle change
      snprintf(texte_angle, sizeof(texte_angle), "Servo : %d deg", angle);
      APPW_SetText(ID_SCREEN_00, ID_TEXT_00, texte_angle);
      angle_affiche = angle;
    }
  }

  // ID_TIMER_00 n est pas en "auto restart" : on le relance pour la periode suivante
  APPW_DoJob(ID_SCREEN_00, ID_TIMER_00, APPW_JOB_START, NULL);
}

/*************************** End of file ****************************/
