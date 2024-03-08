#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <Adafruit_NeoPixel.h>
//#include "SoftwareSerial.h" //  esp不能用這個
#include "DFRobotDFPlayerMini.h"
#include <WebSocketsClient.h>
#include "WIFI_ID.h"

#define SYSTEM_LED_PIN 2  //開機檢測燈
#define BOTTON_PIN 6
#define MP3_RX_PIN 5
#define MP3_TX_PIN 4
#define LED_PIN 9
#define VOL_PIN 7  //感測電池電壓
#define MONITOR_BAUDRATE 921600
#define MP3_BAUDRATE 9600

#define NUM_UNITS 1                                                     // 樂器單元數量
#define NUM_LEDS_PER_UNIT 30                                            // 每個單元的LED數量
#define NUM_LEDS_TOTAL (NUM_UNITS * NUM_LEDS_PER_UNIT)                  // 總LED數量
Adafruit_NeoPixel leds(NUM_LEDS_TOTAL, LED_PIN, NEO_GRB + NEO_KHZ800);  //  定義ws2812燈條
HardwareSerial myHardwareSerial(1);                                     //ESP32可宣告需要一個硬體序列，軟體序列會出錯
DFRobotDFPlayerMini myDFPlayer;                                         //啟動DFPlayer撥放器
WebSocketsClient webSocket;

int port = 80;  // 主機的端口

/*---其他課服端的顏色----
float client1_RGB[3] = { 100.00, 144.00, 232.00 };  //調整樂器單元顏色
float client2_RGB[3] = { 72.00, 193.00, 172.00 };
float client3_RGB[3] = { 255.00, 71.00, 34.00 };
float powerONOFF_RGB[3] = { 255.00, 255.00, 255.00 };
--------------------*/

/*------rgb變數-------*/
float client_RGB[3] = { 100.00, 144.00, 232.00 };  //顏色
float client_Bright = 0.10;                        //亮度
float brightIntervel = 0.04;                       //亮度變化速度
int client_chang = 1;                              //亮度變化方向, +-1
/*------系統變數------*/
bool last_workState = true;  //紀錄開關機狀態
bool workState = true;       //預設開關機狀態
unsigned long previousMillis = 0;
const int interval = 50;
/*------按鈕變數------*/
bool last_bottonState = true;  //紀錄按鈕感測電壓, 壓下->0, 放開->1
bool bottonState = true;       //預設按鈕感測電壓, 壓下->0, 放開->1
/*------mp3變數-------*/
bool isPlaying = false;             //是否正在撥放音樂, 是->0, 否->1
int music_file_hit_instrument = 1;  //擊打音效的檔案編號
/*------電源變數------*/
int bettery_voltage;  //紀錄電池電壓, 0~1024
/*------web變數-------*/
const char *clientName = "clientone";
/*-----開發者指令變數-----*/
int on = 1;
int off = 2;
int perssBotton = 3;
int cheakvoltage = 4;
int deadCmd = 0;

void setup() {
  /*------系統設定-------*/
  Serial.begin(MONITOR_BAUDRATE);

  myHardwareSerial.begin(MP3_BAUDRATE, SERIAL_8N1, MP3_TX_PIN, MP3_RX_PIN);  // Serial的TX,RX
  Serial.println("Initializing DFPlayer ... (May take 1-2 seconds)");
  myDFPlayer.begin(myHardwareSerial);  //將DFPlayer播放器宣告在HardwareSerial控制

  Serial.println(F("begin setup system"));
  pinMode(BOTTON_PIN, INPUT_PULLUP);
  pinMode(VOL_PIN, INPUT);
  pinMode(SYSTEM_LED_PIN, OUTPUT);
  Serial.println(F("system setup succed"));
  /*------元件設定-------*/
  Serial.println(F("begin setup element"));
  leds.begin();
  setupWIFI();
  myDFPlayer.volume(30);
  webSocket.begin(host, port, "/ws");
  webSocket.onEvent(webSocketEvent);

  allSetupOK(); /*
  Serial.println("test");
  myDFPlayer.playMp3Folder(2);  //播放mp3內的0001.mp3 3秒鐘
  delay(5000);
  Serial.println("REtest");
  myDFPlayer.playMp3Folder(1);  //播放mp3內的0001.mp3 3秒鐘*/
}


void loop() {
  /*------刷新系統變數-------*/
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    deloperSerialCmdMode();                  //刷新開發者指令
    webSocket.loop();                        //刷新web
    ONorOFFAnimate();                        //刷新開關機狀態
    bottonState = digitalRead(BOTTON_PIN);   //刷新按鈕感測電壓, 壓下->0, 放開->1
    bettery_voltage = digitalRead(VOL_PIN);  //刷新電池電壓, 0~1024

    /*------壓下按鈕時-------*/
    if (ifBottonPress()) {
      Serial.println("press");
      myDFPlayer.playMp3Folder(music_file_hit_instrument);  //播放mp3內的0001.mp3
      webSocket.sendTXT(clientName);                        //web傳送課服端名字
      if (myDFPlayer.available()) {
        uint8_t type = myDFPlayer.readType();
      }
    }

    /*------次刷新系統變數------*/
    if (workState) client_Bright = 0.8;
    else client_Bright = 0;
    refreshBright();                 //更新亮度
    last_workState = workState;      //紀錄開關機狀態
    last_bottonState = bottonState;  //紀錄按鈕感測電壓
  }
}
/*
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
*/
void allClientVerToZero() {
  client_Bright = 0;
  client_chang = 0;
}
/*
void allBrightToTen() {
  brightToTen(client_Bright, client_chang);
}
void allBrightToZero() {
  brightToZero(client_Bright, client_chang);
}
*/
void ONorOFFAnimate() {
  if (workState == true && last_workState == false) {  //開機動畫
    Serial.println("ON ami");
    allClientVerToZero();
    for (int i = 0; i < 100; i++) {
      client_Bright += 0.01;
      refreshBright();
      delay(40);
    }
    Serial.println(F("power on"));
  } else if (workState == false && last_workState == true) {  //關機動畫
    Serial.println("OFF ami");
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
    Serial.println(F("power off"));
  }
}

void refreshBright() {
  for (int i = 0; i < NUM_LEDS_PER_UNIT; i++) {
    leds.setPixelColor(i, client_Bright * client_RGB[0], client_Bright * client_RGB[1], client_Bright * client_RGB[2]);
  }
  leds.show();
}

int ifBottonPress() {
  if (last_bottonState == true && bottonState == false) {
    return true;
  } else {
    return false;
  }
}

void systemPinBlink(int flashes_times, int interval_microSeconds) {
  for (int i = 0; i < flashes_times; i++) {
    digitalWrite(SYSTEM_LED_PIN, 1);
    delay(interval_microSeconds);
    digitalWrite(SYSTEM_LED_PIN, 0);
    delay(interval_microSeconds);
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
      handleWebSocketMessage((char *)payload);
      // 在這裡處理從主機接收到的消息
      break;
  }
}

void handleWebSocketMessage(char *message) {
  if (strcmp(message, "1") == 0) {
    workState = true;
    Serial.println("ON");
  } else if (strcmp(message, "0") == 0) {
    workState = false;
    Serial.println("OFF");
  }
}

/*
void setupMP3Serial() {  //建立mp3的serial連線, 失敗則頻閃橘色燈
  Serial.println(F("begin setup dfplayer"));
  //delay(2000);
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (1) {
      for (int i = 0; i < NUM_LEDS_PER_UNIT; i++) {
        leds.setPixelColor(i, 242, 133, 0);
      }
      leds.show();
      delay(500);
      for (int i = 0; i < NUM_LEDS_PER_UNIT; i++) {
        leds.setPixelColor(i, 0, 0, 0);
      }
      leds.show();
      delay(500);
      systemPinBlink(2, 1000);
    }
  }
  Serial.print(F("dfplayer setup succed"));
}*/
/*
void setupled(){
  Serial.println(F("begin setup wifi"));
  for (int i = 0; i < NUM_LEDS_PER_UNIT; i++) {
      leds.setPixelColor(i, 255, 0, 0);
    }
    leds.show();

  Serial.print(F("wifi setup succed"));  
}
*/
void setupWIFI() {  //建立wifi連線, 失敗則亮紅色燈
  Serial.println(F("begin setup wifi"));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    for (int i = 0; i < NUM_LEDS_PER_UNIT; i++) {
      leds.setPixelColor(i, 255, 0, 0);
    }
    leds.show();
    systemPinBlink(2, 500);
  }
  Serial.print(F("wifi setup succed, IP: "));
  Serial.println(WiFi.localIP());
}

void allSetupOK() {
  Serial.println(F("all setup pass"));
  systemPinBlink(3, 1000);
  for (int i = 0; i < NUM_LEDS_PER_UNIT; i++) {
    leds.setPixelColor(i, 0, 255, 0);
  }
  leds.show();
  delay(1000);
}

void deloperSerialCmdMode() {
  int cmd;
  if (Serial.available()) {
    cmd = Serial.read() - 30;
  }
  switch (cmd) {
    case 1:
      workState = true;
      cmd = deadCmd;
      break;
    case 2:
      workState = false;
      cmd = deadCmd;
      break;
    case 3:
      myDFPlayer.play(music_file_hit_instrument);  //撥放mp3檔案2, 樂器擊打音效
      webSocket.sendTXT(clientName);               //web傳送課服端名字
      cmd = deadCmd;
      break;
    case 4:
      int i = map(bettery_voltage, 0, 1024, 0, NUM_LEDS_PER_UNIT);
      for (i = 0; i < NUM_LEDS_PER_UNIT; i++) {
        leds.setPixelColor(i, client_Bright * client_RGB[0], client_Bright * client_RGB[1], client_Bright * client_RGB[2]);
      }
      leds.show();
      delay(7000);
      cmd = deadCmd;
      break;
  }
}