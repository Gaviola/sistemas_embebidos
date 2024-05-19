from flask import Flask, render_template, request
import serial
import time

app = Flask(__name__)

ser = serial.Serial(port='/dev/ttyACM1', baudrate=9600, bytesize=serial.EIGHTBITS,
                        parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=1,
                        xonxoff=False, rtscts=False, write_timeout=10, dsrdtr=False,
                        inter_byte_timeout=None, exclusive=None)
@app.route('/')
def home():
    return render_template('index.html')

@app.route('/read_time', methods=['GET'])
def read_time():
    ser.write(b't')
    time.sleep(1)
    seconds = ser.readline().decode().strip()
    return convert_seconds_to_time(seconds)

@app.route('/set_time', methods=['POST'])
def set_time():
    new_time = request.form.get('new_time')
    seconds = convert_time_to_seconds(new_time)
    ser.write(b"w_" + bytes(str(seconds), 'utf-8'))
    return 'OK'

@app.route('/store_time', methods=['POST'])
def store_time():
    ser.write(b's')
    return 'OK'

@app.route('/read_stored_times', methods=['GET'])
def read_stored_times():
    ser.write(b'r')
    time.sleep(1)
    stored_times = ser.readlines()
    return [convert_seconds_to_time(time.decode().strip()) for time in stored_times]

@app.route('/clear_times', methods=['POST'])
def clear_times():
    ser.write(b'c')
    return 'OK'

def convert_seconds_to_time(seconds):
    seconds = int(seconds)
    minutes, seconds = divmod(seconds, 60)
    hours, minutes = divmod(minutes, 60)
    days, hours = divmod(hours, 24)
    months, days = divmod(days, 30)
    years, months = divmod(months, 12)
    return f'{years:02d}:{months:02d}:{days:02d}:{hours:02d}:{minutes:02d}:{seconds:02d}'

def convert_time_to_seconds(time_str):
    years, months, days, hours, minutes, seconds = map(int, time_str.split(':'))
    return (((((years * 12 + months) * 30 + days) * 24 + hours) * 60 + minutes) * 60 + seconds)

if __name__ == '__main__':
    app.run(debug=True)