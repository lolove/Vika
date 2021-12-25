// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define fsr_pin 26
#define water_alr_pin 23
#define wait_time 10
#define buf_pin 35

int buttonState = 0;
int lastButtonState = 0;
int buttonPushCounter = 0;

int alt_sec = 1;
int start_sec = 0;

// Enhance：尚未實現
int nowHV = 0; //現在偵測重量

int alertWaitMin; // 剩餘等待分鐘數

bool alertIsOpened = false; //是否開啟喝水提示功能


void setup() {
  pinMode(buf_pin,OUTPUT);
  pinMode(water_alr_pin, INPUT);
  Serial.begin(115200);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Hi , Louis");
}



void loop() {
  // 壓力感測提取
  int fsr_value = analogRead(fsr_pin); // 讀取FSR
  int led_value = map(fsr_value, 0, 1023, 0, 255); // 從0~1023映射到0~255
  lcd.setCursor(0, 1);
  lcd.print(led_value);
}