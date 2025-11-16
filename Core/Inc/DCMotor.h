
#include "stm32f4xx_hal.h"

#define FORWARD 1
#define BACKWARD -1
#define STOP 0
typedef struct{
	int max_speed;
	TIM_HandleTypeDef *htim;
	uint32_t PWM_Channel;
	uint16_t IN0;
	uint16_t IN1;
	uint16_t input;
	uint16_t exti;
	int pulse;
	uint8_t state;
	GPIO_TypeDef* Port;

} DCMotor;

void DCMotor_Init(DCMotor *motor,int max_speed, TIM_HandleTypeDef *htim,
				uint32_t PWM_Channel, GPIO_TypeDef* Port,uint16_t IN0, uint16_t IN1);

void DCMotor_Set_Speed(DCMotor *motor,uint16_t DutyCycle);

void DCMotor_Run(DCMotor *motor, int direction);
