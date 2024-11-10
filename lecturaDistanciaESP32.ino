#include <WiFi.h>
#include <HTTPClient.h>

// Definir los pines para el TRIG y el ECHO del HC-SR04
const int trigPin = 5;  // Pin TRIG conectado al pin 5 del ESP32
const int echoPin = 18; // Pin ECHO conectado al pin 18 del ESP32

// Variable para almacenar la duración y la distancia
long duration;
float distance;
float sum = 0;             // Suma de las distancias
const int numReadings = 10; // Número de lecturas a tomar
int readingCount = 0;      // Contador de lecturas

// URL del Google Apps Script (reemplaza con tu URL)
const char* scriptURL = "url"; // Cambia esta URL por la de tu script

void setup() {
  // Inicializar el puerto serie para ver la distancia
  Serial.begin(115200);

  // Configurar los pines TRIG y ECHO
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Conexión a la red WiFi
  WiFi.begin("nombreRed", "contraseña"); // Cambia por tu SSID y contraseña
  Serial.print("Conectando...");
  
  // Esperar conexión
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado a la red WiFi");
}

void loop() {
  // Asegurarse de que el pin TRIG esté en LOW al principio
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Enviar un pulso de 10 microsegundos en TRIG
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Leer el tiempo de respuesta en ECHO
  duration = pulseIn(echoPin, HIGH);

  // Calcular la distancia (duración / 2 porque el pulso viaja de ida y vuelta)
  distance = duration * 0.0343 / 2;

  // Imprimir la distancia en el puerto serie
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Sumar la distancia para el cálculo del promedio
  sum += distance;
  readingCount++;

  // Comprobar si se han tomado 10 lecturas
  if (readingCount >= numReadings) {
    // Calcular la distancia promedio
    float averageDistance = sum / numReadings;
    Serial.print("Distancia promedio: ");
    Serial.print(averageDistance);
    Serial.println(" cm");

    // Reiniciar contadores
    sum = 0;
    readingCount = 0;

    // Enviar el promedio a Google Sheets
    sendToGoogleSheets(averageDistance);
  }

  // Esperar 6 segundos antes de la siguiente medición
  delay(6000); 
}

// Función para enviar datos a Google Sheets
void sendToGoogleSheets(float distance) {
  // Crear una consulta HTTP
  String url = String(scriptURL) + "?distance=" + distance;

  // Conexión WiFi
  HTTPClient http;
  http.begin(url);
  
  // Realizar la solicitud GET
  int httpResponseCode = http.GET();
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Respuesta de Google Sheets: " + response);
  } else {
    Serial.print("Error al conectar a Google Script: ");
    Serial.println(httpResponseCode);
  }

  http.end(); // Cierra la conexión
}
