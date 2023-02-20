/*************************************************************

  You’ll need:
   - Blynk IoT app (download from App Store or Google Play)
   - ESP8266 board
   - Decide how to connect to Blynk
     (USB, Ethernet, Wi-Fi, Bluetooth, ...)

  There is a bunch of great example sketches included to show you how to get
  started. Think of them as LEGO bricks  and combine them as you wish.
  For example, take the Ethernet Shield sketch and combine it with the
  Servo example, or choose a USB sketch and add a code from SendData
  example.
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "-"
#define BLYNK_DEVICE_NAME "Lolin"
#define BLYNK_AUTH_TOKEN "-"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "-";
char pass[] = "-";

float sensor = A0;
int analogState = 0;

int led = 0;
//int led1 = 5;
//int led2 = 4;
WidgetLED ledW(5);
WidgetLED ledW2(4);

BlynkTimer timer;

// V1 LED Widget is blinking
void blinkLedWidget()
{
  if (ledW.getValue()) {
    ledW.off();
    Serial.println("LED on LED 1: off");
  } else {
    ledW.on();
    Serial.println("LED on LED 1: on");
  }
  if (ledW2.getValue()) {
    ledW2.off();
    Serial.println("LED on LED 2: off");
  } else {
    ledW2.on();
    Serial.println("LED on LED 2: on");
  }
}
/*void gauge(){
  analogState = analogRead(sensor);
  //int luz = (analogState*100)/999.00;
  Blynk.virtualWrite(V1, analogState);
}*/

void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(led,OUTPUT);
  //pinMode(led1,OUTPUT);
  //pinMode(led2,OUTPUT);
  Blynk.begin(auth, ssid, pass);
  pinMode(A0, INPUT);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  analogState = analogRead(sensor);
  float luz = (analogState*100)/1023;
  Blynk.virtualWrite(V1, luz);
  Serial.println(luz);
  if(luz>=50){
    //digitalWrite(led1,HIGH);
    //digitalWrite(led2,HIGH);
    digitalWrite(led,HIGH);
    delay(500);
    digitalWrite(led,LOW);
    delay(500);
    
  }else{
    digitalWrite(led,LOW);
    /*
    digitalWrite(led1,LOW);
    digitalWrite(led2,LOW);
    delay(500);
    digitalWrite(led1,HIGH);
    digitalWrite(led2,HIGH);
    delay(500);
    */
  }
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}

