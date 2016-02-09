typedef enum {
	MENU_ARRAY, // 0
	VALUE_ARRAY, // 1
	INT_RANGE // 2
} ValueType;

class MenuItem
{
	public:
		int length;
		String name;
		ValueType type;
		MenuItem* menuArray;
		char** valueArray;
		int min;
		int max;
		int step;
		MenuItem* parent;
		
	MenuItem(){}
	
	MenuItem(
		String inputName,
		ValueType inputType
	){
		name = inputName;
		type = inputType;
	}
	
	MenuItem(
		String inputName,
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
		String inputName,
		ValueType inputType,
		char** inputValueArray,
		int inputLength
	){
		name = inputName;
		type = inputType;
		valueArray = inputValueArray;
		length = inputLength;
	}
	
	MenuItem(
		String inputName,
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
