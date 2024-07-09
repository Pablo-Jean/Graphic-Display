/*
 * st7920.c
 *
 *  Created on: Jun 30, 2024
 *      Author: pablo-jean
 *
 *  Based on file created by poe in 07-Jun-19 and edited by Liyanboy74
 */

#include "st7920.h"

/*
 * Privates
 */

#define _PIN_HIGH		1
#define _PIN_LOW		0
#define _SUCCESS			0

#define _ST7920_ERROR_CHECK(x)		if (x != _SUCCESS) { return ST7920_FAIL; }

/* Function Pointers Handles */
static void _delay_ms(st7920_t *st7920, uint32_t ms){
	if (st7920->fxn.delayMs != NULL){
		st7920->fxn.delayMs(ms);
	}
}

static void _pin_reset(st7920_t *st7920, uint8_t Signal){
	if (st7920->fxn.spiReset != NULL){
		st7920->fxn.spiReset(Signal);
	}
}

static void _pin_cs(st7920_t *st7920, uint8_t Signal){
	st7920->fxn.spiCs(Signal);
}

static uint8_t _spi_write(st7920_t *st7920, uint8_t *data, uint32_t len){
	return st7920->fxn.spiWrite(data, len);
}

/* Communication and Control layer */

static uint8_t _write_data(st7920_t *st7920, uint8_t* buff, uint32_t len){
	uint32_t i;
	uint8_t data;

	if (st7920->Mode == ST7920_MODE_SPI){
		for (i=0 ; i<len ; i++){
			_pin_cs(st7920, _PIN_HIGH);

			data = (0xF8 | (1<<1)); // send the SYNC + RS(1)
			_spi_write(st7920, &data, sizeof(data));

			data = (buff[i] & 0xF0); // send the higher nibble first
			_spi_write(st7920, &data, sizeof(data));

			data = ((buff[i]<<4) & 0xF0); // send the lower nibble
			_spi_write(st7920, &data, sizeof(data));

			_pin_cs(st7920, _PIN_LOW);
			for (int j=0 ; j<400 ; j++); // just a delay, TODO need to fix
		}
	}
	else{
		assert(0);
	}

	return _SUCCESS;
}

static uint8_t _write_command(st7920_t *st7920, uint8_t cmd){
	uint8_t data;

	if (st7920->Mode == ST7920_MODE_SPI){
		_pin_cs(st7920, _PIN_HIGH);

		data = (0xF8 | (0<<1)); // send the SYNC + RS(0)
		_spi_write(st7920, &data, sizeof(data));

		data = (cmd & 0xF0); // send the higher nibble first
		_spi_write(st7920, &data, sizeof(data));

		data = ((cmd<<4) & 0xF0); // send the lower nibble
		_spi_write(st7920, &data, sizeof(data));

		_pin_cs(st7920, _PIN_LOW);
		for (int j=0 ; j<400 ; j++); // just a delay, TODO need to fix
	}
	else{
		assert(0);
	}

	return _SUCCESS;
}

static void _reset(st7920_t *st7920){
	// CS = High (not selected)
	_pin_cs(st7920, _PIN_LOW);

	// Reset the DSIPLAY
	_pin_reset(st7920, _PIN_LOW);
	_delay_ms(st7920, 10);		// wait for 10ms
	_pin_reset(st7920, _PIN_HIGH);
	_delay_ms(st7920, 50);		//wait for >40 ms
}

static void _mtx_lock(st7920_t *st7920){
	if (st7920->fxn.mtxLock != NULL){
		st7920->fxn.mtxLock();
	}
}

static void _mtx_unlock(st7920_t *st7920){
	if (st7920->fxn.mtxUnlock != NULL){
		st7920->fxn.mtxUnlock();
	}
}

/*
 * Publics
 */

uint8_t ST7920_Init(st7920_t *st7920, st7920_params_t *params){
	assert(st7920 != NULL);
	assert(params != NULL);
	// if your program stuck here, and is the first call of Init
	// clear all st7920 handler variable, initializing all fields as 0
	assert(st7920->_intern.bInitialized == false);

	assert(params->Mode == ST7920_MODE_SPI);

	st7920->_intern.u32Heigth = params->u32Heigth;
	st7920->_intern.u32Width = params->u32Width;
	st7920->_intern.u32FrameSize = (params->u32Heigth * params->u32Width / 8);

	st7920->fxn.delayMs = params->delayMs;
	st7920->fxn.mtxLock = params->mtxLock;
	st7920->fxn.mtxUnlock = params->mtxUnlock;
	st7920->fxn.spiCs = params->spiCs;
	st7920->fxn.spiReset = params->spiReset;
	st7920->fxn.spiWrite = params->spiWrite;

	if (params->bUseExternalFrameBuffer == false){
		st7920->_intern.pu8FrameBuffer = (uint8_t*)malloc(st7920->_intern.u32FrameSize);
		assert(st7920->_intern.pu8FrameBuffer != NULL);
	}
	else{
		st7920->_intern.pu8FrameBuffer = NULL;
	}

	_mtx_lock(st7920);

	// Reset OLED
	_reset(st7920);

	_write_command(st7920, 0x30);  // 8bit mode
	_delay_ms(st7920, 1);  //  >100us delay

	_write_command(st7920, 0x30);  // 8bit mode
	_delay_ms(st7920, 1);  // >37us delay

	_write_command(st7920, 0x08);  // D=0, C=0, B=0
	_delay_ms(st7920, 1);  // >100us delay

	_write_command(st7920, 0x01);  // clear screen
	_delay_ms(st7920, 15);  // >10 ms delay


	_write_command(st7920, 0x06);  // cursor increment right no shift
	_delay_ms(st7920, 1);  // 1ms delay

	_write_command(st7920, 0x0C);  // D=1, C=0, B=0
	_delay_ms(st7920, 1);  // 1ms delay

	_write_command(st7920, 0x02);  // return to home
	_delay_ms(st7920, 1);  // 1ms delay

	// Enable Graphic Mode
	_write_command(st7920, 0x30);  // 8 bit mode
	_delay_ms(st7920, 1);  // 1ms delay
	_write_command(st7920, 0x34);  // switch to Extended instructions
	_delay_ms(st7920, 1);  // 1ms delay
	_write_command(st7920, 0x36);  // enable graphics
	_delay_ms(st7920, 1);  // 1ms delay

	_mtx_unlock(st7920);

	st7920->_intern.bInitialized = true;

	return ST7920_OK;
}

uint8_t ST7920_SetFrameBuffer(st7920_t *st7920, uint8_t *pu8FrameBuffer){
	assert(st7920 != NULL);
	assert(st7920->_intern.bInitialized == true);
	assert(pu8FrameBuffer != NULL);

	_mtx_lock(st7920);
	st7920->_intern.pu8FrameBuffer = pu8FrameBuffer;
	_mtx_unlock(st7920);

	return ST7920_OK;
}

uint8_t ST7920_Refresh(st7920_t *st7920){
	uint8_t x, y;
	uint16_t Index=0;
	uint8_t Temp[2],Db;
	uint8_t *FrameBuffer;
	uint32_t height, width;

	assert(st7920 != NULL);
	assert(st7920->_intern.bInitialized == true);
	assert(st7920->_intern.pu8FrameBuffer != NULL);

	FrameBuffer = st7920->_intern.pu8FrameBuffer;
	height = st7920->_intern.u32Heigth;
	width = st7920->_intern.u32Width;
	for(y=0;y<height;y++)
	{
		if(y<32)//Up
		{
			_write_command(st7920, 0x80 | y);										//Y(0-31)
			_write_command(st7920, 0x80 | 0);										//X(0-8)
		}
		else
		{
			_write_command(st7920, 0x80 | (y-32));//Y(0-31)
			_write_command(st7920, 0x88 | 0);//X(0-8)
		}
		for(x=0 ; x<(width/16) ; x++)
		{

			Index=(y*width/8) + (x*2);

			Temp[0] = FrameBuffer[Index];
			Temp[1] = FrameBuffer[Index + 1];
			_write_data(st7920, Temp, 2);
		}
	}

	return ST7920_OK;
}

uint8_t ST7920_Write(st7920_t *st7920, uint32_t x, uint32_t y, bool color){
	uint8_t *FrameBuffer;
	uint32_t width, index;

	assert(st7920 != NULL);
	assert(st7920->_intern.bInitialized == true);
	assert(st7920->_intern.pu8FrameBuffer != NULL);

	if (x >= st7920->_intern.u32Width ||
			y >= st7920->_intern.u32Heigth){
		// Pixel is out of range
		return ST7920_FAIL;
	}

	FrameBuffer = st7920->_intern.pu8FrameBuffer;
	width = st7920->_intern.u32Width;

	index = (x/8) + (y*width/8);
	if (color == true){
		FrameBuffer[index] |= (1 << (7-(x%8)));
	}
	else{
		FrameBuffer[index] &= ~(1 << (7-(x%8)));
	}

	return ST7920_OK;
}

uint8_t ST7920_Fill(st7920_t *st7920, uint8_t color){
	uint8_t *FrameBuffer;

	assert(st7920 != NULL);
	assert(st7920->_intern.bInitialized == true);
	assert(st7920->_intern.pu8FrameBuffer != NULL);

	FrameBuffer = st7920->_intern.pu8FrameBuffer;
	if (color == ST7920_COLOR_BLACK){
		memset(FrameBuffer, 0, st7920->_intern.u32FrameSize);
	}
	else{
		memset(FrameBuffer, 0xFF, st7920->_intern.u32FrameSize);
	}

	return ST7920_OK;
}

