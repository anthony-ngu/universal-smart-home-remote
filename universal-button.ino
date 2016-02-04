#include "application.h"
#include "neopixel/neopixel.h"
#include "JSMNSpark/JSMNSpark.h" // for more info on JSMN check out @ https://github.com/zserge/jsmn

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D6
#define PIXEL_COUNT 24
#define PIXEL_TYPE WS2812B

void doEncoderA();
void doEncoderB();

// Button Globals
#define ENC_BUTTON_PIN D4 //push button switch
int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin
long lastButtonDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 100;    // the debounce time; increase if the output flickers

// Neopixel Globals
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// Setup Globals
String jsonString = NULL; // the established JSON string
jsmntok_t jsmnTokens[100]; //JSMN Tokens (a conversion of JSON)
int i = 0;
String beginString = "begin--";
String endString = "--end";

// Rotary Encoder Globals
#define ENC_A A0
#define ENC_B A1
volatile bool A_set = false;
volatile bool B_set = false;
volatile int encoderPos = 0;
int prevPos = 0;
int value = 0;
long lastEncoderDebounceTime = 0;

void setup()
{
    bool success = Particle.function("setupStruct", setupStructure);
    
    pinMode(ENC_A, INPUT_PULLUP);
    pinMode(ENC_B, INPUT_PULLUP);
    attachInterrupt(ENC_A, doEncoderA, CHANGE);
    attachInterrupt(ENC_B, doEncoderB, CHANGE);
  
    pinMode(ENC_BUTTON_PIN, INPUT_PULLUP);
    
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(10); // Stops the LEDs from being blinding
    
    jsonString = ""; // the established JSON string
    Particle.publish("setupInit");
}

void loop()
{
    // goIntoStandby();T);
    
    // strip.setPixelColor(0, strip.Color(0, 255, 0));
    // strip.setPixelColor(23, strip.Color(255, 0, 0));
    // strip.show();
    
    if (prevPos != encoderPos) {
    //     lastEncoderDebounceTime = millis();
    //     if ((millis() - lastEncoderDebounceTime) > debounceDelay) {
            prevPos = encoderPos;
            char str[63];
            sprintf(str, "%d", encoderPos);
            Particle.publish("encoderPos",str);
        // }
    }
    
    int reading = digitalRead(ENC_BUTTON_PIN);
    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
      // reset the debouncing timer
      lastButtonDebounceTime = millis();
    }
    if ((millis() - lastButtonDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
      // than the debounce delay, so take it as the actual current state:
      // if the button state has changed:
      if (reading != buttonState) {
        buttonState = reading;
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
            
            tokenArraySize = jsmn_parse(&parser, jsonStringData, jsmnTokens, 100); // hands back the required token allocation size
            // char str[63];
            // sprintf(str, "tokenArraySize: %d", tokenArraySize);
            // Particle.publish("parser", str);
        	    
        	if (tokenArraySize >= 0)
        	{
        	    // parsed successfuly
        	    for (int i = 0; i < 100; i++) 
        	    {
                	switch(jsmnTokens[i].type){
                	    case JSMN_OBJECT:
                	        // Particle.publish("parser", "object");
                	        break;
                	    case JSMN_ARRAY:
                	        // Particle.publish("parser", "array");
                	        break;
                	    case JSMN_STRING:
                	        // Particle.publish("parser", "string");
                	        break;
                	    case JSMN_PRIMITIVE:
                	        // Particle.publish("parser", "primitive");
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

void doEncoderA(){
  if( digitalRead(ENC_A) != A_set ) {  // debounce once more
    A_set = !A_set;
    // adjust counter + if A leads B
    if ( A_set && !B_set ) 
      encoderPos += 1;
  }
}

// Interrupt on B changing state, same as A above
void doEncoderB(){
   if( digitalRead(ENC_B) != B_set ) {
    B_set = !B_set;
    //  adjust counter - 1 if B leads A
    if( B_set && !A_set ) 
      encoderPos -= 1;
  }
}