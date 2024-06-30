/*
 * GraphicDisplay.h
 *
 *  Created on: Jun 28, 2024
 *      Author: pablo-jean
 */

#ifndef GRAPHICDISPLAY_H_
#define GRAPHICDISPLAY_H_

/*
 * Includes
 */

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "GraphicDisplay_Fonts.h"

#include "drivers/ssd1306/ssd1306.h"

/*
 * Check if gd_config.h exists, otherwise, we load a 'default' configuration
 *
 * NOTE: Some compilers can`t support this directive, but the following list can support.
 * Clang
 * GCC from 5.X
 * Visual Studio from VS2015 Update 2
 *
 * If your compiler give an error in this line, insert the NO_SUPPORT_FOR_HAS_INCLUDED
 * in the preprocessor of your compiler and you need to create the "gd_config.h" header file
 */
#ifndef NO_SUPPORT_FOR_HAS_INCLUDED
	#if __has_include("gd_config.h")
		#include "gd_config.h"
	#else
		#define GD_INCLUDE_FONT_6x8
		#define GD_INCLUDE_FONT_7x10
		#define GD_INCLUDE_FONT_11x18
		#define GD_INCLUDE_FONT_16x26
		#define GD_INCLUDE_FONT_16x24
		#define GD_INCLUDE_FONT_16x15
	#endif
#else
	#include "gd_config.h"
#endif

/*
 * Macros
 */
#define GD_FXN_SET_MTX_LOCK(HANDLE, MTXLOCK)		HANDLE.fxn.mtxLock = MTXLOCK;
#define GD_FXN_SET_MTX_UNLOCK(HANDLE, MTXUNLOCK)	HANDLE.fxn.mtxUnlock = MTXUNLOCK;

/*
 * Enumerates
 */

typedef enum{
	GD_OK,
	GD_FAIL
}gd_error_e;

typedef enum{
	GD_BLACK,
	GD_WHITE
}gd_color_e;

/*
 * Structs
 */

typedef void (*fxnGd_mtxLock)(void);
typedef void (*fxnGd_mtxUnlock)(void);

typedef struct{
	void* pHandle;
	uint8_t (*fxnSetExtFrameBuffer)(void* handle, uint8_t *FrameBuffer);
	uint8_t (*fxnRefreshDisp)(void* handle);
	uint8_t (*fxnSetOn)(void* handle, bool on);
	uint8_t (*fxnSetContrast)(void* handle, uint8_t value);
}gd_driver_t;

typedef struct {
	const uint8_t width;                /**< Font width in pixels */
	const uint8_t height;               /**< Font height in pixels */
	const uint16_t *const data;         /**< Pointer to font data array */
    const uint8_t *const char_width;    /**< Proportional character width in pixels (NULL for monospaced) */
}gd_font_t;

typedef struct{
	uint32_t x;
	uint32_t y;
}gd_vertex_t;

typedef struct{
	uint32_t u32Height;
	uint32_t u32Width;

	struct{
		fxnGd_mtxLock mtxLock;
		fxnGd_mtxUnlock mtxUnlock;
	}fxn;

	gd_driver_t *disp;
	struct{
		uint8_t *pu8FrameBuffer;
		uint32_t u32BufferLen;
		uint32_t u32CurrX;
		uint32_t u32CurrY;
		bool bDisplayOn;
		bool bInitialized;
	}_intern;
}gd_t;

typedef struct{
	void *DisplayHandle;
	gd_driver_t *DisplayDriver;
	uint8_t *pu8FrameBuffer;
	uint32_t u32Height;
	uint32_t u32Width;
	fxnGd_mtxLock mtxLock;
	fxnGd_mtxUnlock mtxUnlock;
}gd_params_t;

/*
 * Exterrns
 */

extern gd_driver_t* Gd_Driver_SSD1306;

/*
 * Publics
 */
gd_error_e GD_Init(gd_t *Gd, gd_params_t *params);
gd_error_e GD_Fill(gd_t *Gd, gd_color_e color);
gd_error_e GD_UpdateScreen(gd_t *Gd);
gd_error_e GD_DrawPixel(gd_t *Gd,  uint32_t x, uint32_t y, gd_color_e color);
gd_error_e GD_WriteChar(gd_t *Gd,  char ch, gd_font_t *Font, gd_color_e color);
gd_error_e GD_WriteString(gd_t *Gd,  char* str, gd_font_t *Font, gd_color_e color);
gd_error_e GD_SetCursor(gd_t *Gd,  uint32_t x, uint32_t y);
gd_error_e GD_Line(gd_t *Gd,  uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, gd_color_e color);
gd_error_e GD_DrawArc(gd_t *Gd,  uint32_t x, uint32_t y, uint32_t radius, uint16_t start_angle, uint16_t sweep, gd_color_e color);
gd_error_e GD_DrawArcWithRadiusLine(gd_t *Gd, uint32_t x, uint32_t y, uint32_t radius, uint16_t start_angle, uint16_t sweep, gd_color_e color);
gd_error_e GD_DrawCircle(gd_t *Gd, uint32_t par_x, uint32_t par_y, uint32_t par_r, gd_color_e color);
gd_error_e GD_FillCircle(gd_t *Gd, uint32_t par_x,uint32_t par_y,uint32_t par_r,gd_color_e par_color);
gd_error_e GD_Polyline(gd_t *Gd, gd_vertex_t *par_vertex, uint16_t par_size, gd_color_e color);
gd_error_e GD_DrawRectangle(gd_t *Gd, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, gd_color_e color);
gd_error_e GD_FillRectangle(gd_t *Gd, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, gd_error_e color);

/**
 * @brief Invert color of pixels in rectangle (include border)
 *
 * @param x1 X Coordinate of top left corner
 * @param y1 Y Coordinate of top left corner
 * @param x2 X Coordinate of bottom right corner
 * @param y2 Y Coordinate of bottom right corner
 * @return gd_error_e status
 */
gd_error_e GD_InvertRectangle(gd_t *Gd, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

gd_error_e GD_DrawBitmap(gd_t *Gd, uint32_t x, uint32_t y, const unsigned char* bitmap, uint32_t w, uint32_t h, gd_color_e color);

/**
 * @brief Sets the contrast of the display.
 * @param[in] value contrast to set.
 * @note Contrast increases as the value increases.
 * @note RESET = 7Fh.
 */
gd_error_e GD_SetContrast(gd_t *Gd, const uint8_t value);

/**
 * @brief Set Display ON/OFF.
 * @param[in] on 0 for OFF, any for ON.
 */
gd_error_e GD_SetDisplayOn(gd_t *Gd, bool on);

/**
 * @brief Reads DisplayOn state.
 * @return  0: OFF.
 *          1: ON.
 */
bool GD_GetDisplayOn(gd_t *Gd);

// Low-level procedures
gd_error_e GD_Reset(gd_t *Gd);
gd_error_e GD_WriteCommand(gd_t *Gd, uint8_t byte);
gd_error_e GD_WriteData(gd_t *Gd, uint8_t* buffer, size_t buff_size);
gd_error_e GD_FillBuffer(gd_t *Gd, uint8_t* buf, uint32_t len);


#endif /* GRAPHICDISPLAY_H_ */
