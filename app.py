from flask import Flask, render_template, request
import serial

app = Flask(__name__)

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, bytesize=serial.EIGHTBITS,
                        parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=1,
                        xonxoff=False, rtscts=False, write_timeout=10, dsrdtr=False,
                        inter_byte_timeout=None, exclusive=None)


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/control_led', methods=['POST'])
def control_led():
    if 'brightness9' in request.form:
        brightness9 = request.form['brightness9']
        arduino.write(f'br_9_{brightness9}\n'.encode())
    if 'brightness10' in request.form:
        brightness10 = request.form['brightness10']
        arduino.write(f'br_10_{brightness10}\n'.encode())
    if 'brightness11' in request.form:
        brightness11 = request.form['brightness11']
        arduino.write(f'br_11_{brightness11}\n'.encode())

    return render_template('index.html')


# Ruta para encender y apagar el LED 13
@app.route('/toggle_led', methods=['POST'])
def toggle_led():
    action = request.form['action']
    if action == 'on' or action == 'off':
        arduino.write('13'.encode())

    return render_template('index.html')


# Ruta para mostrar la intensidad luminosa del LDR
@app.route('/ldr_intensity')
def ldr_intensity():
    arduino.write('bv'.encode())
    bright_intensity = arduino.readline().decode().strip()

    return render_template('index.html', ldr_intensity=bright_intensity)


if __name__ == '__main__':
    app.run(debug=True)
