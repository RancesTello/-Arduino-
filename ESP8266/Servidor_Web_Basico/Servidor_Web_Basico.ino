#include <ESP8266WiFi.h>

//Información de nuestro WIFI
const char *ssid = "-";
const char *password = "-";

//Datos para una IP estática
IPAddress ip(192,168,0,10);     
IPAddress gateway(192,168,0,1);   
IPAddress subnet(255,255,255,0); 

//Definimos el pin donde está conectado el led
int ledPin = 5; 

//Puerto 80 TCP, este puerto se usa para la navegación web http
WiFiServer server(80);
 
void setup() {
  Serial.begin(115200); //Iniciamos comunicación serial
  delay(10);
  
  pinMode(ledPin, OUTPUT);  //Pin del Led como salida
  digitalWrite(ledPin, LOW);  //Ponemos la salida de led en bajo
   
  // Conectandose al Wifi
  Serial.println();
  Serial.println();
  Serial.print("Conectandose a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); //Wifi en modo estación
  WiFi.config(ip, gateway, subnet);//Configuramos la ip estática
  WiFi.begin(ssid, password); //Iniciamos conexión con el nombre y la contraseña del wifi que indicamos

  //Mientras se conecta se imprimiran puntos
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  //Cuando se conecte lo imprimimos
  Serial.println("");
  Serial.println("WiFi Conectado");
   
  // Iniciamos el esp como servidor web
  server.begin();
  Serial.println("Servidor iniciado");
 
  // Imprimimos la dirección IP
  Serial.print("Usa esta URL para comunicar al ESP: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
    
}
 
void loop() {
  // El servidor siempre estará esperando a que se conecte un cliente
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  
  Serial.println("Nuevo cliente"); //Cuando un cliente se conecte vamos a imprimir que se conectó
  while(!client.available()){  //Esperamos a que el ciente mande una solicitud
    delay(1);
  }
   
  // Leemos la primer línea de la solicitud y la guardamos en la variable string request
  String request = client.readStringUntil('\r');
  Serial.println(request); //Imprimimos la solicitud
  client.flush(); //Descartamos los datos que se han escrito en el cliente y no se han leído
   
  // Relacionamos la solicitud
  if (request.indexOf("/LED=OFF") != -1){
    digitalWrite(ledPin, LOW);
  }
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH);
  } 
  
  if (request.indexOf("/LED=BLINK") != -1){
    for(int i=0;i<10;i++)
    {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
    }
  }

  // Respuesta del servidor web
  
  client.println("HTTP/1.1 200 OK"); // La respuesta empieza con una linea de estado  
  client.println("Content-Type: text/html"); //Empieza el cuerpo de la respuesta indicando que el contenido será un documento html
  client.println(""); // Ponemos un espacio
  client.println("<!DOCTYPE HTML>"); //Indicamos el inicio del Documento HTML
  client.println("<html lang=\"en\">");
  client.println("<head>");
  client.println("<meta charset=\"UTF-8\">");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"); //Para que se adapte en móviles
  client.println("<title>Servidor Web ESP8266</title>");
  client.println("</head>");
  client.println("<body>");
  client.println("<br><br>");
  
  client.println("<h1 style=\"text-align: center;\">Servidor Web ESP8266</h1>");
  
  client.println("<p style=\"text-align: center;\">");
  client.println("Click <a href=\"/LED=ON\">Aqui</a> para encender LED en el pin 2 ON<br>"); //Oración que contiene un hipervínculo
  client.println("Click <a href=\"/LED=OFF\">Aqui</a> para apagar el led en el pin 2 OFF<br>");
  client.println("Click <a href=\"/LED=BLINK\">Aqui</a> para parpadar el led en el pin 2<br> <br>");
  

  client.println("<button onclick=location.href=\"/LED=ON\"> Encender LED</button> <br> <br>"); // Botón sencillo que contiene hipervínculo
  client.println("<button onclick=location.href=\"/LED=OFF\" >Apagar LED </button> <br> <br>");
  client.println("<button onclick=location.href=\"/LED=BLINK\">Parpadear LED </button> <br> <br>");

  client.println("</p>");
  client.println("</body>");
  
  client.println("</html>"); //Terminamos el HTML
 
  delay(1);
  Serial.println("Cliente desconectado"); //Imprimimos que terminó el proceso con el cliente desconectado
  Serial.println("");
 
}

 
