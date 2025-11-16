
#include "DCMotor.h"

void DCMotor_Init(DCMotor *motor,int max_speed, TIM_HandleTypeDef *htim,
				uint32_t PWM_Channel, GPIO_TypeDef* Port,uint16_t IN0, uint16_t IN1){
	motor->max_speed = max_speed;
	motor->htim = htim;
	motor->PWM_Channel = PWM_Channel;
	motor->Port = Port;
	motor->IN0 = IN0;
	motor->IN1 = IN1;
	motor->pulse = 0;
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
