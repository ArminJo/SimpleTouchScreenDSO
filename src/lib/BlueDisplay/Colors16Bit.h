/*
 * Colors16Bit.h
 *
 *  SUMMARY
 *  Blue Display is an Open Source Android remote Display for Arduino etc.
 *  It receives basic draw requests from Arduino etc. over Bluetooth and renders it.
 *  It also implements basic GUI elements as buttons and sliders.
 *  GUI callback, touch and sensor events are sent back to Arduino.
 *
 *  Copyright (C) 2015  Armin Joachimsmeyer
 *  armin.joachimsmeyer@gmail.com
 *
 *  This file is part of BlueDisplay https://github.com/ArminJo/android-blue-display.
 *
 *  BlueDisplay is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#ifndef COLORS16BIT_H_
#define COLORS16BIT_H_

#include <stdint.h>

typedef uint16_t Color_t;
/*
 * Basic colors
 */
// RGB to 16 bit 565 schema - 5 red | 6 green | 5 blue
#define COLOR_WHITE     ((Color_t)0xFFFF)
// 01 because 0 is used as flag (e.g. in touch button for default color)
#define COLOR_BLACK     ((Color_t)0X0001)
#define COLOR_RED       ((Color_t)0xF800)
#define COLOR_GREEN     ((Color_t)0X07E0)
#define COLOR_BLUE      ((Color_t)0x001F)
#define COLOR_DARK_BLUE ((Color_t)0x0014)
#define COLOR_YELLOW    ((Color_t)0XFFE0)
#define COLOR_ORANGE    ((Color_t)0XFE00)
#define COLOR_MAGENTA   ((Color_t)0xF81F)
#define COLOR_CYAN      ((Color_t)0x07FF)

// If used as background color for char or text, the background will not filled
#define COLOR_NO_BACKGROUND   ((Color_t)0XFFFE)

#define BLUEMASK 0x1F
#define GET_RED(rgb) ((rgb & 0xF800) >> 8)
#define GET_GREEN(rgb) ((rgb & 0x07E0) >> 3)
#define GET_BLUE(rgb) ((rgb & 0x001F) << 3)
#define RGB(r,g,b)   ((Color_t)(((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3))) //5 red | 6 green | 5 blue

#endif /* COLORS16BIT_H_ */
