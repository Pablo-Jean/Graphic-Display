/*
 * ssd1306.h
 *
 *  Created on: Jun 29, 2024
 *      Author: pablo-jean
 */

#ifndef DRIVERS_SSD1306_SSD1306_H_
#define DRIVERS_SSD1306_SSD1306_H_

/*
 * Includes
 */

#include <assert.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "ssd1306_defs.h"

/*
 * Macros
 */

#define SSD1306_OK				0
#define SSD1306_FAIL			1

/*
 * Enumerates
 */

typedef enum{
	SSD1306_MODE_I2C,
	SSD1306_MODE_SPI
}ssd1306_mode_e;

/*
 * Structs
 */

typedef void	(*fxnDelay_ms)(uint32_t ms);
typedef void 	(*fxn1306_mtxLock)(void);
typedef void 	(*fxn1306_mtxunlock)(void);

typedef uint8_t (*fxn1306i2c_write)(uint8_t i2cAddr, uint8_t reg, uint8_t *buff, uint32_t len);

typedef void	(*fxn1306spi_cs)(uint8_t Signal);
typedef void	(*fxn1306spi_dc)(uint8_t Signal);
typedef void	(*fxn1306spi_reset)(uint8_t Signal);
typedef uint8_t (*fxn1306spi_write)(uint8_t *buff, uint32_t len);

typedef struct{
	ssd1306_mode_e Mode;
	struct{
		// delay
		fxnDelay_ms delayMs;
		// Mutex
		fxn1306_mtxLock mtxLock;
		fxn1306_mtxunlock mtxUnlock;
		// Only for i2c
		fxn1306i2c_write i2cWrite;
		// only for spi
		fxn1306spi_cs spiCs;
		fxn1306spi_dc spiDc;
		fxn1306spi_reset spiReset;
		fxn1306spi_write spiWrite;
	}fxn;
	struct{
		bool bInitialized;
		uint32_t u32FrameSize;
		uint8_t *pu8FrameBuffer;
		uint32_t u32Width;
		uint32_t u32Heigth;
		uint32_t u32Offset;
		uint8_t u8I2CAddr;
	}_intern;
}ssd1306_t;

typedef struct{
	ssd1306_mode_e Mode;
	uint32_t u32Offset;
	uint32_t u32Width;
	uint32_t u32Heigth;
	uint8_t u8I2CAddr;
	bool bUseExternalFrameBuffer;
	bool bMirrorVert;
	bool bMirrorHorz;
	bool bInverseColor;

	//functions
	// delay
	fxnDelay_ms delayMs;
	// Mutex
	fxn1306_mtxLock mtxLock;
	fxn1306_mtxunlock mtxUnlock;
	// Only for i2c
	fxn1306i2c_write i2cWrite;
	// only for spi
	fxn1306spi_cs spiCs;
	fxn1306spi_dc spiDc;
	fxn1306spi_reset spiReset;
	fxn1306spi_write spiWrite;
}ssd1306_params_t;

typedef struct{
	uint8_t X;
	uint8_t Y;
	bool color;
}ssd1306_write_info_t;

/*
 * Publics
 */

uint8_t SSD1306_Init(ssd1306_t *ssd1306, ssd1306_params_t *params);

uint8_t SSD1306_SetFrameBuffer(ssd1306_t *ssd1306, uint8_t *pu8FrameBuffer);

uint8_t SSD1306_SetContrast(ssd1306_t *ssd1306, uint8_t level);

uint8_t SSD1306_SetDisplayOn(ssd1306_t *ssd1306, bool on);

uint8_t SSD1306_Refresh(ssd1306_t *ssd1306);

uint8_t SSD1306_Write(ssd1306_t *ssd1306, ssd1306_write_info_t *WriteInfo);

uint8_t SSD1306_Clear(ssd1306_t *ssd1306);

#endif /* DRIVERS_SSD1306_SSD1306_H_ */
