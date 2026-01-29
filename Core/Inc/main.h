/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "PID.h"
#include "DCMotor.h"
#include "MPU.h"
#include <math.h>
#include <string.h>
#include "kalman.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern DCMotor MotorA;
extern DCMotor MotorB;
extern MPU6050_Raw Raw;
extern PID_Param_t PID_speed;
extern PID_Param_t PID_angle;
extern float curr_speed;
extern float sampling_time;
extern float angle;
extern int count;
extern int count0;
extern int prev_dc;
extern volatile float Setpoint;
extern float Deadzone0;
extern float Deadzone1;
extern float Deadzone2;
extern float Deadzone3;
extern float total_Ax;
extern float total_Az;
extern float total_Gy;
extern float Gyro_angle;
extern float prev_angle;
extern float total_speed;
extern float Kp_speed;
extern float Ki_speed;
extern float Kd_speed;
extern float Kp_angle;
extern float Ki_angle;
extern float Kd_angle;
extern float prev_speed;
extern Kalman_t kalman;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void Balance(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
#define CalSpeedTime 0.01f

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
