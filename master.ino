/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define fsr_pin A0
#define water_alr_pin 7
#define wait_time 10
#define buf_pin 8

int buttonState = 0;
int lastButtonState = 0;
int buttonPushCounter = 0;

int alt_sec = 1;
int start_sec = 0;

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
 