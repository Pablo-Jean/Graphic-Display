/*
 * st7565.c
 *
 *  Created on: Jun 30, 2024
 *      Author: pablo-jean
 *
 *  Based on file created by poe in 07-Jun-19 and edited by Liyanboy74
 */

#include "st7565.h"

/*
 * Privates
 */

#define _PIN_HIGH			1
#define _PIN_LOW			0
#define _SUCCESS			0

#define _BLACK				1
#define _WHITE				0

#define _DISPLAY_OFF   0xAE
#define _DISPLAY_ON    0xAF

#define _SET_DISP_START_LINE  0x40
#define _SET_PAGE  0xB0

#define _SET_COLUMN_UPPER  0x10
#define _SET_COLUMN_LOWER  0x00

#define _SET_ADC_NORMAL  0xA0
#define _SET_ADC_REVERSE 0xA1

#define _SET_DISP_NORMAL 0xA6
#define _SET_DISP_REVERSE 0xA7

#define _SET_ALLPTS_NORMAL 0xA4
#define _SET_ALLPTS_ON  0xA5
#define _SET_BIAS_9 0xA2
#define _SET_BIAS_7 0xA3

#define _RMW  0xE0
#define _RMW_CLEAR 0xEE
#define _INTERNAL_RESET  0xE2
#define _SET_COM_NORMAL  0xC0
#define _SET_COM_REVERSE  0xC8
#define _SET_POWER_CONTROL  0x28
#define _SET_RESISTOR_RATIO  0x20
#define _SET_VOLUME_FIRST  0x81
#define _SET_VOLUME_SECOND  0
#define _SET_STATIC_OFF  0xAC
#define _SET_STATIC_ON  0xAD
#define _SET_STATIC_REG  0x0
#define _SET_BOOSTER_FIRST  0xF8
#define _SET_BOOSTER_234  0
#define _SET_BOOSTER_5  1
#define _SET_BOOSTER_6  3
#define _NOP  0xE3
#define _TEST  0xF0

#define _ST7565_ERROR_CHECK(x)		if (x != _SUCCESS) { return ST7565_FAIL; }

const uint8_t _pagemap[] = { 7, 6, 5, 4, 3, 2, 1, 0 };

/* Function Pointers Handles */
static void _delay_ms(st7565_t *st7565, uint32_t ms){
	if (st7565->fxn.delayMs != NULL){
		st7565->fxn.delayMs(ms);
	}
}

static void _pin_reset(st7565_t *st7565, uint8_t Signal){
	if (st7565->fxn.spiReset != NULL){
		st7565->fxn.spiReset(Signal);
	}
}

static void _pin_cs(st7565_t *st7565, uint8_t Signal){
	st7565->fxn.spiCs(Signal);
}

static void _pin_a0(st7565_t *st7565, uint8_t Signal){
	st7565->fxn.spiA0(Signal);
}

static uint8_t _spi_write(st7565_t *st7565, uint8_t *data, uint32_t len){
	return st7565->fxn.spiWrite(data, len);
}

/* Communication and Control layer */

static uint8_t _write_data(st7565_t *st7565, uint8_t* buff, uint32_t len){
	if (st7565->Mode == ST7565_MODE_SPI){
		_pin_a0(st7565, _PIN_HIGH);
		_pin_cs(st7565, _PIN_LOW);

		_spi_write(st7565, buff, len);

		_pin_cs(st7565, _PIN_HIGH);
	}
	else{
		assert(0);
	}

	return _SUCCESS;
}

static uint8_t _write_command(st7565_t *st7565, uint8_t cmd){
	if (st7565->Mode == ST7565_MODE_SPI){
		_pin_a0(st7565, _PIN_LOW);
		_pin_cs(st7565, _PIN_LOW);

		_spi_write(st7565, &cmd, sizeof(cmd));

		_pin_cs(st7565, _PIN_HIGH);
	}
	else{
		assert(0);
	}

	return _SUCCESS;
}

static void _reset(st7565_t *st7565){
	_pin_cs(st7565, _PIN_HIGH);

	// Reset the Display
	_pin_reset(st7565, _PIN_LOW);
	_delay_ms(st7565, 100);		// wait for 100ms
	_pin_reset(st7565, _PIN_HIGH);
	_delay_ms(st7565, 50);		//wait for >40 ms
}

static void _mtx_lock(st7565_t *st7565){
	if (st7565->fxn.mtxLock != NULL){
		st7565->fxn.mtxLock();
	}
}

static void _mtx_unlock(st7565_t *st7565){
	if (st7565->fxn.mtxUnlock != NULL){
		st7565->fxn.mtxUnlock();
	}
}

/*
 * Publics
 */

uint8_t ST7565_Init(st7565_t *st7565, st7565_params_t *params){
	assert(st7565 != NULL);
	assert(params != NULL);
	// if your program stuck here, and is the first call of Init
	// clear all st7565 handler variable, initializing all fields as 0
	assert(st7565->_intern.bInitialized == false);

	assert(params->Mode == ST7565_MODE_SPI);
	assert(params->spiA0 != NULL);

	st7565->_intern.u32Heigth = params->u32Heigth;
	st7565->_intern.u32Width = params->u32Width;
	st7565->_intern.u32FrameSize = (params->u32Heigth * params->u32Width / 8);

	st7565->fxn.delayMs = params->delayMs;
	st7565->fxn.mtxLock = params->mtxLock;
	st7565->fxn.mtxUnlock = params->mtxUnlock;
	st7565->fxn.spiCs = params->spiCs;
	st7565->fxn.spiA0 = params->spiA0;
	st7565->fxn.spiReset = params->spiReset;
	st7565->fxn.spiWrite = params->spiWrite;

	if (params->bUseExternalFrameBuffer == false){
		st7565->_intern.pu8FrameBuffer = (uint8_t*)malloc(st7565->_intern.u32FrameSize);
		assert(st7565->_intern.pu8FrameBuffer != NULL);
	}
	else{
		st7565->_intern.pu8FrameBuffer = NULL;
	}

	_mtx_lock(st7565);

	// Reset the Display driver
	_reset(st7565);

	// LCD Bias Select
	_write_command(st7565, _SET_BIAS_7);
	// ADC select to normal
	_write_command(st7565, _SET_ADC_NORMAL);
	// Set SHL to normal
	_write_command(st7565, _SET_COM_NORMAL);
	// Set initial display line
	_write_command(st7565, _SET_DISP_START_LINE);

	// turn on voltage converter (VC=1 VR=0 VF=0)
	_write_command(st7565, _SET_POWER_CONTROL | 0x4);
	// wait 50ms
	_delay_ms(st7565, 50);

	// turn on voltage regulator (VC=1 VR=1 VF=0)
	_write_command(st7565, _SET_POWER_CONTROL | 0x6);
	// wait 50ms
	_delay_ms(st7565, 50);

	// turn on voltage follower (VC=1 VR=1 VF=1)
	_write_command(st7565, _SET_POWER_CONTROL | 0x7);
	// wait 10ms
	_delay_ms(st7565, 10);

	// Set LCD operating voltage (reulator resistor)
	_write_command(st7565, _SET_RESISTOR_RATIO | 0x6);

	// Turn on the display
	_write_command(st7565, _DISPLAY_ON);

	// Turn on all Points
	_write_command(st7565, _SET_ALLPTS_NORMAL);

	_mtx_unlock(st7565);


	st7565->_intern.bInitialized = true;
	ST7565_SetContrast(st7565, 50);

	return ST7565_OK;
}

uint8_t ST7565_SetFrameBuffer(st7565_t *st7565, uint8_t *pu8FrameBuffer){
	assert(st7565 != NULL);
	assert(st7565->_intern.bInitialized == true);
	assert(pu8FrameBuffer != NULL);

	_mtx_lock(st7565);
	st7565->_intern.pu8FrameBuffer = pu8FrameBuffer;
	_mtx_unlock(st7565);

	return ST7565_OK;
}

uint8_t ST7565_Refresh(st7565_t *st7565){
	uint8_t x, p;
	uint8_t Temp;
	uint32_t pages, width;
	uint8_t *FrameBuffer;

	assert(st7565 != NULL);
	assert(st7565->_intern.bInitialized == true);
	assert(st7565->_intern.pu8FrameBuffer != NULL);

	FrameBuffer = st7565->_intern.pu8FrameBuffer;
	pages = 8;
	width = st7565->_intern.u32Width;
	_mtx_lock(st7565);
	for(p=0 ; p<pages ; p++)
	{
		// Set the page
		_write_command(st7565, _SET_PAGE | _pagemap[p]);

		// Set the column to refresh
		_write_command(st7565, _SET_COLUMN_LOWER | (0 & 0xF));
		_write_command(st7565, _SET_COLUMN_UPPER | ((0 >> 4) & 0xF));
		_write_command(st7565, _RMW);

		_write_data(st7565, &FrameBuffer[128*p], width);
	}
	_mtx_unlock(st7565);

	return ST7565_OK;
}

uint8_t ST7565_Write(st7565_t *st7565, uint32_t x, uint32_t y, bool color){
	uint8_t *FrameBuffer;
	uint32_t index;

	assert(st7565 != NULL);
	assert(st7565->_intern.bInitialized == true);
	assert(st7565->_intern.pu8FrameBuffer != NULL);

	if (x >= st7565->_intern.u32Width ||
			y >= st7565->_intern.u32Heigth){
		// Pixel is out of range
		return ST7565_FAIL;
	}

	FrameBuffer = st7565->_intern.pu8FrameBuffer;

	index = x + (y/8)*128;
	if (color == true){
		FrameBuffer[index] |= (1<<(7-(y%8)));
	}
	else{
		FrameBuffer[index] &= ~(1<<(7-(y%8)));
	}

	return ST7565_OK;
}

uint8_t ST7565_Fill(st7565_t *st7565, uint8_t color){
	uint8_t *FrameBuffer;

	assert(st7565 != NULL);
	assert(st7565->_intern.bInitialized == true);
	assert(st7565->_intern.pu8FrameBuffer != NULL);

	FrameBuffer = st7565->_intern.pu8FrameBuffer;
	if (color == ST7565_COLOR_BLACK){
		memset(FrameBuffer, 0, st7565->_intern.u32FrameSize);
	}
	else{
		memset(FrameBuffer, 0xFF, st7565->_intern.u32FrameSize);
	}

	return ST7565_OK;
}

uint8_t ST7565_SetContrast(st7565_t *st7565, uint8_t contrast){
	assert(st7565 != NULL);
	assert(st7565->_intern.bInitialized == true);

	if (contrast > 100){
		contrast = 100;
	}

	contrast = contrast * 0x3F / 100;

	_mtx_lock(st7565);

	_write_command(st7565, _SET_VOLUME_FIRST);
	_write_command(st7565, _SET_VOLUME_SECOND | (contrast & 0x3F));

	_mtx_unlock(st7565);

	return ST7565_OK;
}

