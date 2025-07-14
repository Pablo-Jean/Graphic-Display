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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stddef.h>

#include "GraphicDisplay.h"
#include "GraphicDisplay_Fonts.h"
#include "drivers/st7565/st7565.h"
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
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
st7565_t St7565 = {0};
gd_t Graphic = {0};

const uint32_t cobra_width = 64;
const uint32_t cobra_height = 64;
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
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void _dsp_a0(uint8_t s){
	HAL_GPIO_WritePin(DSP_A0_GPIO_Port, DSP_A0_Pin, s);
}

void _dsp_cs(uint8_t s){
	HAL_GPIO_WritePin(DSP_CS_GPIO_Port, DSP_CS_Pin, s);
}

void _dsp_reset(uint8_t s){
	HAL_GPIO_WritePin(DSP_RST_GPIO_Port, DSP_RST_Pin, s);
}

uint8_t _dsp_write(uint8_t *data, uint32_t len){
	HAL_StatusTypeDef e;

	e = HAL_SPI_Transmit(&hspi1, data, len, 100);

	return (uint8_t)e;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	uint32_t i, way;
	/* USER CODE BEGIN 1 */
	st7565_params_t St7565_Params = {
			.Mode = ST7565_MODE_SPI,
			.bUseExternalFrameBuffer = false,
			.u32Heigth = 64,
			.u32Width = 128,
			.delayMs = HAL_Delay,
			.spiA0 = _dsp_a0,
			.spiCs = _dsp_cs,
			.spiReset = _dsp_reset,
			.spiWrite = _dsp_write
	};

	gd_params_t Graphic_Params = {
			.DisplayDriver = Gd_Driver_ST7565,
			.DisplayHandle = (void*)(&St7565),
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
	MX_USART2_UART_Init();
	MX_SPI1_Init();
	/* USER CODE BEGIN 2 */
	ST7565_Init(&St7565, &St7565_Params);

	GD_Init(&Graphic, &Graphic_Params);

	GD_DrawBitmap(&Graphic, 0, 0, cobra_bmp, cobra_width, cobra_height, GD_WHITE);

	GD_SetCursor(&Graphic, 60, 5);
	GD_WriteString(&Graphic, "ST7565", Font_11x18, GD_WHITE);

	GD_DrawRectangle(&Graphic, 78, 43, 122, 62, GD_WHITE);
	GD_FillRectangle(&Graphic, 80, 45, 120, 60, GD_WHITE);

	GD_UpdateScreen(&Graphic);

	HAL_Delay(1000);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	way = 0;
	i = 70;
	while (1)
	{
		if (way == 0){
			GD_DrawCircle(&Graphic, i, 30, 7, GD_BLACK);
			GD_FillCircle(&Graphic, i, 30, 5, GD_BLACK);
			i++;
			GD_DrawCircle(&Graphic, i, 30, 7, GD_WHITE);
			GD_FillCircle(&Graphic, i, 30, 5, GD_WHITE);
			if (i > 120){
				way = 1;
			}
		}
		else{
			GD_DrawCircle(&Graphic, i, 30, 7, GD_BLACK);
			GD_FillCircle(&Graphic, i, 30, 5, GD_BLACK);
			i--;
			GD_DrawCircle(&Graphic, i, 30, 7, GD_WHITE);
			GD_FillCircle(&Graphic, i, 30, 5, GD_WHITE);
			if (i < 70){
				way = 0;
			}
		}
		HAL_Delay(25);

		GD_UpdateScreen(&Graphic);
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
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
	RCC_OscInitStruct.PLL.PLLN = 8;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DSP_A0_GPIO_Port, DSP_A0_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DSP_CS_GPIO_Port, DSP_CS_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DSP_RST_GPIO_Port, DSP_RST_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : DSP_A0_Pin */
	GPIO_InitStruct.Pin = DSP_A0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(DSP_A0_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : DSP_CS_Pin */
	GPIO_InitStruct.Pin = DSP_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(DSP_CS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : DSP_RST_Pin */
	GPIO_InitStruct.Pin = DSP_RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(DSP_RST_GPIO_Port, &GPIO_InitStruct);

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
#ifdef USE_FULL_ASSERT
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
