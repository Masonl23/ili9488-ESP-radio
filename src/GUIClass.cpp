#include "GUIClass.h"

/**
 * Starts the display and touch controllers as well as setting default values
 */
void GUIClass::Start()
{
    InitLcd();
    InitButtons();
    DrawButtons();
    InitPCF();
    DrawMenu();
    ShowBTMode();

    // set starting enum values
    _lcdState = DISPLAY_ON;
    _guiView = HOME_VIEW;
    _btMode = TRANSMITTER_M;
    _sleepMode = SLEEP_30S;
    _sleepPeriod = 30 * 1000;

    _lastTouch = millis();
}

/**
 * Starts the ili9488 16bit screen and creates the sprites
 */
void GUIClass::InitLcd()
{
    pinMode(LCD_CS, OUTPUT);
    pinMode(LCD_BLK, OUTPUT);

    digitalWrite(LCD_CS, LOW);
    digitalWrite(LCD_BLK, HIGH);

    _lcd.init();
    _lcd.setRotation(3);
    _lcd.fillScreen(BG_COLOR);

    _lcd.clearDisplay();
    _tScreen.setOrientation(3);

    // init the sprites
    _battery1Sprite = LGFX_Sprite(&_lcd);
    _battery2Sprite = LGFX_Sprite(&_lcd);
    _battery1Sprite.createSprite(SPRITE_GUAGE_WIDTH, SPRITE_GUAGE_HEIGHT);
    _battery2Sprite.createSprite(SPRITE_GUAGE_WIDTH, SPRITE_GUAGE_HEIGHT);
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

    _buttons[PAIR_B].initButtonUL(&_lcd, S_COL_1, S_ROW_1, S_BTN_W, S_BTN_H, TFT_GREEN, BG_COLOR, TFT_WHITE, "Pair", 1.5);
    _buttons[TX_B].initButtonUL(&_lcd, S_COL_2, S_ROW_1, S_BTN_W, S_BTN_H, TFT_RED, BG_COLOR, TFT_WHITE, "Tx", 1.5);
    _buttons[RX_B].initButtonUL(&_lcd, S_COL_3, S_ROW_1, S_BTN_W, S_BTN_H, TFT_SKYBLUE, BG_COLOR, TFT_WHITE, "Rx", 1.5);

    _buttons[DISP_OPT_30S_B].initButtonUL(&_lcd, S_COL_1, S_ROW_2, S_BTN_W, S_BTN_H, TFT_YELLOW, BG_COLOR, TFT_WHITE, "30s", 1.5);
    _buttons[DISP_OPT_5M_B].initButtonUL(&_lcd, S_COL_2, S_ROW_2, S_BTN_W, S_BTN_H, TFT_YELLOW, BG_COLOR, TFT_WHITE, "5m", 1.5);
    _buttons[DISP_OPT_OFF_B].initButtonUL(&_lcd, S_COL_3, S_ROW_2, S_BTN_W, S_BTN_H, TFT_YELLOW, BG_COLOR, TFT_WHITE, "Off", 1.5);
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

    _lcd.fillRect(S_COL_1, H_GUAGE_ROW_1 - 12, 340, 250, BG_COLOR); // Cover home tab stuff

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
    }
}

/**
 * Displays the current bluetooth mode the device is i
 */
void GUIClass::ShowBTMode()
{
    _lcd.setTextSize(1);
    _lcd.setTextColor(TFT_WHITE);
    _lcd.fillRect(5, 0, 70, 13, BG_COLOR);
    if (_btMode == TRANSMITTER_M)
    {
        _lcd.setCursor(5, 5);
        _lcd.print("Transmitter");
    }
    else
    {
        _lcd.setCursor(5, 5);
        _lcd.print("Reciever");
    }
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
        delay(50);
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
                delay(50);
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
                    _buttons[bttn].drawButton();
                }
                // DisplayMenuMessage("released");
            }

            // button just pressed
            if (_buttons[bttn].justPressed())
            {
                if (!IsTabButtons((BUTTON_NAMES)bttn))
                {
                    _buttons[bttn].drawButton(true);
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
                // DisplayMenuMessage("pressed");
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
    if (_messageOn && millis() - _messageTime > 500)
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
        else if (_sleepMode == SLEEP_5M  && millis() - _lastTouch > (5*60 * 1000))
        {
            digitalWrite(LCD_POWER_PIN, LOW);
            _lcdState = DISPLAY_OFF;
        }
    }
}

/**
 * Displays a messages to the top menu bar of the GUI.
 */
void GUIClass::DisplayMenuMessage(String message)
{
    _lcd.setTextSize(1);
    _lcd.fillRect(200, 0, 230, 13, BG_COLOR);
    _lcd.setTextColor(TFT_GOLD);
    _lcd.setCursor(200, 5);
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
    _lcd.drawLine(0, 14, SCREEN_WIDTH, 14, TFT_WHITE);
    _lcd.drawLine(0, 15, SCREEN_WIDTH, 15, TFT_WHITE);
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
void GUIClass::DrawVoltageGauge()
{
}