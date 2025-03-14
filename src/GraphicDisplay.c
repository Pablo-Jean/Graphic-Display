/*
 * GraphicDisplay.c
 *
 *  Created on: Jun 28, 2024
 *      Author: pablo-jean
 */

#include "GraphicDisplay.h"

/*
 * Privates
 */

/*
 * Extern
 */

/*
 * Display Drivers
 */
/* Typecast Macros */
#define _SET_PIXEL_COLOR_TYPECAST			(uint8_t (*)(void *, uint32_t, uint32_t, uint8_t))
#define _FILL_FRAME_BUFFER_TYPEFCAST		(uint8_t (*)(void *, uint8_t))
#define _REFRESH_DISP_TYPECAST				(uint8_t (*)(void *))
#define _SET_ON_TYPECASTAT					(uint8_t (*)(void *, _Bool))
#define _SET_CONTRAST_TYPECAST				(uint8_t (*)(void *, uint8_t))

/** Diver for SSD1306 **/
gd_driver_t _Gd_Driver_SSD1306Attr = {
		.pHandle = NULL,
		.fxnSetPixelColor = _SET_PIXEL_COLOR_TYPECAST		SSD1306_Write,
		.fxnFillFrameBuffer = _FILL_FRAME_BUFFER_TYPEFCAST 	SSD1306_Fill,
		.fxnRefreshDisp = _REFRESH_DISP_TYPECAST 			SSD1306_Refresh,
		.fxnSetOn = _SET_ON_TYPECASTAT 						SSD1306_SetDisplayOn,
		.fxnSetContrast = _SET_CONTRAST_TYPECAST			SSD1306_SetContrast
};
gd_driver_t *Gd_Driver_SSD1306 = &_Gd_Driver_SSD1306Attr;

/** Diver for ST7920 **/
gd_driver_t _Gd_Driver_ST7920Attr = {
		.pHandle = NULL,
		.fxnSetPixelColor = _SET_PIXEL_COLOR_TYPECAST		ST7920_Write,
		.fxnFillFrameBuffer = _FILL_FRAME_BUFFER_TYPEFCAST 	ST7920_Fill,
		.fxnRefreshDisp = _REFRESH_DISP_TYPECAST 			ST7920_Refresh,
		.fxnSetOn = _SET_ON_TYPECASTAT 						NULL,
		.fxnSetContrast = _SET_CONTRAST_TYPECAST			NULL
};
gd_driver_t *Gd_Driver_ST7920 = &_Gd_Driver_ST7920Attr;

/** Diver for ST7525 **/
gd_driver_t _Gd_Driver_ST7525Attr = {
		.pHandle = NULL,
		.fxnSetPixelColor = _SET_PIXEL_COLOR_TYPECAST		ST7525_Write,
		.fxnFillFrameBuffer = _FILL_FRAME_BUFFER_TYPEFCAST 	ST7525_Fill,
		.fxnRefreshDisp = _REFRESH_DISP_TYPECAST 			ST7525_Refresh,
		.fxnSetOn = _SET_ON_TYPECASTAT 						ST7525_SetDisplayOn,
		.fxnSetContrast = _SET_CONTRAST_TYPECAST			ST7525_SetContrast
};
gd_driver_t *Gd_Driver_ST7525 = &_Gd_Driver_ST7525Attr;

/* Macros */

/* Display Functions */

static void _drv_fill_frame_buffer(gd_t *Gd, gd_color_e color){
	assert(Gd->disp->fxnFillFrameBuffer != NULL);
	Gd->disp->fxnFillFrameBuffer(Gd->disp->pHandle, (uint8_t)color);
}

static void _drv_set_pixel(gd_t *Gd, uint32_t x, uint32_t y, gd_color_e color){
	assert(Gd->disp->fxnSetPixelColor != NULL);
	Gd->disp->fxnSetPixelColor(Gd->disp->pHandle, x, y, (uint8_t)color);
}

static void _drv_set_refresh(gd_t *Gd){
	assert(Gd->disp->fxnRefreshDisp != NULL);
	Gd->disp->fxnRefreshDisp(Gd->disp->pHandle);
}

static void _drv_set_constrat(gd_t *Gd, uint8_t value){
	if (Gd->disp->fxnSetContrast != NULL){
		Gd->disp->fxnSetContrast(Gd->disp->pHandle, value);
	}
}

static void _drv_set_on(gd_t *Gd, bool On){
	if (Gd->disp->fxnSetOn != NULL){
		Gd->disp->fxnSetOn(Gd->disp->pHandle, On);
	}
}

/* Mutex Functions */
static void _mtx_lock(gd_t *Gd){
	if (Gd->fxn.mtxLock != NULL){
		Gd->fxn.mtxLock();
	}
}

static void _mtx_unlock(gd_t *Gd){
	if (Gd->fxn.mtxUnlock != NULL){
		Gd->fxn.mtxUnlock();
	}
}

/* Convert Degrees to Radians */
static float _DegToRad(float par_deg) {
	return par_deg * (3.14f / 180.0f);
}

/* Normalize degree to [0;360] */
static uint16_t _NormalizeTo0_360(uint16_t par_deg) {
	uint16_t loc_angle;
	if(par_deg <= 360) {
		loc_angle = par_deg;
	} else {
		loc_angle = par_deg % 360;
		loc_angle = (loc_angle ? loc_angle : 360);
	}
	return loc_angle;
}


/*
 * Publics
 */

/* Initialize the oled screen */
gd_error_e GD_Init(gd_t *Gd, gd_params_t *params) {
	// Assert if some parameter is invalid
	assert(Gd != NULL);
	assert(params != NULL);
	assert(params->u32Width != 0);
	assert(params->u32Height != 0);
	assert(params->DisplayHandle != NULL);
	assert(params->DisplayDriver != NULL);

	// Allocate buffer if doesn`t exitsts
	Gd->u32Height = params->u32Height;
	Gd->u32Width = params->u32Width;
	Gd->disp = params->DisplayDriver;
	Gd->disp->pHandle = params->DisplayHandle;
	Gd->_intern.u32BufferLen = Gd->u32Height * Gd->u32Width / 8;

	// Set default values for screen object
	Gd->_intern.u32CurrX = 0;
	Gd->_intern.u32CurrX = 0;
	Gd->_intern.bInitialized = true;

	// Clear screen
	GD_Fill(Gd, GD_BLACK);

	// Flush buffer to screen
	GD_UpdateScreen(Gd);

	return GD_OK;
}

/* Fill the whole screen with the given color */
gd_error_e GD_Fill(gd_t *Gd, gd_color_e color) {
	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	_mtx_lock(Gd);
	_drv_fill_frame_buffer(Gd, color);
	_mtx_unlock(Gd);

	return GD_OK;
}

/* Write the screenbuffer with changed to the screen */
gd_error_e GD_UpdateScreen(gd_t *Gd) {
	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	_mtx_lock(Gd);
	_drv_set_refresh(Gd);
	_mtx_unlock(Gd);

	return GD_OK;
}

/*
 * Draw one pixel in the screenbuffer
 * X => X Coordinate
 * Y => Y Coordinate
 * color => Pixel color
 */
gd_error_e GD_DrawPixel(gd_t *Gd, uint32_t x, uint32_t y, gd_color_e color) {
	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	if(x >= Gd->u32Width || y >= Gd->u32Height) {
		// Don't write outside the buffer
		return GD_FAIL;
	}

	_mtx_lock(Gd);
	// Draw in the right color
	_drv_set_pixel(Gd, x, y, color);
//	if(color == GD_WHITE) {
//		Gd->_intern.pu8FrameBuffer[x + (y / 8) * Gd->u32Width] |= 1 << (y % 8);
//	} else {
//		Gd->_intern.pu8FrameBuffer[x + (y / 8) * Gd->u32Width] &= ~(1 << (y % 8));
//	}
	_mtx_unlock(Gd);

	return GD_OK;
}

/*
 * Draw 1 char to the screen buffer
 * ch       => char om weg te schrijven
 * Font     => Font waarmee we gaan schrijven
 * color    => Black or White
 */
gd_error_e GD_WriteChar(gd_t *Gd, char ch, const gd_font_t *Font, gd_color_e color) {
	uint32_t i, b, j;
	gd_color_e pxColor;

	assert(Gd != NULL);
	assert(Font != NULL);
	assert(Gd->_intern.bInitialized == true);

	// Check if character is valid
	if (ch < 32 || ch > 126)
		return GD_FAIL;

	// Check remaining space on current line
	if (Gd->u32Width < (Gd->_intern.u32CurrX + Font->width) ||
			Gd->u32Height < (Gd->_intern.u32CurrY + Font->height))
	{
		// Not enough space on current line
		return GD_FAIL;
	}

	// Use the font to write
	for(i = 0; i < Font->height; i++) {
		b = Font->data[(ch - 32) * Font->height + i];
		for(j = 0; j < Font->width; j++) {
			if((b << j) & 0x8000)  {
				pxColor = color;
			} else {
				pxColor = (gd_color_e)!color;
			}
			GD_DrawPixel(Gd,
					Gd->_intern.u32CurrX + j,
					Gd->_intern.u32CurrY + i,
					pxColor);
		}
	}

	// The current space is now taken
	Gd->_intern.u32CurrX += Font->char_width ? Font->char_width[ch - 32] : Font->width;

	// Return written OK
	return GD_OK;
}

/* Write full string to screenbuffer */
gd_error_e GD_WriteString(gd_t *Gd, char* str, const gd_font_t *Font, gd_color_e color) {
	assert(Gd != NULL);
	assert(Font != NULL);
	assert(Gd->_intern.bInitialized == true);

	while (*str) {
		GD_WriteChar(Gd, *str, Font, color);
		str++;
	}

	// Everything ok
	return GD_OK;
}

/* Position the cursor */
gd_error_e GD_SetCursor(gd_t *Gd, uint32_t x, uint32_t y) {
	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	if(x >= Gd->u32Width || y >= Gd->u32Height) {
		// Don't write outside the buffer
		return GD_FAIL;
	}
	_mtx_lock(Gd);
	Gd->_intern.u32CurrX = x;
	Gd->_intern.u32CurrY = y;
	_mtx_unlock(Gd);

	return GD_OK;
}

/* Draw line by Bresenhem's algorithm */
gd_error_e GD_Line(gd_t *Gd, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, gd_color_e color) {
	int32_t deltaX = (int32_t)abs((int)(x2 - x1));
	int32_t deltaY = (int32_t)abs((int)(y2 - y1));
	int32_t signX = ((x1 < x2) ? 1 : -1);
	int32_t signY = ((y1 < y2) ? 1 : -1);
	int32_t error = deltaX - deltaY;
	int32_t error2;

	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	GD_DrawPixel(Gd, x2, y2, color);
	while((x1 != x2) || (y1 != y2)) {
		GD_DrawPixel(Gd, x1, y1, color);
		error2 = error * 2;
		if(error2 > -deltaY) {
			error -= deltaY;
			x1 += signX;
		}

		if(error2 < deltaX) {
			error += deltaX;
			y1 += signY;
		}
	}

	return GD_OK;
}

/* Draw polyline */
gd_error_e GD_Polyline(gd_t*Gd, gd_vertex_t *par_vertex, uint16_t par_size, gd_color_e color) {
	uint16_t i;

	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);
	assert(par_vertex != NULL);

	for(i = 1; i < par_size; i++) {
		GD_Line(Gd, par_vertex[i - 1].x,
				par_vertex[i - 1].y,
				par_vertex[i].x,
				par_vertex[i].y,
				color);
	}

	return GD_OK;
}

/*
 * DrawArc. Draw angle is beginning from 4 quart of trigonometric circle (3pi/2)
 * start_angle in degree
 * sweep in degree
 */
gd_error_e GD_DrawArc(gd_t *Gd, uint32_t x, uint32_t y, uint32_t radius, uint16_t start_angle, uint16_t sweep, gd_color_e color) {
	static const uint8_t CIRCLE_APPROXIMATION_SEGMENTS = 36;
	float approx_degree;
	gd_error_e e;
	uint32_t approx_segments;
	uint32_t xp1,xp2;
	uint32_t yp1,yp2;
	uint32_t count;
	uint32_t loc_sweep;
	float rad;

	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	loc_sweep = _NormalizeTo0_360(sweep);

	count = (_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
	approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
	approx_degree = loc_sweep / (float)approx_segments;
	while(count < approx_segments)
	{
		rad = _DegToRad(count*approx_degree);
		xp1 = x + (int32_t)(sinf(rad)*radius);
		yp1 = y + (int32_t)(cosf(rad)*radius);
		count++;
		if(count != approx_segments) {
			rad = _DegToRad(count*approx_degree);
		} else {
			rad = _DegToRad(loc_sweep);
		}
		xp2 = x + (int32_t)(sinf(rad)*radius);
		yp2 = y + (int32_t)(cosf(rad)*radius);

		e = GD_Line(Gd, xp1, yp1, xp2, yp2, color);
		if (e != GD_OK){
			return GD_FAIL;
		}
	}

	return GD_OK;
}

/*
 * Draw arc with radius line
 * Angle is beginning from 4 quart of trigonometric circle (3pi/2)
 * start_angle: start angle in degree
 * sweep: finish angle in degree
 */
gd_error_e GD_DrawArcWithRadiusLine(gd_t *Gd, uint32_t x, uint32_t y, uint32_t radius, uint16_t start_angle, uint16_t sweep, gd_color_e color) {
	const uint32_t CIRCLE_APPROXIMATION_SEGMENTS = 36;
	float approx_degree;
	gd_error_e e;
	uint32_t approx_segments;
	uint32_t xp1;
	uint32_t xp2 = 0;
	uint32_t yp1;
	uint32_t yp2 = 0;
	uint32_t count;
	uint32_t loc_sweep;
	float rad;

	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	loc_sweep = _NormalizeTo0_360(sweep);

	count = (_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
	approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
	approx_degree = loc_sweep / (float)approx_segments;

	rad = _DegToRad(count*approx_degree);
	uint8_t first_point_x = x + (int8_t)(sinf(rad)*radius);
	uint8_t first_point_y = y + (int8_t)(cosf(rad)*radius);
	while (count < approx_segments) {
		rad = _DegToRad(count*approx_degree);
		xp1 = x + (int32_t)(sinf(rad)*radius);
		yp1 = y + (int32_t)(cosf(rad)*radius);
		count++;
		if (count != approx_segments) {
			rad = _DegToRad(count*approx_degree);
		} else {
			rad = _DegToRad(loc_sweep);
		}
		xp2 = x + (int32_t)(sinf(rad)*radius);
		yp2 = y + (int32_t)(cosf(rad)*radius);

		e = GD_Line(Gd, xp1, yp1, xp2, yp2, color);
		if (e != GD_OK){
			return GD_FAIL;
		}
	}

	// Radius line
	e = GD_Line(Gd, x, y, first_point_x, first_point_y, color);
	if (e != GD_OK){
		return GD_FAIL;
	}
	e = GD_Line(Gd, x, y, xp2, yp2, color);
	if (e != GD_OK){
		return GD_FAIL;
	}

	return GD_OK;
}

/* Draw circle by Bresenhem's algorithm */
gd_error_e GD_DrawCircle(gd_t *Gd, uint32_t par_x, uint32_t par_y, uint32_t par_r, gd_color_e par_color) {
	int32_t x = -par_r;
	int32_t y = 0;
	int32_t err = 2 - 2 * par_r;
	int32_t e2;

	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	do {
		GD_DrawPixel(Gd, par_x - x, par_y + y, par_color);
		GD_DrawPixel(Gd, par_x + x, par_y + y, par_color);
		GD_DrawPixel(Gd, par_x + x, par_y - y, par_color);
		GD_DrawPixel(Gd, par_x - x, par_y - y, par_color);
		e2 = err;

		if (e2 <= y) {
			y++;
			err = err + (y * 2 + 1);
			if(-x == y && e2 <= x) {
				e2 = 0;
			}
		}

		if (e2 > x) {
			x++;
			err = err + (x * 2 + 1);
		}
	} while (x <= 0);

	return GD_OK;
}

/* Draw filled circle. Pixel positions calculated using Bresenham's algorithm */
gd_error_e GD_FillCircle(gd_t *Gd, uint32_t par_x, uint32_t par_y, uint32_t par_r, gd_color_e par_color) {
	int32_t x = -par_r;
	int32_t y = 0;
	int32_t err = 2 - 2 * par_r;
	int32_t e2;

	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	do {
		for (uint8_t _y = (par_y + y); _y >= (par_y - y); _y--) {
			for (uint8_t _x = (par_x - x); _x >= (par_x + x); _x--) {
				GD_DrawPixel(Gd, _x, _y, par_color);
			}
		}

		e2 = err;
		if (e2 <= y) {
			y++;
			err = err + (y * 2 + 1);
			if (-x == y && e2 <= x) {
				e2 = 0;
			}
		}

		if (e2 > x) {
			x++;
			err = err + (x * 2 + 1);
		}
	} while (x <= 0);

	return GD_OK;
}

/* Draw a rectangle */
gd_error_e GD_DrawRectangle(gd_t *Gd, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, gd_color_e color) {
	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	GD_Line(Gd, x1, y1, x2, y1, color);
	GD_Line(Gd, x2, y1, x2, y2, color);
	GD_Line(Gd, x2, y2, x1, y2, color);
	GD_Line(Gd, x1, y2, x1, y1, color);

	return GD_OK;
}

/* Draw a filled rectangle */
gd_error_e GD_FillRectangle(gd_t *Gd, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, gd_error_e color) {
	uint32_t x_start = ((x1<=x2) ? x1 : x2);
	uint32_t x_end   = ((x1<=x2) ? x2 : x1);
	uint32_t y_start = ((y1<=y2) ? y1 : y2);
	uint32_t y_end   = ((y1<=y2) ? y2 : y1);

	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	for (uint32_t y = y_start; (y <= y_end)&&(y < Gd->u32Height); y++) {
		for (uint32_t x = x_start; (x <= x_end)&&(x < Gd->u32Width); x++) {
			GD_DrawPixel(Gd, x, y, color);
		}
	}

	return GD_OK;
}

/* Draw a bitmap */
/* You can use the https://github.com/faytor/open_lcd_assistant assistant to create you bitmaps */
gd_error_e GD_DrawBitmap(gd_t *Gd, uint32_t x, uint32_t y, const unsigned char* bitmap, uint32_t w, uint32_t h, gd_color_e color) {
	int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
	uint32_t byte = 0;

	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);
	assert(bitmap != NULL);

	if ((x >= Gd->u32Width) || (y >= Gd->u32Height)) {
		return GD_FAIL;
	}

	for (uint32_t j = 0; j < h; j++, y++) {
		for (uint32_t i = 0; i < w; i++) {
			if (i & 7) {
				byte >>= 1;
			} else {
				byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
			}

			if (byte & 0x1) {
				GD_DrawPixel(Gd, x + i, y, color);
			}
			else{
				GD_DrawPixel(Gd, x + i, y, !color);
			}
		}
	}

	return GD_OK;
}

gd_error_e GD_SetContrast(gd_t *Gd, uint8_t value) {
	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	_drv_set_constrat(Gd, value);

	return GD_OK;
}

gd_error_e GD_SetDisplayOn(gd_t *Gd, bool on) {
	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	Gd->_intern.bDisplayOn = on;
	_drv_set_on(Gd, on);

	return GD_OK;
}

bool GD_GetDisplayOn(gd_t *Gd) {
	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	return Gd->_intern.bDisplayOn;
}
