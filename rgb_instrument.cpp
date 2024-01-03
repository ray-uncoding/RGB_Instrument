#include "rgb_instrument.h"

#include <Adafruit_NeoPixel.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

/*
  這段程式碼會用於建立GBR樂器從機物件, 物件實體會有一個微動開關, 一個MP3模駔,  一條有6燈珠的ws2812的rgb燈條, 並且具有wifi功能
  功能敘述如下：
  1. rgb樂器分為主機與從機, 主機只有wifi功能與燈條, 並且與從機保持wifi通訊
  2. 從機系統有開機, 關機, 待機三個狀態, 開機會開啟所有功能, 關機會關閉所有功能並且燈條亮度為0, 待機開啟所有功能, 但初始亮度為0.1並等待指令
  3. 功能一：當按下按鈕時, 撥放mp3音樂檔案, 並且發送按下按鈕的訊號給主機
  4. 功能二：當放開按鈕時, 停止mp3音樂檔案, 並且發送放開按鈕的訊號給主機
*/

RGB_Instrument::RGB_Instrument() {  //構造函數, 創建rgb樂器物件, 設定rgb 微動開關 rx tx mp3_busy 腳位, 並Serial連線MP3
  Serial.println(F("建立樂器物件"));
  pinMode(_RGB_Pin, OUTPUT);           //設定ws2812 rgb燈條pin 為輸出
  pinMode(_Botton_Pin, INPUT_PULLUP);  //設定按鈕pin 為上拉輸入, 常態斷路為1
  pinMode(_MP3_Busy_Pin, INPUT);       //設定mp3 busy pin, 設定為輸入

  Adafruit_NeoPixel leds(_LED_NUM, _RGB_Pin, NEO_GRB + NEO_KHZ800);  //設定ws2812物件 leds
  SoftwareSerial mySoftwareSerial(_RX_Pin, _TX_Pin);                 //設定serial物件 mySoftwareSerial
  DFRobotDFPlayerMini myDFPlayer;                                    //設定DFplayer物件 myDFPlayer
  mySoftwareSerial.begin(9600);                                      //mySoftwareSerial開始連線 baurd rate 9600

  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //檢測到serial連線失敗時, 跳出錯誤訊息
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(_mp3_Volume);  //設定mp3音量為10, 0~30
}
RGB_Instrument::RGB_Instrument(  //構造函數, 創建rgb樂器物件
  int rgb_pin,
  int botton_pin,
  int rx_pin,
  int tx_pin,
  int mp3_busy_pin) {
  Serial.println(F("建立樂器物件"));
  pinMode(rgb_pin, OUTPUT);           //設定ws2812 rgb燈條pin 為輸出
  pinMode(botton_pin, INPUT_PULLUP);  //設定按鈕pin 為上拉輸入, 常態斷路為1
  pinMode(mp3_busy_pin, INPUT);       //設定mp3 busy pin, 設定為輸入

  Adafruit_NeoPixel leds(_LED_NUM, rgb_pin, NEO_GRB + NEO_KHZ800);  //設定ws2812物件 leds
  SoftwareSerial mySoftwareSerial(rx_pin, tx_pin);                  //設定serial物件 mySoftwareSerial
  DFRobotDFPlayerMini myDFPlayer;                                   //設定DFplayer物件 myDFPlayer
  mySoftwareSerial.begin(9600);                                     //mySoftwareSerial開始連線 baurd rate 9600

  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //檢測到serial連線失敗時, 跳出錯誤訊息
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(_mp3_Volume);  //設定mp3音量為10, 0~30
}
RGB_Instrument::~RGB_Instrument() {
  Serial.println("刪除樂器物件");
}

void RGB_Instrument::Trun_Off() {
  _Working_Status = _NonWorking;
}
void RGB_Instrument::Trun_On() {
  _Working_Status = _Working;
}
void RGB_Instrument::Trun_Standby() {
  _Working_Status = _Standby;
}
void RGB_Instrument::Set_Botton_Pin(int userPin) {  //設定按鈕腳位
  _Botton_Pin = userPin;
  pinMode(_Botton_Pin, INPUT_PULLUP);  //設定按鈕pin 為上拉輸入, 常態斷路為1
}
void RGB_Instrument::Set_RGB_Pin(int userPin) {  //設定rgb腳位
  _RGB_Pin = userPin;
  pinMode(_RGB_Pin, OUTPUT);  //設定ws2812 rgb燈條pin 為輸出
}
/*      
void RGB_Instrument::Set_RX_Pin(int userPin){//設定rx腳位
}        
void RGB_Instrument::Set_TX_Pin(int userPin){//設定tx腳位
}
*/
void RGB_Instrument::Set_MP3_Busy_Pin(int userPin) {  //設定mp3 busy腳位
  _MP3_Busy_Pin = userPin;
  pinMode(_MP3_Busy_Pin, INPUT);  //設定mp3 busy pin, 設定為輸入
}
bool RGB_Instrument::Get_Press_Down() {  //取得press down數值, 給主程式來發送wifi訊號
  return _Press_Down;
}
bool RGB_Instrument::Get_Pop_Up() {  //取得pop up數值, 給主程式來發送wifi訊號
  return _Pop_Up;
}
int RGB_Instrument::Get_Working_Status() {  //取得系統狀態, 給主程式來改變功能
  return _Working_Status;
}
void Refresh_System_Status() {  //根據按鈕紀錄電壓值和現有電壓值比對改變_Press_Down, _Pop_Up的數值, 並更新紀錄值與_Play_Music的數值

}
void Refresh_MP3(bool userPlayMusic) {  //撥放或停止mp3

}
void Set_brightness(float userBrightness) {  //更新亮度

}
void Set_color(float R, float G, float B) {  //更新RGB比例

}
void Set_mp3_Volume(int userVolume) {  //更新音量

}