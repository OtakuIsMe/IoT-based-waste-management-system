#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6bFyLWkEh"
#define BLYNK_TEMPLATE_NAME "Project"
#define BLYNK_AUTH_TOKEN "oBoPehRiwe3Ch_rYMG4Ug-YjP2UiITCo"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include<Servo.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>    //Thư viện LCD
LiquidCrystal_I2C lcd(0x27,16,2); //Thiết lập địa chỉ và loại LCD
#define photoresistor A0
#define buzzerPin D0
#define IRsensor D1
#define SDA D2
#define SCL D3
#define ServoMortor D4
#define TRIG_PIN D5
#define ECHO_PIN D6
#define led D7
#define buttonPin D8
Servo myservo;
int pos=0;


char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Duy Manh";
char pass[] = "02111003";

int v0=0;
int v1=0;
const int garbageHeigh = 60;

byte zero[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte one[] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};
byte two[] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};
byte three[] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};
byte four[] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};
byte five[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte divide1 []={
  B11000,
  B11000,
  B11000,
  B11111,
  B11111,
  B11000,
  B11000,
  B11000,
};

byte divide2 []={
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(IRsensor ,INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  myservo.attach(ServoMortor);
  pinMode(buzzerPin, OUTPUT);
  Wire.begin(D2,D3);               //Thiết lập chân kết nối I2C (SDA,SCL);
  lcd.clear();
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
  lcd.createChar(6, divide1);
  lcd.createChar(7, divide2);
  pinMode(led, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  delay(1);
  ledAtNight();
  pressButton();
  openGarbage();
  buzzerRing();
  Blynk.virtualWrite(V2, objectHeightPercent());
  Serial.println(String(v0)+", "+String (v1));
  delay(1);
}

BLYNK_WRITE(V0){
  v0 = param.asInt();
  }
BLYNK_WRITE(V1){
  v1 = param.asInt();
  }

  void ledAtNight(){
      if(isAtNight()){
          digitalWrite(led, HIGH);
        }else{
          digitalWrite(led, LOW);
      }
    }

boolean isAtNight(){
    int sensorValue = analogRead(photoresistor);
    float voltage = sensorValue*(5.0/1023.0);
    if(voltage<1){
      return true;
      }else{
      return false;
      }
  }
void screenOut(int percent) {
  for (int i = 0; i <= percent; i++) {
    if (i == 0) {
      lcd.display();
    }
    lcd.setCursor(1, 0);
    lcd.print("Processing:");
    lcd.print(i);
    lcd.setCursor(10, 1);
    lcd.write(byte(6));
    for (int i = 0; i < 5; i++) {
      lcd.write(byte(7));
    }
    updateProgressBar(objectHeightPercent());
    delay(50);
    if (i == percent) {
      delay(5000);
      lcd.noDisplay();
    }
  }
}
void updateProgressBar(int i) {
  if (i >= 10) {
    int units = i % 10;
    int tensOf = (i - units) / 10;
    for (int i = 0; i < tensOf; i++) {
      lcd.setCursor(i, 1);
      lcd.write(byte(5));
    }
    double num = floor(units / 2);
    if(tensOf!=10){
      lcd.setCursor(tensOf, 1);
      lcd.write(byte(num));
    }
  } else {
    double num = floor(i / 2);
    lcd.setCursor(0, 1);
    lcd.write(byte(num));
  }
}
int buttonState=0;
int lastButtonState=0;
void pressButton(){
    buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      lcd.clear();
      screenOut(objectHeightPercent());
    }
    delay(50);
  }
  lastButtonState=buttonState;
  }
boolean isObjectNear(){
    if(digitalRead(IRsensor)==LOW){
        Serial.println("low");                                            
        return true; 
      }else{
        Serial.println(" High");
        return false;
      }
  }
  int objectHeightPercent(){
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
   const unsigned long duration= pulseIn(ECHO_PIN, HIGH);
   double distance = duration/29/2;
   double percent = (garbageHeigh-distance)/garbageHeigh;
   if(percent<=1 && percent>0){
      Serial.println(percent);
      return int(percent*100);
    }else{
      return 0;
      }
  }
  void buzzerRing(){
      while(isFull()&&!(v1==1)){
        Blynk.run();// check xem có lệnh dừng còi không ?
        Blynk.virtualWrite(V2, objectHeightPercent());
        digitalWrite(buzzerPin, HIGH);
        delay(500);
        digitalWrite(buzzerPin, LOW);
        delay(500);
      }
    }
    void openGarbage(){
      if(isObjectNear() || v0==1){
          for(pos=0;pos<=180;pos+=1){
            myservo.write(pos);
            delay(5);
            }
            while(isObjectNear() || v0==1){
              Blynk.run(); // check xem có lệnh đóng không ?
              Blynk.virtualWrite(V2, objectHeightPercent());
              delay(50);
              }
          for(pos=180;pos>=0;pos-=1){
            myservo.write(pos);
            Serial.println("close");
            delay(5);
            }
        }
    }
 boolean isFull(){
    if(objectHeightPercent()<=80){
      return false;
      }
      return true;
  }
