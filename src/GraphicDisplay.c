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

/* Macros */

/* Functions */

static uint8_t _drv_init_display(gd_t *Gd, void* StartParams){
	return Gd->disp->fxnInitDisp(Gd->disp->pHandle, StartParams);
}

static void _drv_write_display(gd_t *Gd, uint8_t *data, uint32_t len){
	Gd->disp->fxnWriteDisp(Gd->disp->pHandle, data, len);
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

/* Fills the Screenbuffer with values from a given buffer of a fixed length */
gd_error_e ssd1306_FillBuffer(gd_t *Gd, uint8_t* buf, uint32_t len) {
	gd_error_e ret = GD_FAIL;

	if (len <= Gd->_intern.u32BufferLen) {
		memcpy(Gd->_intern.pu8FrameBuffer, buf, len);
		ret = GD_OK;
	}

	return ret;
}

/* Initialize the oled screen */
gd_error_e GD_Init(gd_t *Gd, const uint8_t *FrameBuffer) {
	// Assert if some parameter is invalid
	assert(Gd != NULL);
	assert(Gd->u32Width != 0);
	assert(Gd->u32Height != 0);
	assert(Gd->disp != NULL);
	assert(Gd->disp->pHandle != NULL);
	assert(Gd->disp->fxnWriteDisp != NULL);

	// Allocate buffer if doesn`t exitsts
	Gd->_intern.u32BufferLen = Gd->u32Height * Gd->u32Width / 8;
	if (FrameBuffer == NULL){
		Gd->_intern.pu8FrameBuffer = (uint8_t*)malloc((size_t)Gd->_intern.u32BufferLen);
	}
	else{
		Gd->_intern.pu8FrameBuffer = (uint8_t*)FrameBuffer;
	}
	assert(Gd->_intern.pu8FrameBuffer != NULL);

	// Initialize the Display
	assert (_drv_init_display(Gd, NULL) == 0);
	GD_SetDisplayOn(Gd, true); //--turn on SSD1306 panel

	// Clear screen
	GD_Fill(Gd, GD_BLACK);

	// Flush buffer to screen
	GD_UpdateScreen(Gd);

	// Set default values for screen object
	Gd->_intern.u32CurrX = 0;
	Gd->_intern.u32CurrX = 0;

	Gd->_intern.bInitialized= true;

	return GD_OK;
}

/* Fill the whole screen with the given color */
gd_error_e ssd1306_Fill(gd_t *Gd, gd_color_e color) {
	memset(Gd->_intern.pu8FrameBuffer, (color == GD_BLACK) ? 0x00 : 0xFF, Gd->_intern.u32BufferLen);

	return GD_OK;
}

/* Write the screenbuffer with changed to the screen */
gd_error_e ssd1306_UpdateScreen(gd_t *Gd) {
	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);
	assert(Gd->disp->fxnWriteDisp == NULL);

	_drv_write_display(Gd, Gd->_intern.pu8FrameBuffer, Gd->_intern.u32BufferLen);

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

	// Draw in the right color
	if(color == GD_WHITE) {
		Gd->_intern.pu8FrameBuffer[x + (y / 8) * Gd->u32Width] |= 1 << (y % 8);
	} else {
		Gd->_intern.pu8FrameBuffer[x + (y / 8) * Gd->u32Width] &= ~(1 << (y % 8));
	}

	return GD_OK;
}

/*
 * Draw 1 char to the screen buffer
 * ch       => char om weg te schrijven
 * Font     => Font waarmee we gaan schrijven
 * color    => Black or White
 */
gd_error_e GD_WriteChar(gd_t *Gd, char ch, gd_font_t *Font, gd_color_e color) {
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
gd_error_e GD_WriteString(gd_t *Gd, char* str, gd_font_t *Font, gd_color_e color) {
	gd_error_e e;

	assert(Gd != NULL);
	assert(Font != NULL);
	assert(Gd->_intern.bInitialized == true);

	while (*str) {
		e = GD_WriteChar(Gd, *str, Font, color);
		if (e != GD_OK) {
			// Char could not be written
			return GD_FAIL;
		}
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
	Gd->_intern.u32CurrX = x;
	Gd->_intern.u32CurrY = y;

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
	gd_error_e e;

	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);
	assert(par_vertex != NULL);

	for(i = 1; i < par_size; i++) {
		e = GD_Line(Gd, par_vertex[i - 1].x,
				par_vertex[i - 1].y,
				par_vertex[i].x,
				par_vertex[i].y,
				color);
		if (e != GD_OK){
			return GD_FAIL;
		}
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
		if (GD_DrawPixel(Gd, par_x - x, par_y + y, par_color) != GD_OK){
			return GD_FAIL;
		}
		if (GD_DrawPixel(Gd, par_x + x, par_y + y, par_color) != GD_OK){
			return GD_FAIL;
		}
		if (GD_DrawPixel(Gd, par_x + x, par_y - y, par_color) != GD_OK){
			return GD_FAIL;
		}
		if (GD_DrawPixel(Gd, par_x - x, par_y - y, par_color) != GD_OK){
			return GD_FAIL;
		}
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
				if (GD_DrawPixel(Gd, _x, _y, par_color) != GD_OK){
					return GD_FAIL;
				}
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

	if (GD_Line(Gd, x1, y1, x2, y1, color) != GD_OK){
		return GD_FAIL;
	}
	if (GD_Line(Gd, x2, y1, x2, y2, color) != GD_OK){
		return GD_FAIL;
	}
	if (GD_Line(Gd, x2, y2, x1, y2, color) != GD_OK){
		return GD_FAIL;
	}
	if (GD_Line(Gd, x1, y2, x1, y1, color) != GD_OK){
		return GD_FAIL;
	}

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
			if (GD_DrawPixel(Gd, x, y, color) == GD_OK){
				return GD_FAIL;
			}
		}
	}

	return GD_OK;
}

gd_error_e GD_InvertRectangle(gd_t *Gd, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	if ((x2 >= Gd->u32Width) || (y2 >= Gd->u32Height)) {
		return GD_FAIL;
	}
	if ((x1 > x2) || (y1 > y2)) {
		return GD_FAIL;
	}
	uint32_t i;
	if ((y1 / 8) != (y2 / 8)) {
		/* if rectangle doesn't lie on one 8px row */
		for (uint32_t x = x1; x <= x2; x++) {
			i = x + (y1 / 8) * Gd->u32Width;
			Gd->_intern.pu8FrameBuffer[i] ^= 0xFF << (y1 % 8);
			i += Gd->u32Width;
			for (; i < x + (y2 / 8) * Gd->u32Width; i += Gd->u32Width) {
				Gd->_intern.pu8FrameBuffer[i] ^= 0xFF;
			}
			Gd->_intern.pu8FrameBuffer[i] ^= 0xFF >> (7 - (y2 % 8));
		}
	} else {
		/* if rectangle lies on one 8px row */
		const uint8_t mask = (0xFF << (y1 % 8)) & (0xFF >> (7 - (y2 % 8)));
		for (i = x1 + (y1 / 8) * Gd->u32Width;
				i <= (uint32_t)x2 + (y2 / 8) * Gd->u32Width; i++) {
			Gd->_intern.pu8FrameBuffer[i] ^= mask;
		}
	}

	return GD_OK;
}

/* Draw a bitmap */
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
				byte <<= 1;
			} else {
				byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
			}

			if (byte & 0x80) {
				if (GD_DrawPixel(Gd, x + i, y, color) != GD_OK){
					return GD_FAIL;
				}
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

bool ssd1306_GetDisplayOn(gd_t *Gd) {
	assert(Gd != NULL);
	assert(Gd->_intern.bInitialized == true);

	return Gd->_intern.bDisplayOn;
}
