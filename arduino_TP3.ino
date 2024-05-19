#include <Arduino.h>
#include <EEPROM.h>
#include <Arduino_FreeRTOS.h>
#include <task.h>


#define INTERRUPT_PIN_2 2
#define INTERRUPT_PIN_3 3
#define EEPROM_SIZE 256

void readSeconds();
void readEventsFromEEPROM();
void storeEventToEEPROM();
void clearEEPROM();
void writeSeconds(String newSecondsStr);


volatile unsigned long seconds = 0;

void vTaskClock(void *pvParameters) {
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(1000); // Ejecutar cada 1 segundo

  // Inicializar el tiempo de espera anterior con el tiempo actual
  xLastWakeTime = xTaskGetTickCount();

  while (true) {
    // Incrementar el contador de segundos
    seconds = seconds + 1;

    // Esperar hasta el próximo período de ejecución
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void handleInterrupt() {
  storeEventToEEPROM();
}

void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_2), handleInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN_3), handleInterrupt, FALLING);
  //Inicio de la tarea del reloj
  xTaskCreate(vTaskClock, "reloj", EEPROM_SIZE, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop() {
  if (Serial.available() > 0) {
    processSerialCommand();
  }
}

void processSerialCommand() {
  String command = Serial.readStringUntil('\n');
  command.trim();
  if (command.startsWith("w_")) {
    writeSeconds(command.substring(2));
  } else if (command == "t") {
    readSeconds();
  } else if (command == "s") {
    storeEventToEEPROM();
  } else if (command == "r") {
    readEventsFromEEPROM();
  } else if (command == "c") {
    clearEEPROM();
  }

}

void readSeconds() {
  Serial.println(seconds);
}

void storeEventToEEPROM() {
  //Leer ultimo indice y calcular el siguiente
  int lastIndex = EEPROM.read(0);
  int nextIndex;
  //Si el ultimo indice es 255, el siguiente es 1
  if (lastIndex == 255) {
    nextIndex = 1;
  } else {
    nextIndex = lastIndex + 1;
  }
  if (nextIndex >= EEPROM.length()) {
    nextIndex = sizeof(unsigned long);
  }
  //Escribir los datos
  int address = nextIndex * sizeof(seconds);
  EEPROM.put(address, seconds);
  EEPROM.write(0,nextIndex);

}

void readEventsFromEEPROM() {
  // Leer el último índice
  int lastIndex = EEPROM.read(0);
  if (lastIndex == 255) {
    // Si no hay datos almacenados, imprimir "0"
    Serial.println(0);
  } else{
    for (int i = 1; i <= lastIndex; i++) {
      // Leer el tiempo almacenado en la dirección i
      int address = i * sizeof(unsigned long);
      // Si la dirección es válida, leer el tiempo almacenado
      if (address < EEPROM_SIZE) {
        unsigned long storedTime;
        EEPROM.get(address, storedTime);
        Serial.println(storedTime);
      }
    }
  }
}

void clearEEPROM() {
  // Leer el último índice
  int lastIndex = EEPROM.read(0);
  int address;
  // Borrar todos los datos almacenados en la EEPROM
  for (int i=0; i<=lastIndex; i++) {
    address = (i-1) * sizeof(unsigned long);
    for (int j=0; j<sizeof(unsigned long); j++) {
      EEPROM.write(address + j, 0);
    }
  }
  // Reiniciar el índice
  EEPROM.write(0,0);
}

void writeSeconds(String newSecondsStr) {
  // Convertir el valor de la cadena a un número entero y guardarlo
  unsigned long newSeconds = newSecondsStr.toInt();
  taskENTER_CRITICAL();
  seconds = newSeconds;
  taskEXIT_CRITICAL();
}
