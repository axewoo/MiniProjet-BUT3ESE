#ifndef SERVO_H
#define SERVO_H

void servo_init(void);            // PWM 50 Hz sur PF6 (TIM10_CH1)
void servo_set_angle(int deg);    // deg entre 0 et 180
int  servo_get_angle(void);       // dernier angle demande

#endif
