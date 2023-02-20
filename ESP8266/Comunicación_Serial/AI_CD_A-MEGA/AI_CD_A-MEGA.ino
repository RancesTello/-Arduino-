
int seqSensor = 0;
int seqState = 0;

int luzSensor = 0;
int luzState = 0;

int luzSensor2 = 0;
int luzState2 = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println(char(169));
  Serial.println("¡kimb!");
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  seqSensor = analogRead(A0);
  seqState = (seqSensor*100)/1000;

  luzSensor = analogRead(A1);
  luzState = (luzSensor*100)/1000;

  luzSensor2 = analogRead(A2);
  luzState2 = (luzSensor2*100)/1000;

  Serial.print("Sequedad:  ");
  Serial.print(seqState);
  Serial.println("%");

  Serial.print("Luz:   ");
  Serial.print(luzState);
  Serial.println("%");
  
  Serial.print("Luz 2:   ");
  Serial.print(luzState2);
  Serial.println("%");
  /*if(seq >= 10){

    seq = int(seq / 10); // conviertes el float a int, en este momento puede haber valores del 1 al 10

    seq = seq - 1; // ahora son de 0 a 9 
  } else {
    seq = 0;
  }*/
  
  // Construir una cadena de caracteres que contiene los datos a enviar
  String data = String(seqState) + "," + String(luzState) + "," + String(luzState2) + ";\n";

  // Enviar los datos al ESP32
  Serial1.print(data);
  delay(1000);
}
