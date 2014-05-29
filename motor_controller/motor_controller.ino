// include the library code:
#include <LiquidCrystal.h>

#define numberOfModes 4
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


long pos = 0;
int mode = 0;
bool buttonPressed = false;

bool actionBlip = false;

String names[numberOfModes] = {"Idle", "Relative Move", "Absolute Move", "Input Follower"};
int parameters[numberOfModes] = {0, 1000, 1000, 1};
int parametersMin[numberOfModes] = {0, 100, 100, 1};
int parametersMax[numberOfModes] = {0, 2000, 2000, 10};
int parametersInc[numberOfModes] = {0, 100, 100, 1};


void setup() {


  //delay(1000);
  //Serial.println("M45");
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Motor Test");

  // put your setup code here, to run once:
  Serial.begin(9600);
  initialize();
}

void initialize() {
  setTitle("Initializing");
  delay(1000);
  //Serial.println("X");
  out("S0");
  delay(1000);
  out("P0");
  delay(1000);
  pos = 0;
}

void loop() {

  if (mode ==  0) {
    idle();  
  }
  
  // move relative
  if (mode == 1) {
    relative(parameters[mode]);
  }

  // move absolute
  if (mode == 2) {
    absolute(parameters[mode]);
  }

  // input follower
  if (mode == 3) {
    follow(parameters[mode]);
  }

}

bool checkInput() {
  int x;
  x = analogRead (0);
  lcd.setCursor(10, 1);

  if (!buttonPressed) {
    if (x < 60) {
      mode++;
    }
    else if (x < 200) {
      parameters[mode] += parametersInc[mode];
    }
    else if (x < 400) {
      parameters[mode] -= parametersInc[mode];
    }
    else if (x < 600) {
      mode--;
    }
    else if (x < 800) {
      initialize();
    } 
  } else {
    if (x > 1000) {
      buttonPressed = false;
      return false;
    }
  }

  buttonPressed = true;

  if (mode > numberOfModes - 1) mode = 0;
  if (mode < 0) mode = numberOfModes - 1;
  if (parameters[mode] > parametersMax[mode]) parameters[mode] = parametersMax[mode];
  if (parameters[mode] < parametersMin[mode]) parameters[mode] = parametersMin[mode];

  return true;
}

void absolute(int delayTime) {
  String myString = String(pos);
  out("G" + myString);
  pos += 200;
  responsiveDelay(delayTime);
}

void relative(int delayTime) {
  out("R100");
  responsiveDelay(delayTime);
}

void idle() {
 responsiveDelay(100); 
}

void follow(int scale) {
  int input = analogRead(1);
  input *= scale;
  String myString = String(input);
  out("G" + myString);
  responsiveDelay(25);
}


void setTitle(String s) {
  lcd.setCursor(0, 0);
  lcd.print(s + "           ");
}

void setParameter(String s) {
  lcd.setCursor(12, 1);
  lcd.print(s + "   ");
}

void out(String s) {
  Serial.println(s);
  lcd.setCursor(0, 1);
  lcd.print(s);

  // action blip
  if (actionBlip) {
    lcd.print(".");
  } else {
    lcd.print(" ");
  }
  actionBlip = !actionBlip;

  for (int i = s.length(); i < 11; i++) {
    lcd.print(" ");
  }
}

void responsiveDelay(int time) {
  for (int i = 0; i < time; i += 22) {
    if (checkInput()) {
      setTitle(names[mode]);
      setParameter((String)parameters[mode]);
    }
    delay(20);
  }
}
