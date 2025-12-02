
#include "MPU.h"



uint8_t TxBuffer[] = "Hello World! From STM32 USB CDC Device To Virtual COM Port\r\n";
uint8_t TxBufferLen = sizeof(TxBuffer);
char msg[64];
uint16_t samples = 0;
float gyro_x_offset = 0, gyro_y_offset = 0, gyro_z_offset = 0;
float angle_roll = 0.0;
float alpha = 0.98;


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
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 14,timeOut);

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

