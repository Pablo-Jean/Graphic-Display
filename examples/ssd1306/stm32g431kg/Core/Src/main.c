/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "drivers/ssd1306/ssd1306.h"

#include "GraphicDisplay.h"
#include "GraphicDisplay_Fonts.h"
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
I2C_HandleTypeDef hi2c3;

/* USER CODE BEGIN PV */
#define cobra_width 	 64
#define cobra_height 	 64
const unsigned char cobra_bmp [] ={
0x00,0x00,0x00,0xb4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x03,0x00,0x00,0x00,
0x00,0x00,0xc0,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x18,0x00,0x00,0x00,
0x00,0x00,0x30,0x0e,0x30,0x00,0x00,0x00,0x00,0x00,0x98,0x1b,0x70,0x00,0x00,0x00,
0x00,0x00,0x88,0x30,0xd0,0x00,0x00,0x00,0x00,0x00,0x84,0x68,0x90,0x01,0x00,0x00,
0x00,0x00,0xc6,0xdc,0x30,0x03,0x00,0x00,0x00,0x00,0x82,0x87,0x21,0x06,0x00,0x00,
0x00,0x00,0x02,0x03,0xc3,0x0c,0x00,0x00,0x00,0x00,0x03,0x01,0x86,0x3f,0x00,0x00,
0x00,0x00,0x81,0x01,0x0c,0xe0,0x00,0x00,0x00,0x00,0x81,0x00,0x18,0x80,0x03,0x00,
0x00,0x00,0x83,0x00,0x70,0x00,0x06,0x00,0x00,0x00,0x8f,0x00,0x40,0x00,0x04,0x00,
0x00,0x00,0x99,0x00,0xc0,0x01,0x0c,0x00,0x00,0x00,0x91,0x00,0x00,0x03,0x04,0x00,
0x00,0x00,0xb1,0x01,0x00,0x06,0x04,0x00,0x00,0x00,0x93,0x00,0x00,0x0c,0x0c,0x00,
0x00,0x00,0x11,0x01,0x00,0x08,0x04,0x00,0x00,0x00,0x1a,0x03,0x00,0x18,0x08,0x00,
0x00,0x00,0x0e,0x06,0x00,0x10,0x08,0x00,0x00,0x00,0x06,0x06,0x00,0x30,0x08,0x00,
0x00,0x00,0x04,0x1b,0x00,0x60,0x0c,0x00,0x00,0x00,0x0c,0x32,0x00,0xc0,0x07,0x00,
0x00,0x00,0x18,0x63,0x00,0x80,0x04,0x00,0x00,0x00,0x30,0xc6,0x00,0x00,0x0c,0x00,
0x00,0x00,0x60,0xcc,0x01,0x00,0x08,0x00,0x00,0x00,0xc0,0x78,0x03,0x00,0x38,0x00,
0x00,0x00,0x80,0x01,0x06,0x00,0x68,0x00,0x00,0x00,0x00,0x03,0x1c,0x00,0x0c,0x00,
0x00,0x00,0x20,0x06,0x18,0x00,0x00,0x00,0x00,0x00,0xfe,0xff,0xff,0x7f,0x00,0x00,
0x00,0x00,0x03,0x20,0x08,0xc0,0x00,0x00,0x00,0x00,0x01,0x60,0x0c,0x80,0x00,0x00,
0x00,0x80,0x01,0xc0,0x07,0x80,0x01,0x00,0x00,0x80,0x00,0x80,0x02,0x00,0x01,0x00,
0x00,0x00,0x01,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0xc0,0x00,0x00,
0x00,0xe0,0xff,0xff,0xff,0xff,0x07,0x00,0x00,0x38,0x49,0xb2,0x2c,0x49,0x1a,0x00,
0x00,0x08,0x00,0x60,0x08,0x00,0x10,0x00,0x00,0x0c,0x00,0x40,0x0e,0x00,0x30,0x00,
0x00,0x04,0xf0,0xc0,0x03,0x1e,0x20,0x00,0x00,0x08,0x98,0x01,0x00,0x33,0x30,0x00,
0x00,0x08,0x0c,0x01,0x00,0x61,0x10,0x00,0x00,0xba,0x4c,0x2b,0xd5,0x65,0x5e,0x00,
0x80,0xff,0xff,0xff,0xff,0xff,0xff,0x01,0xc0,0x00,0x00,0x20,0x08,0x00,0x00,0x03,
0x40,0x00,0x00,0x60,0x0c,0x00,0x00,0x02,0x60,0x00,0x14,0xc0,0x07,0x50,0x00,0x06,
0x20,0x00,0x3e,0x00,0x01,0xf8,0x00,0x04,0x40,0x00,0x63,0x00,0x00,0x8c,0x01,0x02,
0xc0,0x00,0x41,0x00,0x00,0x04,0x01,0x03,0xf8,0xff,0xff,0xff,0xff,0xff,0xff,0x1f,
0x0e,0x82,0x20,0x28,0x89,0x20,0x08,0x72,0x02,0x00,0x00,0x60,0x0c,0x00,0x00,0x40,
0x03,0x40,0x00,0x40,0x06,0x00,0x02,0xc0,0x02,0xf0,0x01,0xc0,0x03,0x80,0x0f,0x80,
0x03,0x18,0x03,0x00,0x00,0xc0,0x18,0x40,0x02,0x08,0x02,0x00,0x00,0x40,0x10,0x60,
0xae,0x5a,0x57,0x55,0xb5,0xea,0x5e,0x35,0xf8,0xff,0xff,0xff,0xff,0xff,0xff,0x1f};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t _ssd_i2c_write(uint8_t i2cAddr, uint8_t reg, uint8_t *buff, uint32_t len){
	return (uint8_t)HAL_I2C_Mem_Write(&hi2c3, (i2cAddr << 1), reg, 1, buff, len, 100);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	ssd1306_t Ssd1306 = {0};
	ssd1306_params_t Ssd1306_params = {
			.Mode = SSD1306_MODE_I2C,
			.u32Heigth = 64,
			.u32Width = 128,
			.bUseExternalFrameBuffer = false,
			.delayMs = HAL_Delay,
			.i2cWrite = _ssd_i2c_write,
			.u8I2CAddr = 0x3C
	};

	gd_t Graphic = {0};
	gd_params_t Graphic_params = {
			.DisplayDriver = Gd_Driver_SSD1306,
			.DisplayHandle = (void*)&Ssd1306,
			.u32Height = 64,
			.u32Width = 128
	};
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
  MX_I2C3_Init();
  /* USER CODE BEGIN 2 */
  SSD1306_Init(&Ssd1306, &Ssd1306_params);

  GD_Init(&Graphic, &Graphic_params);
  GD_Fill(&Graphic, GD_BLACK);

  GD_DrawBitmap(&Graphic, 0, 0, cobra_bmp, cobra_width, cobra_height, GD_WHITE);

  GD_SetCursor(&Graphic, 66, 10);
  GD_WriteString(&Graphic, "SSD1306", Font_7x10, GD_WHITE);

  GD_DrawCircle(&Graphic, 90, 30, 7, GD_WHITE);
  GD_FillCircle(&Graphic, 90, 30, 5, GD_WHITE);

  GD_DrawRectangle(&Graphic, 78, 43, 122, 62, GD_WHITE);
  GD_FillRectangle(&Graphic, 80, 45, 120, 60, GD_WHITE);

  GD_UpdateScreen(&Graphic);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x00300617;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

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
