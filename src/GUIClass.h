#include "Parallel16_9488.h"
#include "T_FT6236.h"
#include "INA226.h"
#include "PCF8574.h"
#include "Wire.h"

// Definitons of current screen
#define SCREEN_WIDTH    479
#define SCREEN_HEIGHT   319

/**
 * Tab positions
*/
// tab (setting and home) column positions
#define TAB_ROW             22                          // Row tab y pos
#define TAB_COL_HOME        TAB_LINE_COL + 40           // Col x pos
#define TAB_COL_SETTINGS    TAB_LINE_COL_END - 40 - 66  // Col x pos
// position of tab lines
#define TAB_LINE_ROW        50
#define TAB_LINE_COL        120
#define TAB_LINE_COL_END    450
// for covering stuff between home and settings view
#define TAB_COVER_COL       TAB_LINE_COL
#define TAB_COVER_ROW       H_GUAGE_ROW-10
#define TAB_COVER_RECT_W    340
#define TAB_COVER_RECT_H    250

/**
 * Menu positions
*/
// top menu positions
#define MENU_LINE_ROW           14
#define MENU_BT_TEXT_COL        5
#define MENU_BT_COVER_RECT_W    70
#define MENU_BT_COVER_RECT_H    13
#define MENU_TEXT_ROW           5
#define MENU_MSG_TEXT_COL       200
#define MENU_MSG_COVER_RECT_W   230
#define MENU_MSG_COVER_RECT_H   13


/**
 * Setting's view
*/
// setting tab row positions
#define S_ROW_1         78                           // Settings row 1 y pos
#define S_ROW_2         S_ROW_1 + S_B_ROW_SPACE      // Settings row 2 y pos
#define S_ROW_3         S_ROW_2 + S_B_ROW_SPACE      // Settings row 3 y pos
#define S_B_ROW_SPACE   75                           // spacing between rows
// setting tab column positions
#define S_COL_1         TAB_COL_HOME - 15            // Settings col 1 x pos
#define S_COL_2         S_COL_1 + S_COL_SPACE        // Settings col 2 x pos
#define S_COL_3         S_COL_2 + S_COL_SPACE        // Settings col 3 x pos
#define S_COL_SPACE     100
// setting button sizes
#define S_BTN_W 80                                  // Settings button width
#define S_BTN_H 60                                  // Settings button height

/**
 * Home view 
*/
// home view row positions
#define CNTRL_COL 11                                // Vol up, down and mute col
#define VOL_UP_ROW 32                               // Vol up row
#define MUTE_ROW 142                                // Mute row
#define VOL_DOWN_ROW 222                            // Vol down row
// volume and mute sizes
#define VOL_W 82                                    // Vol width
#define VOL_H 92                                    // Vol height
#define MUTE_H 62                                   // Mute height

/**
 * Gauge definitions
*/
#define H_GUAGE_COL S_COL_1 + 65                    // Gauge col start
#define H_GUAGE_WIDTH 150                           // Guage width
#define H_GUAGE_HEIGHT 25                           // Guage height
// gauge row definitions
#define H_GUAGE_ROW 90-12                            // Starting row of gauges.. change this to change others

// dimensions of each sprite
#define SPRITE_GUAGE_WIDTH 146                      
#define SPRITE_BATTERY_SIZE 140+4                   // size of sprites (square)

/**
 * Brightness slider
*/
#define BRIGHT_SLIDER_WIDTH (S_COL_3-S_COL_1 + 30)
#define BRIGHT_SLIDER_UPPER_LIMIT 276-8
#define BRIGHT_SLIDER_LOWER_LIMIT 10

// Background color for whole system
#define BG_COLOR TFT_BLACK

// pin definitions for peripherals
#define LCD_POWER_PIN 45
#define I2C_SCL 39
#define I2C_SDA 38

// I2C addresses
#define TOUCH_I2C_ADD   0x38
#define PCF_I2C_ADD     0x24
#define INA226_ADD      0x40
#define LCD_BRIGHTNESS_CHANNEL 7                    // channel used to control screen brightness

class GUIClass
{
public:

    /**
     * All the buttons that are used in GUI
    */
    enum BUTTON_NAMES
    {
        VOL_UP_B,
        MUTE_B,
        VOL_DOWN_B,
        HOME_B,
        SKIP_BACK,
        SKIP_FORWARD,
        PLAY_PAUSE,
        SETTINGS_B,
        PAIR_B,
        TX_B,
        RX_B,
        DISP_OPT_30S_B,
        DISP_OPT_5M_B,
        DISP_OPT_OFF_B,
        BRIGHT_SLIDER_B,

        NUM_BUTTONS
    };

    /**
     * Actual buttons on TSA2500 in order which can be called on GPIO expander
    */
    enum BUTTON_PRESSED
    {
        RCX_PIN,
        TRX_PIN,
        PAIR_PIN,
        PLAY_PAUSE_PIN,
        VOL_DOWN_PIN,
        VOL_UP_PIN,
        NUM_PINS
    };
    /**
     * @brief Names for the state of the screen whether its on or off.
     *
     */
    enum DISPLAY_STATE
    {
        DISPLAY_ON,
        DISPLAY_OFF,
        NUM_DISPLAY_STATES
    };

    /**
     * @brief States the GUI can be in whether the home or the settings tab is selected
     *
     */
    enum GUI_VIEW
    {
        HOME_VIEW,
        SETTINGS_VIEW,
        NUM_GUI_VIEWS
    };

    /**
     * @brief The current mode of the device, whether it is in transmitter or
     * reciever mode.
     *
     */
    enum BT_MODE
    {
        TRANSMITTER_M,
        RECIEVER_M,
        NUM_BT_MODE
    };

    /**
     * @brief The current sleep mode the device can be set to.
     *
     */
    enum SLEEP_MODES
    {
        SLEEP_30S, // Screen will turn off after 30 seconds
        SLEEP_5M,  // 5 minutes
        SLEEP_OFF, // will never turn off
        NUM_SLEEP_MODE
    };

    GUIClass();

    // checks if there is any new buttons presses
    void CheckButtonPress();

    // draws the buttons to the screen
    void DrawButtons();

    // inits the projects
    void Start();

    // performs the necessary actions for a buton press
    void ButtonCallback(BUTTON_NAMES &pressed);

    // checks if button is settings view button
    bool IsSettingsButtons(BUTTON_NAMES pressed);
    
    // checks if button is home view button
    bool IsHomeButtons(BUTTON_NAMES pressed);

    // checks if button is a tab 
    bool IsTabButtons(BUTTON_NAMES pressed);

    // checks if theres anything in queue and services it
    void CheckQueue();

    // draws the menu to the lcd
    void DrawMenu();

    // shows the current bt mode 
    void ShowBTMode();

    // displays content to the screen for short bursts
    void DisplayMenuMessage(String message);

    // refreshes gauges and other live data to screen based on periods
    void RefreshData();

    // recreates the guages onto the screen
    void DrawVoltageGauge(float voltageInput);
    void DrawCurrentGauge(float voltageInput);


private:
    // internal members
    LGFX _lcd;
    LGFX_Button _buttons[NUM_BUTTONS];
    DISPLAY_STATE _lcdState;
    GUI_VIEW _guiView;
    BT_MODE _btMode;
    SLEEP_MODES _sleepMode;
    T_FT6236 _tScreen;

    // used for displaying messages
    bool _messageOn = false;
    ulong _messageTime = 0;

    // sprites used for guages
    LGFX_Sprite _spriteBattery;
    LGFX_Sprite _spriteCurrent;
    LGFX_Sprite _spriteBrightSlider;

    // starts the LCD panel
    void InitLcd();

    // creates the buttons
    void InitButtons();

    // starts gpio expansion board
    void InitPCF();

    // starts current sensor module
    void InitINA226();

    // changes the brightness of screen
    void ChangeBrightness();

    // Draws the brightness slider on screen based on new position
    void DrawBrightnessSlider();

    // given raw voltage, function converts to percentage 
    void DrawBatteryPercentage(float rawVoltage);

    // period used to determine how long to sleep after last touch
    ulong _sleepPeriod = 0;

    // time the last touch occured
    ulong _lastTouch = 0;

    // time last graphic update occured
    ulong _lastGraphics = 0;


    // position brightness slider was pressed
    int _sliderTouchX = BRIGHT_SLIDER_UPPER_LIMIT;

    // define external peripherals
    PCF8574 gpio = PCF8574(PCF_I2C_ADD,18,17);
    INA226 _powerSensor = INA226(INA226_ADD,&Wire1);

    // queue used for sending button presses across cores
    QueueHandle_t _queue = xQueueCreate(20, sizeof(int));

    // precise version of std map
    double mapf(double val, double inMin, double inMax, double outMin, double outMax);
};