/*
 * Chart.cpp
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
 *  LCD interface used:
 * 		fillRect()
 * 		drawText()
 * 		drawPixel()
 * 		drawLine()
 * 		TEXT_SIZE_11_WIDTH
 * 		TEXT_SIZE_11_HEIGHT
 *
 * 	Ram usage:
 * 		2 Byte + 56 Bytes per chart
 *
 * 	Code size:
 * 		3 kByte
 *
 */

#include "Chart.h"

#include <stdlib.h> // for ltoa + dtostrf


Chart::Chart() {
    mChartBackgroundColor = CHART_DEFAULT_BACKGROUND_COLOR;
    mAxesColor = CHART_DEFAULT_AXES_COLOR;
    mGridColor = CHART_DEFAULT_GRID_COLOR;
    mLabelColor = CHART_DEFAULT_LABEL_COLOR;
    mFlags = 0;
}

void Chart::initChartColors(const uint16_t aAxesColor, const uint16_t aGridColor, const uint16_t aLabelColor,
        const uint16_t aBackgroundColor) {
    mAxesColor = aAxesColor;
    mGridColor = aGridColor;
    mLabelColor = aLabelColor;
    mChartBackgroundColor = aBackgroundColor;
}

/*
 * aPositionX and aPositionY are the 0 coordinates of the grid
 */
uint8_t Chart::initChart(const uint16_t aPositionX, const uint16_t aPositionY, const uint16_t aWidthX,
        const uint16_t aHeightY, const uint8_t aAxesSize, const bool aHasGrid, const uint8_t aGridXResolution,
        const uint8_t aGridYResolution) {
    mPositionX = aPositionX;
    mPositionY = aPositionY;
    mWidthX = aWidthX;
    mHeightY = aHeightY;
    mAxesSize = aAxesSize;
    if (aHasGrid) {
        mFlags |= (1 << CHART_HAS_GRID);
    }
    mGridXResolution = aGridXResolution;
    mGridYResolution = aGridYResolution;

    return checkParameterValues();
}

uint8_t Chart::checkParameterValues() {
    uint8_t tRetValue = 0;
    // also check for zero :-)
    if (mAxesSize - 1 > CHART_MAX_AXES_SIZE) {
        mAxesSize = CHART_MAX_AXES_SIZE;
        tRetValue = CHART_ERROR_AXES_SIZE;
    }
    uint16_t t2AxesSize = 2 * mAxesSize;
    if (mPositionX < t2AxesSize - 1) {
        mPositionX = t2AxesSize - 1;
        mWidthX = 100;
        tRetValue = CHART_ERROR_POS_X;
    }
    if (mPositionY >  LOCAL_DISPLAY_HEIGHT - t2AxesSize) {
        mPositionY =  LOCAL_DISPLAY_HEIGHT - t2AxesSize;
        tRetValue = CHART_ERROR_POS_Y;
    }
    if (mPositionX + mWidthX >  LOCAL_DISPLAY_WIDTH) {
        mPositionX = 0;
        mWidthX = 100;
        tRetValue = CHART_ERROR_WIDTH;
    }

    if (mHeightY > mPositionY + 1) {
        mHeightY = mPositionY + 1;
        tRetValue = CHART_ERROR_HEIGHT;
    }

    if (mGridXResolution > mWidthX) {
        mGridXResolution = mWidthX / 2;
        tRetValue = CHART_ERROR_GRID_X_RESOLUTION;
    }
    return tRetValue;

}

void Chart::initXLabelInt(const int aXLabelStartValue, const int aXLabelIncrementValue,
        const uint8_t aXMinStringWidth) {
    mXLabelStartValue.IntValue = aXLabelStartValue;
    mXLabelIncrementValue.IntValue = aXLabelIncrementValue;
    mXMinStringWidth = aXMinStringWidth;
    mFlags |= (1 << CHART_X_LABEL_INT) | (1 << CHART_X_LABEL_USED);
}

void Chart::initXLabelFloat(const float aXLabelStartValue, const float aXLabelIncrementValue,
        uint8_t aXMinStringWidthIncDecimalPoint, uint8_t aXNumVarsAfterDecimal) {
    mXLabelStartValue.FloatValue = aXLabelStartValue;
    mXLabelIncrementValue.FloatValue = aXLabelIncrementValue;
    mXNumVarsAfterDecimal = aXNumVarsAfterDecimal;
    mXMinStringWidth = aXMinStringWidthIncDecimalPoint;
    mFlags &= ~(1 << CHART_X_LABEL_INT);
    mFlags |= (1 << CHART_X_LABEL_USED);
}

void Chart::initYLabelInt(const int aYLabelStartValue, const int aYLabelIncrementValue,
        const uint8_t aYMinStringWidth) {
    mYLabelStartValue.IntValue = aYLabelStartValue;
    mYLabelIncrementValue.IntValue = aYLabelIncrementValue;
    mYMinStringWidth = aYMinStringWidth;
    mFlags |= (1 << CHART_Y_LABEL_INT) | (1 << CHART_Y_LABEL_USED);
}
void Chart::initYLabelFloat(const float aYLabelStartValue, const float aYLabelIncrementValue,
        uint8_t aYMinStringWidthIncDecimalPoint, uint8_t aYNumVarsAfterDecimal) {
    mYLabelStartValue.FloatValue = aYLabelStartValue;
    mYLabelIncrementValue.FloatValue = aYLabelIncrementValue;
    mYNumVarsAfterDecimal = aYNumVarsAfterDecimal;
    mYMinStringWidth = aYMinStringWidthIncDecimalPoint;
    mFlags &= ~(1 << CHART_Y_LABEL_INT);
    mFlags |= (1 << CHART_Y_LABEL_USED);
}

/*
 * Render the chart on the lcd
 */
uint8_t Chart::drawChart(void) {
    uint8_t tRetValue = drawAxes(false);
    drawGrid();
    return tRetValue;
}

void Chart::drawGrid(void) {
    if (!mFlags & (1 << CHART_HAS_GRID)) {
        return;
    }
    uint16_t tOffset;
// draw vertical lines
    for (tOffset = mGridXResolution; tOffset <= mWidthX; tOffset += mGridXResolution) {
        mDisplay->fillRect(mPositionX + tOffset, mPositionY - 1, mPositionX + tOffset, mPositionY - mHeightY + 1,
                mGridColor);
    }
// draw horizontal lines
    for (tOffset = mGridYResolution; tOffset <= mHeightY; tOffset += mGridYResolution) {
        mDisplay->fillRect(mPositionX + 1, mPositionY - tOffset, mPositionX + mWidthX - 1, mPositionY - tOffset,
                mGridColor);
    }

}

/*
 * render axes
 * renders indicators if labels but no grid are specified
 * render labels only if at least one increment value != 0
 * returns -11 in no space for X labels or - 12 if no space for Y labels on lcd
 */
uint8_t Chart::drawAxes(bool aClearLabelsBefore) {
    uint8_t tRetValue = drawXAxis(aClearLabelsBefore);
    tRetValue += drawYAxis(aClearLabelsBefore);
    return tRetValue;
}

/**
 * render X AXIS only if integer or float increment value != 0
 */
uint8_t Chart::drawXAxis(bool aClearLabelsBefore) {

    char tLabelStringBuffer[32];

// draw X line
    mDisplay->fillRect(mPositionX - mAxesSize + 1, mPositionY, mPositionX + mWidthX - 1, mPositionY + mAxesSize - 1,
            mAxesColor);

    if (mFlags & (1 << CHART_X_LABEL_USED)) {
        uint16_t tOffset;
        uint16_t tNumberYTop = mPositionY + mAxesSize + 1;
        if (!mFlags & (1 << CHART_HAS_GRID)) {
            tNumberYTop += mAxesSize;
            // draw indicators
            for (tOffset = 0; tOffset <= mWidthX; tOffset += mGridXResolution) {
                mDisplay->fillRect(mPositionX + tOffset, mPositionY + mAxesSize, mPositionX + tOffset, tNumberYTop - 2,
                        mGridColor);
            }
        }

        // draw labels
        if (tNumberYTop >  LOCAL_DISPLAY_HEIGHT - TEXT_SIZE_11_HEIGHT) {
            // no space for labels
            return -10;
        }
        // first offset is negative
        tOffset = 1 - ((TEXT_SIZE_11_WIDTH * mXMinStringWidth) / 2);
        if (aClearLabelsBefore) {
            // clear label space before
            mDisplay->fillRect(mPositionX + tOffset, tNumberYTop, mPositionX + mWidthX - 1,
                    tNumberYTop + TEXT_SIZE_11_HEIGHT - 1, mChartBackgroundColor);
        }

        // initialize both variables to avoid compiler warnings
        long tValue = mXLabelStartValue.IntValue;
        float tValueFloat = mXLabelStartValue.FloatValue;
        if (mFlags & (1 << CHART_X_LABEL_INT)) {
            ltoa(tValue, tLabelStringBuffer, 10);
        } else {
            dtostrf(tValueFloat, mXMinStringWidth, mXNumVarsAfterDecimal, tLabelStringBuffer);
        }
        mDisplay->drawText(mPositionX + tOffset, tNumberYTop, tLabelStringBuffer, 1, mLabelColor,
                mChartBackgroundColor);
        tOffset += mGridXResolution;

        for (; tOffset <= mWidthX; tOffset += mGridXResolution) {
            if (mFlags & (1 << CHART_X_LABEL_INT)) {
                tValue += mXLabelIncrementValue.IntValue;
                ltoa(tValue, tLabelStringBuffer, 10);
            } else {
                tValueFloat += mXLabelIncrementValue.FloatValue;
                dtostrf(tValueFloat, mXMinStringWidth, mXNumVarsAfterDecimal, tLabelStringBuffer);
            }
            mDisplay->drawText(mPositionX + tOffset, tNumberYTop, tLabelStringBuffer, 1, mLabelColor,
                    mChartBackgroundColor);
        }
    }
    return 0;
}

/*
 * If aDoIncrement = true increment X values , else decrement
 * redraw Axis
 * return true if X value was not clipped
 */
bool Chart::stepXLabelInt(const bool aDoIncrement, const int aMinValue, const int aMaxValue) {
    bool tRetval = true;
    if (aDoIncrement) {
        mXLabelStartValue.IntValue += mXLabelIncrementValue.IntValue;
        if (mXLabelStartValue.IntValue > aMaxValue) {
            mXLabelStartValue.IntValue = aMaxValue;
            tRetval = false;
        }
    } else {
        mXLabelStartValue.IntValue -= mXLabelIncrementValue.IntValue;
        if (mXLabelStartValue.IntValue < aMinValue) {
            mXLabelStartValue.IntValue = aMinValue;
            tRetval = false;
        }
    }
    drawXAxis(true);
    return tRetval;
}

/*
 * Increment X Values if aDoIncrement = true
 * redraw Axis
 * return false if decrement would set value below 0
 */
bool Chart::stepXLabelFloat(const bool aDoIncrement) {
    if (aDoIncrement) {
        mXLabelStartValue.FloatValue += mXLabelIncrementValue.FloatValue;
    } else {
        mXLabelStartValue.FloatValue -= mXLabelIncrementValue.FloatValue;
    }
    if (mXLabelStartValue.FloatValue < 0) {
        mXLabelStartValue.FloatValue = 0;
    }
    drawXAxis(true);
    return true;
}

/*
 * render Y AXIS only if integer or float increment value != 0
 */
uint8_t Chart::drawYAxis(bool aClearLabelsBefore) {

    char tLabelStringBuffer[32];

//draw y line
    mDisplay->fillRect(mPositionX - mAxesSize + 1, mPositionY - mHeightY + 1, mPositionX, mPositionY - 1, mAxesColor);

    if (mFlags & (1 << CHART_Y_LABEL_USED)) {
        uint16_t tOffset;
        uint16_t tNumberXLeft = mPositionX - mAxesSize - 1;
        if (!mFlags & (1 << CHART_HAS_GRID)) {
            tNumberXLeft -= mAxesSize;
            // draw indicators
            for (tOffset = 0; tOffset <= mHeightY; tOffset += mGridYResolution) {
                mDisplay->fillRect(tNumberXLeft + 2, mPositionY - tOffset, mPositionX - mAxesSize, mPositionY - tOffset,
                        mGridColor);
            }
        }
        tNumberXLeft -= (mYMinStringWidth * TEXT_SIZE_11_WIDTH);

        // draw labels (numbers)
        // unsigned arithmetic
        if (tNumberXLeft >  LOCAL_DISPLAY_WIDTH) {
            // no space for labels
            return -11;
        }

        // first offset is negative
        tOffset = TEXT_SIZE_11_HEIGHT / 2;
        if (aClearLabelsBefore) {
            // clear label space before
            mDisplay->fillRect(tNumberXLeft, mPositionY - mHeightY + 1, mPositionX - mAxesSize - 1,
                    mPositionY - tOffset + TEXT_SIZE_11_HEIGHT, mChartBackgroundColor);
        }

        // convert to string
        // initialize both variables to avoid compiler warnings
        long tValue = mYLabelStartValue.IntValue;
        float tValueFloat = mYLabelStartValue.FloatValue;
        if (mFlags & (1 << CHART_Y_LABEL_INT)) {
            ltoa(tValue, tLabelStringBuffer, 10);
        } else {
            dtostrf(tValueFloat, mYMinStringWidth, mYNumVarsAfterDecimal, tLabelStringBuffer);
        }
        mDisplay->drawText(tNumberXLeft, mPositionY - tOffset, tLabelStringBuffer, 1, mLabelColor,
                mChartBackgroundColor);
        tOffset += mGridYResolution;

        for (; tOffset <= mHeightY; tOffset += mGridYResolution) {
            if (mFlags & (1 << CHART_Y_LABEL_INT)) {
                tValue += mYLabelIncrementValue.IntValue;
                ltoa(tValue, tLabelStringBuffer, 10);
            } else {
                tValueFloat += mYLabelIncrementValue.FloatValue;
                dtostrf(tValueFloat, mYMinStringWidth, mYNumVarsAfterDecimal, tLabelStringBuffer);
            }
            mDisplay->drawText(tNumberXLeft, mPositionY - tOffset, tLabelStringBuffer, 1, mLabelColor,
                    mChartBackgroundColor);
        }
    }
    return 0;
}

bool Chart::stepYLabelInt(const bool aDoIncrement, const int aMinValue, const int aMaxValue) {
    bool tRetval = true;
    if (aDoIncrement) {
        mYLabelStartValue.IntValue += mYLabelIncrementValue.IntValue;
        if (mYLabelStartValue.IntValue > aMaxValue) {
            mYLabelStartValue.IntValue = aMaxValue;
            tRetval = false;
        }
    } else {
        mYLabelStartValue.IntValue -= mYLabelIncrementValue.IntValue;
        if (mYLabelStartValue.IntValue < aMinValue) {
            mYLabelStartValue.IntValue = aMinValue;
            tRetval = false;
        }
    }
    drawYAxis(true);
    return tRetval;
}

float Chart::stepYLabelFloat(const bool aDoIncrement) {
    if (aDoIncrement) {
        mYLabelStartValue.FloatValue += mYLabelIncrementValue.FloatValue;
    } else {
        mYLabelStartValue.FloatValue -= mYLabelIncrementValue.FloatValue;
    }
    if (mYLabelStartValue.FloatValue < 0) {
        mYLabelStartValue.FloatValue = 0;
    }
    drawYAxis(false);
    return mYLabelStartValue.FloatValue;
}

/*
 * Clear chart area (axes are not included)
 */
void Chart::clear(void) {
    mDisplay->fillRect(mPositionX + 1, mPositionY - 1, mPositionX + mWidthX - 1, mPositionY - mHeightY + 1,
            mChartBackgroundColor);

}

/*
 * Draws the line and returns false if clipping occurs
 */
bool Chart::drawChartData(uint8_t * aDataPointer, uint16_t aDataLength, const uint16_t aDataColor,
        const uint8_t aMode) {

    bool tRetValue = true;
    uint8_t tValue;

    if (aDataLength > mWidthX) {
        aDataLength = mWidthX;
        tRetValue = false;
    }

// used only in line mode
    uint8_t tLastValue = *aDataPointer;
    if (tLastValue > mHeightY - 1) {
        tLastValue = mHeightY - 1;
        tRetValue = false;
    }

    uint16_t tXpos = mPositionX;

    for (; aDataLength > 0; aDataLength--) {
        tValue = *aDataPointer++;
        if (tValue > mHeightY - 1) {
            tValue = mHeightY - 1;
            tRetValue = false;
        }
        if (aMode == CHART_MODE_PIXEL) {
            tXpos++;
            mDisplay->drawPixel(tXpos, mPositionY - tValue, aDataColor);
        } else if (aMode == CHART_MODE_LINE) {
            mDisplay->drawLine(tXpos, mPositionY - tLastValue, tXpos + 1, mPositionY - tValue, aDataColor);
            tXpos++;
            tLastValue = tValue;
        } else if (aMode == CHART_MODE_AREA) {
            tXpos++;
            mDisplay->drawLine(tXpos, mPositionY, tXpos, mPositionY - tValue, aDataColor);
        }
    }
    return tRetValue;
}

uint16_t Chart::getHeightY() const {
    return mHeightY;
}

uint16_t Chart::getPositionX() const {
    return mPositionX;
}

uint16_t Chart::getPositionY() const {
    return mPositionY;
}

uint16_t Chart::getWidthX() const {
    return mWidthX;
}

void Chart::setHeightY(uint16_t heightY) {
    mHeightY = heightY;
}

void Chart::setPositionX(uint16_t positionX) {
    mPositionX = positionX;
}

void Chart::setPositionY(uint16_t positionY) {
    mPositionY = positionY;
}

void Chart::setWidthX(uint16_t widthX) {
    mWidthX = widthX;
}

void Chart::setXLabelStartValue(int xLabelStartValue) {
    mXLabelStartValue.IntValue = xLabelStartValue;
}

void Chart::setXLabelStartValueFloat(float xLabelStartValueFloat) {
    mXLabelStartValue.FloatValue = xLabelStartValueFloat;
}

void Chart::setYLabelStartValue(int yLabelStartValue) {
    mYLabelStartValue.IntValue = yLabelStartValue;
}

void Chart::setYLabelStartValueFloat(float yLabelStartValueFloat) {
    mYLabelStartValue.FloatValue = yLabelStartValueFloat;
}

void Chart::setXLabelIncrementValue(int xLabelIncrementValue) {
    mXLabelIncrementValue.IntValue = xLabelIncrementValue;
}

void Chart::setXLabelIncrementValueFloat(float xLabelIncrementValueFloat) {
    mXLabelIncrementValue.FloatValue = xLabelIncrementValueFloat;
}

void Chart::setYLabelIncrementValue(int yLabelIncrementValue) {
    mYLabelIncrementValue.IntValue = yLabelIncrementValue;
}

void Chart::setYLabelIncrementValueFloat(float yLabelIncrementValueFloat) {
    mYLabelIncrementValue.FloatValue = yLabelIncrementValueFloat;
}

int_float_union Chart::getXLabelStartValue() const {
    return mXLabelStartValue;
}

int_float_union Chart::getYLabelStartValue() const {
    return mYLabelStartValue;
}

