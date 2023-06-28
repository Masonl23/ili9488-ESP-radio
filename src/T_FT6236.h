#include <Wire.h>
#define TOUCH_I2C_ADD 0x38

#define TOUCH_REG_XL 0x04
#define TOUCH_REG_XH 0x03
#define TOUCH_REG_YL 0x06
#define TOUCH_REG_YH 0x05

class T_FT6236 {
public:
    T_FT6236();

    // Get the x point of the screen
    int getTouchPointX();

    // Get the y point of the screen
    int getTouchPointY();

    // Get the (x,y) coordinates of the touch screen
    bool getTouchPair(int tPair[2]);

    // Set the orientation of the screen
    void setOrientation(int rotation);
private:
    // Read the registers from touch screen
    int readTouchReg(int reg);

    int s_rotation = 0;     // default 0. can be 0-3

    // width and height of touch sensor
    int s_width = 319;
    int s_height = 479;

    uint16_t _i2cAdd = 0;
};