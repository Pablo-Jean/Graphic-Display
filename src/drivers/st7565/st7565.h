/**
 * @file 		st7565.h
 * @author 		Pablo Jean Rozario (pablo.jean.eel@gmail.com)
 * @brief 		Driver for ST7565 display driver. This library needs you to implement
 * 				interface control, providing the function pointer to library handle the 
 * 				display.
 * @version 	v1.0.0
 * @date 		2024-07-02
 * 
 * @copyright 	Copyright (c) 2024
 * 
 */

#ifndef DRIVERS_ST7565_ST7565_H_
#define DRIVERS_ST7565_ST7565_H_

#include <assert.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "st7565_defs.h"

/*
 * Macros
 */

#define ST7565_OK				0
#define ST7565_FAIL				1

#define ST7565_COLOR_BLACK		0
#define ST7565_COLOR_WHITE		1

/*
 * Enumerates
 */

/**
 * @enum 	st7565_mode_e
 * @brief 	Set the Mode of ST7565 display. This device can work on 8bits, 4bits and spi mode,
 * 		  	but, here, only the SPI method is implemented, for while.
 * 
 */
typedef enum{
	ST7565_MODE_SPI /**< Initialize ST7565 in SPI mode and serial communication. */
}st7565_mode_e;

/*
 * Structs
 */

/**
 * @brief 		Function pointer definition for milliseconds delay.
 * @param ms	Amount of milliseconds to wait.
 * 
 */
typedef void	(*fxnDelay_ms)(uint32_t ms);

/**
 * @brief 		Function pointer to routine that waits for the mutex and lock it.
 * 				It's useful on RTOS environments.
 * 
 * @note		If driver calls are made over multiple threads, do not omit this. 
 */
typedef void 	(*fxn7565_mtxLock)(void);

/**
 * @brief 		Function pointer to routine that unlocks the mutex.
 * 				It's useful on RTOS environments.
 * 
 * @note		If driver calls are made over multiple threads, do not omit this. 
 */
typedef void 	(*fxn7565_mtxunlock)(void);

/**
 * @brief 		Function pointer to routine to control the Chip Select signal.
 * 				On ST7565, Chip Select is enabled on High signal.
 * 
 * @note 		Used only in SPI mode.
 */
typedef void	(*fxn7565spi_cs)(uint8_t Signal);

/**
 * @brief 		Function pointer to routine to control the Reset pin signal.
 * 
 */
typedef void	(*fxn7565spi_reset)(uint8_t Signal);

/**
 * @brief 		Function pointer to routine to control the RS/A0 pin signal.
 *
 */
typedef void	(*fxn7565spi_a0)(uint8_t Signal);

/**
 * @brief 		Function pointer to routine to transmit packets to ST7565 over SPI.
 * 
 * @note 		Used only in SPI mode.
 */
typedef uint8_t (*fxn7565spi_write)(uint8_t *buff, uint32_t len);

/**
 * @struct 	st7565_t
 * @brief Handler of ST7565. Each display must need your own handler.
 * Yes, this library supports multiples displays.
 *
 * @pre A good practice is to initialize this handler with all parameters as zero.
 */
typedef struct{
	st7565_mode_e Mode;
	struct{
		/**
		 * @brief 	Pointer to function to execute a milliseconds delay.
		 */
		fxnDelay_ms delayMs;

		/**
		 * @brief 	Pointer to function that waits and lock the mutex.
		 */
		fxn7565_mtxLock mtxLock;

		/**
		 * @brief 	Pointer to function that will unlock the mutex.
		 * 
		 */
		fxn7565_mtxunlock mtxUnlock;

		/**
		 * @brief 	Pointer to function that controls the Chip Select pin of ST7565.
		 * @note 	Mandatory when display is on ST7565_MODE_SPI.
		 * 
		 */
		fxn7565spi_cs spiCs;

		/**
		 * @brief 	Pointer to function that controls the A0/RS pin of ST7565.
		 * @note 	Mandatory.
		 *
		 */
		fxn7565spi_cs spiA0;

		/**
		 * @brief 	Pointer to function that controls the Reset pin of ST7565.
		 * @note 	Mandatory when display is on ST7565_MODE_SPI.
		 * 
		 */
		fxn7565spi_reset spiReset;

		/**
		 * @brief 	Pointer to function that write data over SPI.
		 * @note 	Mandatory when display is on ST7565_MODE_SPI.
		 * @pre		Configure the SPI Peripheral in Mode 1 (CPOL=0 and CPHA=1).
		 */
		fxn7565spi_write spiWrite;
	}fxn;
	struct{
		bool bInitialized;
		uint32_t u32FrameSize;
		uint8_t *pu8FrameBuffer;
		uint32_t u32Width;
		uint32_t u32Heigth;
	}_intern;
}st7565_t;

/**
 * @struct st7565_params_t
 * @brief  Struct to initialize and configure the driver to control the ST7565 peripheral.
 * 
 */
typedef struct{
	/**
	 * @brief 	Configures the Mode of the ST7565. This driver can work on 4bit, 8bit or SPI mode.
	 * @note	Only ST7565_MODE_SPI is implemented. Other values will assert the program.
	 * 
	 */
	st7565_mode_e Mode;

	/**
	 * @brief Configure the Width of the display.
	 * 
	 */
	uint32_t u32Width;

	/**
	 * @brief Configure the Height of the display.
	 * 
	 */
	uint32_t u32Heigth;

	/**
	 * @brief 	Sets to \c true to provide a external FrameBuffer array. The size of array must be calculated
	 * 		 	with the formula Width*Height/8. If \c false, the library will allocate the array in Initialization.
	 * @note	To provide the buffer, use @link ST7565_SetFrameBuffer @endlink routine to provide the array.
 	 * 
	 */
	bool bUseExternalFrameBuffer;

	//functions
	/**
	 * @brief 	Pointer to delay milliseconds routine.
	 * 
	 */
	fxnDelay_ms delayMs;

	// Mutex
	/**
	 * @brief 	Pointer to mutex lock. If not used, sets to \c NULL.
	 * 
	 */
	fxn7565_mtxLock mtxLock;

	/**
	 * @brief 	Pointer to mutex unlock. If not used, sets to \c NULL.
	 * 
	 */
	fxn7565_mtxunlock mtxUnlock;

	// only for spi
	/**
	 * @brief 	Pointer to Chip Select controller routine.
	 * @note 	Mandatory when display is on ST7565_MODE_SPI.
	 * 
	 */
	fxn7565spi_cs spiCs;

	/**
	 * @brief 	Pointer to function that controls the A0/RS pin of ST7565.
	 * @note 	Mandatory.
	 *
	 */
	fxn7565spi_cs spiA0;


	/**
	 * @brief 	Pointer to Reset controller routine.
	 * @note 	Mandatory when display is on ST7565_MODE_SPI.
	 * 
	 */
	fxn7565spi_reset spiReset;

	/**
	 * @brief 	Pointer to spi controller routine.
	 * @pre		Configure the SPI Peripheral in Mode 1 (CPOL=0 and CPHA=1).
	 * @note 	Mandatory when display is on ST7565_MODE_SPI.
	 * 
	 */
	fxn7565spi_write spiWrite;
}st7565_params_t;

/*
 * Publics
 */

/**
 * @brief 
 * 
 * @param st7565
 * @param params 
 * @return uint8_t 
 */
uint8_t ST7565_Init(st7565_t *st7565, st7565_params_t *params);

/**
 * @brief 
 * 
 * @param st7565
 * @param pu8FrameBuffer 
 * @return uint8_t 
 */
uint8_t ST7565_SetFrameBuffer(st7565_t *st7565, uint8_t *pu8FrameBuffer);

/**
 * @brief 
 * 
 * @param st7565
 * @return uint8_t 
 */
uint8_t ST7565_Refresh(st7565_t *st7565);

/**
 * @brief 
 * 
 * @param st7565
 * @param x 
 * @param y 
 * @param color 
 * @return uint8_t 
 */
uint8_t ST7565_Write(st7565_t *st7565, uint32_t x, uint32_t y, bool color);

/**
 * @brief 
 * 
 * @param st7565
 * @param color 
 * @return uint8_t 
 */
uint8_t ST7565_Fill(st7565_t *st7565, uint8_t color);

/**
 * @brief
 *
 * @param st7565
 * @param contrast
 * @return uint8_t
 */
uint8_t ST7565_SetContrast(st7565_t *st7565, uint8_t contrast);

#endif /* DRIVERS_ST7565_ST7565_H_ */
