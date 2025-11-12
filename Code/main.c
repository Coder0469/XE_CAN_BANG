#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include <string.h>


#define FLASH_USER_START_ADDR   0x080E0000  // Sector 11 start
#define FLASH_USER_END_ADDR     0x080FFFFF  // Sector 11 end
#define FLASH_PAGE_SIZE         0x20000     // 128 KB per sector (sector 11)


uint32_t Flash_Write_Address = FLASH_USER_START_ADDR;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
void Process_USB_Command(uint8_t* buf, uint32_t len);
uint32_t Flash_Write_Data(uint8_t* data, uint32_t len);
void Flash_Read_Data(void);
void Flash_Erase(void);


int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USB_DEVICE_Init();

  while (1)
  {
      HAL_Delay(10);
  }
}
uint32_t Flash_Write_Data(uint8_t* data, uint32_t len)
{
    HAL_FLASH_Unlock();
    uint32_t bytes_to_write = len;
    uint32_t bytes_written = 0; // Biến đếm số byte đã ghi thành công

    if (Flash_Write_Address + bytes_to_write > FLASH_USER_END_ADDR) {
        bytes_to_write = FLASH_USER_END_ADDR - Flash_Write_Address + 1;
    }

    for(uint32_t i = 0; i < bytes_to_write; i++)
    {
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, Flash_Write_Address, data[i]) == HAL_OK)
        {
            Flash_Write_Address++;
            bytes_written++; // Ghi thành công
        }
        else
        {
            // Lỗi ghi xảy ra, dừng và trả về số byte đã ghi được
            break;
        }
    }
    HAL_FLASH_Lock();
    return bytes_written; // Trả về số byte đã ghi thành công
}
void Flash_Read_Data(void)
{
    uint32_t addr = FLASH_USER_START_ADDR;
    uint8_t buf[64];
    uint32_t cnt = 0;

    while ((addr < FLASH_USER_END_ADDR))
    {
        uint8_t byte = *(__IO uint8_t *)addr++;
        if (byte == '\0' || byte == 0xFF) break; // Dừng khi gặp ký tự kết thúc hoặc vùng trống
        buf[cnt++] = byte;

        if (cnt == 64)
        {
            CDC_Transmit_FS(buf, 64);
            cnt = 0;
        }
    }

    if (cnt > 0)
        CDC_Transmit_FS(buf, cnt);
    else
    	CDC_Transmit_FS("FLASH EMPTY\n",strlen("FLASH EMPTY\n"));
}
void Flash_Erase(void)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Sector = FLASH_SECTOR_11;
    EraseInitStruct.NbSectors = 1;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3; // 2.7 ~ 3.6V

    if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    {
     CDC_Transmit_FS((uint8_t*)"ERASE FAILED! SECTOR NOT CLEAR!\r\n", 33);
    }

    HAL_FLASH_Lock();

    Flash_Write_Address = FLASH_USER_START_ADDR;
}
void Process_USB_Command(uint8_t* buf, uint32_t len){
	if(len>=6 && strncmp((char*)buf, "WRITE ", 6)==0)
     {
         uint32_t data_len = len - 6;
         uint32_t written_len = Flash_Write_Data(buf + 6, data_len);

         // Ghi ký tự kết thúc '\0'
         uint8_t null_byte = '\0';
         Flash_Write_Data(&null_byte, 1);
// Bắt đầu logic báo cáo lỗi ghi
            	if (written_len < data_len)
                  {
                      // Chuyển đổi số thành chuỗi để báo cáo lỗi
                      char msg_buffer[64];
                      int required_len = snprintf(msg_buffer, 64, "WRITE FAILED! Chỉ ghi được %lu/%lu byte.\r\n",
                                                  written_len, data_len);
                      CDC_Transmit_FS((uint8_t*)msg_buffer, (uint32_t)required_len);
                  }
                  else
                  {
                      CDC_Transmit_FS((uint8_t*)"Write complete\r\n", strlen("Write complete\r\n"));
                  }
              }
        else if(len>=4 && strncmp((char*)buf, "READ ", 4)==0)
             {
                 Flash_Read_Data();
                 char msg[] = "\r\n";
                 CDC_Transmit_FS((uint8_t*)msg, strlen(msg));
             }
        else if(len>=5 && strncmp((char*)buf, "ERASE", 5)==0)
             {
                 Flash_Erase();
                 char msg[] = "\r\n";
                 CDC_Transmit_FS((uint8_t*)"Erase complete\r\n", strlen("Erase complete\r\n"));
                 CDC_Transmit_FS((uint8_t*)msg, strlen(msg));
             }
        else
             {
              CDC_Transmit_FS((uint8_t*)"MISTAKE\r\n", strlen("MISTAKE\r\n"));
             }
         }

void SystemClock_Config(void){
           RCC_OscInitTypeDef RCC_OscInitStruct = {0};
           RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

           __HAL_RCC_PWR_CLK_ENABLE();
           __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

           RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
           RCC_OscInitStruct.HSEState = RCC_HSE_ON;
           RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
           RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
           RCC_OscInitStruct.PLL.PLLM = 8;
           RCC_OscInitStruct.PLL.PLLN = 336;
           RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
           RCC_OscInitStruct.PLL.PLLQ = 7;
           if(HAL_RCC_OscConfig(&RCC_OscInitStruct)!=HAL_OK) Error_Handler();
           RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|
                                         RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
           RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
           RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
           RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
           RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
           if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5)!=HAL_OK) Error_Handler();
         }
         static void MX_GPIO_Init(void)
         {
             __HAL_RCC_GPIOH_CLK_ENABLE();
             __HAL_RCC_GPIOA_CLK_ENABLE();
             __HAL_RCC_GPIOB_CLK_ENABLE();
         }

         static void MX_SPI1_Init(void)
         {
         }
void Error_Handler(void)
         {
           __disable_irq();
           while(1);
         }
