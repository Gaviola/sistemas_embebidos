from flask import Flask, render_template, jsonify
import serial
import threading
import time

app = Flask(__name__)

# Configuración del puerto serial (ajusta 'COM3' según tu puerto)
ser = serial.Serial('COM3', 9600, timeout=1)

# Variable global para almacenar la distancia
distance_data = {
    "distance": 0,
    "alert": False
}

# Distancia umbral en cm
threshold_distance = 10

def read_distance():
    global distance_data
    while True:
        if ser.in_waiting > 0:
            try:
                distance = int(ser.readline().decode('utf-8').strip())
                distance_data["distance"] = distance
                distance_data["alert"] = distance <= threshold_distance
            except ValueError:
                pass
        time.sleep(0.1)

# Ruta principal para la página web
@app.route('/')
def index():
    return render_template('Integrador_index.html')

# Ruta para obtener los datos de distancia
@app.route('/distance')
def distance():
    return jsonify(distance_data)

if __name__ == '__main__':
    # Iniciar el hilo para leer la distancia desde el puerto serial
    thread = threading.Thread(target=read_distance)
    thread.daemon = True
    thread.start()

    # Iniciar la aplicación Flask
    app.run(debug=True)
