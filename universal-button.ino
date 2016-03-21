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
ClickButton button1(BUTTON_PIN, HIGH);
int function = 0;

// Neopixel Globals
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
#define BRIGHTNESS 10

// Rotary Encoder Globals
#define ENC_A_PIN D2
#define ENC_B_PIN D3
    
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastencoderValue = 0;
int lastMSB = 0;
int lastLSB = 0;

// OLED Globals
MicroOLED oled;
long lastActionTime; // get current time
bool screenOn = true;
long screenSaverDelay = 60000; // 60 seconds

// Battery Monitoring Globals
double voltage = 0; // Variable to keep track of LiPo voltage
double soc = 0; // Variable to keep track of LiPo state-of-charge (SOC)
bool alert; // Variable to keep track of whether alert has been triggered
int chargingLedValue = 0;
double MAX_LED_VALUE = 100.0;
bool ledValIncreasing = true;
long lastLedChangeTime = (long)millis(); // get current time
long ledAnimationDelay = 100;
int consistencyCounter = 0;
int consistencyCounterLimit = 3;
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
    lastActionTime = (long)millis();
    
    // Initialize Encoder
    pinMode(ENC_A_PIN, INPUT_PULLUP); 
    pinMode(ENC_B_PIN, INPUT_PULLUP);
    
    // attach interrupts
    attachInterrupt(ENC_A_PIN, updateEncoder, CHANGE);
    attachInterrupt(ENC_B_PIN, updateEncoder, CHANGE);

    // Button setup  
    pinMode(BUTTON_PIN, INPUT);
    button1.debounceTime   = 20;   // Debounce timer in ms
    button1.multiclickTime = 250;  // Time limit for multi clicks
    button1.longClickTime  = 1000; // time until "held-down clicks" register
    
    // Neopixel Ring Setup
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(BRIGHTNESS); // Stops the LEDs from being blinding
    strip.show();
    
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
    char* tempArray[] = {"on", "off"};
    char* tempArray2[] = {"Pandora", "HDMI1", "HDMI2", "HDMI3", "HDMI4"};
    char* tempArray3[] = {"play", "pause", "skip"};
    MenuItem powerItem = MenuItem(String("Power"), tempArray, 2);
    MenuItem volumeItem = MenuItem(String("Volume"), -500, 0, 10);
    MenuItem muteItem = MenuItem(String("Mute"),  tempArray, 2);
    MenuItem inputItem = MenuItem(String("Input"),  tempArray2, 5);
    MenuItem playbackItem = MenuItem(String("Controls"), tempArray3, 3);
    
    MenuItem itemArray[] = {powerItem, volumeItem, muteItem, playbackItem, inputItem};
    MenuItem receiverItem = MenuItem(String("Receiver"), itemArray, 5);
    
    MenuItem wemoItem = MenuItem(String("Wemo"), tempArray, 2);
    
    MenuItem deviceArray[] = {receiverItem, wemoItem};
    
    rootMenuItem = MenuItem(String("Devices"), deviceArray, 2);
    currentMenuItem = rootMenuItem;
}

void loop()
{
    char str[20];
    sprintf(str, "%d", encoderValue);
    encoderText = str;
    
    button1.Update();
    if (button1.clicks != 0)
    {
        function = button1.clicks;
        if(function == 1)
        {
            // go one level deeper or select
            if(currentMenuItem.optionType == MENU_ARRAY)
            {
                currentMenuItem = currentMenuItem.children[currentMenuItem.selectedIndex];
            }
            else
            {
                currentMenuItem.chooseSelection();   
            }
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
        lastActionTime = (long)millis();
        screenOn = true;
    }
    
    // Automatic Standbymode
    if (lastActionTime < (long)millis()-screenSaverDelay)
    {
        screenOn = false;
        oled.clear(PAGE);     // Clear the page
        oled.display();       // Refresh the display
        standbyLights();
        // Put the device into stop mode with wakeup using a change interrupt on one of the encoder pins
        System.sleep(BUTTON_PIN,RISING);
    }
    
    // LiPo and Battery Display
    double pastVoltage = voltage;
    voltage = lipo.getVoltage(); // lipo.getVoltage() returns a voltage value (e.g. 3.93)
    soc = lipo.getSOC(); // lipo.getSOC() returns the estimated state of charge (e.g. 79%)
    sprintf(batteryInfo, "%.1f%%", soc);
    if(pastVoltage<voltage)
    {
        consistencyCounter++;
    }
    else if(pastVoltage>voltage)
    {
        consistencyCounter = 0;
        standbyLights();
    }

    if(consistencyCounter > consistencyCounterLimit)
    {
        chargingLights();
    }
    // strip.setPixelColor(1, strip.Color(0, 0, 255));
    // strip.setPixelColor(0, strip.Color(0, 255, 0));
    // strip.setPixelColor(23, strip.Color(255, 0, 0));
    // strip.show();

    if(screenOn)
    {
        // LiPo and Battery Display
        oled.setFontType(0);  // Set font to type 0
        oled.clear(PAGE);     // Clear the page
        printBatteryIcon(soc);
        oled.setCursor(15, 0);
        oled.print(batteryInfo);
        int middleX = oled.getLCDWidth() / 2;
        oled.setCursor(middleX - (oled.getFontWidth() * (currentMenuItem.headerText.length()/2)) - 3, 20);
        oled.print(currentMenuItem.headerText);
        // oled.setCursor(0,20);
        // oled.print(buttonText);
        // oled.setCursor(0,30);
        // oled.print(encoderText);
        oled.setCursor(middleX - (oled.getFontWidth() * (currentMenuItem.selectionText.length()/2)) - 3, 40);
        oled.print(currentMenuItem.selectionText);
        oled.display();       // Refresh the display
    }else{
        oled.clear(PAGE);     // Clear the page
        oled.display();       // Refresh the display
    }
}

void printBatteryIcon(double percentage)
{
    for(int i=0;i<10;i++)
    {
        oled.pixel(i,0);
        oled.pixel(i,5);
    }
    for(int i=0;i<5;i++)
    {
        oled.pixel(0,i);
        oled.pixel(10,i);
    }
    for(int i=0;i<4;i++)
    {
        oled.pixel(11,i+1);
    }
    for(int i=0;i<percentage/10;i++)
    {
        for(int j=0;j<5;j++)
        {
            oled.pixel(i,j);
        }
    }
}

void standbyLights()
{
    for (int i = 0; i < 24; i++)
    {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
}

void chargingLights()
{
    if(lastLedChangeTime < (long)millis() - ledAnimationDelay)
    {
        lastLedChangeTime = (long)millis();
        double value = chargingLedValue/MAX_LED_VALUE;
        if(chargingLedValue > MAX_LED_VALUE)
        {
            ledValIncreasing = false;
        }
        else if(chargingLedValue < 0)
        {
            chargingLedValue = 0;
            ledValIncreasing = true;
        }
        
        for (int i = 0; i < 24; i++)
        {
            strip.setPixelColor(i, strip.Color(0,0,255*value));
        }
        strip.show();
        
        if(ledValIncreasing)
        {
            chargingLedValue++;
        }else{
            chargingLedValue--;
        }
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
    
void updateEncoder() {
	int MSB = digitalRead(ENC_A_PIN); //MSB = most significant bit
	int LSB = digitalRead(ENC_B_PIN); //LSB = least significant bit
    	
	int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
	int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
    
	if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011){
	    encoderValue ++;
	    currentMenuItem.increment();
	}
	if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000){
	    encoderValue --;
	    currentMenuItem.decrement();
	}
    
	lastEncoded = encoded; //store this value for next time
	lastActionTime = (long)millis();
    screenOn = true;
}