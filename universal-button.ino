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

void setup()
{
    Particle.function("setupStructure", setupStructure);
    
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(10); // Stops the LEDs from being blinding
    
    jsonString = ""; // the established JSON string
}

void loop()
{
    goIntoStandby();
    
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
    
    if(args.startsWith("{\"begin\":\"begin\""))
    {
        jsonString = args;
        strip.setPixelColor(1, strip.Color(255, 0, 0));
        strip.show();
    }else if(args.lastIndexOf("}") != -1){
        // it found the closing '}' so double check 
        jsonString += args;
        if(jsonString.endsWith("\"end\":\"end\"}"))
        {
            // parse it and set up the universal button as such
            strip.setPixelColor(2, strip.Color(255, 0, 0));
            strip.show();
            
            tokenArraySize = jsmn_parse(&parser, jsonString, null, 0); // hands back the required token allocation size
        	if (tokenArraySize != JSMN_ERROR_INVAL ||
        	    tokenArraySize != JSMN_ERROR_NOMEM ||
        	    tokenArraySize != JSMN_ERROR_PART)
        	{
        	    jsmntok_t jsmnTokens[tokenArraySize]; //JSMN Tokens (a conversion of JSON)
        	    int result = jsmn_parse(&parser, jsonString, jsmnTokens, tokenArraySize);
        	    if(result == JSMN_SUCCESS)
        	    {
        	        // parsed successfuly
        	        for (int i = 0; i < tokenArraySize; i++) 
        	        {
                		switch(jsmnTokens[i].type){
                		    case JSMN_OBJECT:
                		        break;
                		    case JSMN_ARRAY:
                		        break;
                		    case JSMN_STRING:
                		        break;
                		    case JSMN_PRIMITIVE:
                		        break;
                		    default:
                		        // undefined
                		        return -1;
                		        break;
                		}
                	}
            	}else {
            	    // parse failed
            	    return -1;
            	}
        	}else {
        	    // parse failed
        	    return -1;
        	}
        }   
    }
    return 1;
}