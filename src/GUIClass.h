#include "FT6236.h"
#include "Parallel16_9488.h"
#include "T_FT6236.h"
#include "PCF8574.h"

// Definitons of current screen
#define SCREEN_WIDTH 479
#define SCREEN_HEIGHT 319

// S denoting settings tab
#define S_ROW_1 95                      // Settings row 1 y pos
#define S_ROW_2 S_ROW_1 + S_B_ROW_SPACE // Settings row 2 y pos
#define S_B_ROW_SPACE 80                // spacing between rows

#define S_COL_1 TAB_COL_HOME - 15     // Settings col 1 x pos
#define S_COL_2 S_COL_1 + S_COL_SPACE // Settings col 2 x pos
#define S_COL_3 S_COL_2 + S_COL_SPACE // Settings col 3 x pos
#define S_COL_SPACE 100

#define S_BTN_W 80 // Settings button width
#define S_BTN_H 60 // Settings button height

#define TAB_ROW 22                                  // Row tab y pos
#define TAB_COL_HOME TAB_LINE_COL + 40              // Col x pos
#define TAB_COL_SETTINGS TAB_LINE_COL_END - 40 - 66 // Col x pos

#define CNTRL_COL 11     // Vol up, down and mute col
#define VOL_UP_ROW 32    // Vol up row
#define MUTE_ROW 142     // Mute row
#define VOL_DOWN_ROW 222 // Vol down row
#define VOL_W 82         // Vol width
#define VOL_H 92         // Vol height
#define MUTE_H 62        // Mute height

#define TAB_LINE_ROW 50
#define TAB_LINE_COL 120
#define TAB_LINE_COL_END 450

// Gauge definitions
#define H_GUAGE_COL S_COL_1 + 65 // Gauge col start
#define H_GUAGE_WIDTH 150        // Guage width
#define H_GUAGE_HEIGHT 25        // Guage height

#define H_GUAGE_ROW_1 90 // Starting row of gauges.. change this to change others
#define H_GUAGE_ROW_2 H_GUAGE_ROW_1 + H_GUAGE_ROW_SPACE
#define H_GUAGE_ROW_3 H_GUAGE_ROW_2 + H_GUAGE_ROW_SPACE

#define H_GUAGE_ROW_SPACE 48      // Space between rows
#define H_GUAGE_DATA_COL 225 + 65 // Col where data ie vars are printed to

#define MENU_BATTERY_COL SCREEN_WIDTH - 38

#define SPRITE_GUAGE_WIDTH 146
#define SPRITE_GUAGE_HEIGHT 21

// color definitions
#define BG_COLOR TFT_BLACK

// pin definitions
#define LCD_POWER_PIN 45
#define I2C_SCL 39
#define I2C_SDA 38
#define TOUCH_I2C_ADD   0x38

#define PCF_I2C_ADD     0x24

class GUIClass
{
public:
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

        NUM_BUTTONS
    };
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

    // GUIClass();
    // ~gui


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

    void DrawVoltageGauge();

private:
    // internal members
    LGFX _lcd;
    LGFX_Button _buttons[NUM_BUTTONS];
    DISPLAY_STATE _lcdState;
    GUI_VIEW _guiView;
    BT_MODE _btMode;
    SLEEP_MODES _sleepMode;
    T_FT6236 _tScreen;


    bool _messageOn = false;
    ulong _messageTime = 0;
    // sprites
    LGFX_Sprite _battery1Sprite;
    LGFX_Sprite _battery2Sprite;

    void InitLcd();

    // creates the buttons
    void InitButtons();

    // starts gpio expansion board
    void InitPCF();

    ulong _sleepPeriod = 0;

    ulong _lastTouch = 0;
    // TwoWire gpioWire = TwoWire(0);
    // TwoWire touchWire = TwoWire(1);

    PCF8574 gpio = PCF8574(PCF_I2C_ADD,18,17);

    QueueHandle_t _queue = xQueueCreate(20, sizeof(int));

    double mapf(double val, double inMin, double inMax, double outMin, double outMax);
};