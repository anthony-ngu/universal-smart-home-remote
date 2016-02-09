// #define JSMN_PARENT_LINKS 1 // by defining it JSMN will include links to the parents - does not work right now..

#include "application.h"
#include "SparkFunMAX17043/SparkFunMAX17043.h"
#include "SparkFunMicroOLED/SparkFunMicroOLED.h"
#include "math.h"
#include "neopixel/neopixel.h"
#include "JSMNSpark/JSMNSpark.h" // for more info on JSMN check out @ https://github.com/zserge/jsmn
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
String jsonString; // the established JSON string
jsmntok_t jsmnTokens[50]; //JSMN Tokens (a conversion of JSON)
int i = 0;
String beginString = "begin--";
String endString = "--end";
String infoText = "Hello!";
String buttonText = "Not Pressed";
String encoderText = "0";

// MenuItemArray
MenuItem* menuItemArray;

// Function Declarations
String ParseToString(String dataString, jsmntok_t token);
MenuItemParseResult ParseMenuArray(String dataString, jsmntok_t tokens[], int index);
MenuItemParseResult ParseToMenuItem(String dataString, jsmntok_t tokens[], int index);

void setup()
{
    Particle.function("setupStruct", setupStructure);
    
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
    jsonString = ""; // the established JSON string
    Particle.publish("setupInit");
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
        // Particle.publish("encoder",str);
        encoderText = str;
        pastCounter = counter;
    }
    
    button1.Update();
    function = button1.clicks;
    if(function == 1)
    {
        // Particle.publish("button", "SINGLE click");
        buttonText = "SINGLE_CLICK";
        function = 0;
    }
    else if(function == -1) 
    {
        //Particle.publish("button", "SINGLE LONG click");
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
    oled.print(infoText);
    oled.setCursor(0,20);
    oled.print(buttonText);
    oled.setCursor(0,30);
    oled.print(encoderText);
    oled.display();       // Refresh the display
    
    delay(5);
}

void goIntoStandby()
{
    for (int i = 0; i < 24; i++)
    {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
    }

}

int setupStructure(String args)
{
    jsmn_parser parser; // creates the parser
    jsmn_init(&parser); // initializes the parser
    int tokenArraySize = 0;
    // Particle.publish("received", args);
    strip.setPixelColor(i++, strip.Color(255, 0, 0));
    strip.show();
    
    if(args.startsWith(beginString))
    {
        jsonString = args;
    }else{
        jsonString += args;
        if(args.endsWith(endString))
        {
            String jsonStringData = jsonString.substring(beginString.length(), jsonString.length() - endString.length());
            // Particle.publish("received","endReceived");
            // Particle.publish("receivedString", jsonStringData);
            
            // parse it and set up the universal button as such
            tokenArraySize = jsmn_parse(&parser, jsonStringData, jsmnTokens, 100); // has not been handing back the required token allocation size
            // char str[63];
            // sprintf(str, "tokenArraySize: %d", tokenArraySize);
            // Particle.publish("parser", str);
            
            int sizeOfMenuArray = 0;
            if (tokenArraySize >= 0)
            {
                /* For testing the jsmnToken parsing */
                // for (int i = 0; i < 100; i++){
                //     oled.clear(PAGE);
                //     oled.setCursor(0,0);
                //     char str[3];
                //     sprintf(str, "%d", i);
                //     oled.print(str);
                //     oled.print(jsonStringData.substring(jsmnTokens[i].start, jsmnTokens[i].end));
                //     oled.display();
                //     delay(3000);
                // }
                // parsed successfuly
                // First one is always the overarching one
                menuItemArray = (MenuItem *)malloc(jsmnTokens[0].size/2 * sizeof(MenuItem)); // divided by 2 since key value pairs are considered 2 tokens
                int index = 1;
                sizeOfMenuArray = jsmnTokens[0].size/2;
                for (int i = 0; i < sizeOfMenuArray; i++)
                {
                    // The first should be a String
                    String name = ParseToString(jsonStringData, jsmnTokens[index]);
                    // char str[30];
                    // sprintf(str, "name: %s %d\n",name.c_str(), sizeOfMenuArray);
                    // oled.clear(PAGE);
                    // oled.setCursor(0,0);
                    // oled.print(str);
                    // oled.print(index);
                    // oled.display();
                    // delay(1000);
                    // oled.clear(PAGE);
                    index++;// increment index
                    // The second should be the Value (Item, String, etc.)
                    MenuItemParseResult parseResult = ParseToMenuItem(jsonStringData, jsmnTokens, index);
                    // oled.clear(PAGE);
                    // oled.setCursor(0,0);
                    // char str[30];
                    // sprintf(str, "%s %d\n",name.c_str(), parseResult.newIndex);
                    // oled.print(str);
                    // oled.print(index);
                    // oled.display();
                    // delay(1000);
                    // oled.clear(PAGE);
                    
                    if(parseResult.newIndex < 0)
                    {
                        return -1;
                    }else{
                        index = parseResult.newIndex;
                    }
                    parseResult.item.name = name;
                    menuItemArray[i] = parseResult.item;
                }
            } else {
                // parse failed
                Particle.publish("parser", "failed");
                return -1;
            }
        }   
    }
    return 1;
}

String ParseToString(String dataString, jsmntok_t token)
{
    return dataString.substring(token.start, token.end);
}

MenuItemParseResult ParseToMenuItem(String dataString, jsmntok_t tokens[], int index)
{
    MenuItemParseResult itemParseResult;
    oled.clear(PAGE);
    oled.setCursor(0,0);
    // char str[15];
    // sprintf(str, "%d\n", index);
    // // sprintf(str, "%d - %d \n", tokens[index].start, tokens[index].end);
    // oled.print(str);
    // oled.display();
    // delay(2000);
    // oled.clear(PAGE); // Clear the buffer.
    oled.setCursor(0,0);
    String objectString = dataString.substring(tokens[index].start, tokens[index].end);
                  
    switch(tokens[index].type)
    {
        case JSMN_OBJECT:
            {
                oled.print("MENU_ARRAY ");
                oled.print(objectString);
                oled.display();       // Refresh the display
                delay(2000);
                oled.clear(PAGE); // Clear the buffer.
                itemParseResult =  ParseMenuArray(dataString, tokens, index);
                break;
            }
        case JSMN_ARRAY:
            {
                oled.print("VALUE_ARRAY ");
                oled.print(objectString);
                char str[30];
                sprintf(str, "size:%d start:%d end:%d\n", tokens[index].size, tokens[index].start, tokens[index].end);
                oled.print(str);
                oled.display();       // Refresh the display
                delay(2000);
                oled.clear(PAGE); // Clear the buffer.
                int returnIndex = index;
                String* valueArray = (String *)malloc(tokens[index].size * sizeof(String));
                for (int j = 0 ; j < tokens[index].size; j++)
                {
                    // Assume that all VALUE_ARRAYS only contains strings
                    int tempIndex = index + 1 + j; // add one since the index passed in was for the overall array
                    String tempString = dataString.substring(tokens[tempIndex].start, tokens[tempIndex].end);
                    tempString.concat("\0");
                    valueArray[j] = tempString;
                    oled.setCursor(0,0);
                    oled.print(tempString.toCharArray());
                    oled.display();       // Refresh the display
                    delay(2000);
                    oled.clear(PAGE); // Clear the buffer.
                }
                returnIndex += tokens[index].size * 2 - 1;
                MenuItem tempItem = MenuItem("", VALUE_ARRAY, valueArray, tokens[index].size);
                itemParseResult = MenuItemParseResult(returnIndex, tempItem);
                break;
            }
        case JSMN_STRING:
            {
                // This should be an int range
                // min, max, step
                int minEndIndex = objectString.indexOf(',');
                int maxEndIndex = objectString.lastIndexOf(',');
                String min = objectString.substring(0,minEndIndex);
                String max = objectString.substring(minEndIndex+1,maxEndIndex);
                String step = objectString.substring(maxEndIndex+1);
                MenuItem tempItem;
                
                if(minEndIndex >= 0)
                {
                    oled.print("INT_RANGE ");
                    oled.print("min:"+min+"\n");
                    oled.print("max:"+max+"\n");
                    oled.print("step:"+step+"\n");
                    
                    oled.display();       // Refresh the display
                    delay(2000);
                    oled.clear(PAGE); // Clear the buffer.
                    tempItem = MenuItem("", INT_RANGE, min.toInt(), max.toInt(), step.toInt());
                }else{
                //   throw -2;
                }
                itemParseResult = MenuItemParseResult(index+1, tempItem);
                break;
            }
        case JSMN_PRIMITIVE:
            {
                // Particle.publish("parser", "primitive");
                //  't', 'f' - boolean
                //  'n' - null
                //  '-', '0'..'9' - integer
                // We should never hit this..
                // throw -3;
                break;
            }
        default:
            {
                // undefined
                // throw -4;
                break;
            }
    }
    return itemParseResult;
}

MenuItemParseResult ParseMenuArray(String dataString, jsmntok_t tokens[], int index)
{
    MenuItem *tempMenuItemArray = (MenuItem *)malloc(jsmnTokens[index].size * sizeof(MenuItem)); // divided by 2 since key value pairs are considered 2 tokens
    int tempIndex = index+1;
    int sizeOfMenuArray = jsmnTokens[index].size;
    for (int i = 0; i < sizeOfMenuArray; i++)
    {
        // The first should be a String
        String name = ParseToString(dataString, jsmnTokens[tempIndex]);
        //printf("name: %s\n",name.c_str());
        tempIndex++;// increment index
        // The second should be the Value (Item, String, etc.)
        MenuItemParseResult parseResult = ParseToMenuItem(dataString, jsmnTokens, tempIndex);
        
        //printf("tempIndex: %d\n", parseResult.newIndex);
        
        if(parseResult.newIndex < 0)
        {
            // throw -10;
        }else{
            tempIndex = parseResult.newIndex;
            
        }
        parseResult.item.name = name;
        tempMenuItemArray[i] = parseResult.item;
        //printf("MenuArrayIndex: %d\n", i);
    }
    MenuItem tempItem = MenuItem("", MENU_ARRAY, tempMenuItemArray, sizeOfMenuArray);
    return MenuItemParseResult(tempIndex, tempItem);
}

// Center and print a small title
// This function is quick and dirty. Only works for titles one
// line long.
void printTitle(String title, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;

  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  oled.setCursor(middleX - (oled.getFontWidth() * (title.length()/2)),
                 middleY - (oled.getFontWidth() / 2));
  // Print the title:
  oled.print(title);
  oled.display();
  delay(1500);
  oled.clear(PAGE);
}

void encoder_counter(){
    if (digitalRead(ENC_B))
        counter++ ;
    else
        counter-- ;
}


