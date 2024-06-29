/*
 * GraphicDisplay_Fonts.h
 *
 *  Created on: Jun 28, 2024
 *      Author: pablo-jean
 */

/*
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * https://github.com/afiskon/stm32-ssd1306
 *
 * A big refactor was made, to support any device and not gluing to any specific chipset
 */

#ifndef GRAPHICDISPLAY_FONTS_H_
#define GRAPHICDISPLAY_FONTS_H_

/*
 * Include
 */

#include "GraphicDisplay.h"

/*
 * Macros
 */

#ifdef GD_INCLUDE_FONT_6x8
extern const gd_font_t* Font_6x8;
#endif
#ifdef GD_INCLUDE_FONT_7x10
extern const gd_font_t* Font_7x10;
#endif
#ifdef GD_INCLUDE_FONT_11x18
extern const gd_font_t* Font_11x18;
#endif
#ifdef GD_INCLUDE_FONT_16x26
extern const gd_font_t* Font_16x26;
#endif
#ifdef GD_INCLUDE_FONT_16x24
extern const gd_font_t* Font_16x24;
#endif
#ifdef GD_INCLUDE_FONT_16x15
/** Generated Roboto Thin 15
 * @copyright Google https://github.com/googlefonts/roboto
 * @license This font is licensed under the Apache License, Version 2.0.
*/
extern const gd_font_t* Font_16x15;
#endif

#endif /* GRAPHICDISPLAY_FONTS_H_ */
