/*
 * st7525.c
 *
 *  Created on: Jul 17, 2024
 *      Author: pablo-jean
 */


/*
 * Include
 */

#include "st7525.h"

/*
 * Privates
 */

#define _PIN_HIGH			1
#define _PIN_LOW			0
#define _SUCCESS			0
#define _FAIL				1

#define _ST_ERROR_CHECK(x)		if (x != _SUCCESS) { return ST7525_FAIL; }

/* Function Pointers Handles */
static void _delay_ms(st7525_t *ssd1306, uint32_t ms){
	if (ssd1306->fxn.delayMs != NULL){
		ssd1306->fxn.delayMs(ms);
	}
}

static void _pin_reset(st7525_t *st7525, uint8_t Signal){
	if (st7525->fxn.spiReset != NULL){
		st7525->fxn.spiReset(Signal);
	}
}

static void _pin_a0(st7525_t *st7525, uint8_t Signal){
	st7525->fxn.spiA0(Signal);
}

static void _pin_cs(st7525_t *st7525, uint8_t Signal){
	st7525->fxn.spiCS(Signal);
}

static uint8_t _spi_write(st7525_t *st7525, uint8_t *data, uint32_t len){
	return st7525->fxn.spiWrite(data, len);
}

/* Communication and Control layer */

static uint8_t _write_data(st7525_t *st7525, uint8_t* data, uint32_t len){
	uint8_t e = _FAIL;

	if (st7525->Mode == ST7525_MODE_SPI){

	   _pin_cs(st7525, _PIN_LOW); // select OLED
	   _pin_a0(st7525, _PIN_HIGH);
	   e = _spi_write(st7525, data, len);
	   _pin_cs(st7525, _PIN_HIGH);

	}

	return e;
}

static uint8_t _write_command(st7525_t *st7525, uint8_t cmd){
	uint8_t e = _FAIL;

	if (st7525->Mode == ST7525_MODE_SPI){

	   _pin_cs(st7525, _PIN_LOW); // select OLED
	   _pin_a0(st7525, _PIN_LOW);
	   e = _spi_write(st7525, &cmd, 1);
	   _pin_cs(st7525, _PIN_HIGH);

	}

	return e;
}

void __set_address(st7525_t *st7525, uint8_t page, uint8_t columns){
	uint8_t lowNibbleCol, highNibbleCol;

	lowNibbleCol = (columns & 0xF);
	highNibbleCol = ((columns >> 4) & 0xF);
	_write_command(st7525, ST7525_CMD_SET_PAGE_ADRESS | page);
	_write_command(st7525, ST7525_CMD_SET_COLUMN_MSB  | highNibbleCol);
	_write_command(st7525, ST7525_CMD_SET_COLUMN_LSB  | lowNibbleCol);
}

static void _reset(st7525_t *st7525){
	if (st7525->Mode == ST7525_MODE_SPI){
	    // CS = High (not selected)
	    _pin_cs(st7525, _PIN_HIGH);

	    // Reset the Driver
	    _pin_reset(st7525, _PIN_LOW);
	    _delay_ms(st7525, 100);
	    _pin_reset(st7525, _PIN_HIGH);
	    _delay_ms(st7525, 100);
	}
}

static void _mtx_lock(st7525_t *st7525){
	if (st7525->fxn.mtxLock != NULL){
		st7525->fxn.mtxLock();
	}
}

static void _mtx_unlock(st7525_t *st7525){
	if (st7525->fxn.mtxUnlock != NULL){
		st7525->fxn.mtxUnlock();
	}
}

/*
 * Publics
 */

uint8_t ST7525_Init(st7525_t *st7525, st7525_params_t *params){
	assert(st7525 != NULL);
	assert(params != NULL);
	// if your program stuck here, and is the first call of Init
	// clear all ssd1306 handler variable, initializing all fields as 0
	assert(st7525->_intern.bInitialized == false);

	assert(params->Mode == ST7525_MODE_SPI);

	st7525->_intern.u32Heigth = params->u32Heigth;
	st7525->_intern.u32Width = params->u32Width;
	st7525->_intern.u32FrameSize = (params->u32Heigth * params->u32Width / 8);
	st7525->_intern.eFrameRate = params->eFrameRate;
	st7525->_intern.u32Pages = (st7525->_intern.u32Heigth / 8);
	st7525->_intern.bPending = true;

	st7525->fxn.delayMs = params->delayMs;
	st7525->fxn.mtxLock = params->mtxLock;
	st7525->fxn.mtxUnlock = params->mtxUnlock;
	st7525->fxn.spiCS = params->spiCS;
	st7525->fxn.spiA0 = params->spiA0;
	st7525->fxn.spiReset = params->spiReset;
	st7525->fxn.spiWrite = params->spiWrite;

	if (params->bUseExternalFrameBuffer == false){
		st7525->_intern.pu8FrameBuffer = (uint8_t*)malloc(st7525->_intern.u32FrameSize);
		assert(st7525->_intern.pu8FrameBuffer != NULL);
	}
	else{
		st7525->_intern.pu8FrameBuffer = NULL;
	}

	// Reset Display
	_reset(st7525);

	_mtx_lock(st7525);

	_write_command(st7525, 0xE2);
	_delay_ms(st7525, 200);
	// Set Frame rate for maximum
	_write_command(st7525, ST7525_CMD_SET_FRAME_RATE | st7525->_intern.eFrameRate);
	// Set Bias to 1/9
	_write_command(st7525, ST7525_CMD_SET_BIAS | 0x3);
	// Set scan direction to Normal
	_write_command(st7525, ST7525_CMD_SET_SCAN_DIR | 0x0);
	// Set Contrast to 0x79
	_write_command(st7525, ST7525_CMD_SET_CONTRAST);
	_write_command(st7525, 0x79);
	// Eanbles the Dispaly
	_write_command(st7525, ST7525_CMD_SET_DISPLAY_EN | 0x1);

	_mtx_unlock(st7525);

	st7525->_intern.bInitialized = true;

	return ST7525_OK;
}

uint8_t ST7525_SetFrameBuffer(st7525_t *st7525, uint8_t *pu8FrameBuffer){
	assert(st7525 != NULL);
	assert(st7525->_intern.bInitialized == true);
	assert(pu8FrameBuffer != NULL);

	_mtx_lock(st7525);
	st7525->_intern.pu8FrameBuffer = pu8FrameBuffer;
	_mtx_unlock(st7525);

	return ST7525_OK;
}

uint8_t ST7525_SetContrast(st7525_t *st7525, uint8_t level){
	assert(st7525 != NULL);
	assert(st7525->_intern.bInitialized == true);

	uint8_t contrastAdj = (level*100)/255;
	_write_command(st7525, ST7525_CMD_SET_CONTRAST);
	_write_command(st7525, contrastAdj);

	return ST7525_OK;
}

uint8_t ST7525_SetDisplayOn(st7525_t *st7525, bool on){
	assert(st7525 != NULL);
	assert(st7525->_intern.bInitialized == true);

	_write_command(st7525, ST7525_CMD_SET_DISPLAY_EN | on);

	return ST7525_OK;
}

uint8_t ST7525_Refresh(st7525_t *st7525){
	uint32_t width, pages;
	uint8_t *FrameBuffer;

	assert(st7525 != NULL);
	assert(st7525->_intern.bInitialized == true);
	assert(st7525->_intern.pu8FrameBuffer != NULL);

	if (st7525->_intern.bPending == false){
		return ST7525_OK;
	}

	FrameBuffer = st7525->_intern.pu8FrameBuffer;
	width = st7525->_intern.u32Width;
	pages = st7525->_intern.u32Pages;

	_mtx_lock(st7525);
	__set_address(st7525, 0, 0);
	_write_data(st7525, FrameBuffer, pages*width);
    st7525->_intern.bPending = false;
    _mtx_unlock(st7525);

    return ST7525_OK;
}

uint8_t ST7525_Write(st7525_t *st7525, uint32_t x, uint32_t y, bool color){
	uint8_t *FrameBuffer;
	uint32_t width;

	assert(st7525 != NULL);
	assert(st7525->_intern.bInitialized == true);
	assert(st7525->_intern.pu8FrameBuffer != NULL);

	if (x >= st7525->_intern.u32Width ||
			y >= st7525->_intern.u32Heigth){
		// Pixel is out of range
		return ST7525_FAIL;
	}

	FrameBuffer = st7525->_intern.pu8FrameBuffer;
	width = st7525->_intern.u32Width;

	_mtx_lock(st7525);
	if(color == true) {
		FrameBuffer[x + (y / 8) * width] |= 1 << (y % 8);
	} else {
		FrameBuffer[x + (y / 8) * width] &= ~(1 << (y % 8));
	}
	st7525->_intern.bPending = true;
	_mtx_unlock(st7525);

	return ST7525_OK;
}

uint8_t ST7525_Fill(st7525_t *st7525, uint8_t color){
	uint8_t *FrameBuffer;

	assert(st7525 != NULL);
	assert(st7525->_intern.bInitialized == true);
	assert(st7525->_intern.pu8FrameBuffer != NULL);

	FrameBuffer = st7525->_intern.pu8FrameBuffer;

	_mtx_lock(st7525);
	if (color == ST7525_COLOR_BLACK){
		memset(FrameBuffer, 0, st7525->_intern.u32FrameSize);
	}
	else{
		memset(FrameBuffer, 0xFF, st7525->_intern.u32FrameSize);
	}
	st7525->_intern.bPending = true;
	_mtx_unlock(st7525);

	return ST7525_OK;
}

uint8_t ST7525_SetFrameRate(st7525_t *st7525, st7525_frame_rate_e fr){
	assert(st7525 != NULL);
	assert(st7525->_intern.bInitialized == true);
	assert(fr <= ST7525_FR_168fps);

	_write_command(st7525, ST7525_CMD_SET_FRAME_RATE | fr);

	return ST7525_OK;
}

