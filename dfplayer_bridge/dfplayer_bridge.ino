#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define SYSTEM_LED_PIN 2  //開機檢測燈
#define MP3_RX_PIN 4
#define MP3_TX_PIN 5
//#define MP3_BUZY_PIN 13  //監測音樂撥放與否
#define MONITOR_BAUDRATE 921600
#define MP3_BAUDRATE 9600

SoftwareSerial mySoftwareSerial(MP3_RX_PIN, MP3_TX_PIN);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

/*------系統變數------*/
bool last_workState = true;  //紀錄開關機狀態
bool workState = true;       //預設開關機狀態
int loop_rate = 50;          //刷新率
/*------mp3變數-------*/
bool isPlaying = false;             //是否正在撥放音樂, 是->0, 否->1
int music_file_hit_instrument = 1;  //擊打音效的檔案編號

/*-----開發者指令變數-----*/
int on = 1;
int off = 2;
int perssBotton = 3;
int cheakvoltage = 4;
int deadCmd = 0;

void setup() {
  /*------系統設定-------*/
  Serial.begin(MONITOR_BAUDRATE);
  mySoftwareSerial.begin(MP3_BAUDRATE);
  Serial.println(F("begin setup system"));
  // pinMode(MP3_BUZY_PIN, INPUT);
  pinMode(SYSTEM_LED_PIN, OUTPUT);
  Serial.println(F("system setup succed"));
  /*------元件設定-------*/
  Serial.println(F("begin setup element"));
  setupMP3Serial();
  myDFPlayer.volume(30);

  allSetupOK();

  myDFPlayer.play(music_file_hit_instrument);  //撥放mp3檔案2, 樂器擊打音效
  delay(10000);
}
void loop() {
  /*------刷新系統變數-------*/
  deloperSerialCmdMode();  //刷新開發者指令
  //isPlaying = digitalRead(MP3_BUZY_PIN);   //刷新是否撥放音樂, 是->0, 否->1
  /*------感測到指令時-------*/
  if (ifBottonPress()) {
  }
  /*------次刷新系統變數------*/
  delay(loop_rate);  //刷新率
}

void allClientVerToZero() {
  client_Bright = 0;
  client_chang = 0;
}


void systemPinBlink(int flashes_times, int interval_microSeconds) {
  for (int i = 0; i < flashes_times; i++) {
    digitalWrite(SYSTEM_LED_PIN, 1);
    delay(interval_microSeconds);
    digitalWrite(SYSTEM_LED_PIN, 0);
    delay(interval_microSeconds);
  }
}

void setupMP3Serial() {  //建立mp3的serial連線, 失敗則頻閃橘色燈
  Serial.println(F("begin setup dfplayer"));
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (1) {
      systemPinBlink(2, 1000);
    }
  }
  Serial.print(F("dfplayer setup succed"));
}

void allSetupOK() {
  Serial.println(F("all setup pass"));
  systemPinBlink(3, 1000);
}

void deloperSerialCmdMode() {
  int cmd;
  if (Serial.available()) {
    cmd = Serial.read() - 30;
  }
  switch (cmd) {
    case 1:
      cmd = deadCmd;
      break;
    case 2:
      cmd = deadCmd;
      break;
    case 3:
      myDFPlayer.play(music_file_hit_instrument);  //撥放mp3檔案2, 樂器擊打音效
      cmd = deadCmd;
      break;
    case 4:
      cmd = deadCmd;
      break;
  }
}