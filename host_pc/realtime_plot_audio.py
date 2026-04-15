import sys
import time
import threading
from collections import deque

import numpy as np
import serial
import pyqtgraph as pg
from PyQt6 import QtWidgets, QtCore
import sounddevice as sd  # type: ignore


# =========================
# CONFIG
# =========================
SERIAL_PORT = "/dev/ttyACM0"
BAUDRATE = 115200
PLOT_SAMPLES = 500
AUDIO_SR = 44100
AUDIO_BLOCKSIZE = 256

ANALOG_MIN = 0
ANALOG_MAX = 4095


# =========================
# SHARED DATA
# =========================
analog_buffer = deque([0] * PLOT_SAMPLES, maxlen=PLOT_SAMPLES)
digital_buffer = deque([0] * PLOT_SAMPLES, maxlen=PLOT_SAMPLES)

latest_analog = 0
latest_digital = 0

data_lock = threading.Lock()
running = True
phase = 0.0


# =========================
# SERIAL OPEN
# =========================
def open_serial():
    ser = serial.Serial(
        port=SERIAL_PORT,
        baudrate=BAUDRATE,
        timeout=1,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        xonxoff=False,
        rtscts=False,
        dsrdtr=False,
    )

    # Evita comportamenti strani all'apertura della porta
    try:
        ser.setDTR(False)
        ser.setRTS(False)
    except Exception:
        pass

    # Aspetta che la porta si stabilizzi e pulisci il buffer
    time.sleep(1.0)
    try:
        ser.reset_input_buffer()
    except Exception:
        pass

    return ser


# =========================
# SERIAL THREAD
# =========================
def serial_worker():
    global latest_analog, latest_digital, running

    try:
        ser = open_serial()
        print(f"Opened serial port {SERIAL_PORT} at {BAUDRATE} baud")
    except Exception as e:
        print(f"Failed to open serial port: {e}")
        running = False
        return

    while running:
        try:
            raw = ser.readline()
            if not raw:
                continue

            line = raw.decode("utf-8", errors="ignore").strip()
            if not line:
                continue

            # formato atteso: "1234\t1"
            parts = line.split("\t")
            if len(parts) != 2:
                continue

            analog = int(parts[0].strip())
            digital = int(parts[1].strip())

            # clamp / sanitize
            if analog < ANALOG_MIN:
                analog = ANALOG_MIN
            elif analog > ANALOG_MAX:
                analog = ANALOG_MAX

            digital = 1 if digital != 0 else 0

            with data_lock:
                latest_analog = analog
                latest_digital = digital
                analog_buffer.append(analog)
                digital_buffer.append(digital)

        except ValueError:
            # Riga non numerica o sporca
            continue
        except serial.SerialException as e:
            print(f"Serial error: {e}")
            break
        except Exception:
            continue

    try:
        ser.close()
    except Exception:
        pass

    running = False
    print("Serial thread stopped")


# =========================
# AUDIO CALLBACK
# =========================
def audio_callback(outdata, frames, time_info, status):
    global phase

    if status:
        print(status)

    with data_lock:
        analog = latest_analog
        digital = latest_digital

    # analogico -> frequenza
    freq = 200.0 + (analog / ANALOG_MAX) * 1000.0

    # digitale -> gate on/off
    amplitude = 0.2 if digital == 1 else 0.0

    t = (np.arange(frames) + phase) / AUDIO_SR
    wave = amplitude * np.sin(2.0 * np.pi * freq * t)

    outdata[:, 0] = wave
    if outdata.shape[1] > 1:
        outdata[:, 1] = wave

    phase += frames
    phase %= AUDIO_SR


# =========================
# GUI
# =========================
class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Realtime Serial Plot + Audio")

        central = QtWidgets.QWidget()
        layout = QtWidgets.QVBoxLayout(central)

        self.analog_plot = pg.PlotWidget(title="Analog Signal")
        self.digital_plot = pg.PlotWidget(title="Digital Signal")

        self.analog_curve = self.analog_plot.plot()
        self.digital_curve = self.digital_plot.plot()

        self.analog_plot.setYRange(ANALOG_MIN, ANALOG_MAX)
        self.digital_plot.setYRange(-0.2, 1.2)

        layout.addWidget(self.analog_plot)
        layout.addWidget(self.digital_plot)

        self.setCentralWidget(central)

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update_plots)
        self.timer.start(30)

    def update_plots(self):
        with data_lock:
            a = np.array(analog_buffer, dtype=float)
            d = np.array(digital_buffer, dtype=float)

        self.analog_curve.setData(a)
        self.digital_curve.setData(d)

    def closeEvent(self, event):
        global running
        running = False
        event.accept()


# =========================
# MAIN
# =========================
def main():
    global running

    serial_thread = threading.Thread(target=serial_worker, daemon=True)
    serial_thread.start()

    try:
        stream = sd.OutputStream(
            channels=2,
            callback=audio_callback,
            samplerate=AUDIO_SR,
            blocksize=AUDIO_BLOCKSIZE,
        )
        stream.start()
    except Exception as e:
        print(f"Failed to start audio stream: {e}")
        running = False
        return

    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.resize(900, 700)
    window.show()

    exit_code = 0
    try:
        exit_code = app.exec()
    finally:
        running = False
        try:
            stream.stop()
            stream.close()
        except Exception:
            pass

    sys.exit(exit_code)


if __name__ == "__main__":
    main()