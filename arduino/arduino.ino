const int led9 = 9;
const int led10 = 10;
const int led11 = 11;
const int led13 = 13;
const int ldrPin = A3;

void setup() {
  Serial.begin(9600, SERIAL_8N1);
  pinMode(led9, OUTPUT);
  pinMode(led10, OUTPUT);
  pinMode(led11, OUTPUT);
  pinMode(led13, OUTPUT);
  pinMode(ldrPin, INPUT);
}

void loop() {
  String instruction;
  if (Serial.available() > 0) {
    instruction = Serial.readStringUntil('\n');
  }
  String bright = instruction.substring(0, instruction.indexOf('_'));
  String ins = instruction.substring(instruction.indexOf('_') + 1);
  if (bright == "br") {
    set_bright(ins);  // EJ cambio de brillo de led9 a 200: br_9_200
  }
  else if (instruction == "bv") {
    int vOut = analogRead(ldrPin); // Leer el valor analógico del LDR
    float rLDR = ((10*(10^3)*5)/ vOut) - 10*(10^3);
    float intensidad = (1*(10^6))/rLDR;
    Serial.print("Intensidad luminosa captada por el LDR: ");
    Serial.println(intensidad); // Mostrar el valor por el puerto serial
  }
  else {
    turn_on_off_led(ins);
  }

}

void set_bright(String ins) {
  String led = ins.substring(0, ins.indexOf('_'));
  String intensity = ins.substring(ins.indexOf('_') + 1);
  
  if (led == "9") {
    int ins = intensity.toInt(); // Leemos el valor de brillo enviado
    Serial.println(ins);
    analogWrite(led9, ins); // Establecemos el brillo del LED
    Serial.println(intensity);
  }
  else if (led == "10") {
    int ins = intensity.toInt();
    analogWrite(led10, ins); 
    Serial.println(intensity);
  }
  else if (led == "11") {
    int ins = intensity.toInt();
    analogWrite(led11, ins); 
    Serial.println(intensity);
  }
}

void turn_on_off_led(String led) {
  if (led == "9") {
    if (digitalRead(led9) == 0) {
      digitalWrite(led9, HIGH);
    } else {
      digitalWrite(led9, LOW);
    }
  }
  if (led == "10") {
    if (digitalRead(led10) == 0) {
      digitalWrite(led10, HIGH);
    } else {
      digitalWrite(led10, LOW);
    }
  }
  if (led == "11") {
    if (digitalRead(led11) == 0) {
      digitalWrite(led11, HIGH);
    } else {
      digitalWrite(led11, LOW);
    }
  }
  if (led == "13") {
    if (digitalRead(led13) == 0) {
      digitalWrite(led13, HIGH);
    } else {
      digitalWrite(led13, LOW);
    }
  }
}
