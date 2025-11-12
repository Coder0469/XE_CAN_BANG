/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
#define PI 3.14159265358979323846f
#define MPU6050_ADDR (0x68 << 1)  // địa chỉ I2C của MPU6050
#define SMPLRT_DIV_REG        0x19
#define CONFIG_REG            0x1A
#define GYRO_CONFIG_REG       0x1B
#define ACCEL_CONFIG_REG      0x1C
#define ACCEL_CONFIG_REG_2    0x1D
#define WHO_AM_I_REG          0x75
#define PWR_MGMT_1_REG        0x6B
#define ACCEL_XOUT_H_REG      0x3B
#define timeOut 1000
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t TxBuffer[] = "Hello World! From STM32 USB CDC Device To Virtual COM Port\r\n";
uint8_t TxBufferLen = sizeof(TxBuffer);
char msg[64];
uint16_t samples = 0;
float gyro_x_offset = 0, gyro_y_offset = 0, gyro_z_offset = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
typedef struct {
	int16_t Accel_X_RAW;
	int16_t Accel_Y_RAW;
	int16_t Accel_Z_RAW;

	float Ax;
	float Ay;
	float Az;

	int16_t Gyro_X_RAW;
	int16_t Gyro_Y_RAW;
	int16_t Gyro_Z_RAW;

	float Gx;
	float Gy;
	float Gz;

	float Temp;
} MPU6050_Raw;

void MPU6050_Init(void)
{
    uint8_t check;
    uint8_t Data;

    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, timeOut);

    if (check == 104)  // 0x68
    {
        Data = 0x00;
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &Data, 1, timeOut);

        Data = 0x07;
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, timeOut);

        Data = 0x08;
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, timeOut);

        Data = 0x08;
        HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, timeOut);
    }
    else
    {
        sprintf(msg, "MPU6050 not found!\r\n");
        CDC_Transmit_FS((uint8_t*)msg, strlen(msg));
        HAL_Delay(500);
    }
}
void MPU6050_Read_Data(MPU6050_Raw *Raw){

	uint8_t Rec_Data[14];
	// Read 6 BYTES of data starting from ACCEL_XOUT_H register
	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 14, timeOut);

	Raw->Accel_X_RAW = (int16_t)(Rec_Data[0]  << 8 | Rec_Data [1]);
	Raw->Accel_Y_RAW = (int16_t)(Rec_Data[2]  << 8 | Rec_Data [3]);
	Raw->Accel_Z_RAW = (int16_t)(Rec_Data[4]  << 8 | Rec_Data [5]);
	Raw->Temp        = (int16_t)(Rec_Data[6]  << 8 | Rec_Data [7]);
	Raw->Gyro_X_RAW  = (int16_t)(Rec_Data[8]  << 8 | Rec_Data [9]);
	Raw->Gyro_Y_RAW  = (int16_t)(Rec_Data[10] << 8 | Rec_Data [11]);
	Raw->Gyro_Z_RAW  = (int16_t)(Rec_Data[12] << 8 | Rec_Data [13]);

	/*** convert the RAW values into acceleration in 'g'
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0. So I am dividing by 16384.0
	     for more details check ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 2g  ~  0000 0000  ~ 0x00 16,384
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 4g  ~  0000 1000  ~ 0x08 8192
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 8g  ~  0001 0000  ~ 0x10 4096
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 16g ~  0001 1000  ~ 0x18 2048 		 * ****/
	Raw->Ax = Raw->Accel_X_RAW/8192.0; //16384.0;
	Raw->Ay = Raw->Accel_Y_RAW/8192.0;
	Raw->Az = Raw->Accel_Z_RAW/8192.0;

    /*** convert the RAW values into dps (�/s)
         we have to divide according to the Full scale value set in FS_SEL
         I have configured FS_SEL = 0. So I am dividing by 131.0
         for more details check GYRO_CONFIG Register
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> � 250 �/s      ~  0000 0000  ~ 0x00 131
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=1 -> � 500 �/s      ~  0000 1000  ~ 0x08 65.5
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=2 -> � 1000 �/s   ~  0001 0000  ~ 0x10 32.8
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=3 -> � 2000 �/s   ~  0001 1000  ~ 0x18 16.4  			****/
		if(samples<32) {
		  samples ++;
		  return ;
		} else if(samples <64) {
			gyro_x_offset += Raw->Gyro_X_RAW;
			gyro_y_offset += Raw->Gyro_Y_RAW;
			gyro_z_offset += Raw->Gyro_Z_RAW;
			samples++;
			return;
		} else if(samples==64) {
			gyro_x_offset /= 32;
			gyro_y_offset /= 32;
			gyro_z_offset /= 32;
			samples++;
		} else {
			Raw->Gyro_X_RAW -= gyro_x_offset;
			Raw->Gyro_Y_RAW -= gyro_y_offset;
			Raw->Gyro_Z_RAW -= gyro_z_offset;
		}
	Raw->Gx = Raw->Gyro_X_RAW/65.5;
	Raw->Gy = Raw->Gyro_Y_RAW/65.5;
	Raw->Gz = Raw->Gyro_Z_RAW/65.5;

}
MPU6050_Raw Raw;
float angle_roll = 0.0;
float alpha = 0.98;
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
	MPU6050_Init();
	HAL_Delay(1000);
  /* USER CODE END 2 */
	char msg[100];
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  MPU6050_Read_Data(&Raw);
	  uint32_t t_prev = HAL_GetTick();
	  float pitch_acc = atan2f(-Raw.Ax, sqrtf(Raw.Ay*Raw.Ay + Raw.Az*Raw.Az)) * 180.0 / PI;
	  sprintf(msg, "A: %.2f %.2f %.2f | G: %.2f %.2f %.2f | Pitch: %.2f\r\n",
	          Raw.Ax, Raw.Ay, Raw.Az, Raw.Gx, Raw.Gy, Raw.Gz, pitch_acc);
	  CDC_Transmit_FS((uint8_t*)msg, strlen(msg));
	  HAL_Delay(500);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
void USB_CDC_RxHandler(uint8_t* Buf, uint32_t Len)
{
    CDC_Transmit_FS(TxBuffer, TxBufferLen);
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
