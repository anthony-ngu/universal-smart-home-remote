#include "application.h"
#include "neopixel/neopixel.h"
#include "JSMNSpark/JSMNSpark.h" // for more info on JSMN check out @ https://github.com/zserge/jsmn

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D6
#define PIXEL_COUNT 24
#define PIXEL_TYPE WS2812B

/* Rotary encoder */
// #define ENC_A 14
// #define ENC_B 15
#define ENC_SWITCH_PIN D4 //push button switch

String beginString = "begin--";
String endString = "--end";

int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 100;    // the debounce time; increase if the output flickers

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
String jsonString = NULL; // the established JSON string
jsmntok_t jsmnTokens[100]; //JSMN Tokens (a conversion of JSON)
int i = 0;
    
void setup()
{
    bool success = Particle.function("setupStruct", setupStructure);
    
    // pinMode(ENC_A, INPU
    // pinMode(ENC_B, INPUT);
    // digitalWrite(ENC_B, HIGH);
    pinMode(ENC_SWITCH_PIN, INPUT_PULLUP);
    
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(10); // Stops the LEDs from being blinding
    
    jsonString = ""; // the established JSON string
}

void loop()
{
    // goIntoStandby();T);
    
    // strip.setPixelColor(0, strip.Color(0, 255, 0));
    // strip.setPixelColor(23, strip.Color(255, 0, 0));
    // strip.show();
    
    // static uint8_t counter = 0;      //this variable will be changed by encoder input
    // int8_t tmpdata;
    // 
    // tmpdata = read_encoder();
    // if( tmpdata ) {
    //     char str[63];
    //     sprintf(str, "%d", counter);
    //     Particle.publish("encoder", str);
    //     counter += tmpdata;
    // }
    
    int reading = digitalRead(ENC_SWITCH_PIN);
    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
      // reset the debouncing timer
      lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:
    
      // if the button state has changed:
      if (reading != buttonState) {
        buttonState = reading;
    
        // only toggle the LED if the new button state is HIGH
        if (buttonState == LOW) {
          Particle.publish("button", "pressed");
        }
      }
    }
    lastButtonState = reading;
}

void goIntoStandby()
{
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
}

int setupStructure(String args){
    jsmn_parser parser; // creates the parser
    jsmn_init(&parser); // initializes the parser
    int tokenArraySize = 0;
    Particle.publish("received", args);
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
            Particle.publish("received","endReceived");
            Particle.publish("receivedString", jsonStringData);
            // parse it and set up the universal button as such
            
            tokenArraySize = jsmn_parse(&parser, jsonString, jsmnTokens, 100); // hands back the required token allocation size
            char str[63];
            sprintf(str, "tokenArraySize: %d", tokenArraySize);
        	Particle.publish("parser", str);
        	    
        	if (tokenArraySize >= 0)
        	{
        	    Particle.publish("parser", "failed");
        	    // parsed successfuly
        	    for (int i = 0; i < tokenArraySize; i++) 
        	    {
                	switch(jsmnTokens[i].type){
                	    case JSMN_OBJECT:
                	        Particle.publish("parser", "object");
                	        break;
                	    case JSMN_ARRAY:
                	        Particle.publish("parser", "array");
                	        break;
                	    case JSMN_STRING:
                	        Particle.publish("parser", "string");
                	        break;
                	    case JSMN_PRIMITIVE:
                	        Particle.publish("parser", "primitive");
                	        //  't', 'f' - boolean
                            //  'n' - null
                            //  '-', '0'..'9' - integer
                	        break;
                	    default:
                	        // undefined
                	        return -1;
                	        break;
                	}
                }
            }else {
        	    // parse failed
        	    Particle.publish("parser", "failed");
        	    return -1;
        	}
        }   
    }
    return 1;
}