/*
 * chart.h
 *
 * Draws charts for LCD screen
 * Charts have axes and a data area
 * Data can be printed as pixels or line or area
 * Labels and grid are optional
 *
 *  Created on:  14.02.2012
 * @author Armin Joachimsmeyer
 *      Email:   armin.joachimsmeyer@gmail.com
 * @copyright LGPL v3 (http://www.gnu.org/licenses/lgpl.html)
 * @version 1.5.0
 *
 */

#ifndef CHART_H_
#define CHART_H_

#include "MI0283QT2.h"
#include "BlueDisplay.h"

#include <stdint.h>

#define CHART_DEFAULT_AXES_COLOR 		COLOR_BLACK
#define CHART_DEFAULT_GRID_COLOR 		RGB( 180, 180, 180)
#define CHART_DEFAULT_BACKGROUND_COLOR  COLOR_WHITE
#define CHART_DEFAULT_LABEL_COLOR  		COLOR_BLACK
#define CHART_MAX_AXES_SIZE 			10

// data drawing modes
#define CHART_MODE_PIXEL 				0
#define CHART_MODE_LINE 				1
#define CHART_MODE_AREA 				2

// Error codes
#define CHART_ERROR_POS_X 		-1
#define CHART_ERROR_POS_Y 		-2
#define CHART_ERROR_WIDTH 		-4
#define CHART_ERROR_HEIGHT		-8
#define CHART_ERROR_AXES_SIZE	-16
#define CHART_ERROR_GRID_X_RESOLUTION -32

// Bit positions in mFlags
#define CHART_HAS_GRID 0
#define CHART_X_LABEL_USED 2
#define CHART_X_LABEL_INT 3
#define CHART_Y_LABEL_USED 4
#define CHART_Y_LABEL_INT 5

typedef union {
	int IntValue;
	float FloatValue;
} int_float_union;

class Chart {
public:

	Chart();
	uint8_t initChart(const uint16_t aPositionX, const uint16_t aPositionY, const uint16_t aWidthX, const uint16_t aHeightY,
			const uint8_t aAxesSize, const bool aHasGrid, const uint8_t aGridXResolution, const uint8_t aGridYResolution);
	void initXLabelInt(const int aXLabelStartValue, const int aXLabelIncrementValue, const uint8_t aXMinStringWidth);
	bool stepXLabelInt(const bool aDoIncrement, const int aMinValue, const int aMaxValue);
	void initXLabelFloat(const float aXLabelStartValue, const float aXLabelIncrementValue, uint8_t aXMinStringWidthIncDecimalPoint,
			uint8_t aXNumVarsAfterDecimal);
	bool stepXLabelFloat(const bool aDoIncrement);
	void initYLabelInt(const int aYLabelStartValue, const int aYLabelIncrementValue, const uint8_t aMaxYLabelCharacters);
	bool stepYLabelInt(const bool aDoIncrement, const int aMinValue, const int aMaxValue);
	void initYLabelFloat(const float aYLabelStartValue, const float aYLabelIncrementValue, uint8_t aYMinStringWidthIncDecimalPoint,
			uint8_t aYNumVarsAfterDecimal);
	float stepYLabelFloat(const bool aDoIncrement);
	void initChartColors(const uint16_t aAxesColor, const uint16_t aGridColor, const uint16_t aLabelColor,
			const uint16_t aBackgroundColor);
	uint8_t drawChart(void);
	bool drawChartData(uint8_t *aDataPointer, const uint16_t aDataLength, const uint16_t aDataColor, const uint8_t aMode);
	void drawGrid(void);
	uint8_t drawXAxis(bool aClearLabelsBefore);
	uint8_t drawYAxis(bool aClearLabelsBefore);
	uint8_t drawAxes(bool aClearLabelsBefore);
	void clear(void);
	uint16_t getHeightY() const;
	uint16_t getPositionX() const;
	uint16_t getPositionY() const;
	uint16_t getWidthX() const;
	void setHeightY(const uint16_t heightY);
	void setPositionX(const uint16_t positionX);
	void setPositionY(const uint16_t positionY);
	void setWidthX(const uint16_t widthX);
	void setXLabelStartValue(int xLabelStartValue);
	void setXLabelStartValueFloat(float xLabelStartValueFloat);
	void setYLabelStartValue(int yLabelStartValue);
	void setYLabelStartValueFloat(float yLabelStartValueFloat);
	void setXLabelIncrementValue(int xLabelIncrementValue);
	void setXLabelIncrementValueFloat(float xLabelIncrementValueFloat);
	void setYLabelIncrementValue(int yLabelIncrementValue);
	void setYLabelIncrementValueFloat(float yLabelIncrementValueFloat);
	int_float_union getXLabelStartValue() const;
	int_float_union getYLabelStartValue() const;
private:

	BlueDisplay * mDisplay; // The Display to use

	uint16_t mPositionX;
	uint16_t mPositionY;
	uint16_t mWidthX;
	uint16_t mHeightY;
	uint16_t mChartBackgroundColor;
	uint8_t mAxesSize;
	uint16_t mAxesColor;
	uint8_t mFlags;
	uint8_t mGridXResolution;
	uint8_t mGridYResolution;
	uint16_t mGridColor;
	int_float_union mXLabelStartValue;
	int_float_union mXLabelIncrementValue;
	int_float_union mYLabelStartValue;
	int_float_union mYLabelIncrementValue;

	uint8_t mXNumVarsAfterDecimal;
	uint8_t mYNumVarsAfterDecimal;
	uint8_t mXMinStringWidth;
	uint8_t mYMinStringWidth;
	uint16_t mLabelColor;

	uint8_t checkParameterValues();
};

#endif /* CHART_H_ */
