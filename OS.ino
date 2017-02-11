// 02,07/2017 - 17:47

#include "LiquidCrystal.h"
#include "Timer.h"

#define LCDCOL 16
#define LCDROW 2
#define POINT_ACTIVE 0

#define IR 1
#define LEFT 8
#define RIGHT 9
#define ENTER 7
#define TRIG 10
#define ECHO 13
#define MIC A0
#define LDR A1
#define TEMP A2
#define LCD4 5
#define LCD5 4
#define LCD6 3
#define LCD7 2
#define LCDENB 11
#define LCDRST 0

int inputs[] = {IR, LEFT, RIGHT, ENTER, ECHO, MIC, LDR, TEMP};
int outputs[] = {TRIG};

LiquidCrystal lcd(LCDRST, LCDENB, LCD4, LCD5, LCD6, LCD7);
int relativeCursor = 0; // 0 to (LCDROW - 1)
char displayChars[LCDROW][LCDCOL];
byte pointActive[8] = { B00000, B10000, B11000, B11100, B11000, B10000, B00000,};

enum ori { left, right, center};

class Menu {
private:

public:
  // Stores the orientation of the elements in the Menu
  ori justification;
  // Stores the cursor position in the Menu
  int cursorPosition = 0;
  // Visibility of the cursor
  bool cursorVisible;
  // Title of the Menu
  char* title;
  // The adress of the char* Array with the content of the Menu
  char** texts;
  // The number os elements those is content of the Menu
  int numberOfElements;
  // A Array of adresses of other Menus related to the content of this Menu
  Menu** pointeds;
  // Store the results to be displayed
  char** results;

  // Main constructor
  Menu(char** _texts, char* _title, bool _selectVis, ori _justification) {
    justification = _justification;
    cursorVisible = _selectVis;
    title = _title;
    texts = _texts;
    numberOfElements = 0;
    while(texts[numberOfElements] != "") {
      numberOfElements++;
    }
    pointeds = new Menu*[numberOfElements];
    results = new char*[numberOfElements];
    for(int i = 0; i < numberOfElements; i++) {
      results[i] = "-";
    }
  }

  // Overloaded constructors
  Menu(char** _texts, char* _title, bool _selectVis) {
    Menu(_texts, _title, _selectVis, left);
  }
  Menu(char** _texts, char* _title, ori _justification) {
    Menu(_texts, _title, false, _justification);
  }
  Menu(char** _texts, bool _selectVis, ori _justification) {
    Menu(_texts, _texts[0], _selectVis, _justification);
  }
  Menu(char** _texts, char* _title) {
    Menu(_texts, _title, false, left);
  }
  Menu(char** _texts, bool _selectVis) {
    Menu(_texts, _texts[0], _selectVis, left);
  }
  Menu(char** _texts, ori _justification) {
    Menu(_texts, _texts[0], false, _justification);
  }
  Menu(char** _texts) {
    Menu(_texts, _texts[0], false, left);
  }

  void pointTo(int _index, Menu* _pointed) {
    if(_index < numberOfElements && _index >= 0) {
      pointeds[_index] = _pointed;
    }
  }

  void pointTo(Menu* _pointed) {
    for(int i = 0; i < numberOfElements; i++) {
      pointeds[i] = _pointed;
    }
  }

  void moveCursor(int _factor) {
    cursorPosition += _factor;
    if(cursorPosition > (numberOfElements - 1)) {
      cursorPosition = numberOfElements - 1;
    }
    if(cursorPosition < 0) {
      cursorPosition = 0;
    }
  }
};

char* MainText[] = {"Distance", "Temperature", "Light", "Microphone", "IR Receiver", "Aceleration", "Velocity", "Gyroscope", "Color", ""};
char* DistanceText[] = {"$ cm", "$ in", "$ us", ""};
char* TemperatureText[] = {"$ C", "$ F", "$ K", "value: $", ""};
char* LightText[] = {"$ lux", "$ lm", "value: $", ""};
char* MicrophoneText[] = {"$ dB", "$ Hz", "Tone: $", "value: $", ""};
char* InfraRedText[] = {"HEX $", ""};
char* AcelerometerText[] = {"x: $", "y: $", "z: $", "Acel: $", ""};
char* VelocityText[] = {"x: $", "y: $", "z: $", "Vel: $", ""};
char* GyroText[] = {"x: $", "y: $", "z: $", ""};
char* ColorText[] = {"R: $", "G: $", "B: $", "HEX: $", ""};

// Define all the Menus
Menu MainMenu(MainText, "MainMenu", true, left);
Menu DistanceMenu(DistanceText, "DistanceMenu", true, right);
Menu TemperatureMenu(TemperatureText, "TemperatureMenu", true, right);
Menu LightMenu(LightText, "LightMenu", true, right);
Menu MicrophoneMenu(MicrophoneText, "MicrophoneMenu", true, right);
Menu InfraRedMenu(InfraRedText, "InfraRedMenu", true, right);
Menu AcelerometerMenu(AcelerometerText, "AcelerometerMenu", true, right);
Menu VelocityMenu(VelocityText, "VelocityMenu", true, right);
Menu GyroscopeMenu(GyroText, "GyroscopeMenu", true, right);
Menu ColorMenu(ColorText, "ColorMenu", true, right);

Menu allMenus[] = {MainMenu, DistanceMenu, TemperatureMenu, LightMenu, MicrophoneMenu, InfraRedMenu, AcelerometerMenu, VelocityMenu, GyroscopeMenu, ColorMenu};
// Point to the current Menu to be displayed
Menu* currentMenu = &MainMenu;

void display();
void relateMenus() {
  // Correlate the Menus
  // Main Menu
  MainMenu.pointTo(0, &DistanceMenu);
  MainMenu.pointTo(1, &TemperatureMenu);
  MainMenu.pointTo(2, &LightMenu);
  MainMenu.pointTo(3, &MicrophoneMenu);
  MainMenu.pointTo(4, &InfraRedMenu);
  MainMenu.pointTo(5, &AcelerometerMenu);
  MainMenu.pointTo(6, &VelocityMenu);
  MainMenu.pointTo(7, &GyroscopeMenu);
  MainMenu.pointTo(8, &ColorMenu);
  // Others
  DistanceMenu.pointTo(&MainMenu);
  TemperatureMenu.pointTo(&MainMenu);
  LightMenu.pointTo(&MainMenu);
  MicrophoneMenu.pointTo(&MainMenu);
  InfraRedMenu.pointTo(&MainMenu);
  AcelerometerMenu.pointTo(&MainMenu);
  VelocityMenu.pointTo(&MainMenu);
  GyroscopeMenu.pointTo(&MainMenu);
  ColorMenu.pointTo(&MainMenu);
}
void relateFunctions() {
  // TODO: relate each Menu with their respectives functions
}

void setup() {
  // Define all inputs
  for(int i = 0; i < (sizeof(inputs) / sizeof(int)); i++) {
    pinMode(inputs[i], INPUT);
  }
  // Define all outputs
  for(int i = 0; i < (sizeof(outputs) / sizeof(int)); i++) {
    pinMode(outputs[i], OUTPUT);
  }

  lcd.begin(LCDCOL, LCDROW);
  lcd.createChar(POINT_ACTIVE, pointActive);

  relateMenus();
  relateFunctions();
}

void loop() {
  if(digitalRead(RIGHT) == 0) {
    while(digitalRead(RIGHT) == 0);
    currentMenu->moveCursor(1);
    moveRelativeCursor(1);
    lcd.clear();
  }
  if(digitalRead(LEFT) == 0) {
    while(digitalRead(LEFT) == 0);
    currentMenu->moveCursor(-1);
    moveRelativeCursor(-1);
    lcd.clear();
  }
  if(digitalRead(ENTER) == 0) {
    while(digitalRead(ENTER) == 0);
    currentMenu = currentMenu->pointeds[currentMenu->cursorPosition];
    relativeCursor = 0;
    lcd.clear();
  }
  display();
}

void display() {
  // string that saves the current text to be displayed
  char* lineText;
  // TODO: connect the functions with their relative value to show here
  for(int i = 0; i < LCDROW; i++) {
    // When the cursor is pointed in the last element of the Menu
    if(currentMenu->cursorPosition == currentMenu->numberOfElements - 1) {
      relativeCursor = LCDROW - 1;
      if(currentMenu->numberOfElements < LCDROW) {
        relativeCursor = currentMenu->numberOfElements - 1;
      }
    }
    // get the text based in the current menu and the cursor position on the display
    lineText = currentMenu->texts[currentMenu->cursorPosition - relativeCursor + i];
    // replace "$" for the value it must display
    strrep(&lineText, "$", currentMenu->results[currentMenu->cursorPosition - relativeCursor + i]);
    // set the cursor to the beginning of the display
    lcd.setCursor(0, i);  //COl, ROW
    // If the cursor is visible
    if(currentMenu->cursorVisible) {
      if(relativeCursor == i) {
        lcd.write(byte(POINT_ACTIVE));
      } else {
        lcd.print(" ");
      }
    }
    // Justify the Menu to right
    if(currentMenu->justification == right) {
      lcd.setCursor(LCDCOL - strlen(lineText), i);
    }
    // Justify the Menu to center
    if(currentMenu->justification == center) {
      lcd.setCursor((LCDCOL - strlen(lineText)) / 2, i);
    }
    lcd.print(lineText);
    // delete useless memory
    if (lineText != currentMenu->texts[currentMenu->cursorPosition - relativeCursor + i]) {
      delete lineText;
    }
  }
}

void strrep(char** source, char* search , char* replace) {
  char* newstr;
  char* found = NULL;
  char* after = NULL;
  int foundPosition = 0;
  found = strstr(*source, search);
  if(found != NULL) {
    foundPosition = found - *source;
    // search for more replacement accurences in after string
    after = found + strlen(search);
    strrep(&after, search, replace);
    // newstr = (char*) realloc(*source, sizeof(char) * (1 + foundPosition + strlen(replace) + strlen(after)));
    newstr = new char[1 + foundPosition + strlen(replace) + strlen(after)];

    strncpy(newstr, *source, foundPosition);
    strncpy(newstr + foundPosition, replace, strlen(replace));
    strcpy(newstr + foundPosition + strlen(replace), after);

    *source = newstr;
  }
}

void moveRelativeCursor(int _factor) {
  relativeCursor += _factor;
  if(relativeCursor > (LCDROW - 1)) {
    relativeCursor = LCDROW - 1;
  }
  if(relativeCursor < 0) {
    relativeCursor = 0;
  }
}

// TODO: READING FUNCTINOS
void distanceRead() {

}
void temperatureRead() {

}
void lightRead() {

}
void microphoneRead() {

}
void irRead() {

}
