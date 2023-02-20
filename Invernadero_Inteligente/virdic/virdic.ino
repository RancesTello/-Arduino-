#include <SoftwareSerial.h>

#include <dht.h>
#include <EEPROM.h>
#include <GravityTDS.h>

GravityTDS gravityTds;
SoftwareSerial mySerial(3,4);

#define TdsSensorPin A2
#define DHT11_PIN 2
dht DHT;

int calorRelt = 30.00;
int seqRelt = 100;
float clRelt = 800;

int tmp;
int hum;
bool voltajeTemp;

String b;
int activador;
bool voltajeSeq;
bool voltajeSeq2;

int sequedad;
int sequedad2;

float temperature = 25, tdsValue = 0;
bool voltajeCl;

int rele = 5;
int rele2 = 6;
int rele3 = 7;


void setup() {

  Serial.begin(115200);
  pinMode(rele, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
  digitalWrite(rele, HIGH);
  digitalWrite(rele2, HIGH);
  digitalWrite(rele3, HIGH);
  while(!Serial){
    ;
  }
  mySerial.begin(115200);
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);
  gravityTds.setAdcRange(1024);
  gravityTds.begin();
}

void loop() {
  float sensorVal = analogRead(A0);
  float sensorVal2 = analogRead(A1);
  char letra = mySerial.read();
  sequedad = (sensorVal * 100)/1023;
  sequedad2 = (sensorVal2 * 100)/1023;
  gravityTds.setTemperature(temperature);
  gravityTds.update();
  tdsValue = gravityTds.getTdsValue();
  DHT.read11(DHT11_PIN);
  tmp = DHT.temperature;
  hum = DHT.humidity;

  condicionales(sequedad,tdsValue);
  bluetooth(letra,tmp,hum,sequedad,sequedad2,tdsValue);

  Serial.println(consolaTemp(b));
  Serial.println(consolaSoil(sequedad,b));
  Serial.println(consolaSoil(sequedad2,b));
  Serial.println(consolaCloro(tdsValue,b));
  Serial.println(" ");
  
  delay(500);
}

void reley (int a){
  
  switch(a){
    case 1: digitalWrite(rele, LOW);
            break;
    case 2: digitalWrite(rele, HIGH);
            break;
    case 3: digitalWrite(rele2, LOW);
            break;
    case 4: digitalWrite(rele2, HIGH);
            break;
    case 5: digitalWrite(rele3, LOW);
            break;
    case 6: digitalWrite(rele3, HIGH);
            break;
  } 
}


void consolaTemp(String b){
  Serial.println("╔─────────────╗");
  Serial.print("  Temperatura: "); 
  Serial.print(DHT.temperature);
  Serial.println(" C");
  Serial.print("  Humedad: "); 
  Serial.print(DHT.humidity); 
  Serial.println(" %");
  Serial.print("  Voltaje: ");
  Serial.println(b);
  Serial.println("╚─────────────╝");
  Serial.println(" ");
}

void consolaSoil(int porcentaje, String b){
  Serial.println("╔─────────────╗");
  Serial.print("  Humedad del suelo: "); 
  Serial.print(porcentaje);
  Serial.println(" %");
  Serial.print("  Voltaje: ");
  Serial.println(b);
  if(porcentaje >= 50){
    Serial.println("La tierra está muy seca");
  } else if (porcentaje <= 30) {
    Serial.println("La tierra está muy humeda");
  }
  Serial.println("╚─────────────╝");
  Serial.println(" ");
}

void consolaCloro(float tds, String b){
  Serial.println("╔─────────────╗");
  Serial.print("  Cloro: "); 
  Serial.print(tds);
  Serial.println("ppm");
  Serial.print("  Voltaje: ");
  Serial.println(b);
  Serial.println("╚─────────────╝");
  Serial.println(" ");
}


void condicionales(int sequedad, float tdsValue){
   if(DHT.temperature >= calorRelt){
      activador = 1;
      voltajeTemp = true;
      reley(activador);  
  } else {
      activador = 2;
      voltajeTemp = false;
      reley(activador); 
  } 
  if (sequedad >= seqRelt){
      activador = 3;
      voltajeSeq = true;
      reley(activador);  
  } else {
      activador = 4;
      voltajeSeq = false;
      reley(activador);
  }
  if(tdsValue >= clRelt){
    activador = 6; 
    voltajeCl = true;
    Serial.println("Agua contaminada, cambiar");
    reley(activador);
  } else {
    activador = 5;
    voltajeCl = false;
    reley(activador);
  }
}

void bluetooth (char letra,int tmp, int hum, int sequedad, int sequedad2, float tdsValue){
  
  if(mySerial.available()){
    Serial.write(mySerial.read());
  }
  if(Serial.available()){
    mySerial.write(Serial.read());
  }
  if(letra == 'a'){
    digitalWrite(rele, LOW);
    b = "true";
    consolaTemp(b);
    delay(10000);
  } else if (letra == 'A'){
    digitalWrite(rele, HIGH);
  } else if (letra == 'b'){
    digitalWrite(rele2, LOW);
    b = "true";
    consolaSoil(sequedad,b);
    delay(10000);
  } else if (letra == 'B'){
    digitalWrite(rele2, HIGH);
  } else if (letra == 'c'){
    digitalWrite(rele3, LOW);
    b = "true";
    consolaCloro(tdsValue,b);
    delay(10000);
  } else if (letra == 'C'){
    digitalWrite(rele3, HIGH);
  }
  mySerial.print("*T"+String(tmp)+"*");
  mySerial.print("*H"+String(hum)+"*");
  mySerial.print("*S"+String(sequedad)+"*");
  mySerial.print("*D"+String(sequedad2)+"*");
  mySerial.print("*C"+String(tdsValue)+"*");
}

