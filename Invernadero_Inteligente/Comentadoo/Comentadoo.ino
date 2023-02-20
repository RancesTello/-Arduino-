// DECLARACION DE LIBRERIAS
#include <dht.h> //Libreria del sensor de temperatura
#include <LiquidCrystal.h> //Libreria de la pantalla lcd
#include <EEPROM.h>
#include <GravityTDS.h> // Libreria para el funcionamiento del sensor de Cloro

// DEFINICIONES
#define COLS 16 // se define cuantos cuadrados tiene la pantalla lcd por fila
#define ROWS 2 // Se define el número de filas
#define DHT11_PIN 4 // El sensor de temperatura usa el pin 4
#define TdsSensorPin A2 // Se define que pin análogo usa el sensor de cloro

// VARIABLES

// Variables del sensor de temperatura
dht DHT;
int tmp;
int hum;
int calorRelt = 25.00;
bool voltajeTemp;

// Variables del sensor de resequedad
String b;
int activador;
bool voltajeSeq;
bool voltajeSeq2;

int sequedad;
int sequedad2;

int seqRelt = 50;

// Variables que serán usados para el sensor de cloro
float temperature = 25, tdsValue = 0;
bool voltajeCl;
float clRelt = 500;

// Pines de los botones
const int boton1 = 2;   
const int boton2 = 3;
const int boton3 = 13;
const int boton4 = 45;

int botonEstado1 = 0;
int botonEstado2 = 0;
int botonEstado3 = 0;
int botonEstado4 = 0;

// Pines de los relés
int rele = 11;
int rele2 = 12;
int rele3 = 47;

//Pines usados por la pantalla lcd
LiquidCrystal lcd(5,6,7,8,9,10); 

void setup() {

  Serial.begin(9600);
  // Se declara que los relés serán OUTPUT = Salida de datos
  pinMode(rele, OUTPUT); 
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
  // Se declara que los botones se´ran entradas de datos
  pinMode(boton1, INPUT);
  pinMode(boton2, INPUT);
  pinMode(boton3, INPUT);
  pinMode(boton4, INPUT);
  // Se configura el pin, referencia de voltaje, rango, etc del sensor de cloro
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0); // voltaje de referencia en ADC, por defecto 5.0V en Arduino UNO
  gravityTds.setAdcRange(1024);// //1024 para ADC de 10 bits; 4096 para ADC de 12 bits
  gravityTds.begin(); // Inicialización
  // Cuantas columnas y filas tiene a disposición la pantalla LCD
  lcd.begin(COLS, ROWS);
  //Pequeño mensaje de inicio en la pantalla LCD
  lcd.setCursor(0,0);
  lcd.print("Colegio Rafael");
  lcd.setCursor(0,1);
  lcd.print("Quintero V.");

}

void loop() {
  // Se hace la lectura del estado de los botones, ya sea LOW o HIGH
  botonEstado1 = digitalRead(boton1);
  botonEstado2 = digitalRead(boton2);
  botonEstado3 = digitalRead(boton3);
  botonEstado4 = digitalRead(boton4);
  
  // Se hace lectura del valor que de el sensor de resequedad
  float sensorVal = analogRead(A0);
  // Regla de 3 para convertir el valor a porcentaje
  sequedad = (sensorVal * 100)/1023;
  
  // se establece la temperatura y se ejecuta la compensación de temperatura
  gravityTds.setTemperature(temperature);
  gravityTds.update(); // muestra y calcula
  tdsValue = gravityTds.getTdsValue(); // Obtiene el valor de la operación

  // Se lee el valor del pin de temperatura
  DHT.read11(DHT11_PIN);

  // Condicional.. Si el calor del sensor de temperatura es mayor o igual a uno relativo.. pasa el siguiente código..
  if(DHT.temperature >= calorRelt){
      activador = 1; // Una variable será 1 y será la que indicará que relé se activará o apagará
      voltajeTemp = true; // Una variable bool para mostrar en la pantalla LCD si el relé está encendido en la pantalla LCD
      reley(activador);  // Se hace el llamado a la función relé con la variable activador con el número del case que se debe activar
  } else {
      activador = 2;
      voltajeTemp = false;
      reley(activador); 
  }

  // Si la resequedad de la tierra es mayor o igual a una resequedad relativa.. pasará lo siguiente...
  if (sequedad >= seqRelt){
      activador = 3;
      voltajeSeq = true;
      reley(activador);  
  } else {
      activador = 4;
      voltajeSeq = false;
      reley(activador);
  }

  // Si el nivel de Cloro es muy alto... el relé se apaga y hace que el motor que envía agua se detenga
  if(tdsValue >= clRelt){
    activador = 6; 
    voltajeCl = true;
    reley(activador);
  } else { // Si el nivel de cloro es bajo o nulo.. el relé junto al motor seguirá en funcionamiento
    activador = 5;
    voltajeCl = false;
    reley(activador);
  }

  // Condicionales de los botones que si están presionados / High, harán x cosa...
  if(botonEstado1 == HIGH) tempLCD(voltajeTemp); // Se hace el llamado a la función del LCD de temperatura con la variable booleana explicada anteriormente 
  if(botonEstado2 == HIGH) soilLCD(voltajeSeq,sequedad); // Se hace el llamado a la función del LCD del sensor del suelo con la variable booleana anteriormente explicada y el resultado de la operación para que sea un número entero
  
  if(botonEstado3 == HIGH) {
    // Se lee el valor en una variable de decimales del sensor del suelo que usa el pin Analogo A1
    float sensorVal2 = analogRead(A1);
    // Se hace la respectiva operación para pasar del numero decimal a un entero de 1 a 100
    sequedad2 = (sensorVal2 * 100)/1023;
    // Se hace llamado a la funcion del LCD del sensor del suelo con la variable booleana explicada anteriormente y la variable con el resultado de la operación
    soilLCD(voltajeSeq2,sequedad2);
    // Condicional; Si el valor en entero del sensor en menor a una sequedad relativa.. pasará lo siguiente...
    if(sequedad2 <= seqRelt) {
      digitalWrite(rele2, LOW);// Se le dice al relé 2 / Relé del sensor del suelo que deje pasar energía / que se active 
      Serial.println("--!la tierra está muy seca, motor encendido!--"); // En consola dirá el mensaje
      // En la pantalla lcd dirá el siguiente mensaje durante 9s
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("La tierra esta");
      lcd.setCursor(0,1);
      lcd.print("siendo regada");
    }
    else digitalWrite(rele2, HIGH); // Si la condición no se cumple, se le dice al relé que se apague / que ya no deje pasar energía
    delay(9000); // Una espera de 9 segundos para la demostración
    lcd.clear(); // Se limpia la pantalla lcd
    soilLCD(voltajeSeq2,sequedad2); // Se llamará la función para que muestre los valores en la pantalla
  } else { // Si el boton está apagado / LOW
    digitalWrite(rele2, HIGH); // Se le dice al relé del sensor del suelo que se mantenga apagado / que se apague
  }
  if(botonEstado4 == HIGH) { // Si el boton 4 fue presionado pasará lo siguiente..
    cloroLCD(tdsValue,voltajeCl); // Se llamrá a la función LCD donde mostrará los valores del cloro
  }
  // Espera de 3s para volver al comienzo del loop, actualizar las lecturas y volver a hacer las comprobaciones 
  delay(3000);
}

// Función que maneja los relés
void reley (int a){ // Acepta la variable llamada activador con el número que indica que relé se activa y si se enciende y apaga
  // rele = Relé manejado por en sensor de temperatura DHT11
  // rele2 = Relé manejado por el sensor de humedad de la tierra
  switch(a){
    case 1: digitalWrite(rele, LOW); // Indica que el relé se encienda
            break;
    case 2: digitalWrite(rele, HIGH); // Indica que el relé se apague
            break;
    case 3: digitalWrite(rele2, LOW); // Indica que el relé se encienda
            break;
    case 4: digitalWrite(rele2, HIGH); // Indica que el relé se apague
            break;
  } 
}

// Función de la pantalla LCD con los grados del sensor de temperatura
void tempLCD(bool a){ // La función acepta la variable booleana que indicá si está pasando energía el relé o no
  tmp = DHT.temperature; // La variable global tmp ahora tendrá como valor el grado de calor del sensor de temperatura
  hum = DHT.humidity; // La variable global hm ahora tendrá como valor el grado de humedad del sensor de humedad
  a ? b = "true" : b = "false"; // Operador ternario... Si la variable booleana tiene el valor 'true' entonces la variable global tipo string tendra como palabra: true y si la variable booleana es 'false', la variable string tendra como palabra: false

  lcd.clear(); // Se limpia la pantalla LCD por si hay un mensaje antes 
  lcd.setCursor(0,0); // Se seleciona la fila superior de la pantalla LCD
  // Se imprime en la pantalla el calor y humedad con sus respectivos grados
  lcd.print("C:");
  lcd.print(tmp);
  lcd.print("C      H:");
  lcd.print(hum);
  lcd.print("%");
  // Se selecciona la fila inferior de la pantalla LCD
  lcd.setCursor(0,1);
  // Se imprime si está pasando energía o no, según el operador ternario es false o true
  lcd.print("Energia: ");
  lcd.print(b);
  // Se llama la función de la consola junto a la variable string que dice si está pasando la energía o no
  consolaTemp(b);
}

// Función del LCD del sensor de la sequedad de la tierra
void soilLCD(bool a,int porcentaje){
    a ? b = "true" : b = "false";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sequedad: ");
  lcd.print(porcentaje);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Energia: ");
  lcd.print(b);
  // Se llama a la función de la consola con la variable del porcentaje de sequedad de la tierra
  consolaSoil(porcentaje);
}
// Función donde en la pantalla lcd mostrará los niveles de Cloro, comprar valores con la siguiente tabla: https://how2electronics.com/wp-content/uploads/2020/03/TDS-Reading.jpg
void cloroLCD(float tds, bool a){
  a ? b = "true" : b = "false";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Cloro: ");
  lcd.print(tds);
  lcd.setCursor(0,1);
  lcd.print("Energia: ");
  lcd.print(b);
  // Se llama a la función de consola donde mostrará los valores del sensor de cloro
  consolaCloro(tds,b);
}

// Función de consola del sensor de temperatura
void consolaTemp(String b){

  // Imprime en consola la temperatura, humedad y si está pasando energía o no
  Serial.println("╔─────────────╗"); // Simple margen de decoración
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

// Función de la consola del sensor de sequedad de la tierra
void consolaSoil(int b){ // La variable 'b' contiene el porcentaje del sensor de sequedad de la tierra mandado en soilLCD
  // Se imprime la sequedad del suelo
  Serial.println("╔─────────────╗");
  Serial.print("  Sequedad del suelo: "); 
  Serial.print(b);
  Serial.println(" %");
  if(b >= 70){ // Si el porcentaje del sensor es mayor o igual a 70.. se imprimirá en consola lo siguiente...
    Serial.println("La tierra está muy seca");
  } else if ( b <= 30) { // Si el porcentaje del sensor es menor o igual a 30.. se imprimirá en consola lo siguiente...
    Serial.println("La tierra está muy humeda");
  }
  Serial.println("╚─────────────╝");
  Serial.println(" ");
}

// Función que mostrará en consola / monitor de serie los valores del sensor de cloro y si el relé está pasando energía o no
void consolaCloro(float tds, String b){
  Serial.println("╔─────────────╗");
  Serial.print("  Cloro: "); 
  Serial.print(tds);
  Serial.print("  Voltaje: ");
  Serial.println(b);
  Serial.println("╚─────────────╝");
  Serial.println(" ");
}
