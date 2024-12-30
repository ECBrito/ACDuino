#include <OneWire.h>

// Configuração dos pinos
int SensorPressaoPin = A0; // Pino analógico para o Sensor Pressão
int releVentoinhaPin = 7;   // Pino digital para o Relé 1/Ventoinha
int releCompressorPin = 8;   // Pino digital para o Relé 2/Compressor

OneWire ds(10);  // on pin 10 (a 4.7K resistor is necessary)



void setup() {
  // Configura os pinos dos sensores como entrada
  pinMode(releVentoinhaPin, INPUT);
  pinMode(releCompressorPin, INPUT);

  // Configura os pinos dos relés como saída
  pinMode(releVentoinhaPin, OUTPUT);
  pinMode(releCompressorPin, OUTPUT);

  // Inicializa os relés desligados
  digitalWrite(releVentoinhaPin, LOW);
  digitalWrite(releCompressorPin, LOW);

  // Inicializa o monitor serial
  Serial.begin(9600);
}

void loop() {

  // Lê os valores dos sensores
  int sensor1Value = analogRead(releVentoinhaPin); // Valor do Sensor 1 (0 a 1023)

  // Sensor Temperatura
  byte data[9];
  byte addr[8];
  float celsius;

  if (!ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion
  delay(750); // wait for conversion

  ds.reset();
  ds.select(addr);
  ds.write(0xBE); // read scratchpad

  for (int i = 0; i < 9; i++) {
    data[i] = ds.read();
  }

  int16_t raw = (data[1] << 8) | data[0];
  celsius = (float)raw / 16.0;


  // Converte os valores para tensão (0 a 5V)
  float voltage1 = sensor1Value * (5.0 / 1023.0);

  Serial.print("Temperature: ");
  Serial.print(celsius);
  Serial.println(" °C");

  // Mostra os valores no monitor serial
  Serial.print("Sensor 1: ");
  Serial.print(voltage1);
  Serial.println(" V");


  // Exemplo de condições para acionar os relés
  if (voltage1 > 2.5) { // Liga o Relé 1 se a tensão do Sensor 1 for maior que 2.5V
    digitalWrite(releVentoinhaPin, HIGH);
  } else {
    digitalWrite(releVentoinhaPin, LOW);
  }



  // Aguarda 500ms antes de repetir
  delay(500);
}
