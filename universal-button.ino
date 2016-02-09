// #define JSMN_PARENT_LINKS 1 // by defining it JSMN will include links to the parents - does not work right now..

#include "application.h"
#include "SparkFunMAX17043/SparkFunMAX17043.h"
#include "SparkFunMicroOLED/SparkFunMicroOLED.h"
#include "math.h"
#include "neopixel/neopixel.h"
// #include "JSMNSpark/JSMNSpark.h" // for more info on JSMN check out @ https://github.com/zserge/jsmn
#include "clickButton/clickButton.h"
#include "menuItem.h"

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D5
#define PIXEL_COUNT 24
#define PIXEL_TYPE WS2812B

// Button Globals
#define BUTTON_PIN D4
ClickButton button1(BUTTON_PIN, LOW, CLICKBTN_PULLUP);
int function = 0;

// Neopixel Globals
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// Rotary Encoder Globals
#define ENC_A D2
#define ENC_B D3
volatile int counter = 0;
int pastCounter = 0;

// OLED Globals
MicroOLED oled;

// Battery Monitoring Globals
double voltage = 0; // Variable to keep track of LiPo voltage
double soc = 0; // Variable to keep track of LiPo state-of-charge (SOC)
bool alert; // Variable to keep track of whether alert has been triggered
char batteryInfo[30];

// Setup Globals
int i = 0;
// String headerText = "Omnii";
// String selectionText = "";

String buttonText = "Not Pressed";
String encoderText = "0";
MenuItem rootMenuItem;
MenuItem currentMenuItem;

// Function Declarations
void encoder_counter();
void goIntoStandby();
void printData(char * str);

void setup()
{   
    // Encoder setup
    pinMode(ENC_A, INPUT_PULLUP);
    pinMode(ENC_B, INPUT_PULLUP);
    attachInterrupt(ENC_A, encoder_counter, FALLING); 

    // Button setup  
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    button1.debounceTime   = 20;   // Debounce timer in ms
    button1.multiclickTime = 250;  // Time limit for multi clicks
    button1.longClickTime  = 1000; // time until "held-down clicks" register
    
    // Neopixel Ring Setup
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(10); // Stops the LEDs from being blinding
    
    // Set up the MAX17043 LiPo fuel gauge:
    lipo.begin(); // Initialize the MAX17043 LiPo fuel gauge
    // Quick start restarts the MAX17043 in hopes of getting a more accurate
    // guess for the SOC.
    lipo.quickStart();
    
    // OLED Setup
    oled.begin();    // Initialize the OLED
    oled.clear(ALL); // Clear the display's internal memory
    oled.display();  // Display what's in the buffer (splashscreen)
    delay(1000);     // Delay 1000 ms
    oled.clear(PAGE); // Clear the buffer.
    
    // Info Setup
    // currentMenuItem = MenuItem("volume",-500, 0, 10, NULL);
    rootMenuItem = MenuItem();
    // String tempArray[2] = {String("on"), String("off")};
    // String tempArray2[2] = {String("Pandora"), String("HDMI1")};
    // MenuItem powerItem = MenuItem(String("power"), tempArray, 2);
    MenuItem volumeItem = MenuItem(String("volume"),-500, 0, 10);
    // MenuItem muteItem = MenuItem(String("mute"),  tempArray, 2);
    // MenuItem inputItem = MenuItem(String("input"),  tempArray2, 2);
    
    // MenuItem itemArray[4] = {powerItem, volumeItem, muteItem, inputItem};
    // rootMenuItem = MenuItem(String("Receiver"), itemArray, 4);
    // currentMenuItem = rootMenuItem;
    currentMenuItem = volumeItem;
}

void loop()
{
    // NeoPixel Ring
    // goIntoStandby();
    strip.setPixelColor(1, strip.Color(0, 0, 255));
    strip.setPixelColor(0, strip.Color(0, 255, 0));
    strip.setPixelColor(23, strip.Color(255, 0, 0));
    strip.show();
    
    // Encoder
    if(pastCounter != counter)
    {
        char str[20];
        sprintf(str, "%d", counter);
        encoderText = str;
        pastCounter = counter;
    }
    
    button1.Update();
    function = button1.clicks;
    if(function == 1)
    {
        // go one level deeper or select
        currentMenuItem.chooseSelection();
        buttonText = "SINGLE_CLICK";
        function = 0;
    }else if(function == 2)
    {
        // go one level back
        buttonText = "DOUBLE_CLICK";
        function = 0;
    }
    else if(function == -1) 
    {
        // go back to the home screen
        currentMenuItem = rootMenuItem;
        buttonText = "LONG_CLICK";
        function = 0;
    }else{
        buttonText = "";
    }
    
    // LiPo and Battery Display
    voltage = lipo.getVoltage(); // lipo.getVoltage() returns a voltage value (e.g. 3.93)
    soc = lipo.getSOC(); // lipo.getSOC() returns the estimated state of charge (e.g. 79%)
    sprintf(batteryInfo, "%.1f %%", soc);
    oled.setFontType(0);  // Set font to type 1
    oled.clear(PAGE);     // Clear the page
    oled.setCursor(0, 0);
    oled.print(batteryInfo);
    oled.setCursor(0,10);
    oled.print(currentMenuItem.headerText);
    oled.setCursor(0,20);
    oled.print(buttonText);
    oled.setCursor(0,30);
    oled.print(encoderText);
    oled.setCursor(0,40);
    oled.print(currentMenuItem.selectionText);
    oled.display();       // Refresh the display
}

void goIntoStandby()
{
    for (int i = 0; i < 24; i++)
    {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }

}

void encoder_counter(){
    if (digitalRead(ENC_B))
    {
        counter++ ;
        currentMenuItem.increment();
    }else{
        counter-- ;
        currentMenuItem.decrement();
    }
}

void printData(char * str)
{
    oled.clear(PAGE);
    oled.setCursor(0,0);
    oled.print(str);
    oled.display();
    delay(2000);
    oled.clear(PAGE);
}


