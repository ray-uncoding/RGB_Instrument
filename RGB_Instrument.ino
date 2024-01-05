#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <Adafruit_NeoPixel.h>

#define NUM_UNITS 3                                                     // 樂器單元數量
#define NUM_LEDS_PER_UNIT 2                                             // 每個單元的LED數量
#define NUM_LEDS_TOTAL (NUM_UNITS * NUM_LEDS_PER_UNIT)                  // 總LED數量
#define LED_PIN 11                                                      // 連接第一個LED的腳位
Adafruit_NeoPixel leds(NUM_LEDS_TOTAL, LED_PIN, NEO_GRB + NEO_KHZ800);  //  定義ws2812燈條

float client1_RGB[3] = { 200.00, 50.00, 50.00 };  //調整樂器單元顏色
float client2_RGB[3] = { 50.00, 200.00, 50.00 };
float client3_RGB[3] = { 50.00, 50.00, 200.00 };

float client1_Bright = 0.10;  //調整樂器單元亮度
float client2_Bright = 0.10;
float client3_Bright = 0.10;
float brightIntervel = 0.04;

int client1_chang = 1;  //調整樂器單元亮度變化
int client2_chang = 1;
int client3_chang = 1;

bool last_workState;
bool workState = false;  //關機

int loop_rate = 50;

void setup() {
  Serial.begin(921600);
  /*-----------*/
  leds.begin();  //led初始化
  leds.show();
  /*-----------*/
}
void loop() {
  deloperSerialCmdMode();
  
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
  client1_Bright = 0;
  client2_Bright = 0;
  client3_Bright = 0;
  client1_chang = 0;
  client2_chang = 0;
  client3_chang = 0;
}
void allBrightToTen() {
  brightToTen(client1_Bright, client1_chang);
  brightToTen(client2_Bright, client2_chang);
  brightToTen(client3_Bright, client3_chang);
}
void allBrightToZero() {
  brightToZero(client1_Bright, client1_chang);  //-0.04 to 0
  brightToZero(client2_Bright, client2_chang);
  brightToZero(client3_Bright, client3_chang);
}
void ONorOFFAnimate() {
  if (workState == true && last_workState == false) {  //開機動畫
    allClientVerToZero();
    for (int i = 0; i < 100; i++) {
      client1_Bright += 0.001;
      client2_Bright += 0.001;
      client3_Bright += 0.001;
      refreshBright();
      delay(40);
    }
  } else if (workState == false && last_workState == true) {  //關機動畫
    for (int i = 0; i < 100; i++) {
      if (client1_Bright > 0) {
        client1_Bright -= 0.01;
      }
      if (client2_Bright > 0) {
        client2_Bright -= 0.01;
      }
      if (client3_Bright > 0) {
        client3_Bright -= 0.01;
      }
      if (client1_Bright <= 0 && client1_Bright <= 0 && client1_Bright <= 0) {
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
    leds.setPixelColor(i, client1_Bright * client1_RGB[0], client1_Bright * client1_RGB[1], client1_Bright * client1_RGB[2]);
  }
  for (int i = NUM_LEDS_PER_UNIT; i < 2 * NUM_LEDS_PER_UNIT; i++) {
    leds.setPixelColor(i, client2_Bright * client2_RGB[0], client2_Bright * client2_RGB[1], client2_Bright * client2_RGB[2]);
  }
  for (int i = 2 * NUM_LEDS_PER_UNIT; i < 3 * NUM_LEDS_PER_UNIT; i++) {
    leds.setPixelColor(i, client3_Bright * client3_RGB[0], client3_Bright * client3_RGB[1], client3_Bright * client3_RGB[2]);
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
      bottonEvent(client1_Bright, client1_chang);
      cmd = 'p';
      break;
    case 'b':
      bottonEvent(client2_Bright, client2_chang);
      cmd = 'p';
      break;
    case 'c':
      bottonEvent(client3_Bright, client3_chang);
      cmd = 'p';
      break;
  }
}