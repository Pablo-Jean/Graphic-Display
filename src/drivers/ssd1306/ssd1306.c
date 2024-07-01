/*
 * ssd1306.c
 *
 *  Created on: Jun 29, 2024
 *      Author: pablo-jean
 */

#include "ssd1306.h"

/*
 * Privates
 */

#define _PIN_HIGH		1
#define _PIN_LOW		0
#define _SUCCESS			0

#define _SSD_ERROR_CHECK(x)		if (x != _SUCCESS) { return SSD1306_FAIL; }

/* Function Pointers Handles */
static void _delay_ms(ssd1306_t *ssd1306, uint32_t ms){
	if (ssd1306->fxn.delayMs != NULL){
		ssd1306->fxn.delayMs(ms);
	}
}

static void _pin_reset(ssd1306_t *ssd1306, uint8_t Signal){
	if (ssd1306->fxn.spiReset != NULL){
		ssd1306->fxn.spiReset(Signal);
	}
}

static void _pin_dc(ssd1306_t *ssd1306, uint8_t Signal){
	ssd1306->fxn.spiDc(Signal);
}

static void _pin_cs(ssd1306_t *ssd1306, uint8_t Signal){
	ssd1306->fxn.spiCs(Signal);
}

static uint8_t _spi_write(ssd1306_t *ssd1306, uint8_t *data, uint32_t len){
	return ssd1306->fxn.spiWrite(data, len);
}

static uint8_t _i2c_write(ssd1306_t *ssd1306, uint8_t reg, uint8_t* data, uint32_t len){
	return ssd1306->fxn.i2cWrite(ssd1306->_intern.u8I2CAddr, reg, data, len);
}

/* Communication and Control layer */

static uint8_t _write_data(ssd1306_t *ssd1306, uint8_t* data, uint32_t len){
	if (ssd1306->Mode == SSD1306_MODE_I2C){
		return _i2c_write(ssd1306, SSD1306_REG_DATA, data, len);
	}
	else{
		uint8_t e;

	   _pin_cs(ssd1306, _PIN_LOW); // select OLED
	   _pin_dc(ssd1306, _PIN_HIGH);
	   e = _spi_write(ssd1306, data, len);
	   _pin_cs(ssd1306, _PIN_HIGH);

	   return e;
	}
}

static uint8_t _write_command(ssd1306_t *ssd1306, uint8_t cmd){
	if (ssd1306->Mode == SSD1306_MODE_I2C){
		return _i2c_write(ssd1306, SSD1306_REG_COMMAND, &cmd, sizeof(cmd));
	}
	else{
		uint8_t e;

	   _pin_cs(ssd1306, _PIN_LOW); // select OLED
	   _pin_dc(ssd1306, _PIN_LOW);
	   e = _spi_write(ssd1306, &cmd, 1);
	   _pin_cs(ssd1306, _PIN_HIGH);

	   return e;
	}
}

static void _reset(ssd1306_t *ssd1306){
	if (ssd1306->Mode == SSD1306_MODE_SPI){
	    // CS = High (not selected)
	    _pin_cs(ssd1306, _PIN_HIGH);

	    // Reset the OLED
	    _pin_reset(ssd1306, _PIN_LOW);
	    _delay_ms(ssd1306, 10);
	    _pin_reset(ssd1306, _PIN_HIGH);
	    _delay_ms(ssd1306, 10);
	}
}

static void _mtx_lock(ssd1306_t *ssd1306){
	if (ssd1306->fxn.mtxLock != NULL){
		ssd1306->fxn.mtxLock();
	}
}

static void _mtx_unlock(ssd1306_t *ssd1306){
	if (ssd1306->fxn.mtxUnlock != NULL){
		ssd1306->fxn.mtxUnlock();
	}
}

/*
 * Publics
 */

uint8_t SSD1306_Init(ssd1306_t *ssd1306, ssd1306_params_t *params){
	assert(ssd1306 != NULL);
	assert(params != NULL);
	// if your program stuck here, and is the first call of Init
	// clear all ssd1306 handler variable, initializing all fields as 0
	assert(ssd1306->_intern.bInitialized == false);

	assert(params->u32Heigth == 32 || params->u32Heigth == 64 || params->u32Heigth == 128);
	assert(params->u32Width == 32 || params->u32Width == 64 || params->u32Width == 128);
	// check if i2c address isn`t Zero or grater than 0x7F (using more than 7 bits)
	assert(params->u8I2CAddr > 0x00 && params->u8I2CAddr < 0x80);
	assert(params->Mode == SSD1306_MODE_I2C || params->Mode == SSD1306_MODE_SPI);

	ssd1306->_intern.u32Heigth = params->u32Heigth;
	ssd1306->_intern.u32Width = params->u32Width;
	ssd1306->_intern.u8I2CAddr = params->u8I2CAddr;
	ssd1306->_intern.u32FrameSize = (params->u32Heigth * params->u32Width / 8);

	ssd1306->fxn.delayMs = params->delayMs;
	ssd1306->fxn.mtxLock = params->mtxLock;
	ssd1306->fxn.mtxUnlock = params->mtxUnlock;
	ssd1306->fxn.i2cWrite = params->i2cWrite;
	ssd1306->fxn.spiCs = params->spiCs;
	ssd1306->fxn.spiDc = params->spiDc;
	ssd1306->fxn.spiReset = params->spiReset;
	ssd1306->fxn.spiWrite = params->spiWrite;

	if (params->bUseExternalFrameBuffer == false){
		ssd1306->_intern.pu8FrameBuffer = (uint8_t*)malloc(ssd1306->_intern.u32FrameSize);
		assert(ssd1306->_intern.pu8FrameBuffer != NULL);
	}
	else{
		ssd1306->_intern.pu8FrameBuffer = NULL;
	}

	// Reset OLED
	_reset(ssd1306);

	// Wait for the screen to boot
	_delay_ms(ssd1306, 100);

	// Init OLED
	_SSD_ERROR_CHECK(_write_command(ssd1306, 0xAE));; //display off

	_mtx_lock(ssd1306);

	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x20)); //Set Memory Addressing Mode
	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x00)); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
	// 10b,Page Addressing Mode (RESET); 11b,Invalid

	_SSD_ERROR_CHECK(_write_command(ssd1306, 0xB0)); //Set Page Start Address for Page Addressing Mode,0-7

	if (params->bMirrorVert == true){
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0xC0)); // Mirror vertically
	}
	else{
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0xC8)); //Set COM Output Scan Direction
	}

	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x00)); //---set low column address
	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x10)); //---set high column address

	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x40)); //--set start line address - CHECK

	if (params->bMirrorHorz == true){
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0xA0)); // Mirror horizontally
	}
	else{
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0xA1)); //--set segment re-map 0 to 127 - CHECK
	}

	if (params->bInverseColor == true){
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0xA7)); //--set inverse color
	}
	else{
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0xA6)); //--set normal color
	}

	// Set multiplex ratio.
	if (ssd1306->_intern.u32Heigth == 128){
		// Found in the Luma Python lib for SH1106.
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0xFF));
	}
	else{
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0xA8)); //--set multiplex ratio(1 to 64) - CHECK
	}

	if (ssd1306->_intern.u32Heigth == 32){
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0x1F)); //
	}
	else if (ssd1306->_intern.u32Heigth == 64){
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0x3F)); //
	}
	else if (ssd1306->_intern.u32Heigth == 128){
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0x3F)); // Seems to work for 128px high displays too.
	}

	_SSD_ERROR_CHECK(_write_command(ssd1306, 0xA4)); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

	_SSD_ERROR_CHECK(_write_command(ssd1306, 0xD3)); //-set display offset - CHECK
	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x00)); //-not offset

	_SSD_ERROR_CHECK(_write_command(ssd1306, 0xD5)); //--set display clock divide ratio/oscillator frequency
	_SSD_ERROR_CHECK(_write_command(ssd1306, 0xF0)); //--set divide ratio

	_SSD_ERROR_CHECK(_write_command(ssd1306, 0xD9)); //--set pre-charge period
	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x22)); //

	_SSD_ERROR_CHECK(_write_command(ssd1306, 0xDA)); //--set com pins hardware configuration - CHECK
	if (ssd1306->_intern.u32Heigth == 32){
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0x02));	//32
	}
	else if (ssd1306->_intern.u32Heigth == 64){
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0x12)); //64
	}
	else if (ssd1306->_intern.u32Heigth == 128){
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0x12)); //128
	}

	_SSD_ERROR_CHECK(_write_command(ssd1306, 0xDB)); //--set vcomh
	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x20)); //0x20,0.77xVcc

	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x8D)); //--set DC-DC enable
	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x14)); //

	_mtx_unlock(ssd1306);

	ssd1306->_intern.bInitialized = true;

	_SSD_ERROR_CHECK(SSD1306_SetContrast(ssd1306, 0xFF));
	_SSD_ERROR_CHECK(SSD1306_SetDisplayOn(ssd1306, true)); //--turn on SSD1306 panel

	return SSD1306_OK;
}

uint8_t SSD1306_SetFrameBuffer(ssd1306_t *ssd1306, uint8_t *pu8FrameBuffer){
	assert(ssd1306 != NULL);
	assert(ssd1306->_intern.bInitialized == true);
	assert(pu8FrameBuffer != NULL);

	_mtx_lock(ssd1306);
	ssd1306->_intern.pu8FrameBuffer = pu8FrameBuffer;
	_mtx_unlock(ssd1306);

	return SSD1306_OK;
}

uint8_t SSD1306_SetContrast(ssd1306_t *ssd1306, uint8_t level){
    uint8_t kSetContrastControlRegister = 0x81;

	assert(ssd1306 != NULL);
	assert(ssd1306->_intern.bInitialized == true);

	_mtx_lock(ssd1306);
	_SSD_ERROR_CHECK(_write_command(ssd1306, kSetContrastControlRegister));
	_SSD_ERROR_CHECK(_write_command(ssd1306, level));
	_mtx_unlock(ssd1306);

	return SSD1306_OK;
}

uint8_t SSD1306_SetDisplayOn(ssd1306_t *ssd1306, bool on){
	assert(ssd1306 != NULL);
	assert(ssd1306->_intern.bInitialized == true);

	_mtx_lock(ssd1306);
	if (on == true){
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0xAF));
	}
	else{
		_SSD_ERROR_CHECK(_write_command(ssd1306, 0xAE));
	}
	_mtx_unlock(ssd1306);

	return SSD1306_OK;
}

uint8_t SSD1306_Refresh(ssd1306_t *ssd1306){
	uint32_t i, width, offset;
	uint8_t *FrameBuffer;
	uint8_t offsetUpper, offsetLower;

	assert(ssd1306 != NULL);
	assert(ssd1306->_intern.bInitialized == true);
	assert(ssd1306->_intern.pu8FrameBuffer != NULL);
    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages
	FrameBuffer = ssd1306->_intern.pu8FrameBuffer;
	width = ssd1306->_intern.u32Width;

	offset = ssd1306->_intern.u32Offset;
	offsetUpper = ((offset >> 4) & 0x7);
	offsetLower = (offset & 0xF);
	_mtx_lock(ssd1306);
    for(i = 0; i < (ssd1306->_intern.u32Heigth/8) ; i++) {
    	_SSD_ERROR_CHECK(_write_command(ssd1306, 0xB0 + i)); // Set the current RAM page address.
    	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x00 | offsetLower));
    	_SSD_ERROR_CHECK(_write_command(ssd1306, 0x10 | offsetUpper));
    	_SSD_ERROR_CHECK(_write_data(ssd1306, &FrameBuffer[width*i], width));
    }
    _mtx_unlock(ssd1306);

    return SSD1306_OK;
}

uint8_t SSD1306_Write(ssd1306_t *ssd1306, uint32_t x, uint32_t y, bool color){
	uint8_t *FrameBuffer;
	uint32_t width;

	assert(ssd1306 != NULL);
	assert(ssd1306->_intern.bInitialized == true);
	assert(ssd1306->_intern.pu8FrameBuffer != NULL);

	if (x >= ssd1306->_intern.u32Width ||
			y >= ssd1306->_intern.u32Heigth){
		// Pixel is out of range
		return SSD1306_FAIL;
	}

	FrameBuffer = ssd1306->_intern.pu8FrameBuffer;
	width = ssd1306->_intern.u32Width;

	_mtx_lock(ssd1306);
	if(color == true) {
		FrameBuffer[x + (y / 8) * width] |= 1 << (y % 8);
	} else {
		FrameBuffer[x + (y / 8) * width] &= ~(1 << (y % 8));
	}
	_mtx_unlock(ssd1306);

	return SSD1306_OK;
}

uint8_t SSD1306_Fill(ssd1306_t *ssd1306, uint8_t color){
	uint8_t *FrameBuffer;

	assert(ssd1306 != NULL);
	assert(ssd1306->_intern.bInitialized == true);
	assert(ssd1306->_intern.pu8FrameBuffer != NULL);

	FrameBuffer = ssd1306->_intern.pu8FrameBuffer;
	if (color == SSD1306_COLOR_BLACK){
		memset(FrameBuffer, 0, ssd1306->_intern.u32FrameSize);
	}
	else{
		memset(FrameBuffer, 0xFF, ssd1306->_intern.u32FrameSize);
	}

	return SSD1306_OK;
}
