#include <OneWire.h>

// Configuração dos pinos
int SensorPressaoPin = A0; // Pino analógico para o Sensor Pressão
int releVentoinhaPin = 7;   // Pino digital para o Relé 1/Ventoinha
int releCompressorPin = 8;   // Pino digital para o Relé 2/Compressor

OneWire ds(10);  // on pin 10 (a 4.7K resistor is necessary)

bool compressorDesligadoPorTemperatura = false;

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
  Serial.println("Sistema iniciado. Monitorando sensores...");
}

void loop() {
  // Lê os valores dos sensores
  int sensorPressaoValue = analogRead(SensorPressaoPin); // Valor do Sensor de Pressão (0 a 1023)

  // Sensor Temperatura
  byte data[9];
  byte addr[8];
  float celsius;

  if (!ds.search(addr)) {
    ds.reset_search();
    delay(250);
    Serial.println("Nenhum sensor de temperatura detectado.");
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
  float voltagePressao = sensorPressaoValue * (5.0 / 1023.0);
  float pressaoPSI = voltagePressao * 100; // Supondo que 0-5V representa 0-500 PSI
  float pressaoBar = pressaoPSI * 0.0689476; // Conversão de PSI para bar

  Serial.println("------------------------");
  Serial.print("Temperature: ");
  Serial.print(celsius);
  Serial.println(" °C");

  Serial.print("Pressão: ");
  Serial.print(pressaoBar);
  Serial.println(" bar");

  // Condições para acionar os relés
  if (celsius <= 3) {
    compressorDesligadoPorTemperatura = true;
  } else if (celsius >= 10) {
    compressorDesligadoPorTemperatura = false;
  }

  if (pressaoBar > 2 && pressaoBar < 30 && celsius > 1 && !compressorDesligadoPorTemperatura) {
    digitalWrite(releVentoinhaPin, HIGH);
    digitalWrite(releCompressorPin, HIGH);
    Serial.println("Relés ATIVADOS: Ventoinha e Compressor ligados.");
  } else {
    digitalWrite(releVentoinhaPin, LOW);
    digitalWrite(releCompressorPin, LOW);
    Serial.println("Relés DESATIVADOS: Ventoinha e Compressor desligados.");
  }

  // Aguarda 500ms antes de repetir
  delay(500);
}
