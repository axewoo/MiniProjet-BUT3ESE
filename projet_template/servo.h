/*----------------------------------------------------------------------------
 * servo.h
 * Seance 1, etudiant B : commande du servomoteur DM996 sur PF8 (TIM13 voie 1).
 *---------------------------------------------------------------------------*/
#ifndef SERVO_H
#define SERVO_H

#define SERVO_ANGLE_MIN   0      /* degres */
#define SERVO_ANGLE_MAX   180    /* degres */

/* Configure TIM13 en PWM 50 Hz sur PF8 et place le servo a 90 degres. */
void servo_init(void);

/* Oriente le servo a deg degres.
 * Retourne 0 si l angle est accepte, -1 s il est hors de [0, 180]
 * (le servo ne bouge pas : servira pour la reponse ERR du serveur UDP). */
int servo_set_angle(int deg);

/* Dernier angle demande au servo (en degres). */
int servo_get_angle(void);

#endif /* SERVO_H */
