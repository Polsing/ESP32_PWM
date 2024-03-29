#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

#define VM 36
#define GPIOPINRED 25
#define GPIOPINGREEN 26
#define GPIOPINBLUE 27
#define BUZZER_PIN 33
#define btnPin 2

void controlLEDRGB(int r, int g, int b);
void buzzerSound();
void btnSetup(int ptm);

int PWM_FREQUENCY = 5000;
int PWM_CHANNEL0 = 0;
int PWM_CHANNEL1 = 1;
int PWM_CHANNEL2 = 2;
int PWM_CHANNEL3 = 3;
int PWM_RESOUTION = 8;

bool enableHeater = false;
bool storeMin = true;
uint8_t loopCnt = 0;

int minValue;
int maxValue;
int btnState = HIGH;
int lastbtnState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned long debouncDelay = 50;

bool storeMin = true;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

void setup() {
  Serial.begin(9600);
  ledcSetup(PWM_CHANNEL0, PWM_FREQUENCY, PWM_RESOUTION);
  ledcSetup(PWM_CHANNEL1, PWM_FREQUENCY, PWM_RESOUTION);
  ledcSetup(PWM_CHANNEL2, PWM_FREQUENCY, PWM_RESOUTION);
  ledcSetup(PWM_CHANNEL3, PWM_FREQUENCY, PWM_RESOUTION);

  ledcAttachPin(GPIOPINRED, PWM_CHANNEL0);
  ledcAttachPin(GPIOPINGREEN, PWM_CHANNEL1);
  ledcAttachPin(GPIOPINBLUE, PWM_CHANNEL2);
  ledcAttachPin(BUZZER_PIN, PWM_CHANNEL3);

  while (!Serial)
    delay(10);

  Serial.println("SHT31 test");
  if (!sht31.begin(0x44)) {
    Serial.println("Couldn't find SHT31");
    while (1)
      delay(1);
  }

  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");
}

void loop() {
  int valueMap = map(analogRead(VM), 0, 4095, 0, 100);
  btnSetup(valueMap);
  
  float t = sht31.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read temperature");
    return;
  }
  
    Serial.println("valueMap: " + String(valueMap) +  " ValueMin: " + String(minValue) + " ValueMax: " + String(maxValue));
  if (t >= maxValue + 3) {
    Serial.println("Temp *C = " + String(t) + " สีแดงอ่อน");
    controlLEDRGB(255, 42, 68);
    delay(1000);
  } else if (t <= minValue - 3) {
    Serial.println("Temp *C = " + String(t) + " สีฟ้าอ่อน");
    controlLEDRGB(54, 194, 255);
    delay(1000);
  } else {
    controlLEDRGB(255, 127, 0);
    Serial.println("Temp *C = " + String(t) + " สีสม");
    delay(1000);
  }

  if(t > 39){
    buzzerSound();
    controlLEDRGB(255, 0, 0);
    Serial.println("Temp *C = " + String(t) + " hot!!!!");
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

void controlLEDRGB(int r, int g, int b) {
  ledcWrite(PWM_CHANNEL0, map(r, 255, 0, 0, 255));
  ledcWrite(PWM_CHANNEL1, map(g, 255, 0, 0, 255));
  ledcWrite(PWM_CHANNEL2, map(b, 255, 0, 0, 255));
}

void buzzerSound(){
  for (int i = 0; i < 2; i++) {
    ledcWriteTone(PWM_CHANNEL3, 2000);
    delay(500);
    ledcWriteTone(PWM_CHANNEL3, 0); // Turn off the tone
    delay(500);
  }
}

void btnSetup(int ptm){
  int reading = digitalRead(btnPin);
  if (reading != lastbtnState) 
    lastDebounceTime = millis();


  if ((millis() - lastDebounceTime) > debouncDelay) {
    if (reading != btnState) {
      btnState = reading;
      if (btnState == LOW) {
        if (storeMin)
          minValue = ptm;
        else
          maxValue = ptm;

        storeMin = !storeMin;
      }
    }
  }

  lastbtnState = reading;
}
