const int trigPin = 9;
const int echoPin = 10;
const int ledPin = 13;  // Pin del LED de alarma
const int thresholdDistance = 20;  // Distancia umbral en cm

long duration;
int distance;

void setup() {
  // Inicializa el pin de trig como salida:
  pinMode(trigPin, OUTPUT);
  // Inicializa el pin de echo como entrada:
  pinMode(echoPin, INPUT);
  // Inicializa el pin del LED como salida:
  pinMode(ledPin, OUTPUT);
  // Inicializa la comunicación serie a 9600 bps:
  Serial.begin(9600);
}

void loop() {
  // Limpia el pin de trig:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Envía un pulso de 10 microsegundos:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Lee el tiempo que tarda en recibir el eco:
  duration = pulseIn(echoPin, HIGH);

  // Calcula la distancia (duración / 2) / 29.1:
  distance = duration * 0.034 / 2;

  // Comprueba si la distancia es menor o igual al umbral:
  if (distance <= thresholdDistance) {
    // Enciende el LED de alarma:
    digitalWrite(ledPin, HIGH);
    Serial.print(distance);
  } else {
    // Apaga el LED de alarma:
    digitalWrite(ledPin, LOW);
    Serial.print(distance);
  }

  // Espera 500 ms antes de la próxima medición:
  delay(500);
}