#include <ESP8266WiFi.h>  //Incluimos librerías
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

//Datos de nuestro WiFi
const char *ssid = "-";
const char *password = "-";

//Configurar una dirección IP Estática
IPAddress ip(192,168,0,10);     
IPAddress gateway(192,168,0,2);
IPAddress subnet(255,255,255,0);

const int led = 5; //Pin donde estará la señal al rele
const int led2 = 4;

String Estado_led; //Variable String para mandar el estado del rele a nuestra página web
String Estado_led2;

String Estado_sensor;

AsyncWebServer server(80); //Puerto TCP

String processor(const String& var) //Función Processor para regresar valores a página Web
{
  if(var == "ESTADO_LED")
    {
     if(digitalRead(led) == 1)
     {
      Estado_led = "Encendido";
     }
     else
     {
      Estado_led = "Apagado";
     }
    return Estado_led;
    }
  if(var == "ESTADO_LED2")
  {
    if(digitalRead(led2) == 1 )
    {
      Estado_led2 = "Encendido";
    }
    else
    {
      Estado_led2 = "Apagado";
    }
    return Estado_led2;
  }
 if(var == "SENSOR")
 {
    int sensor = analogRead(A0);
    int luz = (sensor*100)/1023;
    Estado_sensor = String(luz);
    Serial.println(sensor);
    Serial.println(luz);
    Serial.println(Estado_sensor);
    return Estado_sensor;
 }
}

void setup(){
  // Iniciamos comunicación Serial y ponemos el pin del rele como salida
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);

  //Iniciamos el sistema Flash
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Conectamos a WiFi Modo estación con dirección IP estática
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  //Mientras se conecta a la red que se imprima conectando a WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi..");
  }
  //Imprimimos IP en el monitor serial
  Serial.println(WiFi.localIP());

  //A partir de aqui vamos a resolver las solicitudes del cliente

  // Cuando ingresen a nuestra página
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Cuando nuestro archivo Html solicite el archivo Css
  server.on("/estilos.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/estilos.css", "text/css");
  });

  server.on("/astro.gif", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/astro.gif", "gif");
  });

  // Cuando pulsemos el botón de encendido
  server.on("/LED=ON", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(led, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Cuando pulsemos el botón de apagado
  server.on("/LED=OFF", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(led, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  /*server.on("/LED=BLINK", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(led, HIGH);
    delay(2000);
    digitalWrite(led, LOW);
    delay(2000);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });*/

  server.on("/LED2=ON", HTTP_GET, [](AsyncWebServerRequest * request){
    digitalWrite(led2, HIGH);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/LED2=OFF", HTTP_GET, [](AsyncWebServerRequest * request){
    digitalWrite(led2, LOW);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/REFRESH", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  //Iniciamos servidor
  server.begin();
}
void loop() {
//No se usará debido a la librería del servidor web asíncrono
}
