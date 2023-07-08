#include "T_FT6236.h"


T_FT6236::T_FT6236(){
    // Wire1.begin(38,39);
}

void T_FT6236::Begin(){
    Wire1.begin(38,39);
}



/**
 * @brief Get the x pair of the screen
 * 
 * @return int value of x coordinate
 */
int T_FT6236::getTouchPointX()
{
    int XL = 0;
    int XH = 0;

    XH = readTouchReg(TOUCH_REG_XH);
    // Serial.println(XH >> 6,HEX);
    if (XH >> 6 == 1)
        return -1;
    XL = readTouchReg(TOUCH_REG_XL);

    return ((XH & 0x0F) << 8) | XL;
}

/**
 * @brief Get the y value of the screen
 * 
 * @return int value of y coordinate
 */
int T_FT6236::getTouchPointY()
{
    int YL = 0;
    int YH = 0;

    YH = readTouchReg(TOUCH_REG_YH);
    YL = readTouchReg(TOUCH_REG_YL);

    return ((YH & 0x0F) << 8) | YL;
}

/**
 * @brief Gets the touch pair of the screen
 * 
 * @param pair 
 * @return int 
 */
bool T_FT6236::getTouchPair(int pair[2])
{
    int XL = 0;
    int XH = 0;
    int YL = 0;
    int YH = 0;

    XH = readTouchReg(TOUCH_REG_XH);
    if (XH >> 6 == 1)
    {
        pair[0] = -1;
        pair[1] = -1;
        return false;
    }
    XL = readTouchReg(TOUCH_REG_XL);
    YH = readTouchReg(TOUCH_REG_YH);
    YL = readTouchReg(TOUCH_REG_YL);

    pair[0] = ((XH & 0x0F) << 8) | XL;
    pair[1] = ((YH & 0x0F) << 8) | YL;

    if (s_rotation == 1){
        int tempP = pair[1];
        pair[1] = s_width- pair[0];
        pair[0] = tempP;
    }
    else if (s_rotation == 3){
        int tempP = pair[1];
        pair[1] = pair[0];
        pair[0] = s_height - tempP;
    }

    return true;
}

/**
 * @brief Sets the orientation of the screen
 * 
 * @param rotation 
 */
void T_FT6236::setOrientation(int rotation)
{
    s_rotation = rotation;
}

/**
 * @brief Reads the touch register contents
 * 
 * @param reg 
 * @return int 
 */
int T_FT6236::readTouchReg(int reg)
{
    int data = 0;
    Wire1.beginTransmission(0x38);
    Wire1.write(reg);
    Wire1.endTransmission();
    Wire1.requestFrom(0x38, 1);
    if (Wire1.available())
    {
        data = Wire1.read();
    }
    return data;
}