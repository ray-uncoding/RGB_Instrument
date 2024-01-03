#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <Adafruit_NeoPixel.h>


const char *ssid = "Liangyu";
const char *password = "10635493";
int buttonStates[3] = { 0, 0, 0 };  // 每個從機的按鈕狀態
const char *deviceNames[3] = { "Device1", "Device2", "Device3" };
AsyncWebServer server(80);

#define NUM_UNITS 3                                                     // 樂器單元數量
#define NUM_LEDS_PER_UNIT 6                                             // 每個單元的LED數量
#define NUM_LEDS_TOTAL (NUM_UNITS * NUM_LEDS_PER_UNIT)                  // 總LED數量
#define LED_PIN 11                                                      // 連接第一個LED的腳位
Adafruit_NeoPixel leds(NUM_LEDS_TOTAL, LED_PIN, NEO_GRB + NEO_KHZ800);  //  定義ws2812燈條

float device1_RGB[3] = { 200.00, 200.00, 0.00 };  //調整樂器單元顏色
float device2_RGB[3] = { 255.00, 0.00, 0.00 };
float device3_RGB[3] = { 0.00, 0.00, 255.00 };

int t1, t2, t3 = 0;
int inverse1, inverse2, inverse3 = -1;
int push1, push2, push3 = 200;
bool start_on1, start_on2, start_on3, OP_start = false;
bool animate = true;
int frq = 20;

// 目前公式 亮度 y(t)=t/2500, if:0<t<2500; y(t)=(-t/2500)+1, if:2500<t<5000; 單位:ms; 解析度:20ms;
// if y(t)<=0.1, y(t)=0.1; if y(t)>=0.9, y(t)=0.9;
void print_power_on(){
  Serial.println("*////////////////////////////////////////////");
  Serial.println("///////////////// power on! /////////////////");
  Serial.println("*////////////////////////////////////////////");
}

void print_power_off(){
  Serial.println("*////////////////////////////////////////////");
  Serial.println("///////////////// power off! ////////////////");
  Serial.println("*////////////////////////////////////////////");
}
void setColor(int ledNum, int red, int green, int blue) {  // 設定LED顏色
  leds.setPixelColor(ledNum, red, green, blue);
  leds.show();
}
void setUnit_color1(int unit, int t, float v) {
  float a = t / v;
  int init_ledNum = unit * NUM_LEDS_PER_UNIT;
  float R = a * device1_RGB[0];
  float G = a * device1_RGB[1];
  float B = a * device1_RGB[2];
  for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
    setColor(init_ledNum + led, R, G, B);
  }
}
void setUnit_color2(int unit, int t, float v) {
  float a = t / v;
  int init_ledNum = unit * NUM_LEDS_PER_UNIT;
  float R = a * device2_RGB[0];
  float G = a * device2_RGB[1];
  float B = a * device2_RGB[2];
  for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
    setColor(init_ledNum + led, R, G, B);
  }
}
void setUnit_color3(int unit, int t, float v) {
  float a = t / v;
  int init_ledNum = unit * NUM_LEDS_PER_UNIT;
  float R = a * device3_RGB[0];
  float G = a * device3_RGB[1];
  float B = a * device3_RGB[2];
  for (int led = 0; led < NUM_LEDS_PER_UNIT; led++) {
    setColor(init_ledNum + led, R, G, B);
  }
}

void poweron() {
  print_power_on();
  delay(2000);

  for (int i = 0; i <= 100; i++) {
    t1 = i;
    t2 = i;
    t3 = i;
    setUnit_color1(0, t1, 1000.00);
    setUnit_color2(1, t2, 1000.00);
    setUnit_color3(2, t3, 1000.00);

    Serial.print("t1: ");
    Serial.println(t1);
    Serial.print("t2: ");
    Serial.println(t2);
    Serial.print("t3: ");
    Serial.println(t3);
    Serial.println();

    delay(20);  //開2秒
  }
  t1 = 10;
  t2 = 10;
  t3 = 10;
}
void poweroff() {
  print_power_off();
  delay(2000);

  for (int i = 100; i >= 0; i--) {
    t1 = i;
    t2 = i;
    t3 = i;
    setUnit_color1(0, t1, 1000.00);
    setUnit_color2(1, t2, 1000.00);
    setUnit_color3(2, t3, 1000.00);

    Serial.print("t1: ");
    Serial.println(t1);
    Serial.print("t2: ");
    Serial.println(t2);
    Serial.print("t3: ");
    Serial.println(t3);
    Serial.println();

    delay(20);  //2秒
  }
  t1 = 0;
  t2 = 0;
  t3 = 0;
}

void one() {
  start_on1 = true;
  inverse1 = -inverse1;
  push1 = 200;

  Serial.println("inverse1 chang!");
}
void two() {
  start_on2 = true;
  inverse2 = -inverse2;
  push2 = 200;

  Serial.println("inverse2 chang!");
}
void three() {
  start_on3 = true;
  inverse3 = -inverse3;
  push3 = 200;

  Serial.println("inverse3 chang!");
}


void handleButtonPress(int deviceNumber) {
  switch (deviceNumber) {
    case 1:
      one();
      break;
    case 2:
      two();
      break;
    case 3:
      three();
      break;
    default:
      break;
  }
}


void one_loop() {
  if (t1 >= 90) {
    t1 = 90;
    inverse1 = -1;
  } else if (t1 <= 10) {
    t1 = 10;
    if (push1 > 0) {
      inverse1 = 1;
    } else {
      inverse1 = 0;
    }
  }

  if (!start_on1) {
    inverse1 = 0;
  }
  if (push1 > 0) {
    push1 = push1 - frq;
  }
}
void two_loop() {
  if (t2 >= 90) {
    t2 = 90;
    inverse2 = -1;
  } else if (t2 <= 10) {
    t2 = 10;
    if (push2 > 0) {
      inverse2 = 1;
    } else {
      inverse2 = 0;
    }
  }

  if (!start_on2) {
    inverse2 = 0;
  }
  if (push2 > 0) {
    push2 = push2 - frq;
  }
}
void three_loop() {
  if (t3 >= 90) {
    t3 = 90;
    inverse3 = -1;
  } else if (t3 <= 10) {
    t3 = 10;
    if (push3 > 0) {
      inverse3 = 1;
    } else {
      inverse3 = 0;
    }
  }
  if (!start_on3) {
    inverse3 = 0;
  }
  if (push3 > 0) {
    push3 = push3 - frq;
  }
}

void setup() {
  Serial.begin(921600);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    delay(3000);
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><body>";
    html += "<h1>Music ON</h1>";
    for (int i = 0; i < 3; i++) {
      html += "<p>" + String(deviceNames[i]) + ": " + String(buttonStates[i]) + "</p>";
    }
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/updateDevice1", HTTP_GET, [](AsyncWebServerRequest *request) {
    buttonStates[0] = request->getParam("state")->value().toInt();
    if (buttonStates[0]) handleButtonPress(1);
    request->send(200, "text/plain", "OK");
  });

  server.on("/updateDevice2", HTTP_GET, [](AsyncWebServerRequest *request) {
    buttonStates[1] = request->getParam("state")->value().toInt();
    if (buttonStates[1]) handleButtonPress(2);
    request->send(200, "text/plain", "OK");
  });

  server.on("/updateDevice3", HTTP_GET, [](AsyncWebServerRequest *request) {
    buttonStates[2] = request->getParam("state")->value().toInt();
    if (buttonStates[2]) handleButtonPress(3);
    request->send(200, "text/plain", "OK");
  });

  server.on("/updateOP_start", HTTP_GET, [](AsyncWebServerRequest *request) {
    animate = request->getParam("state")->value().toInt();
    request->send(200, "text/plain", "OK");
  });


  server.begin();
  Serial.println("Host is ready!");
}

void loop() {
  if (OP_start == true && animate == true) {  //正常運行
    t1 = t1 + inverse1;
    t2 = t2 + inverse2;
    t3 = t3 + inverse3;

    one_loop();
    two_loop();
    three_loop();

    setUnit_color1(0, t1, 100.00);
    setUnit_color2(1, t2, 100.00);
    setUnit_color3(2, t3, 100.00);
  } else if (OP_start == false && animate == true) {  //開機動畫
    poweron();
    OP_start = true;
  } else if (OP_start == true && animate == false) {  //關機動畫
    poweroff();
    OP_start = false;
    start_on1 = false;
    start_on2 = false;
    start_on3 = false;
  }
  delay(frq);
  
  Serial.println(t1);
  Serial.println(t2);
  Serial.println(t3);
  Serial.println();
  
}
