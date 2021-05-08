#ifndef configFile
#define configFile

/* DEVICE */
#define AVAILABLE_CORES   ESP.getChipCores()
#define DISCONNECTED_PIN  32
SemaphoreHandle_t   programConfigSemaphore            = NULL;
struct ProgramConfig programConfig = {modeOFF, 50, 0, 0, 0};



/*  OLED SCREEN   */
#define Display_SCL_Pin   22            // IO22
#define Display_SDA_Pin   21            // IO21
#define SCREEN_WIDTH      128           // OLED display width, in pixels
#define SCREEN_HEIGHT     32            // OLED display height, in pixels
#define OLED_RESET        4             // Reset pin
#define SCREEN_ADDRESS    0x3C          //< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define WHITE             SSD1306_WHITE
#define BLACK             SSD1306_BLACK
#define FPS               60
int displayCore         = 0;            // Overwritten later. It's the core, on which the I^2C communication will take place on (it's also the core, the Arduino IDE starts on).



/*  ROTARY ENCODER  */
#define ROTARY_ENCODER_PIN_A                          18
#define ROTARY_ENCODER_PIN_B                          5
#define ROTARY_ENCODER_BUTTON_PIN                     19
#define ROTARY_ENCODER_BUTTON_POLLING_RATE            60
TaskHandle_t      rotaryEncoderInterruptTaskHandle  = NULL;
const int         rotaryEncoderDebounceTime         = 33;
int               rotaryEncoderValue                = 0;
boolean           rotaryEncoderButtonPressed        = false;
SemaphoreHandle_t rotaryEncoderSemaphore            = NULL;


/* WIFI SERVER + STATION */
#define   SERVER_PORT               80  // default port for HTTP, so you don't have to specify port in the browser
IPAddress AccessPointIPAddress;
#define   RECONNECTION_MAX_COUNT    15
#define   RECONNECTION_TIMEOUT      2000
IPAddress StationIPAddress;



#endif
