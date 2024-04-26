from flask import Flask, render_template, jsonify
import serial
import threading
import time

app = Flask(__name__)
arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, bytesize=serial.EIGHTBITS,
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
        reading_thread = threading.Thread(target=read_from_arduino)
        reading_thread.start()
    else:
        if reading_thread is not None:
            reading_thread.join()
    return 'Lectura iniciada' if reading else 'Lectura detenida'


@app.route('/get_reading')
def get_reading():
    return jsonify({'reading': last_reading})


if __name__ == '__main__':
    app.run(debug=True)