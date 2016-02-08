#include "jsmn.c"
#include "jsmn.h"
#include <stddef.h>
#include <stdio.h>
#include <string>

using namespace std;

typedef enum {
    MENU_ARRAY, // 0
    VALUE_ARRAY, // 1
    INT_RANGE, // 2
    STRING // 3
} ValueType;

class MenuItem
{
public:
    int length;
    string name;
    ValueType type;
    MenuItem* menuArray;
    string* valueArray;
    int min;
    int max;
    int step;
    string value;
    MenuItem* parent;
    
    MenuItem(){}
    
    MenuItem(
             string inputName,
             ValueType inputType
             ){
        name = inputName;
        type = inputType;
    }
    
    MenuItem(
             string inputName,
             ValueType inputType,
             MenuItem inputMenuArray[],
             int inputLength
             ){
        name = inputName;
        type = inputType;
        menuArray = inputMenuArray;
        length = inputLength;
    }
    
    MenuItem(
             string inputName,
             ValueType inputType,
             string inputValueArray[],
             int inputLength
             ){
        name = inputName;
        type = inputType;
        valueArray = inputValueArray;
        length = inputLength;
    }
    
    MenuItem(
             string inputName,
             ValueType inputType,
             int inputMin,
             int inputMax,
             int inputStep
             ){
        name = inputName;
        type = inputType;
        min = inputMin;
        max = inputMax;
        step = inputStep;
    }
    
    MenuItem(
             ValueType inputType,
             string inputValue
             ){
        type = inputType;
        value = inputValue;
    }
};

class MenuItemParseResult {
public:
    int newIndex; // the accumulated result of all its children
    MenuItem item; // the actual MenuItem returned
    MenuItemParseResult(){}
    
    MenuItemParseResult(int inputSize, MenuItem inputItem){
        newIndex = inputSize;
        item = inputItem;
    }
};

// Setup Globals
string jsonStringData; // the established JSON string
jsmntok_t jsmnTokens[100]; //JSMN Tokens (a conversion of JSON)
int i = 0;
string beginString = "begin--";
string endString = "--end";
string infoText = "Hello!";
string buttonText = "Not Pressed";
string encoderText = "0";

// MenuItemArray
MenuItem* menuItemArray;

// Function Declarations
// MenuItemParseResult ParseToMenuItem(string dataString, jsmntok_t tokens[], int index);
string ParseToString(string dataString, jsmntok_t token);
MenuItemParseResult ParseMenuArray(string dataString, jsmntok_t tokens[], int index);
MenuItemParseResult ParseToMenuItem(string dataString, jsmntok_t tokens[], int index);

void printMenuItem(MenuItem item, int levelDeep);
void printTabs(int levelDeep);

int main()
{
    jsmn_parser parser;
    jsonStringData = "{\"Receiver\":{\"power\":[\"on\",\"off\"],\"volume\":\"-500,0,10\",\"mute\":[\"on\",\"off\"],\"input\":[\"Pandora\",\"HDMI1\"]}}"; // the established JSON string
    printf("%s\n",jsonStringData.c_str());
    int tokenArraySize = jsmn_parse(&parser, jsonStringData.c_str(), jsonStringData.length(), jsmnTokens, 100); // has not been handing back the required token allocation size
    printf("tokenArraySize: %d\n", tokenArraySize);
    int sizeOfMenuArray = 0;
    
    if (tokenArraySize >= 0)
    {
        // parsed successfuly
        // First one is always the overarching one
        menuItemArray = (MenuItem *)valloc(jsmnTokens[0].size/2 * sizeof(MenuItem)); // divided by 2 since key value pairs are considered 2 tokens
        int index = 1;
        sizeOfMenuArray = jsmnTokens[0].size/2;
        for (int i = 0; i < sizeOfMenuArray; i++)
        {
            // The first should be a String
            string name = ParseToString(jsonStringData, jsmnTokens[index]);
            printf("name: %s\n",name.c_str());
            index++;// increment index
            // The second should be the Value (Item, String, etc.)
            MenuItemParseResult parseResult = ParseToMenuItem(jsonStringData, jsmnTokens, index);
            // // faking it
            // MenuItemParseResult parseResult;
            // parseResult.newIndex = 5;
            // parseResult.item = MenuItem();
            
            if(parseResult.newIndex < 0)
            {
                return -1;
            }else{
                index = parseResult.newIndex;
            }
            // MenuItem tempItem = MenuItem(name, MENU_ARRAY);
            // menuItemArray[i] = tempItem;
            parseResult.item.name = name;
            menuItemArray[i] = parseResult.item;
        }
    } else {
        // parse failed
        printf("parseFailed");
        return -1;
    }
    
     printf("\n\n\n---------------------------------\n");
     printf("Printing MenuItemArray\n");
     for(int i = 0; i<sizeOfMenuArray ;i++)
     {
       printMenuItem(menuItemArray[i], 0);
     }
}

string ParseToString(string dataString, jsmntok_t token)
{
    //.return dataString.substring(token.start, token.end-token.start);
    return dataString.substr(token.start, token.end-token.start);
}

MenuItemParseResult ParseToMenuItem(string dataString, jsmntok_t tokens[], int index)
{
    MenuItemParseResult itemParseResult;
    // string objectString = dataString.substring(tokens[index].start, tokens[index].end);
    string objectString = dataString.substr(tokens[index].start, tokens[index].end-tokens[index].start);
    // oled.clear(PAGE);
    // oled.setCursor(0,0);
#ifdef JSMN_PARENT_LINKS
    char str[63];
    sprintf(str, "%d-", token.parent);
    printf(str);
#endif
    switch(tokens[index].type)
    {
        case JSMN_OBJECT:
        {
            printf("MENU_ARRAY ");
            printf("size: %d\n",tokens[index].size);
            printf("objectString: %s\n",objectString.c_str());
            
            itemParseResult =  ParseMenuArray(dataString, tokens, index);
            break;
        }
        case JSMN_ARRAY:
        {
            // Particle.publish("parser", "array");
            printf("VALUE_ARRAY ");
            printf("size: %d\n",tokens[index].size);
            printf("objectString: %s\n",objectString.c_str());
            
            int returnIndex = index;
            string* valueArray = (string *)valloc(tokens[index].size * sizeof(string));
            for (int j = 0 ; j < tokens[index].size; j++)
            {
                // Assume that all VALUE_ARRAYS only contains strings
                int tempIndex = index + 1 + j; // add one since the index passed in was for the overall array
                // string tempString = dataString.substring(tokens[tempIndex].start, tokens[tempIndex].end);
                string tempString = dataString.substr(tokens[tempIndex].start, tokens[tempIndex].end-tokens[tempIndex].start);
                valueArray[j] = tempString;
            }
            returnIndex += tokens[index].size * 2 - 1;
            // Test printout
            for(int i = 0; i < tokens[index].size; i++)
            {
                printf("%s ", valueArray[i].c_str());
            }
            printf("\n");
            printf("tokenIndex: %d\n",returnIndex);
            MenuItem tempItem = MenuItem("", VALUE_ARRAY, valueArray, tokens[index].size);
            itemParseResult = MenuItemParseResult(returnIndex, tempItem);
            break;
        }
        case JSMN_STRING:
        {
            // Particle.publish("parser", "string");
            // This should be an int range
            // min, max, step
            // int minEndIndex = objectString.indexOf(',');
            // int maxEndIndex = objectString.lastIndexOf(',');
            int minEndIndex = (int)objectString.find_first_of(',');
            int maxEndIndex = (int)objectString.find_last_of(',');
            MenuItem tempItem;
            // printf("minEndIndex: %d\n", minEndIndex);
            // printf("maxEndIndex: %d\n", maxEndIndex);
            if(minEndIndex < 0)
            {
                // Should never reach this..
                // Not an INT_RANGE
                // printf("STRING ");
                // printf(objectString);
                // tempItem = MenuItem(STRING, objectString);
            }else{
                // string min = objectString.substring(0,minEndIndex);
                // string max = objectString.substring(minEndIndex+1,maxEndIndex);
                // string step = objectString.substring(maxEndIndex+1);
                
                string min = objectString.substr(0,minEndIndex);
                string max = objectString.substr(minEndIndex+1,maxEndIndex);
                string step = objectString.substr(maxEndIndex+1);
                
                printf("INT_RANGE ");
                // printf("min:"+min+"\n");
                // printf("max:"+max+"\n");
                // printf("step:"+step+"\n");
                
                printf("min: %d\n", stoi(min.c_str()));
                printf("max: %d\n", stoi(max.c_str()));
                printf("step: %d\n", stoi(step.c_str()));
                
                //tempItem = MenuItem("", INT_RANGE, min.toInt(), max.toInt(), step.toInt());
                tempItem = MenuItem("", INT_RANGE, stoi(min.c_str()), stoi(max.c_str()), stoi(step.c_str()));
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
            throw -8;
            break;
        }
        default:
        {
            // undefined
            throw -9;
            break;
        }
    }
#ifdef JSMN_PARENT_LINKS
    delay(5000);
#endif
    return itemParseResult;
}

MenuItemParseResult ParseMenuArray(string dataString, jsmntok_t tokens[], int index)
{
    MenuItem *tempMenuItemArray = (MenuItem *)valloc(jsmnTokens[0].size * sizeof(MenuItem)); // divided by 2 since key value pairs are considered 2 tokens
    int tempIndex = index+1;
    int sizeOfMenuArray = jsmnTokens[index].size;
    for (int i = 0; i < sizeOfMenuArray; i++)
    {
        // The first should be a String
        string name = ParseToString(jsonStringData, jsmnTokens[tempIndex]);
        printf("name: %s\n",name.c_str());
        tempIndex++;// increment index
        // The second should be the Value (Item, String, etc.)
        MenuItemParseResult parseResult = ParseToMenuItem(jsonStringData, jsmnTokens, tempIndex);
        
        printf("tempIndex: %d\n", parseResult.newIndex);
        
        if(parseResult.newIndex < 0)
        {
            throw -10;
        }else{
            //tempIndex += parseResult.newIndex;
            tempIndex = parseResult.newIndex;
            
        }
        parseResult.item.name = name;
        tempMenuItemArray[i] = parseResult.item;
        printf("MenuArrayIndex: %d\n", i);
    }
    MenuItem tempItem = MenuItem("", MENU_ARRAY, tempMenuItemArray, sizeOfMenuArray);
    return MenuItemParseResult(tempIndex, tempItem);
}

// Test Stuff
void printMenuItem(MenuItem item, int levelDeep){
    printTabs(levelDeep);
    printf("%s : ",item.name.c_str());
    
    switch(item.type)
    {
        case MENU_ARRAY:
            printf("MENU_ARRAY\n");
            for(int i = 0; i < item.length; i++)
            {
                printMenuItem(item.menuArray[i], levelDeep+1);
            }
            break;
        case VALUE_ARRAY:
            printf("VALUE_ARRAY\n");
            printTabs(levelDeep+1);
            for(int i = 0; i < item.length; i++)
            {
                printf("%s ", item.valueArray[i].c_str());
            }
            printf("\n");
            break;
        case INT_RANGE:
            printf("INT_RANGE\n");
            printTabs(levelDeep+1);
            printf("min: %d, max: %d, step: %d\n", item.min, item.max, item.step);
            break;
        default:
            printf("not a valid type");
            break;
    };
    printTabs(levelDeep);
//    printf("%s", item.parent->name.c_str());
}

void printTabs(int levelDeep){
    for(int i = 0; i<levelDeep; i++){
        printf("\t");
    }
}



