#include <dht.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <GravityTDS.h>

GravityTDS gravityTds;

#define TdsSensorPin A2
#define COLS 16
#define ROWS 2
#define DHT11_PIN 4
dht DHT;

int calorRelt = 28.00;
int seqRelt = 50;
float clRelt = 500;

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

const int boton1 = 2;   
const int boton2 = 3;
const int boton3 = 13;
const int boton4 = 45;

int botonEstado1 = 0;
int botonEstado2 = 0;
int botonEstado3 = 0;
int botonEstado4 = 0;

int rele = 51;
int rele2 = 49;
int rele3 = 47;

LiquidCrystal lcd(5,6,7,8,9,10); 

void setup() {

  Serial.begin(115200);
  pinMode(rele, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
  pinMode(boton1, INPUT);
  pinMode(boton2, INPUT);
  pinMode(boton3, INPUT);
  pinMode(boton4, INPUT);
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);
  gravityTds.setAdcRange(1024);
  gravityTds.begin();
  lcd.begin(COLS, ROWS); 
  lcd.setCursor(0,0);
  lcd.print("Colegio Rafael");
  lcd.setCursor(0,1);
  lcd.print("Quintero V.");

}

void loop() {
  botonEstado1 = digitalRead(boton1);
  botonEstado2 = digitalRead(boton2);
  botonEstado3 = digitalRead(boton3);
  botonEstado4 = digitalRead(boton4);
  float sensorVal = analogRead(A0);
  sequedad = (sensorVal * 100)/1023;
  gravityTds.setTemperature(temperature);
  gravityTds.update();
  tdsValue = gravityTds.getTdsValue();
  
  DHT.read11(DHT11_PIN);
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
  
  if(botonEstado1 == HIGH) tempLCD(voltajeTemp);
  if(botonEstado2 == HIGH) soilLCD(voltajeSeq,sequedad); 
  if(botonEstado3 == HIGH) {
    float sensorVal2 = analogRead(A1);
    sequedad2 = (sensorVal2 * 100)/1023;
    soilLCD(voltajeSeq2,sequedad2);
    if(sequedad2 >= seqRelt) {
      digitalWrite(rele2, LOW);
      Serial.println("--!la tierra está muy seca, motor encendido!--");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("La tierra esta");
      lcd.setCursor(0,1);
      lcd.print("siendo regada");
    }
    else digitalWrite(rele2, HIGH);
    delay(9000);
    lcd.clear();
    soilLCD(voltajeSeq2,sequedad2);
  } else {
    digitalWrite(rele2, HIGH);
  }
  if(botonEstado4 == HIGH) {
    cloroLCD(tdsValue,voltajeCl);
  }
  delay(3000);
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

void tempLCD(bool a){
  tmp = DHT.temperature;
  hum = DHT.humidity;
  a ? b = "true" : b = "false";

  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("C:");
  lcd.print(tmp);
  lcd.print("C      H:");
  lcd.print(hum);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Energia: ");
  lcd.print(b);
  consolaTemp(b);
}

void soilLCD(bool a,int porcentaje){
  a ? b = "true" : b = "false";
  if(a == true) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("La tierra esta");
    lcd.setCursor(0,1);
    lcd.print("siendo regada");
    delay(4000);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sequedad: ");
  lcd.print(porcentaje);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Energia: ");
  lcd.print(b);
  
  consolaSoil(porcentaje);
}

void cloroLCD(float tds, bool a){
  a ? b = "false" : b = "true";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Cloro: ");
  lcd.print(tds);
  lcd.print("ppm");
  lcd.setCursor(0,1);
  lcd.print("Energia: ");
  lcd.print(b);
  consolaCloro(tds,b);
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

void consolaSoil(int b){
  Serial.println("╔─────────────╗");
  Serial.print("  Humedad del suelo: "); 
  Serial.print(b);
  Serial.println(" %");
  if(b >= 70){
    Serial.println("La tierra está muy seca");
  } else if ( b <= 30) {
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
