#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <Adafruit_NeoPixel.h>

#define POWER_ONOFF_PIN 6
#define NUM_UNITS 3           // 樂器單元數量
#define NUM_LEDS_PER_UNIT 13  // 每個單元的LED數量
#define NUM_POWER_ONOFF_LED 18
#define NUM_LEDS_TOTAL (NUM_UNITS * NUM_LEDS_PER_UNIT + NUM_POWER_ONOFF_LED)  // 總LED數量
#define LED_PIN 11                                                            // 連接第一個LED的腳位
Adafruit_NeoPixel leds(NUM_LEDS_TOTAL, LED_PIN, NEO_GRB + NEO_KHZ800);        //  定義ws2812燈條

float client1_RGB[3] = { 100.00, 144.00, 232.00 };  //調整樂器單元顏色
float client2_RGB[3] = { 72.00, 193.00, 172.00 };
float client3_RGB[3] = { 255.00, 71.00, 34.00 };
float powerONOFF_RGB[3] = { 255.00, 255.00, 255.00 };

float client1_Bright = 0.10;  //調整樂器單元亮度
float client2_Bright = 0.10;
float client3_Bright = 0.10;
float powerONOFF_Bright = 0.50;
float brightIntervel = 0.04;

int client1_chang = 1;  //調整樂器單元亮度變化
int client2_chang = 1;
int client3_chang = 1;

bool last_workState;
bool workState = true;  //關機

int loop_rate = 50;

void setup() {
  Serial.begin(921600);
  /*-----------*/
  leds.begin();  //led初始化
  leds.show();
  /*-----------*/
  pinMode(POWER_ONOFF_PIN, INPUT_PULLUP);
}
void loop() {
  ONorOFFAnimate();
  deloperSerialCmdMode();
  /*------on-------*/
  if (workState) {
    allBrightToTen();
    Serial.println(F("on"));
  }
  /*------off------*/
  else {
    allBrightToZero();
    Serial.println(F("off"));
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
  powerONOFF_Bright = 0;
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
  bool term = digitalRead(POWER_ONOFF_PIN);
  workState = term;
  if (workState == true && last_workState == false) {  //開機動畫
    allClientVerToZero();
    for (int i = 0; i < 100; i++) {
      client1_Bright += 0.001;
      client2_Bright += 0.001;
      client3_Bright += 0.001;
      powerONOFF_Bright += 0.005;
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
      if (powerONOFF_Bright > 0) {
        powerONOFF_Bright -= 0.05;
      }
      if (client1_Bright <= 0 || client2_Bright <= 0 || client3_Bright <= 0 || powerONOFF_Bright <= 0) {
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
  for (int i = 3 * NUM_LEDS_PER_UNIT; i < NUM_LEDS_TOTAL; i++) {
    leds.setPixelColor(i, powerONOFF_Bright * powerONOFF_RGB[0], powerONOFF_Bright * powerONOFF_RGB[1], powerONOFF_Bright * powerONOFF_RGB[2]);
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