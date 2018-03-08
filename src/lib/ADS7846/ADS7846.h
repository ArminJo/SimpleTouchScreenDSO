#ifndef ADS7846_h
#define ADS7846_h

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#ifdef __cplusplus
}
#endif
#include <MI0283QT2.h>

#define CAL_POINT_X1 (20)
#define CAL_POINT_Y1 (20)
#define CAL_POINT1   {CAL_POINT_X1,CAL_POINT_Y1}

#define CAL_POINT_X2 (300)
#define CAL_POINT_Y2 (120)
#define CAL_POINT2   {CAL_POINT_X2,CAL_POINT_Y2}

#define CAL_POINT_X3 (160)
#define CAL_POINT_Y3 (220)
#define CAL_POINT3   {CAL_POINT_X3,CAL_POINT_Y3}

#define MIN_PRESSURE    (5) //minimum pressure

typedef struct {
	uint16_t x;
	uint16_t y;
} TP_POINT;

typedef struct {
	long x;
	long y;
} CAL_POINT;

typedef struct {
	long a;
	long b;
	long c;
	long d;
	long e;
	long f;
	long div;
} CAL_MATRIX;

class ADS7846 {
public:
	TP_POINT lcd; //calibrated pos (screen)
	TP_POINT tp; //raw pos (touch panel)
	TP_POINT tp_last; //last raw pos (touch panel)
	CAL_MATRIX tp_matrix; //calibrate matrix
	uint8_t pressure; //touch panel pressure
	//future: uint8_t action;                 //actions: move_right, move_left, move_up, move_down

	ADS7846();
	void init(void);
	uint8_t setCalibration(CAL_POINT *lcd, CAL_POINT *tp);
	uint8_t writeCalibration(uint16_t eeprom_addr);
	uint8_t readCalibration(uint16_t eeprom_addr);
	uint8_t doCalibration(MI0283QT2 *lcd, uint16_t eeprom_addr, uint8_t check_eeprom);
	void calibrate(void);
	uint16_t getX(void);
	uint16_t getY(void);
	uint16_t getXraw(void);
	uint16_t getYraw(void);
	uint8_t getPressure(void);
	void service(void);
	uint16_t readChannel(uint8_t channel, bool use12Bit, bool useDiffMode, uint8_t numberOfReadingsToIntegrate);

	// A/D input channel for readChannel()
#define CMD_TEMP0       (0x00)
	// 2,5V reference 2,1 mV/Celsius 600 mV at 25 Celsius 12 Bit
	// 25 Celsius reads 983 / 0x3D7 and 1 celsius is 3,44064 => Celsius is 897 / 0x381
#define CMD_X_POS       (0x10)
#define CMD_BATT        (0x20) // read Vcc /4
#define CMD_Z1_POS      (0x30)
#define CMD_Z2_POS      (0x40)
#define CMD_Y_POS       (0x50)
#define CMD_AUX	       	(0x60)
#define CMD_TEMP1       (0x70)

private:
	uint8_t rd_spi(void);
	void wr_spi(uint8_t data);
};

#endif //ADS7846_h
