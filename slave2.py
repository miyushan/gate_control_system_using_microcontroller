import tkinter as tk
from tkinter import ttk
import serial
from serial_communication import send_to_arduino_slave, read_from_arduino
import time


root = tk.Tk()
root.title("SLAVE 2")
root.geometry('300x250')
root.resizable(False, False)

SLAVE1_PORT = 'COM4'
slave_ser = serial.Serial(SLAVE1_PORT, 9600)


def get_response():
    start_time = time.time()

    while (slave_ser.inWaiting() == 0):
        if time.time() - start_time > 3:
            return "Try Again"

    result = read_from_arduino(slave_ser)

    return result

def submit_token():
    result_label.configure(text="")
    submit_button.configure(state="disabled")
    send_to_arduino_slave(token_entry.get(), slave_ser)

    token_entry.delete(0, tk.END)

    result = get_response()
    result_label.configure(text=result)
    
    submit_button.configure(state="active")


ttk.Label(root).pack()

token_label = ttk.Label(root, text="Enter Token")
token_label.pack(pady=5)

token_entry = ttk.Entry(root, width=40)
token_entry.pack(pady=5)

ttk.Label(root).pack()

submit_button = ttk.Button(root, text='Submit', command=lambda:submit_token())
submit_button.pack()

result_label = ttk.Label(root, text="")
result_label.pack(side='bottom', pady='15')

root.mainloop()
