/*
 * st7920.h
 *
 *  Created on: Jun 30, 2024
 *      Author: pablo-jean
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
#define ST7920_FAIL			1

#define ST7920_COLOR_BLACK		0
#define ST7920_COLOR_WHITE		1

/*
 * Enumerates
 */

typedef enum{
	ST7920_MODE_SPI
}st7920_mode_e;

/*
 * Structs
 */

typedef void	(*fxnDelay_ms)(uint32_t ms);
typedef void 	(*fxn7920_mtxLock)(void);
typedef void 	(*fxn7920_mtxunlock)(void);

typedef void	(*fxn1306spi_cs)(uint8_t Signal);
typedef void	(*fxn1306spi_reset)(uint8_t Signal);
typedef uint8_t (*fxn1306spi_write)(uint8_t *buff, uint32_t len);

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
