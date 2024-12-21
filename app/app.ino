#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

/** define the used key numbers */
#define HC06_RXD 7
#define HC06_TXD 8
#define VM_OUT 3

/** define the reaction categories for the bluetooth input */
#define ALERT_BLOOD_SUGAR "ALT_BLD"
#define ALERT_MEDICINE "ALT_MED"

/** call modules class */
LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial bluetooth(HC06_TXD, HC06_RXD);

/** received data through bluetooth */
String data = "";
bool loading = false;

/** functional modularization */
void Activate();
void ClearLCD();
void DoBloodSugar(int);
void DoMedicine(int);
void CleanUp();

void setup() {
  /** init vibration motor */
  pinMode(VM_OUT, OUTPUT);

  /** init bluetooth */
  Serial.begin(9600);
  bluetooth.begin(9600);

  /** init ICD I2C */
  lcd.init();
  lcd.noBacklight();
}

void loop() {
  if (bluetooth.available()) {
    char c = bluetooth.read();
    if (c == '#') {
      Activate();
    } else data.concat(c);
  }
  delay(500);
}

void Activate() {
  int idx = data.indexOf('/');
  String tag = data.substring(0,idx);
  if (tag == ALERT_BLOOD_SUGAR) {
    DoBloodSugar(idx);
  } else if (tag == ALERT_MEDICINE) {
    DoMedicine(idx);
  } else {
    Serial.println("ERROR - Wrong tag name");
  }
  data = "";
}

void DoBloodSugar(int idxOfArgs) {
  int idx = data.indexOf('/', idxOfArgs+1);
  String tag = data.substring(idxOfArgs+1, idx);
  if (tag == "ON") {
    /** LCD */
    lcd.backlight(); ClearLCD();

    lcd.setCursor(0,0); lcd.print("Measure your");
    lcd.setCursor(0,1); lcd.print("blood sugar");
    
    /** vibration motor */
    analogWrite(VM_OUT, 255); delay(200);
    analogWrite(VM_OUT, 0);   delay(50);
    analogWrite(VM_OUT, 255); delay(500);
    analogWrite(VM_OUT, 0);

    loading = true;
  } else if (loading) {
    ClearLCD();
    
    lcd.setCursor(0,0); lcd.print("Successfully");
    lcd.setCursor(0,1); lcd.print("checked");
    
    delay(2000); CleanUp();
  } else {
    Serial.println("ERROR - Unexpected");
  }
}

void DoMedicine(int idxOfArgs) {
  int idx = data.indexOf('/', idxOfArgs+1);
  String tag = data.substring(idxOfArgs+1, idx);
  if (tag == "ON") {
    int typeIdx = data.indexOf('/', idx+1);
    String typeTag = data.substring(idx+1, typeIdx);
    if (typeTag == "MED") {
      typeTag = "Medicine";
    } else if (typeTag == "INS") {
      typeTag = "Insulin";
    } else {
      Serial.println("Error - Wrong tag name");
      return;
    }

    int nameIdx = data.indexOf('/', typeIdx+1);
    String nameTag = data.substring(typeIdx+1, nameIdx);
    int colorIdx = data.indexOf('/', nameIdx+1);
    String colorTag = data.substring(nameIdx+1, colorIdx);
    int countIdx = data.indexOf('/', colorIdx+1);
    String countTag = data.substring(colorIdx+1, countIdx);

    /** LCD */
    lcd.backlight(); ClearLCD();

    lcd.setCursor(0,0); lcd.print(typeTag + " time");
    lcd.setCursor(0,1); lcd.print(nameTag + "(" + colorTag + ") " + countTag + "p");

    /** vibration motor */
    analogWrite(VM_OUT, 255); delay(1000);
    analogWrite(VM_OUT, 0);

    loading = true;
  } else if (loading) {
    ClearLCD();

    lcd.setCursor(0,0); lcd.print("Successfully");
    lcd.setCursor(0,1); lcd.print("recorded");
    
    delay(2000); CleanUp();
  } else {
    Serial.println("ERROR - Unexpected");
  }
}

void CleanUp() {
  lcd.noBacklight(); ClearLCD();
  loading = false;
}

void ClearLCD() {
  lcd.setCursor(0,0); lcd.print("                ");
  lcd.setCursor(0,1); lcd.print("                ");
}
