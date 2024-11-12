/*
* Name: clock and temp project
* Author: Simon Erlén
* Date: 2024-11-12
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor aswell as it says if the tempreture is comfortable or not.
*/


#include <RTClib.h>
#include <Wire.h>
#include "U8glib.h"
#include "Servo.h"

#define servoPin 7

RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
Servo myservo;



void setup() {
  // init communication
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  u8g.setFont(u8g_font_unifont);
  // Init Hardware
  myservo.attach(servoPin);
  myservo.write(0);
  for(int ledPin = 3; ledPin <=5; ledPin ++){
    pinMode(ledPin, OUTPUT);
  }

 
}

void loop() {

  oledWrite(getTime());
  Serial.println("Temprature: " + String(getTemp()) + "C" );
  myservo.write(getTemp());

  delay(1000);
  ledSystem();
}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {

  DateTime now = rtc.now();

  return ("TIME: " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temprature as float
*/
float getTemp() {
  int thermosPin = A1;
  int Vo;
  float R1 = 10000;
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  //steinhart-hart coeficients for thermistor;

  Vo = analogRead(thermosPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);  //calculate resistance on thermistor
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  T = T - 273.15;

  
  //convert Kelvin to Celcius
  // T = (T * 9.0)/ 5.0 + 32.0; //convert Celcius to Farenheit
  
  return (T);
  
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text) {
  u8g.firstPage();
  do {
    u8g.drawStr(5, 33, text.c_str());
  } while (u8g.nextPage());
}

/*
* takes a temprature value and maps it to corresppnding degree on a servo
*Parameters: - value: temprature
*Returns: void
*/
//void servoWrite(float value) {

//}

void servoWrite(){
  myservo.write(map(getTemp(),15, 35, 0, 180));
}


void ledSystem(){
  if (getTemp<20){
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);

  
  }else if (getTemp() > 20 && getTemp() < 26){
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);

  }else{
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
  }
  //Detta är ett system som får den gröna lampan att lysa när temperatuen är behaglig. Den röda lyser när det är för varmt och när den blåa när det är för kallt.

  

}