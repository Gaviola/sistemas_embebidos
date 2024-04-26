#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

// Define los pines utilizados
#define LDR_PIN A0
#define LED_ALARM 12
#define LED_READING 11

//Semaforos y variables globales
SemaphoreHandle_t xSerialSemaphore;
bool suspended = false;

// Handle para suspender tareas
TaskHandle_t taskHandlerSerialOutput;
TaskHandle_t taskHandlerLedAlarm;
TaskHandle_t taskHandlerLedReading;


// Prototipos de funciones
void TaskLDR(void *pvParameters);
void TaskSerialOutput(void *pvParameters);
void TaskLEDAlarm(void *pvParameters);
void TaskLEDReading(void *pvParameters);

void setup() {
  Serial.begin(9600);
  pinMode(LDR_PIN, INPUT);
  pinMode(LED_ALARM, OUTPUT);
  pinMode(LED_READING, OUTPUT);

  // Crear semáforos
  xSerialSemaphore = xSemaphoreCreateMutex();

  // Crear tareas
  xTaskCreate(TaskSerialOutput, "Serial Output Task", 128, NULL, 2, &taskHandlerSerialOutput);
  xTaskCreate(TaskLEDAlarm, "LED Alarm Task", 128, NULL, 3, &taskHandlerLedAlarm);
  xTaskCreate(TaskLEDReading, "LED Reading Task", 128, NULL, 1, &taskHandlerLedReading);

  // La tarea que activa la alarma empieza suspendida
  vTaskSuspend(taskHandlerLedAlarm);

}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'b') { //'b' es el comando para simular el botón
      if (suspended) {
        vTaskResume(taskHandlerSerialOutput); //Reanuda las tareas de lectura y escritura de intensidad
        vTaskResume(taskHandlerLedReading);
        suspended = false;
      } else {
        //vTaskSuspend(taskHandlerLDR); //Suspende las tareas de lectura y escritura de intensidad
        vTaskSuspend(taskHandlerSerialOutput);
        vTaskSuspend(taskHandlerLedAlarm);
        vTaskSuspend(taskHandlerLedReading);
        digitalWrite(LED_READING, LOW);
        digitalWrite(LED_ALARM, LOW);
        suspended = true;
      }
    }
  }
}

void TaskSerialOutput(void *pvParameters) {
  while (true) {
    if (xSemaphoreTake(xSerialSemaphore, (TickType_t)10) == pdTRUE) {
      // Envía el valor del LDR por el puerto serial cada 3 segundos
      int ldrValue = analogRead(LDR_PIN);
      Serial.println(ldrValue);
      // Si el valor supera 800, activar la alarma
      if (ldrValue > 800) {
        vTaskResume(taskHandlerLedAlarm);
      }
      xSemaphoreGive(xSerialSemaphore);
    }

    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}


void TaskLEDAlarm(void *pvParameters) {
  while (true) {
    // Hacer parpadear el LED de la alarma cada 0.5 segundos si está activada
    if (digitalRead(LED_ALARM) == HIGH) {
      digitalWrite(LED_ALARM, LOW);
    } else {
      digitalWrite(LED_ALARM, HIGH);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void TaskLEDReading(void *pvParameters) {
  while (true) {
    // Hacer parpadear el LED de la lectura cada segundo si la lectura está activada
    if (digitalRead(LED_READING) == HIGH) {
      digitalWrite(LED_READING, LOW);
    } else {
      digitalWrite(LED_READING, HIGH);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

