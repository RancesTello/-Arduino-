void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial){
    ;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n'); // Leer los datos hasta que se reciba una nueva línea
    data.trim(); // Eliminar cualquier espacio en blanco adicional

    // Analizar los datos en variables separadas
    int variable1 = data.substring(0, data.indexOf(",")).toInt();
    int variable2 = data.substring(data.indexOf(",") + 1, data.indexOf(";")).toInt();

    // Hacer algo con las variables capturadas, por ejemplo, imprimir en el monitor serial
    Serial.print("Variable 1: ");
    Serial.println(variable1);
    Serial.print("Variable 2: ");
    Serial.println(variable2);
  }
}
