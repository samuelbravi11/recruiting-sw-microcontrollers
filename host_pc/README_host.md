# Host PC Software – Realtime Plot and Audio

## Overview

This application runs on the host PC and interfaces with the STM32 NUCLEO board via serial communication.

It:
- reads real-time data from the board
- plots analog and digital signals
- generates audio from the incoming signal

The data comes from a potentiometer connected to the STM32.

---

## Data Format

The STM32 must send data in this exact format:

<analog>\t<digital>\r\n

Example:

1024	0
2048	1
3000	1

C example:

printf("%u\t%u\r\n", analog_value, digital_value);

Only this format is parsed. Any other string is ignored.

---

## Features

- Real-time plotting:
  - analog signal (ADC)
  - digital signal (threshold)

- Audio synthesis:
  - analog → frequency
  - digital → on/off gate

- Non-blocking serial read

---

## Requirements

Python 3.10+

Install dependencies:

pip install -r requirements.txt

---

## Setup (recommended)

python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt

---

## Run

python realtime_plot_audio.py

---

## Serial Configuration

Port: /dev/ttyACM0  
Baudrate: 115200  
Mode: 8N1  

DTR and RTS are disabled to avoid MCU reset on open.

---

## Plots

Two real-time plots:

- Analog: range 0–4095
- Digital: 0 or 1

---

## Audio

- analog controls frequency (200–1200 Hz)
- digital controls amplitude:
  - 1 → sound ON
  - 0 → sound OFF