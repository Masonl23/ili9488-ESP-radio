#include "GUIClass.h"

GUIClass::GUIClass()
{
}

/**
 * Starts the display and touch controllers as well as setting default values
 */
void GUIClass::Start()
{
    // set starting enum values
    _lcdState = DISPLAY_ON;
    _guiView = HOME_VIEW;
    _btMode = TRANSMITTER_M;
    _sleepMode = SLEEP_30S;
    _sleepPeriod = 30 * 1000;

    // start the peripherals
    InitLcd();
    InitPCF();
    InitINA226();

    // start the buttons draw menus
    InitButtons();
    DrawButtons();
    DrawMenu();
    ShowBTMode();

    _lastTouch = millis();

    // start the LEDC channel to control brightness
    ledcSetup(LCD_BRIGHTNESS_CHANNEL, 5000, 8);
    ledcAttachPin(LCD_POWER_PIN, LCD_BRIGHTNESS_CHANNEL);
    ledcWrite(LCD_BRIGHTNESS_CHANNEL, 255); // default full bright
}

/**
 * Prints the brightness slider to the screen
*/
void GUIClass::DrawBrightnessSlider()
{
    _spriteBrightSlider.clear();
    _spriteBrightSlider.drawString("Screen Brightness",95,S_BTN_H/2-5);

    // outside bar
    _spriteBrightSlider.drawRoundRect(0, 14, BRIGHT_SLIDER_WIDTH, S_BTN_H/2, 10, TFT_PURPLE);
    _spriteBrightSlider.drawRoundRect(1, 15, BRIGHT_SLIDER_WIDTH-2, S_BTN_H/2 -2, 10, TFT_PURPLE);
    // vertical 'position' bar
    _spriteBrightSlider.fillRoundRect(_sliderTouchX-10,3,20,S_BTN_H-6,10,TFT_WHITE);

    // push sprite and change brightness
    _spriteBrightSlider.pushSprite(&_lcd, S_COL_1, S_ROW_3);
    ChangeBrightness();
}

/**
 * Starts the power sensor
 */
void GUIClass::InitINA226()
{
    // _powerSensor = new INA226(INA226_ADD,&Wire1);
    _powerSensor.begin();
    _powerSensor.setAverage(1);
    _powerSensor.setMaxCurrentShunt(.8, 0.1, false);
}

/**
 * Starts the ili9488 16bit screen and creates the sprites
 */
void GUIClass::InitLcd()
{
    _tScreen.Begin();
    _tScreen.setOrientation(3);
    pinMode(LCD_CS, OUTPUT);
    pinMode(LCD_BLK, OUTPUT);

    digitalWrite(LCD_CS, LOW);
    digitalWrite(LCD_BLK, HIGH);

    _lcd.init();
    _lcd.setRotation(3);
    _lcd.fillScreen(BG_COLOR);
    _lcd.clearDisplay();

    // allocate memory for the sprites
    _spriteBattery.createSprite(SPRITE_BATTERY_SIZE, SPRITE_BATTERY_SIZE);
    _spriteCurrent.createSprite(SPRITE_BATTERY_SIZE, SPRITE_BATTERY_SIZE);
    _spriteBrightSlider.createSprite(BRIGHT_SLIDER_WIDTH, S_BTN_H);
}

/**
 * Creates all the buttons used in the GUI.
 */
void GUIClass::InitButtons()
{
    // create the volume and mute button
    _buttons[VOL_UP_B].initButtonUL(&_lcd, CNTRL_COL, VOL_UP_ROW, VOL_W, VOL_H, TFT_BLACK, TFT_WHITE, TFT_BLACK, "+", 3);
    _buttons[MUTE_B].initButtonUL(&_lcd, CNTRL_COL, MUTE_ROW, VOL_W, MUTE_H, TFT_WHITE, TFT_BLACK, TFT_WHITE, "M", 2);
    _buttons[VOL_DOWN_B].initButtonUL(&_lcd, CNTRL_COL, VOL_DOWN_ROW, VOL_W, VOL_H, TFT_BLACK, TFT_WHITE, TFT_BLACK, "-", 3);

    _buttons[SKIP_BACK].initButtonUL(&_lcd, TAB_LINE_COL + 25, VOL_DOWN_ROW + 15, VOL_H, VOL_W - 10, TFT_BLACK, TFT_WHITE, TFT_BLACK, "<", 3);
    _buttons[SKIP_FORWARD].initButtonUL(&_lcd, TAB_LINE_COL + (2 * VOL_H) + 45, VOL_DOWN_ROW + 15, VOL_H, VOL_W - 10, TFT_BLACK, TFT_WHITE, TFT_BLACK, ">", 3);
    _buttons[PLAY_PAUSE].initButtonUL(&_lcd, TAB_LINE_COL + (VOL_H) + 35, VOL_DOWN_ROW + 10, VOL_H, VOL_W, TFT_WHITE, TFT_BLACK, TFT_WHITE, "Play|Pause", 1.2);

    // home and settings tab
    _buttons[HOME_B].initButtonUL(&_lcd, TAB_COL_HOME - 30, TAB_ROW, 94, 33, TFT_BLACK, TFT_BLACK, TFT_WHITE, "Home", 1.5);
    _buttons[SETTINGS_B].initButtonUL(&_lcd, TAB_COL_SETTINGS - 10, TAB_ROW, 94, 33, TFT_BLACK, TFT_BLACK, TFT_WHITE, "Settings", 1.5);

    // pair, transmit and reciever buttons
    _buttons[PAIR_B].initButtonUL(&_lcd, S_COL_1, S_ROW_1, S_BTN_W, S_BTN_H, TFT_GREEN, BG_COLOR, TFT_WHITE, "Pair", 1.5);
    _buttons[TX_B].initButtonUL(&_lcd, S_COL_2, S_ROW_1, S_BTN_W, S_BTN_H, TFT_RED, BG_COLOR, TFT_WHITE, "Tx", 1.5);
    _buttons[RX_B].initButtonUL(&_lcd, S_COL_3, S_ROW_1, S_BTN_W, S_BTN_H, TFT_SKYBLUE, BG_COLOR, TFT_WHITE, "Rx", 1.5);

    // sleep options buttons
    _buttons[DISP_OPT_30S_B].initButtonUL(&_lcd, S_COL_1, S_ROW_2, S_BTN_W, S_BTN_H, TFT_YELLOW, BG_COLOR, TFT_WHITE, "30s", 1.5);
    _buttons[DISP_OPT_5M_B].initButtonUL(&_lcd, S_COL_2, S_ROW_2, S_BTN_W, S_BTN_H, TFT_YELLOW, BG_COLOR, TFT_WHITE, "5m", 1.5);
    _buttons[DISP_OPT_OFF_B].initButtonUL(&_lcd, S_COL_3, S_ROW_2, S_BTN_W, S_BTN_H, TFT_YELLOW, BG_COLOR, TFT_WHITE, "Off", 1.5);

    // slider for brightness
    _buttons[BRIGHT_SLIDER_B].initButtonUL(&_lcd, S_COL_1, S_ROW_3, BRIGHT_SLIDER_WIDTH, S_BTN_H, TFT_WHITE, BG_COLOR, TFT_WHITE, "Brightness");
}

/**
 * Draws the buttons on the screen based on the current view.
 */
void GUIClass::DrawButtons()
{
    _buttons[VOL_UP_B].drawButton();
    _buttons[MUTE_B].drawButton();
    _buttons[VOL_DOWN_B].drawButton();
    _buttons[HOME_B].drawButton();
    _buttons[SETTINGS_B].drawButton();

    _lcd.drawLine(TAB_LINE_COL, TAB_LINE_ROW, TAB_LINE_COL_END, TAB_LINE_ROW, TFT_WHITE); // draw line under tabs
    _lcd.drawLine(TAB_LINE_COL, TAB_LINE_ROW + 1, TAB_LINE_COL_END, TAB_LINE_ROW + 1, TFT_WHITE);

    // cover the setting/home view buttons
    _lcd.fillRect(TAB_LINE_COL, TAB_COVER_ROW, TAB_COVER_RECT_W, TAB_COVER_RECT_H, BG_COLOR); // Cover home tab stuff

    if (_guiView == HOME_VIEW)
    {
        _buttons[SKIP_BACK].drawButton();
        _buttons[SKIP_FORWARD].drawButton();
        _buttons[PLAY_PAUSE].drawButton();
    }
    else if (_guiView == SETTINGS_VIEW)
    {
        _buttons[PAIR_B].drawButton();
        _buttons[TX_B].drawButton();
        _buttons[RX_B].drawButton();
        _buttons[DISP_OPT_30S_B].drawButton();
        _buttons[DISP_OPT_5M_B].drawButton();
        _buttons[DISP_OPT_OFF_B].drawButton();

        // BRIGHT_SLIDER_B is special, it has its own drawButton since its a slider
        DrawBrightnessSlider();
    }
}

/**
 * Displays the current bluetooth mode the device is i
 */
void GUIClass::ShowBTMode()
{
    _lcd.setTextSize(1);
    _lcd.setTextColor(TFT_WHITE);
    _lcd.fillRect(MENU_BT_TEXT_COL, 0, MENU_BT_COVER_RECT_W, MENU_BT_COVER_RECT_H, BG_COLOR);
    if (_btMode == TRANSMITTER_M)
    {
        _lcd.setTextColor(TFT_RED);
        _lcd.setCursor(MENU_BT_TEXT_COL, MENU_TEXT_ROW);
        _lcd.print("Transmitter");
    }
    else
    {
        _lcd.setTextColor(TFT_SKYBLUE);
        _lcd.setCursor(MENU_BT_TEXT_COL, MENU_TEXT_ROW);
        _lcd.print("Reciever");
    }
    _lcd.setTextColor(TFT_WHITE);
}

/**
 * Starts the gpio expander and sets pinmode as output and to low
 */
void GUIClass::InitPCF()
{
    gpio.begin();
    for (byte i = 0; i < NUM_PINS; i++)
    {
        gpio.pinMode(i, OUTPUT);
        gpio.digitalWrite(i, LOW);
    }
}

/**
 * Main function to be called repeatedly in a loop which constantly polls the touch
 * screen and checks if any touches occur and if so if they are within a button. If button
 * press occurs its flag is set and is then serviced in justPressed which then handles
 * each button accordingly.
 */
void GUIClass::CheckButtonPress()
{
    int touch[2] = {0, 0};

    bool pressed = _tScreen.getTouchPair(touch);
    if (pressed && _lcdState == DISPLAY_OFF)
    {
        // screen debouncing
        delay(100);
        if (_tScreen.getTouchPair(touch))
        {
            digitalWrite(LCD_POWER_PIN, HIGH);
            _lcdState = DISPLAY_ON;
            _lastTouch = millis();
            delay(500);
        }
    }
    else
    {
        for (uint8_t bttn = 0; bttn < NUM_BUTTONS; bttn++)
        {
            // if button press chekc
            if (pressed && _buttons[bttn].contains(touch[0], touch[1]))
            {
                // button debouncing, removes ghost clicks from happening
                delay(25);
                pressed = _tScreen.getTouchPair(touch);
                if (pressed && _buttons[bttn].contains(touch[0], touch[1]))
                {

                    if (_guiView == HOME_VIEW)
                    {
                        if (!IsSettingsButtons((BUTTON_NAMES)bttn))
                        {
                            _buttons[bttn].press(true);
                            _lastTouch = millis();
                        }
                    }
                    else if (_guiView == SETTINGS_VIEW)
                    {
                        if (!IsHomeButtons((BUTTON_NAMES)bttn))
                        {
                            if (bttn == BRIGHT_SLIDER_B)
                            {
                                _sliderTouchX = touch[0] - S_COL_1 - 50;
                                if (_sliderTouchX < BRIGHT_SLIDER_LOWER_LIMIT){
                                    _sliderTouchX = BRIGHT_SLIDER_LOWER_LIMIT;
                                }
                                if(_sliderTouchX > BRIGHT_SLIDER_UPPER_LIMIT){
                                    _sliderTouchX = BRIGHT_SLIDER_UPPER_LIMIT;
                                }
                                // DisplayMenuMessage(String(_sliderTouchX));
                                DrawBrightnessSlider();
                            }

                            _buttons[bttn].press(true);

                            _lastTouch = millis();
                        }
                    }

                    _lastTouch = millis();
                }
            }
            else if (!_buttons[bttn].contains(touch[0], touch[1]))
            {
                _buttons[bttn].press(false);
            }
        }

        for (uint8_t bttn = 0; bttn < NUM_BUTTONS; bttn++)
        {
            if (_buttons[bttn].justReleased())
            {
                if (!IsTabButtons((BUTTON_NAMES)bttn))
                {
                    if (bttn != BRIGHT_SLIDER_B)
                    {
                        _buttons[bttn].drawButton();
                    }
                }
            }

            // button just pressed
            if (_buttons[bttn].justPressed())
            {
                if (!IsTabButtons((BUTTON_NAMES)bttn))
                {
                    if (bttn != BRIGHT_SLIDER_B)
                    {
                        _buttons[bttn].drawButton(true);
                    }
                }
                else
                {
                    if (bttn == SETTINGS_B && _guiView != SETTINGS_VIEW)
                    {
                        _guiView = SETTINGS_VIEW;
                        DrawButtons();
                    }
                    else if (bttn == HOME_B && _guiView != HOME_VIEW)
                    {
                        _guiView = HOME_VIEW;
                        DrawButtons();
                    }
                }

                BUTTON_NAMES name = (BUTTON_NAMES)bttn;
                ButtonCallback(name);
            }
        }
    }
}

/**
 * Handles callback when buttons are pressed
 */
void GUIClass::ButtonCallback(BUTTON_NAMES &pressed)
{
    if (pressed == VOL_UP_B)
    {
        DisplayMenuMessage("Volume up");
    }
    else if (pressed == MUTE_B)
    {
        DisplayMenuMessage("Mute");
    }
    else if (pressed == VOL_DOWN_B)
    {
        DisplayMenuMessage("Volume down");
    }
    else if (pressed == HOME_B)
    {
        DisplayMenuMessage("Home");
    }
    else if (pressed == SKIP_BACK)
    {
        DisplayMenuMessage("Skip back");
    }
    else if (pressed == SKIP_FORWARD)
    {
        DisplayMenuMessage("Skip forward");
    }
    else if (pressed == PLAY_PAUSE)
    {
        DisplayMenuMessage("Play/Pause");
    }
    else if (pressed == SETTINGS_B)
    {
        DisplayMenuMessage("Settings");
    }
    else if (pressed == PAIR_B)
    {
        DisplayMenuMessage("Pair BT");
    }
    else if (pressed == TX_B)
    {
        DisplayMenuMessage("Tx");
        if (_btMode == RECIEVER_M)
        {
            _btMode = TRANSMITTER_M;
            ShowBTMode();
        }
        else
        {
            DisplayMenuMessage("Searching for Reciever");
        }
    }
    else if (pressed == RX_B)
    {
        DisplayMenuMessage("Rx");
        if (_btMode == TRANSMITTER_M)
        {
            _btMode = RECIEVER_M;
            ShowBTMode();
        }
        else
        {
            DisplayMenuMessage("Searching for Transmitter");
        }
    }
    else if (pressed == DISP_OPT_30S_B)
    {
        DisplayMenuMessage("Sleep 30s");
        _sleepPeriod = 30 * 1000;
        _sleepMode = SLEEP_30S;
    }
    else if (pressed == DISP_OPT_5M_B)
    {
        DisplayMenuMessage("Sleep 5m");
        _sleepPeriod = 5 * 60 * 1000;
        _sleepMode = SLEEP_5M;
    }
    else if (pressed == DISP_OPT_OFF_B)
    {
        DisplayMenuMessage("Sleep off");
        _sleepMode = SLEEP_OFF;
    }
    else if (pressed == BRIGHT_SLIDER_B)
    {
        // nothing to be done here
    }
    else
    {
        DisplayMenuMessage("Other button");
    }

    // send button press to other core to keep gui responsive
    int buttonPressed = (int)pressed;
    xQueueSend(_queue, &buttonPressed, 0);
}

/**
 * Refreshes the GUI if data needs to de drawn on a cyclic period.
 * IE the top messages when button are pressed and the gauges used.
 */
void GUIClass::RefreshData()
{
    // check message if we need to cover
    if (_messageOn && millis() - _messageTime > 750)
    {
        _messageOn = false;
        _lcd.fillRect(200, 0, 230, 13, BG_COLOR);
    }

    // check if we need to turn the screen off
    if (_lcdState == DISPLAY_ON && _sleepMode != SLEEP_OFF)
    {
        if (_sleepMode == SLEEP_30S && millis() - _lastTouch > (30 * 1000))
        {
            digitalWrite(LCD_POWER_PIN, LOW);
            _lcdState = DISPLAY_OFF;
        }
        else if (_sleepMode == SLEEP_5M && millis() - _lastTouch > (5 * 60 * 1000))
        {
            digitalWrite(LCD_POWER_PIN, LOW);
            _lcdState = DISPLAY_OFF;
        }
    }

    if (_guiView == HOME_VIEW)
    {
        if (millis() - _lastGraphics > 150)
        {
            float voltageReading = (float)random(0, 2) * 1.13 + 17.5;
            float currentReading =(float)random(-2, 2) * 1.11 + 6;

            DrawVoltageGauge(voltageReading);
            DrawCurrentGauge(currentReading);
            //         // DrawVoltageGauge((float)_powerSensor.getBusVoltage());
            //         // DrawCurrentGauge((float)_powerSensor.getCurrent());

            DrawBatteryPercentage(voltageReading);
            _lastGraphics = millis();
        }
    }
}

/**
 * Given a voltage reading, functions converts it to a percentage and prints it to
 * the right size of menu
*/
void GUIClass::DrawBatteryPercentage(float rawVolage){
    _lcd.setTextSize(1);
    _lcd.setTextColor(TFT_WHITE);
    
}

/**
 * Displays a messages to the top menu bar of the GUI.
 */
void GUIClass::DisplayMenuMessage(String message)
{
    _lcd.setTextSize(1);
    _lcd.fillRect(MENU_MSG_TEXT_COL, 0, MENU_MSG_COVER_RECT_W, MENU_MSG_COVER_RECT_H, BG_COLOR);
    _lcd.setTextColor(TFT_GOLD);
    _lcd.setCursor(MENU_MSG_TEXT_COL, MENU_TEXT_ROW);
    _lcd.print(message);
    _messageOn = true;
    _messageTime = millis();
}

/**
 * Draws the menu bar to the GUI
 */
void GUIClass::DrawMenu()
{
    _lcd.setTextColor(TFT_WHITE);
    _lcd.setTextSize(1);

    // draw line for the menu
    _lcd.drawLine(0, MENU_LINE_ROW, SCREEN_WIDTH, MENU_LINE_ROW, TFT_WHITE);
    _lcd.drawLine(0, MENU_LINE_ROW+1, SCREEN_WIDTH, MENU_LINE_ROW+1, TFT_WHITE);
}

/**
 * Used in hardware core, constantly polls the queue to see if there
 * is any button presses that were sent over, it then services these
 * calls and removes from the queue.
 */
void GUIClass::CheckQueue()
{
    int pressed = 0;
    uint32_t shortDelay = 200;
    uint32_t longDelay = 800;

    if (uxQueueMessagesWaiting(_queue) > 0)
    {
        xQueueReceive(_queue, &pressed, 0);
        BUTTON_NAMES bPressed = (BUTTON_NAMES)pressed;
        Serial.println(bPressed);
        if (bPressed == VOL_UP_B)
        {
            gpio.digitalWrite(VOL_UP_PIN, HIGH);
            delay(longDelay);
            gpio.digitalWrite(VOL_UP_PIN, LOW);
            Serial.println("Vol up button");
        }
        else if (bPressed == MUTE_B)
        {
            Serial.println("Mutebutton");
            gpio.digitalWrite(PLAY_PAUSE_PIN, HIGH);
            delay(shortDelay);
            gpio.digitalWrite(PLAY_PAUSE_PIN, LOW);
        }
        else if (bPressed == VOL_DOWN_B)
        {
            Serial.println("Vol down button");
            gpio.digitalWrite(VOL_DOWN_PIN, HIGH);
            delay(longDelay);
            gpio.digitalWrite(VOL_DOWN_PIN, LOW);
        }
        else if (bPressed == SKIP_BACK)
        {
            gpio.digitalWrite(VOL_DOWN_PIN, HIGH);
            delay(shortDelay);
            gpio.digitalWrite(VOL_DOWN_PIN, LOW);
            int newVal = 0;
            if (uxQueueMessagesWaiting(_queue) > 0)
            {
                xQueuePeek(_queue, &newVal, 0);
                if (newVal == SKIP_BACK)
                {

                    delay(200);
                    gpio.digitalWrite(VOL_DOWN_PIN, LOW);
                    delay(200);
                    gpio.digitalWrite(VOL_DOWN_PIN, HIGH);
                    delay(200);
                    gpio.digitalWrite(VOL_DOWN_PIN, LOW);
                    xQueueReceive(_queue, &newVal, 0);
                }
            }
        }
        else if (bPressed == SKIP_FORWARD)
        {
            gpio.digitalWrite(VOL_UP_PIN, HIGH);
            delay(shortDelay);
            gpio.digitalWrite(VOL_UP_PIN, LOW);
            int newVal = 0;
            if (uxQueueMessagesWaiting(_queue) > 0)
            {
                xQueuePeek(_queue, &newVal, 0);
                if (newVal == SKIP_FORWARD)
                {

                    delay(200);
                    gpio.digitalWrite(VOL_UP_PIN, LOW);
                    delay(200);
                    gpio.digitalWrite(VOL_UP_PIN, HIGH);
                    delay(200);
                    gpio.digitalWrite(VOL_UP_PIN, LOW);
                    xQueueReceive(_queue, &newVal, 0);
                }
            }
        }
        else if (bPressed == PLAY_PAUSE)
        {
            gpio.digitalWrite(PLAY_PAUSE_PIN, HIGH);
            delay(shortDelay);
            gpio.digitalWrite(PLAY_PAUSE_PIN, LOW);
        }
        else if (bPressed == RX_B)
        {
            gpio.digitalWrite(RCX_PIN, HIGH);
            delay(200);
            gpio.digitalWrite(RCX_PIN, LOW);
            int newVal = 0;
            if (uxQueueMessagesWaiting(_queue) > 0)
            {
                xQueuePeek(_queue, &newVal, 0);
                if (newVal == RCX_PIN)
                {
                    delay(200);
                    gpio.digitalWrite(RCX_PIN, LOW);
                    delay(200);
                    gpio.digitalWrite(RCX_PIN, HIGH);
                    delay(200);
                    gpio.digitalWrite(RCX_PIN, LOW);
                    xQueueReceive(_queue, &newVal, 0);
                }
            }
        }
        else if (bPressed == TX_B)
        {
            gpio.digitalWrite(TRX_PIN, HIGH);
            delay(200);
            gpio.digitalWrite(TRX_PIN, LOW);
            int newVal = 0;
            if (uxQueueMessagesWaiting(_queue) > 0)
            {
                xQueuePeek(_queue, &newVal, 0);
                if (newVal == TRX_PIN)
                {

                    delay(200);
                    gpio.digitalWrite(TRX_PIN, LOW);
                    delay(200);
                    gpio.digitalWrite(TRX_PIN, HIGH);
                    delay(200);
                    gpio.digitalWrite(TRX_PIN, LOW);
                    xQueueReceive(_queue, &newVal, 0);
                }
            }
        }
        else if (bPressed == PAIR_B)
        {
            gpio.digitalWrite(PAIR_PIN, HIGH);
            delay(200);
            gpio.digitalWrite(PAIR_PIN, LOW);
            delay(200);
            gpio.digitalWrite(PAIR_PIN, HIGH);
            delay(200);
            gpio.digitalWrite(PAIR_PIN, LOW);
        }
    }
}

/**
 * Checks if the button pressed is a settings view button.
 * This means is a button that is visible only when in the settings tab.
 */
bool GUIClass::IsSettingsButtons(BUTTON_NAMES pressed)
{
    if (pressed == PAIR_B)
    {
        return true;
    }
    if (pressed == TX_B)
    {
        return true;
    }
    if (pressed == RX_B)
    {
        return true;
    }
    if (pressed == DISP_OPT_30S_B)
    {
        return true;
    }
    if (pressed == DISP_OPT_5M_B)
    {
        return true;
    }
    if (pressed == DISP_OPT_OFF_B)
    {
        return true;
    }
    if (pressed == BRIGHT_SLIDER_B)
    {
        return true;
    }

    return false;
}

/**
 * Returns true if button is in the home view group.
 */
bool GUIClass::IsHomeButtons(BUTTON_NAMES pressed)
{
    if (pressed == SKIP_BACK)
    {
        return true;
    }
    if (pressed == SKIP_FORWARD)
    {
        return true;
    }
    if (pressed == PLAY_PAUSE)
    {
        return true;
    }

    return false;
}

/**
 * Returns true if the button is home or settings button
 */
bool GUIClass::IsTabButtons(BUTTON_NAMES pressed)
{
    if (pressed == HOME_B)
    {
        return true;
    }
    if (pressed == SETTINGS_B)
    {
        return true;
    }
    return false;
}

/**
 * Draws the voltage gauge to the screen
 */
void GUIClass::DrawVoltageGauge(float rpm_input)
{
    int sprite_topCircleGuageSize = 140;
    int in_radius_small = 48;
    int out_radius_small = 65;
    const int sprite_topCircleDataRow = 60 - 8;
    const int angle0 = 135;          // 225
    const int angle1 = angle0 + 270; // 315

    const int minValue = 16;
    const int maxValue = 21;
    _spriteBattery.clear();
    _spriteBattery.drawArc(sprite_topCircleGuageSize / 2, sprite_topCircleGuageSize / 2, in_radius_small, out_radius_small, angle0, angle1, TFT_WHITE);
    _spriteBattery.drawArc(sprite_topCircleGuageSize / 2, sprite_topCircleGuageSize / 2, in_radius_small - 1, out_radius_small + 1, angle0, angle1, TFT_WHITE);
    _spriteBattery.drawArc(sprite_topCircleGuageSize / 2, sprite_topCircleGuageSize / 2, in_radius_small - 2, out_radius_small + 2, angle0, angle1, TFT_WHITE);

    float calculatedAngle = mapf(rpm_input, minValue, maxValue, angle0, angle1);
    if (calculatedAngle >= angle1)
    {
        calculatedAngle = angle1 - 1;
    }
    if (calculatedAngle <= angle0 + 1)
    {
        calculatedAngle = angle0 + 5;
    }
    _spriteBattery.setTextSize(2.0);
    _spriteBattery.setCursor(41, sprite_topCircleDataRow + 2);
    _spriteBattery.printf("%05.2f", rpm_input);
    _spriteBattery.setCursor(42, sprite_topCircleDataRow + 28);
    _spriteBattery.print("Volts");

    // _spriteBattery.fillArc(_spriteBatterySize / 2, _spriteBatterySize / 2, in_radius_small + 2, out_radius_small - 2, angle0 + 1, calculatedAngle, TFT_BLUE);
    _spriteBattery.fillArc(sprite_topCircleGuageSize / 2, sprite_topCircleGuageSize / 2, in_radius_small + 1, out_radius_small - 1, angle0 + 1, calculatedAngle, TFT_BLUE);
    int tipMinAngle = calculatedAngle - 5;
    if (tipMinAngle <= angle0)
        tipMinAngle = calculatedAngle - 1;
    _spriteBattery.fillArc(sprite_topCircleGuageSize / 2, sprite_topCircleGuageSize / 2, in_radius_small - 5, out_radius_small + 5, tipMinAngle, calculatedAngle, TFT_DARKGRAY);

    // left for marking 0
    int middleRadius = (out_radius_small - in_radius_small) / 2 + in_radius_small - 2;
    int xCoord = sin(radians(angle0 - 15 + 90)) * middleRadius + 70 - 5;
    int yCoord = -cos(radians(angle0 - 15 + 90)) * middleRadius + 65;
    _spriteBattery.setCursor(xCoord, yCoord);
    _spriteBattery.setTextSize(1.2);
    _spriteBattery.print(minValue);

    // right for marking max value
    xCoord = sin(radians(angle1 + 15 + 90)) * middleRadius + 70;
    _spriteBattery.setCursor(xCoord, yCoord);
    _spriteBattery.print(maxValue);
    _spriteBattery.pushSprite(&_lcd, S_COL_1, H_GUAGE_ROW);
}

/**
 * Draws the current gauge onto the screen
 */
void GUIClass::DrawCurrentGauge(float rpm_input)
{
    int sprite_topCircleGuageSize = 140;
    int in_radius_small = 48;
    int out_radius_small = 65;
    const int sprite_topCircleDataRow = 60 - 8;
    const int angle0 = 135;          // 225
    const int angle1 = angle0 + 270; // 315
    _spriteCurrent.clear();
    _spriteCurrent.drawArc(sprite_topCircleGuageSize / 2, sprite_topCircleGuageSize / 2, in_radius_small, out_radius_small, angle0, angle1, TFT_WHITE);
    _spriteCurrent.drawArc(sprite_topCircleGuageSize / 2, sprite_topCircleGuageSize / 2, in_radius_small - 1, out_radius_small + 1, angle0, angle1, TFT_WHITE);
    _spriteCurrent.drawArc(sprite_topCircleGuageSize / 2, sprite_topCircleGuageSize / 2, in_radius_small - 2, out_radius_small + 2, angle0, angle1, TFT_WHITE);

    float calculatedAngle = mapf(rpm_input, 0, 12, angle0, angle1);
    if (calculatedAngle >= angle1)
    {
        calculatedAngle = angle1 - 1;
    }
    if (calculatedAngle <= angle0 + 1)
    {
        calculatedAngle = angle0 + 5;
    }
    _spriteCurrent.setTextSize(2.0);
    _spriteCurrent.setCursor(41, sprite_topCircleDataRow + 2);
    _spriteCurrent.printf("%05.2f", rpm_input);
    _spriteCurrent.setCursor(48, sprite_topCircleDataRow + 28);
    _spriteCurrent.print("Amps");

    // _spriteCurrent.fillArc(_spriteCurrentSize / 2, _spriteCurrentSize / 2, in_radius_small + 2, out_radius_small - 2, angle0 + 1, calculatedAngle, TFT_BLUE);
    _spriteCurrent.fillArc(sprite_topCircleGuageSize / 2, sprite_topCircleGuageSize / 2, in_radius_small + 1, out_radius_small - 1, angle0 + 1, calculatedAngle, TFT_RED);
    int tipMinAngle = calculatedAngle - 5;
    if (tipMinAngle <= angle0)
        tipMinAngle = calculatedAngle - 1;
    _spriteCurrent.fillArc(sprite_topCircleGuageSize / 2, sprite_topCircleGuageSize / 2, in_radius_small - 5, out_radius_small + 5, tipMinAngle, calculatedAngle, TFT_DARKGRAY);

    // left for marking 0
    int middleRadius = (out_radius_small - in_radius_small) / 2 + in_radius_small - 2;
    int xCoord = sin(radians(angle0 - 15 + 90)) * middleRadius + 70 - 5;
    int yCoord = -cos(radians(angle0 - 15 + 90)) * middleRadius + 65;
    _spriteCurrent.setCursor(xCoord, yCoord);
    _spriteCurrent.setTextSize(1.2);
    _spriteCurrent.print(0);

    // right for marking max value
    xCoord = sin(radians(angle1 + 15 + 90)) * middleRadius + 70;
    _spriteCurrent.setCursor(xCoord, yCoord);
    _spriteCurrent.print(12);
    _spriteCurrent.pushSprite(&_lcd, S_COL_1 + SPRITE_GUAGE_WIDTH + 10, H_GUAGE_ROW);
}

/**
 * Changes the brightness of the LCD screen
 */
void GUIClass::ChangeBrightness()
{
    // math to convert ratio of button position to pwm 0-255
    int mappedBright = map(_sliderTouchX,0,BRIGHT_SLIDER_UPPER_LIMIT,0,255);
    // math to convert ratio of button position to 0-100 %
    int percentBright = map(_sliderTouchX,0,BRIGHT_SLIDER_UPPER_LIMIT,0,100);

    // write brightness percentage
    char buffer[30];
    sprintf(buffer, "Brightness %d%%", percentBright);
    DisplayMenuMessage(buffer);

    // safe guard incase we go over
    if(mappedBright >255){
        mappedBright = 255;
    }
    ledcWrite(LCD_BRIGHTNESS_CHANNEL, mappedBright);
}

/**
 * Maps values and retuns as double so we can get precision
 */
double GUIClass::mapf(double x, double in_min, double in_max, double out_min, double out_max)
{

    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}