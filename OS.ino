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

class Tab {
private:
  #define ALL -1
public:
  // Stores the orientation of the elements in the tab
  ori justification;
  // Stores the cursor position in the tab
  int cursorPosition = 0;
  // Visibility of the cursor
  bool cursorVisible;
  // Title of the tab
  String title;
  // The adress of the String Array with the content of the tab
  String* texts;
  // The number os elements those is content of the tab
  int numberOfElements;
  // A Array of adresses of other tabs related to the content of this tab
  Tab** pointeds;
  // Store the results to be displayed
  String* results;

  // Main constructor
  Tab(String* _texts, String _title, bool _selectVis, ori _justification) {
    justification = _justification;
    cursorVisible = _selectVis;
    title = _title;
    texts = _texts;
    numberOfElements = 0;
    while(texts[numberOfElements] != "") {
      numberOfElements++;
    }
    pointeds = new Tab*[numberOfElements];
    results = new String[numberOfElements];
    for(int i = 0; i < numberOfElements; i++) {
      results[i] = "";
    }
  }

  // Overloaded constructors
  Tab(String* _texts, String _title, bool _selectVis) {
    Tab(_texts, _title, _selectVis, left);
  }
  Tab(String* _texts, String _title, ori _justification) {
    Tab(_texts, _title, false, _justification);
  }
  Tab(String* _texts, bool _selectVis, ori _justification) {
    Tab(_texts, _texts[0], _selectVis, _justification);
  }
  Tab(String* _texts, String _title) {
    Tab(_texts, _title, false, left);
  }
  Tab(String* _texts, bool _selectVis) {
    Tab(_texts, _texts[0], _selectVis, left);
  }
  Tab(String* _texts, ori _justification) {
    Tab(_texts, _texts[0], false, _justification);
  }
  Tab(String* _texts) {
    Tab(_texts, _texts[0], false, left);
  }

  void pointTo(int _index, Tab* _pointed) {
    if(_index < numberOfElements && _index > ALL) {
      pointeds[_index] = _pointed;
    }
    if(_index == ALL) {
      for(int i = 0; i < numberOfElements; i++) {
        pointeds[i] = _pointed;
      }
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

String MainText[] = {"Distance", "Temperature", "Light", "Microphone", "IR Receiver", "Aceleration", "Velocity", "Gyroscope", "Color", ""};
String DistanceText[] = {"$ cm", "$ in", "$ us", ""};
String TemperatureText[] = {"$ C", "$ F", "$ K", "value: $", ""};
String LightText[] = {"$ lux", "$ lm", "value: $", ""};
String MicrophoneText[] = {"$ dB", "$ Hz", "Tone: $", "value: $", ""};
String InfraRedText[] = {"HEX $", ""};
String AcelerometerText[] = {"x: $", "y: $", "z: $", "Acel: $", ""};
String VelocityText[] = {"x: $", "y: $", "z: $", "Vel: $", ""};
String GyroText[] = {"x: $", "y: $", "z: $", ""};
String ColorText[] = {"R: $", "G: $", "B: $", "HEX: $", ""};

// Define all the tabs
Tab MainTab(MainText, "MainTab", true, left);
Tab DistanceTab(DistanceText, "DistanceTab", true, right);
Tab TemperatureTab(TemperatureText, "TemperatureTab", true, right);
Tab LightTab(LightText, "LightTab", true, right);
Tab MicrophoneTab(MicrophoneText, "MicrophoneTab", true, right);
Tab InfraRedTab(InfraRedText, "InfraRedTab", true, right);
Tab AcelerometerTab(AcelerometerText, "AcelerometerTab", true, right);
Tab VelocityTab(VelocityText, "VelocityTab", true, right);
Tab GyroscopeTab(GyroText, "GyroscopeTab", true, right);
Tab ColorTab(ColorText, "ColorTab", true, right);

// ... pointTo(index, Tab)

Tab allTabs[] = {MainTab, DistanceTab, TemperatureTab, LightTab, MicrophoneTab, InfraRedTab, AcelerometerTab, VelocityTab, GyroscopeTab, ColorTab};
// Point to the current tab to be displayed
Tab* currentTab = &MainTab;

void display();
void relateTabs() {
  Serial.begin(9600);
  while(!Serial);
  // Correlate the tabs
  // Main Tab
  MainTab.pointTo(0, &DistanceTab);
  MainTab.pointTo(1, &TemperatureTab);
  MainTab.pointTo(2, &LightTab);
  MainTab.pointTo(3, &MicrophoneTab);
  MainTab.pointTo(4, &InfraRedTab);
  MainTab.pointTo(5, &AcelerometerTab);
  MainTab.pointTo(6, &VelocityTab);
  MainTab.pointTo(7, &GyroscopeTab);
  MainTab.pointTo(8, &ColorTab);
  // Others
  DistanceTab.pointTo(ALL, &MainTab);
  TemperatureTab.pointTo(ALL, &MainTab);
  LightTab.pointTo(ALL, &MainTab);
  MicrophoneTab.pointTo(ALL, &MainTab);
  InfraRedTab.pointTo(ALL, &MainTab);
  AcelerometerTab.pointTo(ALL, &MainTab);
  VelocityTab.pointTo(ALL, &MainTab);
  GyroscopeTab.pointTo(ALL, &MainTab);
  ColorTab.pointTo(ALL, &MainTab);
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

  relateTabs();
}

void loop() {
  if(digitalRead(RIGHT) == 0) {
    while(digitalRead(RIGHT) == 0);
    currentTab->moveCursor(1);
    moveRelativeCursor(1);
    lcd.clear();
  }
  if(digitalRead(LEFT) == 0) {
    while(digitalRead(LEFT) == 0);
    currentTab->moveCursor(-1);
    moveRelativeCursor(-1);
    lcd.clear();
  }
  if(digitalRead(ENTER) == 0) {
    while(digitalRead(ENTER) == 0);
    currentTab = currentTab->pointeds[currentTab->cursorPosition];
    relativeCursor = 0;
    lcd.clear();
  }
  display();
}

void display() {
  String lineText = "";
  for(int i = 0; i < LCDROW; i++) {
    // When the cursor is pointed in the last element of the tab
    if(currentTab->cursorPosition == currentTab->numberOfElements - 1) {
      relativeCursor = LCDROW - 1;
      if(currentTab->numberOfElements < LCDROW) {
        relativeCursor = currentTab->numberOfElements - 1;
      }
    }
    lineText = currentTab->texts[currentTab->cursorPosition - relativeCursor + i];
    // BUG: When using String.replace() in lineText as on the next code line, lineText becomes = "". Why?
    // lineText.replace("$", "0");
    lcd.setCursor(0, i);  //COl, ROW
    // If the cursor is visible
    if(currentTab->cursorVisible) {
      if(relativeCursor == i) {
        lcd.write(byte(POINT_ACTIVE));
      } else {
        lcd.print(" ");
      }
    }
    // Justify the tab to right
    if(currentTab->justification == right) {
      lcd.setCursor(LCDCOL - lineText.length(), i);
    }
    lcd.print(lineText);
    // if(lineText == "") {
    //   Serial.println("ERROR");
    // }
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
