#ifndef MI0283QT2_h
#define MI0283QT2_h

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#include <avr/pgmspace.h>
#include "fonts.h"
#ifdef __cplusplus
}
#endif
#include "Print.h"
#include "Colors16Bit.h"

#ifndef DEC
# define DEC (10)
#endif
#ifndef HEX
# define HEX (16)
#endif
#ifndef OCT
# define OCT (8)
#endif
#ifndef BIN
# define BIN (2)
#endif

// Landscape format
static const unsigned int LOCAL_DISPLAY_HEIGHT = 240;
static const unsigned int LOCAL_DISPLAY_WIDTH = 320;

class MI0283QT2: public Print { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	uint16_t lcd_orientation;
	uint16_t lcd_width, lcd_height;

	MI0283QT2();
	void init(uint8_t clock_div); //2 4 8 16 32
	void led(uint8_t power); //0-100

	void setOrientation(uint16_t o); //0 90 180 270
	uint16_t getWidth(void);
	uint16_t getHeight(void);
	void setArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	void setCursor(uint16_t x, uint16_t y);

	void clearDisplay(uint16_t color);
	void drawStart(void);
	inline void draw(uint16_t color);
	inline void drawStop(void);
	void drawPixel(uint16_t x0, uint16_t y0, uint16_t color);
	void drawPixelFast(uint16_t x0, uint8_t y0, uint16_t color);
	void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
	void drawLineFastOneX(uint16_t x0, uint16_t y0, uint16_t y1, uint16_t color);
	void drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
	void fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
	void fillRectRel(uint16_t aXStart, uint16_t aYStart, uint16_t aWidth, uint16_t aHeight, uint16_t aColor);
	void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);
	void fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);

	uint16_t drawChar(uint16_t x, uint16_t y, char c, uint8_t size, uint16_t color, uint16_t bg_color);
	uint16_t drawText(uint16_t x, uint16_t y, const char *s, uint8_t size, uint16_t color, uint16_t bg_color);
	uint16_t drawText(uint16_t x, uint16_t y, int i, uint8_t size, uint16_t color, uint16_t bg_color);
	uint16_t drawText(uint16_t x, uint16_t y, unsigned int i, uint8_t size, uint16_t color, uint16_t bg_color);
	uint16_t drawText(uint16_t x, uint16_t y, long l, uint8_t size, uint16_t color, uint16_t bg_color);
	uint16_t drawText(uint16_t x, uint16_t y, unsigned long l, uint8_t size, uint16_t color, uint16_t bg_color);
	uint16_t drawText(uint16_t x, uint16_t y, String &s, uint8_t size, uint16_t color, uint16_t bg_color);
	uint16_t drawTextPGM(uint16_t x, uint16_t y, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color);

	uint16_t drawMLText(uint16_t aPosX, uint16_t aPosY, const char *aStringPtr, uint8_t aTextSize, uint16_t aColor, uint16_t aBGColor);
	uint16_t drawMLTextPGM(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color);

	uint16_t drawInteger(uint16_t x, uint16_t y, char val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color); //base = DEC, HEX, OCT, BIN
	uint16_t drawInteger(uint16_t x, uint16_t y, unsigned char val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color);
	uint16_t drawInteger(uint16_t x, uint16_t y, int val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color);
	uint16_t drawInteger(uint16_t x, uint16_t y, long val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color);

	void printOptions(uint8_t size, uint16_t color, uint16_t bg_color);
	void printClear(void);
	void printXY(uint16_t x, uint16_t y);
	uint16_t printGetX(void);
	uint16_t printGetY(void);
	void printPGM(PGM_P s);
	virtual size_t write(uint8_t c);
	virtual size_t write(const char *s);
	virtual size_t write(const uint8_t *s, size_t size);

private:
	uint8_t p_size;
	uint16_t p_fg, p_bg;
	uint16_t p_x, p_y;

    void reset(void);
	void wr_cmd(uint8_t reg, uint8_t param);
	void wr_data(uint16_t data);
	void wr_spi(uint8_t data);
	void delay_10ms(uint8_t ms);
};

/*
 * fast divide by 11 for MI0283QT2 driver arguments
 */
uint16_t getLocalTextSize(uint16_t aTextSize);


/*
 * MI0283QT2 TFTDisplay - must provided by main program
 * external declaration saves ROM (210 Bytes) and RAM ( 20 Bytes)
 * and avoids missing initialization :-)
 */
#define TOUCHGUI_SAVE_SPACE

#ifdef TOUCHGUI_SAVE_SPACE
extern MI0283QT2 LocalDisplay;
#endif
#endif //MI0283QT2_h
