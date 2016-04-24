/*
 * TouchButton.cpp
 *
 * Renders touch buttons for lcd
 * A button can be a simple clickable text
 * or a box with or without text or even an invisible touch area
 *
 * @date  30.01.2012
 * @author  Armin Joachimsmeyer
 * armin.joachimsmeyer@gmail.com
 * @copyright LGPL v3 (http://www.gnu.org/licenses/lgpl.html)
 * @version 1.5.0
 *
 *  Local display interface used:
 * 		mDisplay->fillRectRel()
 * 		mDisplay->drawText()
 * 		LOCAL_DISPLAY_WIDTH
 * 		LOCAL_DISPLAY_HEIGHT
 *
 * 	Ram usage:
 * 		7 byte + 24 bytes per button
 *
 * 	Code size:
 * 		1,5 kByte
 *
 */

#include "TouchButton.h"
#include "BlueDisplay.h"

#include <stddef.h> // for NULL

TouchButton * TouchButton::sButtonListStart = NULL;
uint16_t TouchButton::sDefaultCaptionColor = TOUCHBUTTON_DEFAULT_CAPTION_COLOR;

TouchButton::TouchButton() {
//    mButtonColor = sDefaultButtonColor;
//    mCaptionColor = sDefaultCaptionColor;
//    mIsActive = false;
//    mPGMCaption = false;
	mNextObject = NULL;
	if (sButtonListStart == NULL) {
		// first button
		sButtonListStart = this;
	} else {
		// put object in button list
		TouchButton * tObjectPointer = sButtonListStart;
		// search last list element
		while (tObjectPointer->mNextObject != NULL) {
			tObjectPointer = tObjectPointer->mNextObject;
		}
		//insert actual button in last element
		tObjectPointer->mNextObject = this;
	}
}

void TouchButton::setDefaultCaptionColor(const uint16_t aDefaultCaptionColor) {
	sDefaultCaptionColor = aDefaultCaptionColor;
}

/*
 * Set parameters for touch button
 * if aWidthX == 0 render only text no background box
 * if aCaptionSize == 0 don't render anything, just check touch area -> transparent button ;-)
 */
int8_t TouchButton::initButton(uint16_t aPositionX, uint16_t aPositionY, uint16_t aWidthX, uint16_t aHeightY, uint16_t aButtonColor,
		const char * aCaption, uint8_t aCaptionSize, uint8_t aFlags, const int16_t aValue,
		void (*aOnTouchHandler)(TouchButton * const, int16_t)) {

	mWidthX = aWidthX;
	mHeightY = aHeightY;
	mButtonColor = aButtonColor;
	mCaptionColor = sDefaultCaptionColor;
	mCaption = aCaption;
	mCaptionSize = aCaptionSize;
	mOnTouchHandler = aOnTouchHandler;
	mValue = aValue;
#ifdef USE_BUTTON_POOL
	mFlags = (aFlags & (~INTERNAL_FLAG_MASK)) | (mFlags & INTERNAL_FLAG_MASK);
#else
	mFlags = aFlags;
#endif
	return setPosition(aPositionX, aPositionY);
}

int8_t TouchButton::setPosition(const uint16_t aPositionX, const uint16_t aPositionY) {
	int8_t tRetValue = 0;
	mPositionX = aPositionX;
	mPositionY = aPositionY;

	// check values
	if (aPositionX + mWidthX > LOCAL_DISPLAY_WIDTH) {
		mWidthX = LOCAL_DISPLAY_WIDTH - aPositionX;
		tRetValue = TOUCHBUTTON_ERROR_X_RIGHT;
	}
	if (aPositionY + mHeightY > LOCAL_DISPLAY_HEIGHT) {
		mHeightY = LOCAL_DISPLAY_HEIGHT - aPositionY;
		tRetValue = TOUCHBUTTON_ERROR_Y_BOTTOM;
	}
	return tRetValue;
}

/*
 * renders the button on lcd
 */
void TouchButton::drawButton() {
	// Draw rect
	LocalDisplay.fillRectRel(mPositionX, mPositionY, mWidthX, mHeightY, mButtonColor);
	drawCaption();
}

/**
 * deactivates the button and redraws its screen space with @a aBackgroundColor
 */
void TouchButton::removeButton(const uint16_t aBackgroundColor) {
	mFlags &= ~FLAG_IS_ACTIVE;
	// Draw rect
	LocalDisplay.fillRectRel(mPositionX, mPositionY, mWidthX, mHeightY, aBackgroundColor);

}

void TouchButton::drawCaption() {
	mFlags |= FLAG_IS_ACTIVE;

	if (mCaptionSize > 0) { // don't render anything if caption size == 0
		if (mCaption != NULL) {
			uint16_t tXCaptionPosition;
			uint16_t tYCaptionPosition;
			// try to position the string in the middle of the box
			uint8_t tLength = getCaptionLength((char *) mCaption);
			if (tLength >= mWidthX) { // unsigned arithmetic
				// String too long here
				tXCaptionPosition = mPositionX;
			} else {
				tXCaptionPosition = mPositionX + ((mWidthX - tLength) / 2);
			}
			tYCaptionPosition = mPositionY + ((mHeightY - mCaptionSize) / 2);
			if (mFlags & FLAG_BUTTON_CAPTION_IS_IN_PGMSPACE) {
				LocalDisplay.drawTextPGM(tXCaptionPosition, tYCaptionPosition, (char *) mCaption, getLocalTextSize(mCaptionSize), mCaptionColor,
						mButtonColor);
			} else {
				LocalDisplay.drawText(tXCaptionPosition, tYCaptionPosition, (char *) mCaption, getLocalTextSize(mCaptionSize), mCaptionColor,
						mButtonColor);
			}
		}
	}
}
/**
 * Check if touch event is in button area
 * if yes - return true
 * if no - return false
 */
bool TouchButton::checkButtonInArea(uint16_t aTouchPositionX, uint16_t aTouchPositionY) {
	if (aTouchPositionX < mPositionX || aTouchPositionX > mPositionX + mWidthX || aTouchPositionY < mPositionY
			|| aTouchPositionY > (mPositionY + mHeightY)) {
		return false;
	}
	return true;
}

/**
 * Check if touch event is in button area
 * if yes - call callback function and return true
 * if no - return false
 */
bool TouchButton::checkButton(uint16_t aTouchPositionX, uint16_t aTouchPositionY) {
	if ((mFlags & FLAG_IS_ACTIVE) && mOnTouchHandler != NULL && checkButtonInArea(aTouchPositionX, aTouchPositionY)) {
		/*
		 *  Touch position is in button - call callback function
		 */
		if (mFlags & BUTTON_FLAG_DO_BEEP_ON_TOUCH) {
			FeedbackToneOK();
		}
#ifdef REMOTE_DISPLAY_SUPPORTED
		if ((mFlags & FLAG_USE_BDBUTTON_FOR_CALLBACK)
				&& (&TouchButtonAutorepeat::autorepeatTouchHandler
						!= (void (*)(TouchButtonAutorepeat *, int16_t)) mOnTouchHandler)) {
			mOnTouchHandler((TouchButton *) this->mBDButtonPtr, mValue);
		} else {
			mOnTouchHandler(this, mValue);
		}
#else
		mOnTouchHandler(this, mValue);
#endif
		return true;
	}
	return false;
}

/*
 * Static convenience method - checks all buttons for matching touch position.
 */
uint8_t TouchButton::checkAllButtons(unsigned int aTouchPositionX, unsigned int aTouchPositionY) {
	TouchButton * tButtonPointer = sButtonListStart;
// walk through list
	while (tButtonPointer != NULL) {
		if ((tButtonPointer->mFlags & FLAG_IS_ACTIVE) && tButtonPointer->checkButton(aTouchPositionX, aTouchPositionY)) {
#ifdef REMOTE_DISPLAY_SUPPORTED
			sButtonTouched = true;
#endif
			return BUTTON_TOUCHED;
		}
		tButtonPointer = tButtonPointer->mNextObject;
	}
#ifdef REMOTE_DISPLAY_SUPPORTED
	sButtonTouched = false;
#endif
	return NOT_TOUCHED;
}

/*
 * Static convenience method - deactivate all buttons (e.g. before switching screen)
 */
void TouchButton::deactivateAllButtons() {
	TouchButton * tObjectPointer = sButtonListStart;
// walk through list
	while (tObjectPointer != NULL) {
		tObjectPointer->deactivate();
		tObjectPointer = tObjectPointer->mNextObject;
	}
}

/*
 * Static convenience method - activate all buttons
 */
void TouchButton::activateAllButtons() {
	TouchButton * tObjectPointer = sButtonListStart;
// walk through list
	while (tObjectPointer != NULL) {
		tObjectPointer->activate();
		tObjectPointer = tObjectPointer->mNextObject;
	}
}

uint8_t TouchButton::getCaptionLength(char * aCaptionPointer) const {
	uint8_t tLength = 0;
	uint8_t tFontWidth = getTextWidth(mCaptionSize);
	if (mFlags & FLAG_BUTTON_CAPTION_IS_IN_PGMSPACE) {
		while (pgm_read_byte(aCaptionPointer++) != 0) {
			tLength += (tFontWidth);
		}
	} else {
		while (*aCaptionPointer++ != 0) {
			tLength += (tFontWidth);
		}
	}
	return tLength;
}
# ifdef DEBUG
/*
 * for debug purposes
 * needs char aStringBuffer[23+<CaptionLength>]
 */
void TouchButton::toString(char * aStringBuffer) const {
	sprintf(aStringBuffer, "X=%03u Y=%03u X1=%03u Y1=%03u B=%02u %s", mPositionX, mPositionY, mPositionX + mWidthX - 1,
			mPositionY + mHeightY - 1, mTouchBorder, mCaption);
}
# endif

const char * TouchButton::getCaption() const {
	return mCaption;
}

uint16_t TouchButton::getValue() const {
	return mValue;
}

/*
 * Set caption
 */
void TouchButton::setCaption(const char * aCaption) {
	mFlags &= ~FLAG_BUTTON_CAPTION_IS_IN_PGMSPACE;
	mCaption = aCaption;
}
void TouchButton::setCaptionPGM(PGM_P aCaption) {
	mFlags |= FLAG_BUTTON_CAPTION_IS_IN_PGMSPACE;
	mCaption = aCaption;
}

/*
 * changes box color and redraws button
 */
void TouchButton::setButtonColor(const uint16_t aColor) {
	mButtonColor = aColor;
}

void TouchButton::setCaptionColor(const uint16_t aColor) {
	mCaptionColor = aColor;
}

void TouchButton::setValue(const int16_t aValue) {
	mValue = aValue;

}
uint16_t TouchButton::getPositionX() const {
	return mPositionX;
}

uint16_t TouchButton::getPositionY() const {
	return mPositionY;
}
uint16_t TouchButton::getPositionXRight() const {
	return mPositionX + mWidthX - 1;
}

uint16_t TouchButton::getPositionYBottom() const {
	return mPositionY + mHeightY - 1;
}

/*
 * activate for touch checking
 */
void TouchButton::activate() {
	mFlags |= FLAG_IS_ACTIVE;
}

/*
 * deactivate for touch checking
 */
void TouchButton::deactivate() {
	mFlags &= ~FLAG_IS_ACTIVE;
}
