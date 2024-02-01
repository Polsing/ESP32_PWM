#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

#define VM 36
#define GPIOPINRED 25 
#define GPIOPINGREEN 26 
#define GPIOPINBLUE 27

void controlLEDRGB(int r , int g , int b);

int PWM_FREQUENCY = 5000;
int PWM_CHANNEL0 = 0;
int PWM_CHANNEL1 = 1;
int PWM_CHANNEL2 = 2;
int PWM_RESOUTION = 8;

bool enableHeater = false;
uint8_t loopCnt = 0;
Adafruit_SHT31 sht31 = Adafruit_SHT31();


void setup() {
  Serial.begin(9600);
  ledcSetup(PWM_CHANNEL0, PWM_FREQUENCY,PWM_RESOUTION);
  ledcSetup(PWM_CHANNEL1, PWM_FREQUENCY,PWM_RESOUTION);
  ledcSetup(PWM_CHANNEL2, PWM_FREQUENCY,PWM_RESOUTION);

  ledcAttachPin(GPIOPINRED, PWM_CHANNEL0);
  ledcAttachPin(GPIOPINGREEN, PWM_CHANNEL1);
  ledcAttachPin(GPIOPINBLUE, PWM_CHANNEL2);

  while (!Serial)
  delay(10);  

  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");
}

void loop() {
  float t = sht31.readTemperature();
  if(isnan(t)){
    Serial.println("Failed to read temperature");
    return;
  }
  int valueMap = map(analogRead(VM),0,4095,0,100);
  if(t >= valueMap + 3){
  Serial.println("Temp *C = " + String(t) + " VM: " + String(valueMap) + " สีเเดงอ่อน");
  controlLEDRGB(255, 42, 68);
  delay(1000);
  }
  else if(t <= valueMap - 3){
  Serial.println("Temp *C = " + String(t) + " VM: " + String(valueMap) + " สีฟ้าอ่อน");
  controlLEDRGB(54, 194, 255);
  delay(1000);
  }
  else{
  controlLEDRGB(255, 127, 0);
  Serial.println("Temp *C = " + String(t) + " VM: " + String(valueMap) + " สีสม");
  delay(1000);
  }

  if (loopCnt >= 30) {
  enableHeater = !enableHeater;
  sht31.heater(enableHeater);
  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");
  loopCnt = 0;
  }

  loopCnt++;
}

void controlLEDRGB(int r, int g , int b){
  ledcWrite(PWM_CHANNEL0,map(r,255,0,0,255));
  ledcWrite(PWM_CHANNEL1,map(g,255,0,0,255));
  ledcWrite(PWM_CHANNEL2,map(b,255,0,0,255));
}