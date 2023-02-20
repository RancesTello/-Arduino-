#include <ESP8266WiFi.h>
#include <ThingESP.h>

ThingESP8266 thing("Rances", "ControLED", "Rances");

int LED = D2;

float sensor = A0;
int analogState = 0;

const int PIRPin= D3;

unsigned long previousMillis = 0;
const long INTERVAL = 6000;

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(PIRPin, INPUT);
  thing.SetWiFi("-", "-");
  thing.initDevice();
}


String HandleResponse(String query)
{
    analogState = analogRead(sensor);
    float luz = (analogState*100)/1023;
    Serial.println(luz);
    String luzz = "Sensor: "+String(luz)+"%";
    Serial.println(luzz);
    
  if (query == "led on") {
    digitalWrite(LED, 1);
    return "Done: LED Turned ON";
  }

  else if (query == "led off") {
    digitalWrite(LED, 0);
    return "Done: LED Turned OFF";
  }

  else if (query == "luz") {
    return luzz;
  }

  else if (query == "sensor" || query == "sensorOff") {
    Serial.println(query);
    
    String resp;
    bool band;
    query == "sensorOff" ? band = false : band = true; 
    
      do
      {
        int value = digitalRead(PIRPin);
        if (value == HIGH)
        {
          Serial.println("Activo");
          resp = "ACTIVO";
          if(band == false) 
          {
            break;
            resp = "APAGADO";
          }
        }
        delay(2000);
      }while(band == true);
    return resp;
  }

  else if (query == "led status")
    return digitalRead(LED) ? "LED is OFF" : "LED is ON";
  else return "Your query was invalid..";

}


void loop()
{
  
  thing.Handle();
  
}
