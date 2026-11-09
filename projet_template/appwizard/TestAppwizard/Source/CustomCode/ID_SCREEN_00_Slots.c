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
#include "../../../../servo.h"
#include "../../../../capteur.h"
#include <stdio.h>
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
  //code à exécuter à mettre ici
	if (Val==1){
	GPIOI->ODR |=(1<<3); // PI3 mis à 1 donc allumer
	}
	else{
	GPIOI->ODR &= ~(1<<3); // PI3 mis à 0 donc éteint
	}
}
	 static void afficher_angle(void)
	 {
		 static char texte[32];
		 static int angle_affiche = -1;
		 static int dist_affichee = -2;
		 int angle = servo_get_angle();
		 int d = capteur_get_dixiemes();

		 if (angle == angle_affiche && d == dist_affichee) return;
		 angle_affiche = angle;
		 dist_affichee = d;
		 if (d < 0)
			 sprintf(texte, "%d deg  TIMEOUT", angle);
		 else
			 sprintf(texte, "%d deg  %d.%d cm", angle, d / 10, d % 10);
		 APPW_SetText(ID_SCREEN_00, ID_TEXT_00, texte);
		 APPW_SetValue(ID_SCREEN_00, ID_GAUGE_00, (angle * 4095) / 180);
	 }


/*********************************************************************
*
*       ID_SCREEN_00__ID_SLIDER_00__WM_NOTIFICATION_VALUE_CHANGED
*/
void ID_SCREEN_00__ID_SLIDER_00__WM_NOTIFICATION_VALUE_CHANGED(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  int Val,Slider;
  Val = APPW_GetValue(ID_SCREEN_00, ID_SLIDER_00, NULL);
  //Val vaut une valeur entre 0 et 100
  //code à exécuter à mettre ici
	
}

/*********************************************************************
*
*       ID_SCREEN_00__ID_ROTARY_00__WM_NOTIFICATION_VALUE_CHANGED
*/
void ID_SCREEN_00__ID_ROTARY_00__WM_NOTIFICATION_VALUE_CHANGED(APPW_ACTION_ITEM * pAction, WM_HWIN hScreen, WM_MESSAGE * pMsg, int * pResult) {
  int Val;
	
  Val = APPW_GetValue(ID_SCREEN_00, ID_ROTARY_00, NULL);
  //Val vaut une valeur entre -300 et 300
  //code à exécuter à mettre ici
	servo_set_angle(((Val + 300)*180)/600);
	afficher_angle();
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
	afficher_angle();
//	static char texte[24];
//	sprintf(texte, "Servo : %d deg", servo_get_angle());
//	APPW_SetText(ID_SCREEN_00, ID_TEXT_00, texte);
//	APPW_SetValue(ID_SCREEN_00, ID_GAUGE_00, (servo_get_angle() * 4095) / 180);
	APPW_DoJob(ID_SCREEN_00, ID_TIMER_00, APPW_JOB_START, NULL);

}

/*************************** End of file ****************************/
