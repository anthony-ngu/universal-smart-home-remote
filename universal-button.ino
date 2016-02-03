#include "application.h"
#include "neopixel/neopixel.h"
#include "JSMNSpark/JSMNSpark.h" // for more info on JSMN check out @ https://github.com/zserge/jsmn

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D6
#define PIXEL_COUNT 24
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
String jsonString = NULL; // the established JSON string
jsmntok_t jsmnTokens[100]; //JSMN Tokens (a conversion of JSON)
int i = 0;
    
void setup()
{
    bool success = Particle.function("setupStruct", setupStructure);
    
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(10); // Stops the LEDs from being blinding
    
    jsonString = ""; // the established JSON string
}

void loop()
{
    // goIntoStandby();
    
    // strip.setPixelColor(0, strip.Color(0, 255, 0));
    // strip.setPixelColor(23, strip.Color(255, 0, 0));
    // strip.show();
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
    
    if(args.startsWith("{\"begin\":\"begin\""))
    {
        jsonString = args;
    }else{
        jsonString += args;
        if(args.endsWith("\"end\":\"end\"}"))
        {
            Particle.publish("received","endReceived");
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