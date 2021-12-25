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
  
  
  //判斷是否開啟喝水提示
  buttonState = digitalRead(water_alr_pin);
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH, then the button
      // went from off to on
      buttonPushCounter += 1;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
      start_sec = millis() / 6000;
    } else {
      // if the current state is LOW, then the button
      // went from on to off
      Serial.println("off");
    }
    // delay a little bit to avoid debouncing
    delay(5); // Wait for 5 millisecond(s)
  }
  
  lastButtonState = buttonState;
  if (buttonPushCounter % 2 == 0) {
    // 開啟提示
    // digitalWrite(LED_BUILTIN, HIGH);
    if((millis() / 6000 ) - start_sec == alt_sec){
      // enhance:蜂鳴器發出聲音
      digitalWrite(buf_pin,HIGH);
      delay(100);
      digitalWrite(buf_pin,LOW);
      //delay(200);
      // Reset
      start_sec = (millis() / 6000);
    }
    lcd.setCursor(0, 1);
    lcd.print("Open:");
    lcd.print(alt_sec - (millis()/ 6000 - start_sec ));
    lcd.print("m");
    
  } else {
    // 關閉提示
    lcd.setCursor(0, 1);
    lcd.print("alert close");
    // digitalWrite(LED_BUILTIN, LOW);
  }

  
  

  // 壓力感測提取
  int fsr_value = analogRead(fsr_pin); // 讀取FSR
  int led_value = map(fsr_value, 0, 1023, 0, 255); // 從0~1023映射到0~255
  
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  //lcd.setCursor(0, 1);
  //lcd.print(led_value);
}