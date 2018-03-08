#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#ifdef __cplusplus
}
#endif
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <digitalWriteFast.h>
#include "ADS7846.h"
#include <MI0283QT2.h>

#define LCD_WIDTH       (320)
#define LCD_HEIGHT      (240)

#define CMD_START       (0x80)
#define CMD_12BIT       (0x00)
#define CMD_8BIT        (0x08)
#define CMD_DIFF        (0x00)
#define CMD_SINGLE      (0x04)

#define CHANNEL_MASK 0x70
// Power modes
#define CMD_PWD         (0x00)
#define ADC_ON    		(0x01)
#define REF_ON    		(0x02)
#define CMD_ALWAYSON    (0x03)
// Set 2,5V reference on. Only useful when using readChannel(), otherwise take CMD_PWD!
#define POWER_MODE CMD_ALWAYSON

//#define SOFTWARE_SPI
#ifdef __AVR_ATmega32U4__
#define SOFTWARE_SPI
#endif

#if (defined(__AVR_ATmega1280__) || \
     defined(__AVR_ATmega1281__) || \
     defined(__AVR_ATmega2560__) || \
     defined(__AVR_ATmega2561__))      //--- Arduino Mega ---
#define SOFTWARE_SPI
//# define BUSY_PIN       (5)
//# define IRQ_PIN        (3)
# define CS_PIN         (6)
# if defined(SOFTWARE_SPI)
#  define MOSI_PIN      (11)
#  define MISO_PIN      (12)
#  define CLK_PIN       (13)
# else
#  define MOSI_PIN      (51)
#  define MISO_PIN      (50)
#  define CLK_PIN       (52)
# endif

#elif (defined(__AVR_ATmega644__) || \
       defined(__AVR_ATmega644P__))    //--- Arduino 644 ---
//# define BUSY_PIN       (15)
//# define IRQ_PIN        (11)
# define CS_PIN         (14)
# define MOSI_PIN       (5)
# define MISO_PIN       (6)
# define CLK_PIN        (7)

#else                                  //--- Arduino Uno ---
//# define BUSY_PIN       (5)
//# define IRQ_PIN        (3)
# define CS_PIN         (6)
# define MOSI_PIN       (11)
# define MISO_PIN       (12)
# define CLK_PIN        (13)

#endif

#define CS_DISABLE()    digitalWriteFast(CS_PIN, HIGH)
#define CS_ENABLE()     digitalWriteFast(CS_PIN, LOW)

#define MOSI_HIGH()     digitalWriteFast(MOSI_PIN, HIGH)
#define MOSI_LOW()      digitalWriteFast(MOSI_PIN, LOW)

#define MISO_READ()     digitalReadFast(MISO_PIN)

#define CLK_HIGH()      digitalWriteFast(CLK_PIN, HIGH)
#define CLK_LOW()       digitalWriteFast(CLK_PIN, LOW)

//#define BUSY_READ()     digitalReadFast(BUSY_PIN)

//#define IRQ_READ()      digitalReadFast(IRQ_PIN)

//-------------------- Constructor --------------------

ADS7846::ADS7846(void) { // @suppress("Class members should be properly initialized")
    return;
}

//-------------------- Public --------------------

void ADS7846::init(void) {
    //init pins
    pinMode(CS_PIN, OUTPUT);
    CS_DISABLE();
    pinMode(CLK_PIN, OUTPUT);
    pinMode(MOSI_PIN, OUTPUT);
    pinMode(MISO_PIN, INPUT);
    digitalWriteFast(MISO_PIN, HIGH);
    //pull-up
#ifdef IRQ_PIN
    pinMode(IRQ_PIN, INPUT);
    digitalWriteFast(IRQ_PIN, HIGH); //pull-up
#endif
#ifdef BUSY_PIN
    pinMode(BUSY_PIN, INPUT);
    digitalWriteFast(BUSY_PIN, HIGH); //pull-up
#endif

#if !defined(SOFTWARE_SPI)
    //SS has to be output or input with pull-up
# if (defined(__AVR_ATmega1280__) || \
      defined(__AVR_ATmega1281__) || \
      defined(__AVR_ATmega2560__) || \
      defined(__AVR_ATmega2561__))     //--- Arduino Mega ---
#  define SS_PORTBIT (0) //PB0
# elif (defined(__AVR_ATmega644__) || \
        defined(__AVR_ATmega644P__))   //--- Arduino 644 ---
#  define SS_PORTBIT (4) //PB4
# else                                 //--- Arduino Uno ---
#  define SS_PORTBIT (2) //PB2
# endif
    if (!(DDRB & (1 << SS_PORTBIT))) //SS is input
    {
        PORTB |= (1 << SS_PORTBIT); //pull-up on
    }

#endif

    //set vars
    tp_matrix.div = 0;
    tp.x = 0;
    tp.y = 0;
    tp_last.x = 0;
    tp_last.y = 0;
    lcd.x = 0;
    lcd.y = 0;
    pressure = 0;
    return;
}

uint8_t ADS7846::setCalibration(CAL_POINT *lcd, CAL_POINT *tp) {
    tp_matrix.div = ((tp[0].x - tp[2].x) * (tp[1].y - tp[2].y)) - ((tp[1].x - tp[2].x) * (tp[0].y - tp[2].y));

    if (tp_matrix.div == 0) {
        return 0;
    }

    tp_matrix.a = ((lcd[0].x - lcd[2].x) * (tp[1].y - tp[2].y)) - ((lcd[1].x - lcd[2].x) * (tp[0].y - tp[2].y));

    tp_matrix.b = ((tp[0].x - tp[2].x) * (lcd[1].x - lcd[2].x)) - ((lcd[0].x - lcd[2].x) * (tp[1].x - tp[2].x));

    tp_matrix.c = (tp[2].x * lcd[1].x - tp[1].x * lcd[2].x) * tp[0].y
            + (tp[0].x * lcd[2].x - tp[2].x * lcd[0].x) * tp[1].y + (tp[1].x * lcd[0].x - tp[0].x * lcd[1].x) * tp[2].y;

    tp_matrix.d = ((lcd[0].y - lcd[2].y) * (tp[1].y - tp[2].y)) - ((lcd[1].y - lcd[2].y) * (tp[0].y - tp[2].y));

    tp_matrix.e = ((tp[0].x - tp[2].x) * (lcd[1].y - lcd[2].y)) - ((lcd[0].y - lcd[2].y) * (tp[1].x - tp[2].x));

    tp_matrix.f = (tp[2].x * lcd[1].y - tp[1].x * lcd[2].y) * tp[0].y
            + (tp[0].x * lcd[2].y - tp[2].x * lcd[0].y) * tp[1].y + (tp[1].x * lcd[0].y - tp[0].x * lcd[1].y) * tp[2].y;

    return 1;
}

uint8_t ADS7846::writeCalibration(uint16_t eeprom_addr) {
    if (tp_matrix.div != 0) {
        eeprom_write_byte((uint8_t*) eeprom_addr++, 0x55);
        eeprom_write_block((void*) &tp_matrix, (void*) eeprom_addr, sizeof(CAL_MATRIX));
        return 1;
    }

    return 0;
}

uint8_t ADS7846::readCalibration(uint16_t eeprom_addr) {
    uint8_t c;

    c = eeprom_read_byte((uint8_t*) eeprom_addr++);
    if (c == 0x55) {
        eeprom_read_block((void*) &tp_matrix, (void*) eeprom_addr, sizeof(CAL_MATRIX));
        return 1;
    }

    return 0;
}

uint8_t ADS7846::doCalibration(MI0283QT2 *lcd, uint16_t eeprom_addr, uint8_t check_eeprom) //touch panel calibration routine
        {
    uint8_t i;
    CAL_POINT lcd_points[3] = { CAL_POINT1, CAL_POINT2, CAL_POINT3 }; //calibration point postions
    CAL_POINT tp_points[3];

    //calibration data in EEPROM?
    if (readCalibration(eeprom_addr) && check_eeprom) {
        return 0;
    }

    //clear screen and wait for touch release
    lcd->clearDisplay(COLOR_WHITE);
    lcd->drawTextPGM((lcd->getWidth() / 2) - 50, (lcd->getHeight() / 2) - 10, PSTR("Calibration"), 1, COLOR_BLACK,
            COLOR_WHITE);
    while (getPressure() > MIN_PRESSURE) {
        service();
    };

    //show calibration points
    for (i = 0; i < 3;) {
        //draw points
        lcd->drawCircle(lcd_points[i].x, lcd_points[i].y, 2, COLOR_BLACK);
        lcd->drawCircle(lcd_points[i].x, lcd_points[i].y, 5, COLOR_BLACK);
        lcd->drawCircle(lcd_points[i].x, lcd_points[i].y, 10, COLOR_RED); // red

        //run service routine
        service();

        //press dectected? -> save point
        if (getPressure() > MIN_PRESSURE) {
            lcd->fillCircle(lcd_points[i].x, lcd_points[i].y, 2, COLOR_RED);
            tp_points[i].x = getXraw();
            tp_points[i].y = getYraw();
            i++;

            //wait and redraw screen
            delay(100);
            lcd->clearDisplay(COLOR_WHITE);
            lcd->drawTextPGM((lcd->getWidth() / 2) - 50, (lcd->getHeight() / 2) - 10, PSTR("Calibration"), 1,
                    COLOR_BLACK, COLOR_WHITE);
        }
    }

    //calulate calibration matrix
    setCalibration(lcd_points, tp_points);

    //save calibration matrix
    writeCalibration(eeprom_addr);

    //wait for touch release
    while (getPressure() > MIN_PRESSURE) {
        service();
    };

    return 1;
}

void ADS7846::calibrate(void) {
    long x, y;

    //calc x pos
    if (tp.x != tp_last.x) {
        tp_last.x = tp.x;
        x = tp.x;
        y = tp.y;
        x = ((tp_matrix.a * x) + (tp_matrix.b * y) + tp_matrix.c) / tp_matrix.div;
        if (x < 0) {
            x = 0;
        } else if (x >= LCD_WIDTH) {
            x = LCD_WIDTH - 1;
        }
        lcd.x = x;
    }

    //calc y pos
    if (tp.y != tp_last.y) {
        tp_last.y = tp.y;
        x = tp.x;
        y = tp.y;
        y = ((tp_matrix.d * x) + (tp_matrix.e * y) + tp_matrix.f) / tp_matrix.div;
        if (y < 0) {
            y = 0;
        } else if (y >= LCD_HEIGHT) {
            y = LCD_HEIGHT - 1;
        }
        lcd.y = y;
    }

    return;
}

uint16_t ADS7846::getX(void) {
    return lcd.x;
}

uint16_t ADS7846::getY(void) {
    return lcd.y;
}

uint16_t ADS7846::getXraw(void) {
    return tp.x;
}

uint16_t ADS7846::getYraw(void) {
    return tp.y;
}

uint8_t ADS7846::getPressure(void) {
    return pressure;
}

void ADS7846::service(void) {
    uint8_t a, b, i;
    uint16_t x, y;

    //SPI speed-down
#if !defined(SOFTWARE_SPI)
    uint8_t spcr, spsr;
    spcr = SPCR;
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); //enable SPI, Master, clk=Fcpu/16
    spsr = SPSR;
    SPSR = (1 << SPI2X); //clk*2 -> clk=Fcpu/8
#endif

    //get pressure
    CS_ENABLE();
    wr_spi(CMD_START | CMD_8BIT | CMD_DIFF | CMD_Z1_POS);
    a = rd_spi();
    wr_spi(CMD_START | CMD_8BIT | CMD_DIFF | CMD_Z2_POS);
    b = 127 - rd_spi();
    CS_DISABLE();
    pressure = a + b;

    if (pressure >= MIN_PRESSURE) {
        for (x = 0, y = 0, i = 8; i != 0; i--) //8 samples
                {
            CS_ENABLE();
            //get X data
            wr_spi(CMD_START | CMD_12BIT | CMD_DIFF | CMD_X_POS);
            a = rd_spi();
            b = rd_spi();
            x += 1023 - ((a << 2) | (b >> 6)); //12bit: ((a<<4)|(b>>4)) //10bit: ((a<<2)|(b>>6))
            //get Y data
            wr_spi(CMD_START | CMD_12BIT | CMD_DIFF | CMD_Y_POS);
            a = rd_spi();
            b = rd_spi();
            y += ((a << 2) | (b >> 6)); //12bit: ((a<<4)|(b>>4)) //10bit: ((a<<2)|(b>>6))
            CS_DISABLE();
        }
        x >>= 3; //x/8
        y >>= 3; //y/8

        if ((x >= 10) && (y >= 10)) {
            tp.x = x;
            tp.y = y;
            calibrate();
        }
    } else {
        pressure = 0;
    }

    //restore SPI settings
#if !defined(SOFTWARE_SPI)
    SPCR = spcr;
    SPSR = spsr;
#endif
}

// read individual A/D channels like temperature or Vcc
uint16_t ADS7846::readChannel(uint8_t channel, bool use12Bit, bool useDiffMode, uint8_t numberOfReadingsToIntegrate) {
    channel <<= 4;
    // mask for channel 0 to 7
    channel &= CHANNEL_MASK;
    uint16_t tRetValue = 0;
    uint8_t low, high, i;

//SPI speed-down
#if !defined(SOFTWARE_SPI)
    uint8_t spcr, spsr;
    spcr = SPCR;
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); //enable SPI, Master, clk=Fcpu/16
    spsr = SPSR;
    SPSR = (1 << SPI2X); //clk*2 -> clk=Fcpu/8
#endif

//read channel
    uint8_t tMode = CMD_SINGLE;
    if (useDiffMode) {
        tMode = CMD_DIFF;
    }

    CS_ENABLE();
    for (i = numberOfReadingsToIntegrate; i != 0; i--) {
        if (use12Bit) {
            wr_spi(CMD_START | CMD_12BIT | tMode | channel);
            high = rd_spi();
            low = rd_spi();
            tRetValue += (high << 4) | (low >> 4); //12bit: ((a<<4)|(b>>4)) //10bit: ((a<<2)|(b>>6))
        } else {
            wr_spi(CMD_START | CMD_8BIT | tMode | channel);
            tRetValue += rd_spi();
        }
    }
    CS_DISABLE();
//restore SPI settings
#if !defined(SOFTWARE_SPI)
    SPCR = spcr;
    SPSR = spsr;
#endif

    return tRetValue / numberOfReadingsToIntegrate;
}

//-------------------- Private --------------------

uint8_t ADS7846::rd_spi(void) {
#if defined(SOFTWARE_SPI)
    uint8_t bit, data;

    MOSI_LOW();
    for(data=0, bit=8; bit!=0; bit--)
    {
        CLK_HIGH();
        data <<= 1;
        if(MISO_READ())
        {
            data |= 1;
        }
        else
        {
            //data |= 0;
        }
        CLK_LOW();
    }
    return data;

#else
    SPDR = 0x00;
    while (!(SPSR & (1 << SPIF)))
        ;
    return SPDR ;
#endif
}

void ADS7846::wr_spi(uint8_t data) {
#if defined(SOFTWARE_SPI)
    uint8_t mask;

    for(mask=0x80; mask!=0; mask>>=1)
    {
        CLK_LOW();
        if(mask & data)
        {
            MOSI_HIGH();
        }
        else
        {
            MOSI_LOW();
        }
        CLK_HIGH();
    }
    CLK_LOW();

#else
    SPDR = data;
    while (!(SPSR & (1 << SPIF)))
        ;
#endif

    return;
}
