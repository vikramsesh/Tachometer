/*
   Author: Vikram Seshadri
   06/12/2019

   Description:
   Using an IR sensor to detect pulses when a reflective tape is detected.
   By using these pulses rps and rpm are calculated.

   v1 edits:
   - Adding gear ratio
*/

#define LCDRED 23
#define LCDBLUE 25
#define LCDGREEN 27
#define SDSS  53

#define LEFTBUTTONPIN 14
#define UPBUTTONPIN 15
#define DOWNBUTTONPIN 16
#define RIGHTBUTTONPIN 17
#define SELECTBUTTONPIN 19

// include the library code:
#include <LiquidCrystal.h>
#include <SD.h>

LiquidCrystal lcd(7, 6, 24, 22, 28, 26);

int sensorvalue;
int state1 = HIGH;
int state2;
float rps;
float rpm;
float blade_rpm;
float maxrpm = 0;
float blade_max_rpm = 0;

long prevMillis = 0;
long interval = 200;
long currentTime;
long prevTime = 1;
long diffTime;
int cnt = 0;

String dataString = "";
String sensor = "Base";

bool leftrightflag = 0;
bool loopflag = 0;
bool flag = 0;
bool whileflag = 0;

int selectButtonState = 0;
int leftButtonState = 0;
int rightButtonState = 0;

int analogpin = 55;
int gear_ratio = 1;

char x[8]; //timer
/*************************************************************/
/* SD Class */
class CSVFile
{
  public:
    char filenamechar[100];
    File file;
    //SdFat SD;
    String base_filename;
    int file_number;
    String full_filename;

    CSVFile(String bfilename)
    {
      base_filename = bfilename;
      file_number = 1;
      full_filename = bfilename + String(file_number) + String(F(".csv"));
      full_filename.toCharArray(filenamechar, 100);
    }

    void beginNow()
    {
      SD.begin(SDSS);
    }

    void openFile()
    {
      file = SD.open(filenamechar, FILE_WRITE);
    }

    void closeFile()
    {
      file.close();
    }

    void writeToCSV(String data)
    {
      file.print(data);
      file.print(String(F("\r\n")));
    }

    void writeHeaders()
    {
      file.println(String(F("Sample#,Time(sec.),RPS,Motor RPM,Max. Motor RPM,Gear Ratio,Blade RPM,Blade Max. RPM,Sensor, "))); //Headers
    }

    void _update_file_number()
    {
      // See if the file already exists, if so, see if it has _###
      // If file doesn't exist, add _###
      // If file does exists, make _### 1 more

      while (SD.exists(filenamechar)) {
        file_number += 1;
        full_filename = base_filename + String(file_number) + String(F(".csv"));
        full_filename.toCharArray(filenamechar, 100);
      }
    }
};

//setup csv file object
String filename = "RPM";
CSVFile csv(filename);

void screen1()
{
  selectButtonState = digitalRead(SELECTBUTTONPIN);
  leftButtonState = digitalRead(LEFTBUTTONPIN);
  rightButtonState = digitalRead(RIGHTBUTTONPIN);

  if (flag == 0) {
    lcd.setCursor(0, 0);
    lcd.display();
    lcd.print("Choose sensor");
    lcd.setCursor(0, 1);
    lcd.print("Top < | > Base");
    flag = 1;
  }

  if (leftButtonState == LOW) {
    leftrightflag = 1;
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Top sensor");
    lcd.setCursor(0, 1);
    lcd.print("selected");
  }

  if (rightButtonState == LOW) {
    leftrightflag = 0;
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Base sensor");
    lcd.setCursor(0, 1);
    lcd.print("selected");
  }

  if (selectButtonState == LOW) {
    if (leftrightflag == 1) {
      analogpin = 54;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.display();
      lcd.print("Top sensor");
      lcd.setCursor(0, 1);
      lcd.print("selected");
      Serial.println("Top Sensor selected");
      delay (1000);
    }

    if (leftrightflag == 0) {
      analogpin = 55;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.display();
      lcd.print("Base sensor");
      lcd.setCursor(0, 1);
      lcd.print("selected");
      Serial.println("Base Sensor selected");
      delay (1000);
    }
    loopflag = 1;
    flag = 0;
  }
}

void screen2() {
  selectButtonState = digitalRead(SELECTBUTTONPIN);
  leftButtonState = digitalRead(LEFTBUTTONPIN);
  rightButtonState = digitalRead(RIGHTBUTTONPIN);
  delay(150);

  if (flag == 0) {
    selectButtonState = HIGH;
    leftButtonState = HIGH;
    rightButtonState = HIGH;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.display();
    lcd.print("Gear Ratio <|>");
    lcd.setCursor(0, 1);
    lcd.print("1:1");
    flag = 1;
  }

  if (leftButtonState == LOW) {
    leftrightflag = 1;
    //    delay(300);
    if (gear_ratio <= 1) {
      lcd.display();
      lcd.setCursor(0, 1);
      lcd.print(String(gear_ratio) + ":1");
    }
    else {
      gear_ratio = gear_ratio - 1;
      lcd.display();
      lcd.setCursor(0, 1);
      lcd.print(String(gear_ratio) + ":1");
    }
  }

  if (rightButtonState == LOW) {
    //    delay(300);
    leftrightflag = 0;
    if (gear_ratio >= 6) {
      lcd.display();
      lcd.setCursor(0, 1);
      lcd.print(String(gear_ratio) + ":1");
    }
    else {
      gear_ratio = gear_ratio + 1;
      lcd.display();
      lcd.setCursor(0, 1);
      lcd.print(String(gear_ratio) + ":1");
    }
  }

  if (selectButtonState == LOW) {
    //    delay(300);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.display();
    lcd.print("Gear Ratio is");
    lcd.setCursor(0, 1);
    lcd.print(String(gear_ratio) + ":1");
    Serial.println("Gear Ratio: " + String(gear_ratio) + ":1");
    whileflag = 1;

  }
  //break;
}
void setup()
{
  Serial.begin(9600);
  pinMode(13, OUTPUT);     // assign pin 13 led as indicator because we cannot see the IR light
  lcd.begin(16, 2);
  pinMode(LCDRED, OUTPUT);
  pinMode(LCDBLUE, OUTPUT);
  pinMode(LCDGREEN, OUTPUT);
  lcd.display();

  pinMode(LEFTBUTTONPIN, INPUT_PULLUP);
  pinMode(RIGHTBUTTONPIN, INPUT_PULLUP);
  pinMode(UPBUTTONPIN, INPUT_PULLUP);
  pinMode(DOWNBUTTONPIN, INPUT_PULLUP);
  pinMode(SELECTBUTTONPIN, INPUT_PULLUP);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.display();
  lcd.print("Tachometer");
  lcd.setCursor(0, 1);
  lcd.print("Welcomes you");
  delay(2000);
  lcd.clear();

  //Select Sensor Button function
  while (1) {
    if (loopflag == 0) {
      screen1();
    }
    else
    {
      screen2();
      if (whileflag == 1) {
        delay (1000);
        break;
      }
    }
  }

  Serial.print("Initializing SD card...");
  lcd.clear();
  lcd.display();
  lcd.print("Initializing SD card...");
  delay (1000);

  // see if the card is present and can be initialized:
  if (SD.begin(SDSS)) {
    csv.beginNow();
    csv._update_file_number();
    csv.openFile();
    csv.writeHeaders();
    csv.closeFile();
    Serial.println("Card Present");
    lcd.clear();
    lcd.display();
    lcd.print("Card Present");
    delay(2000);
  }

  else {
    Serial.println("No SD card found");
    lcd.clear();
    lcd.display();
    lcd.print("No SD card found");
    delay(2000);
  }
}

void sensor_detect() {
  double timer = 0;
  if (sensorvalue == 1 )
    state1 = HIGH;
  else
    state1 = LOW;
  digitalWrite(13, state1);  // as iR light is invisible for us, the led on pin 13
  // indicate the state of the circuit.

  if (state2 != state1) {  //counts when the state change, thats from (dark to light) or
    //from (light to dark), remember that IR light is invisible to us.
    if (state2 > state1) {

      currentTime = micros();   // Get the arduino time in microseconds
      diffTime = currentTime - prevTime;  // calculate the time diff from the last meet-up
      rps = 1000000 / diffTime; // calculate how many rev per second, good to know
      rpm = 60000000 / diffTime; // calculate how many rev per minute
      blade_rpm = rpm / gear_ratio;
      unsigned long currentMillis = millis();

      // print to serial at every interval - defined at the variables declaration
      if (currentMillis - prevMillis > interval) { // see if now already an interval long

        timer = millis();
        timer = timer / (1000);
        dtostrf(timer, 4, 3, x);

        if (rpm > maxrpm) {
          maxrpm = rpm;
          lcd.setCursor(0, 0);
          lcd.clear();
          lcd.display();
          lcd.print("Max RPM:");
          lcd.print(maxrpm);
          blade_max_rpm = maxrpm/gear_ratio;
          lcd.setCursor(0,1);
          lcd.print("Blade Max:");
          lcd.print(blade_max_rpm);
        }

        cnt += 1 ;
        csv.openFile();
        prevMillis = currentMillis;

        dataString += cnt;
        dataString += ',';
        dataString += x;
        dataString += ',';
        dataString += rps;
        dataString += ',';
        dataString += rpm;
        dataString += ',';
        dataString += maxrpm;
        dataString += ',';
        dataString += "(" + String(gear_ratio) + ":1)";
        dataString += ',';
        dataString += blade_rpm;
        dataString += ',';
        dataString += blade_max_rpm;
        dataString += ',';
        dataString += sensor;

        Serial.print("Sample: "); Serial.print(cnt); Serial.print(" Time: "); Serial.print(x);
        Serial.print(" RPS: "); Serial.print(rps);  Serial.print(" RPM: "); Serial.print(rpm);
        Serial.print(" Gear Ratio: "); Serial.print(String(gear_ratio) + ":1"); 
        Serial.print(" Max RPM: "); Serial.print(maxrpm); Serial.print("Blade RPM: "); Serial.print("blade_rpm");
        Serial.print(" Blade Max: "); Serial.print(blade_max_rpm);
        Serial.print(" Sensor: "); Serial.println(sensor);

        csv.writeToCSV(dataString);
        csv.closeFile();
        dataString = "";
      }

      prevTime = currentTime;
    }
    state2 = state1;
  }
}

void loop()
{
  // read from pin 0
  if (analogpin == 54) {
    sensor = "Top";
    while (1) {
      sensorvalue = digitalRead(analogpin);
      sensor_detect();
    }
  }
  else if (analogpin == 55) {
    sensor = "Base";
    while (1) {
      sensorvalue = digitalRead(analogpin);
      sensor_detect();
    }
  }
}
