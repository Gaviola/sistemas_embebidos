from flask import Flask, render_template, jsonify
import serial
import threading
import time

app = Flask(__name__)
arduino = serial.Serial(port='/dev/ttyACM1', baudrate=9600, bytesize=serial.EIGHTBITS,
                        parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=1,
                        xonxoff=False, rtscts=False, write_timeout=10, dsrdtr=False,
                        inter_byte_timeout=None, exclusive=None)

reading = False
reading_thread = None
last_reading = None


def read_from_arduino():
    global last_reading
    while reading:
        if arduino.in_waiting > 0:
            last_reading = arduino.readline().decode().strip()
        time.sleep(1)


@app.route('/')
def index():
    return render_template('tp2_index.html')


@app.route('/toggle_reading')
def toggle_reading():
    global reading, reading_thread
    reading = not reading
    if reading:
        arduino.write('b'.encode())  # El comando b inicia/termina las lectura
        reading_thread = threading.Thread(target=read_from_arduino)
        reading_thread.start()
        return 'Lectura Iniciada'
    else:
        if reading_thread is not None:
            arduino.write('b'.encode())
            reading_thread.join()
            return 'Lectura Detenida'


@app.route('/get_reading')
def get_reading():
    return jsonify({'reading': last_reading})


if __name__ == '__main__':
    app.run(debug=True)