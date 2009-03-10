#ifndef _MOTOR_H
#define _MOTOR_H 1

#define PWM_LINKS		OCR1A 
#define PWM_LINKS_ON	TCCR1A |=  (1<<COM1A1) 
#define PWM_LINKS_OFF   TCCR1A &= ~(1<<COM1A1) 

#define PWM_RECHTS		OCR1B 
#define PWM_RECHTS_ON	TCCR1A |=  (1<<COM1B1) 
#define PWM_RECHTS_OFF  TCCR1A &= ~(1<<COM1B1) 


extern void motor_init(void);
extern void motor_l(int8_t speed);
extern void motor_r(int8_t speed);
#endif
