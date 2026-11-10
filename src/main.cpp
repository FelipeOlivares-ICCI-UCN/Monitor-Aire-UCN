#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// PANTALLA OLED
#define ANCHO 128
#define ALTO 64
#define OLED_RESET -1

Adafruit_SSD1306 pantalla(ANCHO, ALTO, &Wire, OLED_RESET);

// PINES
const int SENSOR_MQ135 = A0;

const int LED_VERDE = 5;
const int LED_AMARILLO = 6;
const int LED_ROJO = 7;

const int RELE = 8;

// UMBRALES DE PRUEBA (LECTURA ADC)
const int LIMITE_VERDE = 300;
const int LIMITE_ROJO = 500;

// Tiempo de calentamiento inicial de demostracion
const unsigned long CALENTAMIENTO = 60000;

// Intervalo de lectura
const unsigned long INTERVALO = 1000;

unsigned long inicio;
unsigned long ultimaLectura = 0;

int lectura = 0;

enum Estado {
  NORMAL,
  ADVERTENCIA,
  CRITICO
};

Estado estadoActual = NORMAL;


// APAGAR INDICADORES
void apagarIndicadores() {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(RELE, LOW);
}


// PROMEDIAR LECTURAS DEL SENSOR
int leerSensor() {
  long suma = 0;

  for (int i = 0; i < 10; i++) {
    suma += analogRead(SENSOR_MQ135);
    delay(10);
  }

  return suma / 10;
}


// MOSTRAR INFORMACION EN OLED
void mostrarPantalla(int valor, Estado estado) {

  pantalla.clearDisplay();

  pantalla.setTextColor(SSD1306_WHITE);

  pantalla.setTextSize(1);
  pantalla.setCursor(0, 0);
  pantalla.println("MONITOR AIRE");

  pantalla.drawLine(0, 12, 128, 12, SSD1306_WHITE);

  pantalla.setTextSize(2);
  pantalla.setCursor(0, 19);

  pantalla.print("ADC: ");
  pantalla.println(valor);

  pantalla.setTextSize(1);
  pantalla.setCursor(0, 48);

  if (estado == NORMAL) {
    pantalla.println("ESTADO: NORMAL");
  }

  else if (estado == ADVERTENCIA) {
    pantalla.println("ESTADO: ALERTA");
  }

  else {
    pantalla.println("ESTADO: CRITICO");
  }

  pantalla.display();
}


// EVALUAR CALIDAD DEL AIRE
void evaluarAire(int valor) {

  if (valor < LIMITE_VERDE) {

    estadoActual = NORMAL;

    apagarIndicadores();

    digitalWrite(LED_VERDE, HIGH);

  }

  else if (valor <= LIMITE_ROJO) {

    estadoActual = ADVERTENCIA;

    apagarIndicadores();

    digitalWrite(LED_AMARILLO, HIGH);

  }

  else {

    estadoActual = CRITICO;

    apagarIndicadores();

    digitalWrite(LED_ROJO, HIGH);

    // Activar ventilacion
    digitalWrite(RELE, HIGH);

  }

}


// INICIO DEL SISTEMA
void setup() {

  Serial.begin(9600);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);

  pinMode(RELE, OUTPUT);

  pinMode(SENSOR_MQ135, INPUT);

  apagarIndicadores();

  // Iniciar OLED
  if (!pantalla.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error al iniciar OLED");

    while (true) {
      delay(100);
    }
  }

  pantalla.clearDisplay();
  pantalla.setTextSize(1);
  pantalla.setTextColor(SSD1306_WHITE);
  pantalla.setCursor(0, 15);

  pantalla.println("MONITOR AIRE");
  pantalla.println("Iniciando sistema...");
  pantalla.display();

  inicio = millis();

  Serial.println("MONITOR AIRE INICIADO");

}


// FUNCION PRINCIPAL
void loop() {

  // Calentamiento inicial
  if (millis() - inicio < CALENTAMIENTO) {

    pantalla.clearDisplay();
    pantalla.setTextSize(1);
    pantalla.setTextColor(SSD1306_WHITE);
    pantalla.setCursor(0, 15);

    pantalla.println("AIRSENSE");
    pantalla.println("Calentando sensor...");
    pantalla.println("Espere por favor");

    pantalla.display();

    return;
  }

  // Lectura cada segundo
  if (millis() - ultimaLectura >= INTERVALO) {

    ultimaLectura = millis();

    lectura = leerSensor();

    evaluarAire(lectura);

    mostrarPantalla(lectura, estadoActual);

    Serial.print("Lectura MQ135: ");
    Serial.print(lectura);

    Serial.print(" | Estado: ");

    if (estadoActual == NORMAL) {
      Serial.println("NORMAL");
    }

    else if (estadoActual == ADVERTENCIA) {
      Serial.println("ADVERTENCIA");
    }

    else {
      Serial.println("CRITICO - VENTILACION ACTIVADA");
    }

  }

}