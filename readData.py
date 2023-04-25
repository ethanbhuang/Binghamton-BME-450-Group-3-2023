import serial
import csv
from time import localtime, strftime
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import os
import numpy as np

# in TERMINAL, type "py readData.py"
# ctrl+C to end stream

# Arduino COM port 
port = "COM5"
# Baud rate of Arduino program
baud = 9600
# Absolute path of the working directory (generated files will be stored here)
cwd = os.getcwd()
# Generates a new directory to hold generated files
dir_name = input("Data file name: ")
# Formats dir_name tp csv
csv_name = "{}.csv".format(dir_name)
# Formats dir_name to txt
info_name = "{}.txt".format(dir_name)
# Formats dir_name to png
plot_name = "{}.png".format(dir_name)
# Gets absolute path of new directory from with name dir_name
dir_path = os.path.join(cwd, dir_name)

# Check if a directory with name dir_name already exists
# If not, make new directory
if (not os.path.exists(dir_path)):
    os.mkdir(dir_path)

# Global matplotlib data stores
x = []
y = []
# Count # of frequency sweeps performed by Arduino
count = 0

# todo fix formatting of matplotlib

# Try to connect to the serial port
try:
    ser = serial.Serial(port, baud, timeout=10)
    print(f"Connected to Arduino port: {port}")
except:
    print(f"Port {port} was not available")
    quit()

# Try to write the info file
# If the serial port closes unexpectedly, quit
try:
    with open(os.path.join(dir_path, info_name), "w") as f:
        # Log the connection port
        f.write("Connected to Arduino port: {}\n".format(port))
        
        while (ser.is_open):
            getData = ser.readline()
            dataString = getData.decode('utf-8')
            
            data = dataString[0:][:-2]

            # If the device is calibrated, break from loop and close text file
            if (data == "Device calibrated"):
                print("Logging to CSV: ")
                f.close()
                break
            else:
                f.write("{}\n".format(data))
                print(data)
except Exception as e:
    print(e)
    quit()

with open(os.path.join(dir_path, csv_name), "w", newline = '', encoding='utf-8') as f:
    fieldnames = ['time', 'impedance', 'error']
    writer = csv.DictWriter(f, fieldnames=fieldnames)
    writer.writerow({'time': "Time", 'impedance': 'Impedance', 'error': 'Error'})
    data = None
    last_data = None

    while (ser.is_open):
        try:
            #display the data to the terminal
            time = strftime("%H:%M:%S", localtime())
            getData = ser.readline()
            dataString = getData.decode('utf-8')
            
            data = dataString[0:][:-2]
            last_data = data

            print(f"\t{time} {data}")
            writer.writerow({'time': time, 'impedance':data, 'error':0})
        except KeyboardInterrupt as e:
            print("Serial stream forcefully ended")
            break
        except Exception as e:
            print(e)
            print(f"\t{time} {data}")
            writer.writerow({'time': time, 'impedance':data, 'error':1})
            data = last_data

        x.append(time)
        y.append(float(data))
        count += 1

print("Serial closed")

with open(os.path.join(dir_path, info_name), 'a') as f:
    f.write("# Sweeps: {}\n".format(count))

# print(x)
# print(y)

if (len(x) > len(y)):
    x.pop()
elif (len(y) > len(x)):
    y.pop()

try:
    fig, ax = plt.subplots()
    plt.title("Bladder Impedance Across Time")
    plt.grid(visible=True, 
             axis="y")
    plt.xticks(rotation=70)
    plt.xlabel("Time (HH:MM:SS)")
    plt.ylabel("Impedance (ohms)")
    ax.set_xticks(np.arange(0, len(x), len(x)/10))
    plt.plot(x, y)
    plt.savefig(fname = os.path.join(dir_path, plot_name),
                bbox_inches = "tight",
                pad_inches = 0.25)
    plt.show()
except Exception as e:
    print(e)

# def update(i, xs, ys);
