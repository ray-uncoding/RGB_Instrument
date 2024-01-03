#ifndef _RGB_INSTRUMENT_H_
#define _RGB_INSTRUMENT_H_

#include <Arduino.h>
/*
  這段程式碼會用於建立GBR樂器從機物件, 物件實體會有一個微動開關, 一個MP3模駔,  一條有6燈珠的ws2812的rgb燈條, 並且具有wifi功能
  功能敘述如下：
  1. rgb樂器分為主機與從機, 主機只有wifi功能與燈條, 並且與從機保持wifi通訊
  2. 從機系統有開機, 關機, 待機三個狀態, 開機會開啟所有功能, 關機會關閉所有功能並且燈條亮度為0, 待機開啟所有功能, 但初始亮度為0.1並等待指令
  3. 功能一：當按下按鈕時, 撥放mp3音樂檔案, 並且發送按下按鈕的訊號給主機
  4. 功能二：當放開按鈕時, 停止mp3音樂檔案, 並且發送放開按鈕的訊號給主機
*/
class RGB_Instrument {
public:
  RGB_Instrument();  //構造函數, 創建rgb樂器物件
  RGB_Instrument(
    int rgb_pin,
    int botton_pin,
    int rx_pin,
    int tx_pin,
    int mp3_busy_pin);  //構造函數, 創建rgb樂器物件
  ~RGB_Instrument();    //析構函數, 刪除rgb樂器物件
  void Trun_Off();      //將系統狀態變數改為關機, _brightness逐漸變成0, 停用所有功能
  void Trun_On();       //將系統狀態變數改為開機, 啟用所有功能
  void Trun_Standby();  //將系統狀態變數改為待機, _brightness從0逐漸變成0.1, 啟用所有功能

  void Set_Botton_Pin(int userPin);  //設定按鈕腳位
  void Set_RGB_Pin(int userPin);     //設定rgb腳位
  //void Set_RX_Pin(int userPin);        //設定rx腳位
  //void Set_TX_Pin(int userPin);        //設定tx腳位
  void Set_MP3_Busy_Pin(int userPin);  //設定mp3 busy腳位

  bool Get_Press_Down();     //取得press down數值, 給主程式來發送wifi訊號
  bool Get_Pop_Up();         //取得pop up數值, 給主程式來發送wifi訊號
  int Get_Working_Status();  //取得系統狀態, 給主程式來改變功能

  void Refresh_System_Status();               //根據按鈕紀錄電壓值和現有電壓值比對改變_Press_Down, _Pop_Up的數值, 並更新紀錄值與_Play_Music的數值
  void Refresh_MP3(bool userPlayMusic);       //撥放或停止mp3
  void Set_brightness(float userBrightness);  //更新亮度
  void Set_color(float R, float G, float B);  //更新RGB比例
  void Set_mp3_Volume(int userVolume);        //更新音量

private:
  int _NonWorking = 0;      //關機
  int _Working = 1;         //開機
  int _Standby = 2;         //待機
  int _Working_Status = 0;  //系統狀態

  int _Botton_Pin = 2;    //微動開關 pin
  int _RGB_Pin = 11;      //ws2812 pin
  int _RX_Pin = 44;       //mp3 rx pin
  int _TX_Pin = 43;       //mp3 tx pin
  int _MP3_Busy_Pin = 1;  //mp3 busy pin

  float _brightness = 0;     //燈條亮度變數 0~1,當按鈕觸發press down, 此值變動
  float _color[3];           //燈條顏色變數鎮列, 為RGB顏色比例
  int _LED_NUM = 6;          //燈珠數量變數
  bool _Play_Music = false;  //是否撥放音樂的變數，當按鈕觸發press down, 此值為true, 當按鈕觸發pop up,常態,或播完,此值為false
  int _mp3_Volume = 10;      //mp3音量變數, 0~30

  bool _Botton_Voltage = 0;       //初始化按鈕電壓值
  bool _Last_Botton_Voltage = 0;  //初始化按鈕紀錄電壓值
  bool _Press_Down = 0;           //按鈕是否按下
  bool _Pop_Up = 0;               //按鈕是否放開
};
#endif