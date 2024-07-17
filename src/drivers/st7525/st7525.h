/*
 * st7525.h
 *
 *  Created on: Jul 17, 2024
 *      Author: pablo-jean
 */

#ifndef GRAPHICDISPLAY_SRC_DRIVERS_ST7525_ST7525_H_
#define GRAPHICDISPLAY_SRC_DRIVERS_ST7525_ST7525_H_


/*
 * Includes
 */

#include <assert.h>
#include <drivers/st7525/st7525_defs.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>


/*
 * Macros
 */

#define ST7525_OK				0
#define ST7525_FAIL				1

#define ST7525_COLOR_BLACK		0
#define ST7525_COLOR_WHITE		1

/*
 * Enumerates
 */

// TODO : Implement parallel 8bit and I2C mode
typedef enum{
	ST7525_MODE_SPI
}st7525_mode_e;

typedef enum{
	ST7525_FR_76fps		= 0x0,
	ST7525_FR_95fps		= 0x1,
	ST7525_FR_132fps	= 0x2,
	ST7525_FR_168fps	= 0x3,
}st7525_frame_rate_e;

/*
 * Structs
 */

typedef void	(*fxnDelay_ms)(uint32_t ms);
typedef void 	(*st7525_mtxLock)(void);
typedef void 	(*st7525_mtxunlock)(void);

typedef void	(*fxn7525spi_cs)(uint8_t Signal);
typedef void	(*fxn7525spi_a0)(uint8_t Signal);
typedef void	(*fxn7525spi_reset)(uint8_t Signal);
typedef uint8_t (*fxn7525spi_write)(uint8_t *buff, uint32_t len);

typedef struct{
	st7525_mode_e Mode;
	struct{
		// delay
		fxnDelay_ms delayMs;
		// Mutex
		st7525_mtxLock mtxLock;
		st7525_mtxunlock mtxUnlock;
		// only for spi
		fxn7525spi_a0 spiA0;
		fxn7525spi_cs spiCS;
		fxn7525spi_reset spiReset;
		fxn7525spi_write spiWrite;
	}fxn;
	struct{
		bool bInitialized;
		bool bPending;
		st7525_frame_rate_e eFrameRate;
		uint32_t u32FrameSize;
		uint8_t *pu8FrameBuffer;
		uint32_t u32Width;
		uint32_t u32Heigth;
		uint32_t u32Pages;
	}_intern;
}st7525_t;

typedef struct{
	st7525_mode_e Mode;
	uint32_t u32Width;
	uint32_t u32Heigth;
	st7525_frame_rate_e eFrameRate;
	bool bUseExternalFrameBuffer;

	//functions
	// delay
	fxnDelay_ms delayMs;
	// Mutex
	st7525_mtxLock mtxLock;
	st7525_mtxunlock mtxUnlock;
	// only for spi
	fxn7525spi_cs spiCS;
	fxn7525spi_a0 spiA0;
	fxn7525spi_reset spiReset;
	fxn7525spi_write spiWrite;
}st7525_params_t;

/*
 * Publics
 */

uint8_t ST7525_Init(st7525_t *st7525, st7525_params_t *params);

uint8_t ST7525_SetFrameBuffer(st7525_t *st7525, uint8_t *pu8FrameBuffer);

uint8_t ST7525_SetContrast(st7525_t *st7525, uint8_t level);

uint8_t ST7525_SetDisplayOn(st7525_t *st7525, bool on);

uint8_t ST7525_SetFrameRate(st7525_t *st7525, st7525_frame_rate_e fr);

uint8_t ST7525_Refresh(st7525_t *st7525);

uint8_t ST7525_Write(st7525_t *st7525, uint32_t x, uint32_t y, bool color);

uint8_t ST7525_Fill(st7525_t *st7525, uint8_t color);

#endif /* GRAPHICDISPLAY_SRC_DRIVERS_ST7525_ST7525_H_ */
