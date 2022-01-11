#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "ESP32Servo.h"
#define BUZZZER_PIN 18 // ESP32 pin GIOP18 connected to piezo buzzer
#define FORCE_SENSOR_PIN 36
#define at

/*************************************************
 * Public Constants
 *************************************************/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

/*************************************************
 * Public Constants
 *************************************************/

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//Send data
char ssid[] = "TP-Link_5761";                //無線網路AP
char wifi_pwd[] = "sugardaddy";               //無線網路AP的密碼
char mqtt_server[] = "163.17.135.246"; //MQTT server 的IP
char sub_topic[] = "vika";             //訂閱的主題
char client_Id[] = "linkit-8000";      //自己的識別代號

String msgStr = "";
char json[25];

int status = WL_IDLE_STATUS;

WiFiClient mtclient;

PubSubClient client(mtclient); // 定義一個 MQTT client

long lastMsg = 0;
char msg[50];
int value = 0;

// Button
const int buttonPin = 4;
int buttonState = 0;
int lastButtonState = 0;

// Drink alert
volatile int count;  // Trigger
int totalInterrupts; // counts the number of triggering of the alarm

#define LED_PIN 32

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

int initFc = 100000000;

// Code with critica section
void IRAM_ATTR onTime()
{
    portENTER_CRITICAL_ISR(&timerMux);
    count++;
    portEXIT_CRITICAL_ISR(&timerMux);
}

// Alert setting
int alertTime = 5;
bool isAlertOpened = true;

// Melody

int melody[] = {
    NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};

int noteDurations[] = {
    4, 8, 8, 4, 4, 4, 4, 4};

void setup()
{
    Serial.begin(9600);
    // initialize LCD
    lcd.init();
    // turn on LCD backlight
    lcd.backlight();

    setup_wifi();
    client.setServer(mqtt_server, 1883); //連線到 MQTT server
    client.setCallback(callback);

    pinMode(buttonPin, INPUT);

    // timer
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTime, true);

    // Sets an alarm to sound every second
    timerAlarmWrite(timer, 1000000, true);
    timerAlarmEnable(timer);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    int fc = get_force_num();
    if((initFc - fc)*(initFc - fc) < 2500){
        fc = initFc;
    }else{
        initFc = fc;
    }
    lcd_print_first_ln("Hello, ESP32");
    lcd_print_second_ln("va: ");
    lcd.print(fc);
    // 建立MQTT訊息（JSON格式的字串）
    if(isAlertOpened == false){
        msgStr = msgStr + "{\"water\":" + (fc) + ",\"time\":" + (-1) + "}";
    }else{
        msgStr = msgStr + "{\"water\":" + (fc) + ",\"time\":" + (alertTime) + "}";
    }
    
    Serial.print(msgStr);
    msgStr.toCharArray(json, 255);
    client.publish(sub_topic, json);
    msgStr = "";
    client.loop();
    //delay(1000);

    // set cursor to first column, first row

    buttonState = digitalRead(buttonPin);
    Serial.println(buttonState);
    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH
    if (buttonState == HIGH)
    {
        // turn LED on
        Serial.println("HIGH");
    }
    else
    {
        // turn LED off
        Serial.println("LOW");
    }

    if (alertTime == 0 && isAlertOpened == true)
    {
        melody_alert();
        alertTime = 5;
    }

    if (count > 0)
    {
        // Comment out enter / exit to deactivate the critical section
        portENTER_CRITICAL(&timerMux);
        count--;
        portEXIT_CRITICAL(&timerMux);

        totalInterrupts++;
        Serial.print("totalInterrupts");
        Serial.println(totalInterrupts);
        if (totalInterrupts % 2 == 0)
        {
            // Lights up the LED if the counter is even
            alertTime--;
            Serial.println(alertTime);
        }
        else
        {
            // Then swith off
            //  Serial.println("INININININININI");
        }
    }
    buttonState = digitalRead(buttonPin);
    if (buttonState != lastButtonState)
    {

        if (buttonState == HIGH)
        {
            Serial.println("****************");
            Serial.println(isAlertOpened);
            if(isAlertOpened) isAlertOpened=false;
            else isAlertOpened =true;
            Serial.println(isAlertOpened);
            if (isAlertOpened == true)
                alertTime = 5;
        }
        else
        {
            // if the current state is LOW, then the button
            // went from on to off
            Serial.println("off");
        }
        // delay a little bit to avoid debouncing
        delay(5); // Wait for 5 millisecond(s)
    }
    lastButtonState = buttonState;

    if (isAlertOpened == true)
    {
        lcd.print(" al: ");
        lcd.print(alertTime);
    }

    delay(1000);
    lcd.clear();
}

// Drink alert
int drink_alert()
{
}

void melody_alert()
{
    for (int thisNote = 0; thisNote < 8; thisNote++)
    {
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(BUZZZER_PIN, melody[thisNote], noteDuration);

        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(BUZZZER_PIN);
    }
}

// Button control
bool is_restart_btn_click()
{
}

// Force sensor
int get_force_num()
{
    int forceRead = analogRead(FORCE_SENSOR_PIN);
    Serial.println("<Force Sensor> Force snsor value: " + forceRead);
    return forceRead;
}

// LCD Controller
void lcd_print_first_ln(char *str)
{
    lcd.setCursor(0, 0);
    lcd.print(str);
}

void lcd_print_second_ln(char *str)
{
    lcd.setCursor(0, 1);
    lcd.print(str);
}

// Wifi
void printWifiStatus()
{ //print Wifi status
    // print the ssid of the network you're attached to:
    Serial.print("ssid: ");
    Serial.println(WiFi.localIP());

    // print your WiFi shield's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println("dBm");
}

void setup_wifi()
{ //setup Wifi
    // attempt to connect to Wifi network:
    WiFi.mode(WIFI_STA); //設置WiFi模式
    WiFi.begin(ssid, wifi_pwd);

    Serial.print("WiFi connecting");

    //當WiFi連線時會回傳WL_CONNECTED，因此跳出迴圈時代表已成功連線
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.print("IP位址:");
    Serial.println(WiFi.localIP()); //讀取IP位址
    Serial.print("WiFi RSSI:");
    Serial.println(WiFi.RSSI()); //讀取WiFi強度
}

void callback(char *topic, byte *payload, unsigned int length)
{ //MQTT sub
    Serial.print("Input Message arrived [");
    Serial.print(sub_topic);
    Serial.print("] ");
    Serial.print((char)payload[0]);
    if ((char)payload[0] == '1')
    {
        //    digitalWrite(LED_BUILTIN, HIGH);
    }
    else if ((char)payload[0] == '0')
    {
        //    digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
        Serial.print("value error");
    }
    Serial.println();
}

void reconnect()
{ //reconnect MQTT
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = client_Id;
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str()))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            // ... and resubscribe
            client.subscribe(sub_topic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}