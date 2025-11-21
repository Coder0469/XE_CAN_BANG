
#include "DCMotor.h"
#include <math.h>
void DCMotor_Init(DCMotor *motor, TIM_HandleTypeDef *htim,
				uint32_t PWM_Channel, GPIO_TypeDef* Port,uint16_t IN0, uint16_t IN1){
	motor->speed = 0;
	motor->htim = htim;
	motor->PWM_Channel = PWM_Channel;
	motor->Port = Port;
	motor->IN0 = IN0;
	motor->IN1 = IN1;
	motor->pulse = 0;
	motor->prev_pulse = 0;
	motor->state = 0;


}

void DCMotor_Set_Speed(DCMotor *motor,uint16_t DutyCycle){
	 DutyCycle = DutyCycle * motor->htim->Instance->ARR/100;
	__HAL_TIM_SET_COMPARE(motor->htim,motor->PWM_Channel, DutyCycle);
}

void DCMotor_Run(DCMotor *motor, int direction){
	if(direction == 1){
		HAL_GPIO_WritePin(motor->Port, motor->IN0, SET);
		HAL_GPIO_WritePin(motor->Port, motor->IN1, RESET);
	}
	else if(direction == -1){
		HAL_GPIO_WritePin(motor->Port, motor->IN0, RESET);
		HAL_GPIO_WritePin(motor->Port, motor->IN1, SET);
	}
	else{
		HAL_GPIO_WritePin(motor->Port, motor->IN0, RESET);
		HAL_GPIO_WritePin(motor->Port, motor->IN1, RESET);
	}
}

void DCMotor_CalculateSpeed(DCMotor *motor, float duration){
	duration = duration+0.001;
	if(motor->prev_pulse < motor->pulse - 1000) motor->prev_pulse = motor->pulse - 1000;
	motor->speed = (motor->pulse - motor->prev_pulse)/duration;
	if(motor->speed < 0) motor->speed = -motor->speed;
	motor->prev_pulse = motor->pulse;
}
