#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <Adafruit_NeoPixel.h>

#define NUM_UNITS 3                                                     // 樂器單元數量
#define NUM_LEDS_PER_UNIT 6                                             // 每個單元的LED數量
#define NUM_LEDS_TOTAL (NUM_UNITS * NUM_LEDS_PER_UNIT)                  // 總LED數量
#define LED_PIN 39                                                      // 連接第一個LED的腳位
Adafruit_NeoPixel leds(NUM_LEDS_TOTAL, LED_PIN, NEO_GRB + NEO_KHZ800);  //  定義ws2812燈條

#define BOTTON_PIN 37

float client_RGB[3] = { 200.00, 50.00, 50.00 };  //調整樂器單元顏色

float client_Bright = 0.10;  //調整樂器單元亮度
float brightIntervel = 0.04;

int client_chang = 1;  //調整樂器單元亮度變化

bool last_workState;
bool workState = false;  //關機

bool last_bottonState = true;
bool bottonState = true;

int loop_rate = 50;

void setup() {
  Serial.begin(921600);
  /*-----------*/
  leds.begin();  //led初始化
  leds.show();
  /*-----------*/
  pinMode(BOTTON_PIN, INPUT_PULLUP);
}
void loop() {
  deloperSerialCmdMode();
  bottonState = digitalRead(BOTTON_PIN);
  if(ifBottonPress()){
    bottonEvent(client_Bright, client_chang);
  }
  ONorOFFAnimate();
  /*------on-------*/
  if (workState) {
    allBrightToTen();
  }
  /*------off------*/
  else {
    allBrightToZero();
  }
  /*------loop rate------*/
  refreshBright();
  last_workState = workState;
  last_bottonState = bottonState;
  delay(loop_rate);
}

void brightToZero(float &client_Bright, int &client_chang) {
  if (client_Bright > 0.00) {
    client_chang = -1;
    client_Bright = client_Bright - brightIntervel;
  } else {
    client_chang = 0;
    client_Bright = 0.00;
  }
}
void brightToTen(float &client_Bright, int &client_chang) {
  if (client_Bright > 0.80) {
    client_chang = -1;
    client_Bright = client_Bright - brightIntervel;
  } else if (client_Bright > 0.10) {
    client_Bright = client_Bright + (client_chang * brightIntervel);
  } else {
    client_Bright = 0.10;
    client_chang = 0;
  }
}
void bottonEvent(float &client_Bright, int &client_chang) {
  if (workState == true) {
    if (client_chang == 0) {
      client_chang = 1;
      client_Bright = 0.10 + brightIntervel;
    } else {
      client_chang = -client_chang;
    }
  }
}
void allClientVerToZero() {
  client_Bright = 0;
  client_chang = 0;

}
void allBrightToTen() {
  brightToTen(client_Bright, client_chang);
}
void allBrightToZero() {
  brightToZero(client_Bright, client_chang);
}
void ONorOFFAnimate() {
  if (workState == true && last_workState == false) {  //開機動畫
    allClientVerToZero();
    for (int i = 0; i < 100; i++) {
      client_Bright += 0.001;
      refreshBright();
      delay(40);
    }
  } else if (workState == false && last_workState == true) {  //關機動畫
    for (int i = 0; i < 100; i++) {
      if (client_Bright > 0) {
        client_Bright -= 0.01;
      }
      if (client_Bright <= 0) {
        allClientVerToZero();
        i = 100;
      }
      refreshBright();
      delay(50);
    }
  }
}
void refreshBright() {
  for (int i = 0; i < NUM_LEDS_PER_UNIT; i++) {
    leds.setPixelColor(i, client_Bright * client_RGB[0], client_Bright * client_RGB[1], client_Bright * client_RGB[2]);
  }
  leds.show();
}
void deloperSerialCmdMode() {
  char cmd;
  if (Serial.available()) {
    cmd = Serial.read();
  }
  switch (cmd) {
    case 'n':
      workState = true;
      cmd = 'p';
      break;
    case 'f':
      workState = false;
      cmd = 'p';
      break;
    case 'a':
      bottonEvent(client_Bright, client_chang);
      cmd = 'p';
      break;
  }
}
int ifBottonPress(){
  if(last_bottonState == true && bottonState == false){
    return true;
  }else{
    return false;
  }
}