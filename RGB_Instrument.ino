#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <Adafruit_NeoPixel.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <WebSocketsClient.h>

#define NUM_UNITS 1                                                     // 樂器單元數量
#define NUM_LEDS_PER_UNIT 27                                            // 每個單元的LED數量
#define NUM_LEDS_TOTAL (NUM_UNITS * NUM_LEDS_PER_UNIT)                  // 總LED數量
#define LED_PIN 11                                                      // 連接第一個LED的腳位
Adafruit_NeoPixel leds(NUM_LEDS_TOTAL, LED_PIN, NEO_GRB + NEO_KHZ800);  //  定義ws2812燈條
/*
SoftwareSerial mySoftwareSerial(15, 16);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;
*/
#define BOTTON_PIN 9
#define BUZY_PIN 6


// Replace with your network credentials
const char *ssid = "Liangyu";
const char *password = "10635493";

/*
// Replace with your network credentials
const char *ssid = "pan0428";
const char *password = "04836920";
*/

const char *host = "192.168.43.221";  // 主機的 IP 地址
int port = 80;                         // 主機的端口

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

/*
float client1_RGB[3] = { 100.00, 144.00, 232.00 };  //調整樂器單元顏色
float client2_RGB[3] = { 72.00, 193.00, 172.00 };
float client3_RGB[3] = { 255.00, 71.00, 34.00 };
float powerONOFF_RGB[3] = { 255.00, 255.00, 255.00 };
*/

WebSocketsClient webSocket;

float client_RGB[3] = { 100.00, 144.00, 232.00 };  //調整樂器單元顏色

float client_Bright = 0.10;  //調整樂器單元亮度
float brightIntervel = 0.04;

int client_chang = 1;  //調整樂器單元亮度變化

bool last_workState;
bool workState = false;  //關機

bool last_bottonState = true;
bool bottonState = true;

bool isPlaying = false;

int loop_rate = 50;

void setup() {
  Serial.begin(921600);
  //mySoftwareSerial.begin(9600);
  /*-----------*/
  leds.begin();  //led初始化
  leds.show();
  /*-----------*/
  pinMode(BOTTON_PIN, INPUT_PULLUP);
  pinMode(BUZY_PIN, INPUT);
/*
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    //mySoftwareSerial.begin(9600);
    //delay(1000);
    while (1) {
    }
  }
  Serial.println(F("begin!"));
  myDFPlayer.volume(30);  //Set volume value. From 0 to 30*/
  workState = true;


  //wifi 連線
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  webSocket.begin(host, port, "/ws");
  webSocket.onEvent(webSocketEvent);
}
void loop() {
  webSocket.loop();
  deloperSerialCmdMode();
  bottonState = digitalRead(BOTTON_PIN);
  if (ifBottonPress()) {
    bottonEvent(client_Bright, client_chang);
   // myDFPlayer.play(2);
    webSocket.sendTXT("clientone");
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
      //myDFPlayer.play(1);
      Serial.println("a");
      cmd = 'p';
      break;
  }
}
int ifBottonPress() {
  if (last_bottonState == true && bottonState == false) {
    return true;
  } else {
    return false;
  }
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("WebSocket connected");
      break;
    case WStype_TEXT:
      Serial.println("Received text: " + String((char *)payload));
      // 在這裡處理從主機接收到的消息
      break;
  }
}