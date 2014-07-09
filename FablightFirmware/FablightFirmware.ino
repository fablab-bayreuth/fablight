#include <SoftwareSerial.h>
#include "global.h"
#include "LabColor.h"
#include "PWM.h"

#define BT_KEY 8
#define BT_RX 13
#define BT_TX 12
#define BT_BAUD 9600

SoftwareSerial bt(BT_TX, BT_RX);

uint8_t colorToPin[4] = { PIN_W, PIN_R, PIN_G, PIN_B };

void setup() {
  
  initPWM();
  
  Serial.begin(38400);
  bt.begin(BT_BAUD);
}

unsigned long startTime = 0;
uint8_t startColor[4] = { 0, 0, 0, 0 };

struct action {
  uint16_t fadeTime;
  uint8_t color[4];
} action[2];

int8_t actionIndex = 0;

void loop() {
  doAction();
  readAction();
}

void doAction() {
  static int8_t doing = 0;
  if(doing == 1) {
    unsigned long t = millis() - startTime;
    if(action[0].fadeTime == 0 || t >= action[0].fadeTime) {
      doing = 0;
      for(int i = 0; i < 4; i++) {
        setPWM(i, (uint16_t)action[0].color[i]);
        startColor[i] = action[0].color[i];
      }
      actionIndex = 0;
    } else {
      for(int i = 0; i < 4; i++) {
        setPWM(i, (uint16_t)(startColor[i] + ((action[0].color[i] - startColor[i]) * t) / action[0].fadeTime));
      }
    }
  } else if(actionIndex == 1) {
    startTime = millis();
    doing = 1;
  }
}

void readAction() {
  static int index = 0;

  while(bt.available()) {
    char c = bt.read();
    Serial.write(c);
    if(c == '\r') {
      if(index == sizeof(action[0]) * 2) {
        actionIndex++;
      }
      bt.write('.');
      Serial.write('.');
      index = 0;
      continue;
    } else {
      if(c >= '0' && c <= '9') {
        c -= '0';
      } else if(c >= 'A' && c <= 'F') {
        c -= 'A' - 10;
      } else {
        continue;
      }
    }
    switch(index) {
      case 0: action[actionIndex].fadeTime  = c << 12; break;
      case 1: action[actionIndex].fadeTime |= c <<  8; break;
      case 2: action[actionIndex].fadeTime |= c <<  4; break;
      case 3: action[actionIndex].fadeTime |= c; break;
      case 4: case 6: case 8: case 10:
              action[actionIndex].color[index/2 - 2] =  c << 4; break;
      case 5: case 7: case 9: case 11:
              action[actionIndex].color[index/2 - 2] |= c; break;
    }
    index++;
  }
}

