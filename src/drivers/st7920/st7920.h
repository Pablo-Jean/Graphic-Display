/**
 * @file 		st7920.h
 * @author 		Pablo Jean Rozario (pablo.jean.eel@gmail.com)
 * @brief 		Driver for ST7920 display driver. This library needs you to implement
 * 				interface control, providing the function pointer to library handle the 
 * 				display.
 * @version 	v1.0.0
 * @date 		2024-07-02
 * 
 * @copyright 	Copyright (c) 2024
 * 
 */

#ifndef DRIVERS_ST7920_ST7920_H_
#define DRIVERS_ST7920_ST7920_H_

#include <assert.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "st7920_defs.h"

/*
 * Macros
 */

#define ST7920_OK				0
#define ST7920_FAIL				1

#define ST7920_COLOR_BLACK		0
#define ST7920_COLOR_WHITE		1

/*
 * Enumerates
 */

/**
 * @enum 	st7920_mode_e
 * @brief 	Set the Mode of ST7920 display. This device can work on 8bits, 4bits and spi mode,
 * 		  	but, here, only the SPI method is implemented, for while.
 * 
 */
typedef enum{
	ST7920_MODE_SPI /**< Initialize ST7920 in SPI mode and serial communication. */
}st7920_mode_e;

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
typedef void 	(*fxn7920_mtxLock)(void);

/**
 * @brief 		Function pointer to routine that unlocks the mutex.
 * 				It's useful on RTOS environments.
 * 
 * @note		If driver calls are made over multiple threads, do not omit this. 
 */
typedef void 	(*fxn7920_mtxunlock)(void);

/**
 * @brief 		Function pointer to routine to control the Chip Select signal.
 * 				On ST7920, Chip Select is enabled on High signal.
 * 
 * @note 		Used only in SPI mode.
 */
typedef void	(*fxn1306spi_cs)(uint8_t Signal);

/**
 * @brief 		Function pointer to routine to control the Reset pin signal.
 * 
 */
typedef void	(*fxn1306spi_reset)(uint8_t Signal);

/**
 * @brief 		Function pointer to routine to transmit packets to ST7920 over SPI.
 * 
 * @note 		Used only in SPI mode.
 */
typedef uint8_t (*fxn1306spi_write)(uint8_t *buff, uint32_t len);

/**
 * @struct 	st7920_t
 * @brief Handler of ST7920. Each display must need your own handler.
 * Yes, this library supports multiples displays.
 *
 * @pre A good practice is to initialize this handler with all parameters as zero.
 */
typedef struct{
	st7920_mode_e Mode;
	struct{
		// delay
		fxnDelay_ms delayMs;
		// Mutex
		fxn7920_mtxLock mtxLock;
		fxn7920_mtxunlock mtxUnlock;
		// only for spi
		fxn1306spi_cs spiCs;
		fxn1306spi_reset spiReset;
		fxn1306spi_write spiWrite;
	}fxn;
	struct{
		bool bInitialized;
		uint32_t u32FrameSize;
		uint8_t *pu8FrameBuffer;
		uint32_t u32Width;
		uint32_t u32Heigth;
	}_intern;
}st7920_t;

typedef struct{
	st7920_mode_e Mode;
	uint32_t u32Width;
	uint32_t u32Heigth;
	bool bUseExternalFrameBuffer;

	//functions
	// delay
	fxnDelay_ms delayMs;
	// Mutex
	fxn7920_mtxLock mtxLock;
	fxn7920_mtxunlock mtxUnlock;
	// only for spi
	fxn1306spi_cs spiCs;
	fxn1306spi_reset spiReset;
	fxn1306spi_write spiWrite;
}st7920_params_t;

/*
 * Publics
 */

uint8_t ST7920_Init(st7920_t *st7920, st7920_params_t *params);

uint8_t ST7920_SetFrameBuffer(st7920_t *st7920, uint8_t *pu8FrameBuffer);

uint8_t ST7920_Refresh(st7920_t *st7920);

uint8_t ST7920_Write(st7920_t *st7920, uint32_t x, uint32_t y, bool color);

uint8_t ST7920_Fill(st7920_t *st7920, uint8_t color);

#endif /* DRIVERS_ST7920_ST7920_H_ */
