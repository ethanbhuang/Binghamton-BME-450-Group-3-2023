# Binghamton-BME-450-Group-3-2023

This repository contains software written for the Binghamton University BME 450 Team 3 senior capstone project, a bladder impedance sensor.

## Arduino Software Description

This project uses the [DFRobot Bluno](https://www.dfrobot.com/product-1044.html), an Arduino Uno Rev3 based product that integrates Arduino capabilities with Bluetooth 4.0 (BLE), enabling wireless device communications. Also used is the [Analog Devices AD5933](https://www.analog.com/en/products/ad5933.html) Impedance Converter, along with the associated [AD5933 library](https://github.com/mjmeli/arduino-ad5933).

The Bluno acts as the logic component for the device and is responsible for communications between the AD5933 and the mobile application. The Bluno's driver code is within the _bladderSensor_ directory, and is called _bladderSensor.ino_.

The directory _EEPROMC_ includes code required by _bladderSensor.ino_ to function. The _EEPROMC_ library enables calibration value permanence and allows users to only have to calibrate the device once in its lifetime.

The directory _recalibrate_ includes code that allows the user to recalibrate their device withouut the Bluno's driver code.

## Python Software Description

A Python script _readData.py_ was written to allow for simplified data logging and visualization. To use the script, connect the device to any COM port. Within a terminal, run the script using the following command:

> `py readData.py`

You will be prompted to enter a directory in which data will be stored. Kepp in mind that if a directory already exists with the entered name, **it will be overwritten**. The script outputs three files for a directory named example:

-   example.csv
-   example.png
-   example.txt

The generated files include logs of each frequency sweep performed, a visualization of data recorded, and device status data, respectively.

## Android Software Description

The Android application reads data sent by the Arduino device and displays them to the screen.

# Installation

The easiest way to install code written for this project is by using git. Open your terminal, navigating to the directory you'd like as the parent directory, and running the following command:

> `git clone https://github.com/ethanbhuang/Binghamton-BME-450-Group-3-2023.git`

If you do not have git, the project may be installed as a .zip and extracted.

## Arduino

To install the Arduino software, you must download the [Arduino IDE](https://www.arduino.cc/en/software). Having done that, navigate to where your Arduino libraries are stored. This is usually _USER/Documents/Arduino_. Copy and paste the _EEPROMC_ files into the directory _libraries_. You should now be able to upload code to your Arduino device.

## Python

To install the Python software, you must download [Python](https://www.python.org/downloads/). We recommend version 2.7 and above to ensure compatability. After installing Python, install the following packages using pip, the included package installation tool:

-   numpy
-   matpolotlib
-   csv
-   serial

Use pip with the following command:

> `pip install [PACKAGE_NAME]`

## Android

To install the Android application, you must download the [Android Studio IDE](https://developer.android.com/studio). Open and build the project _BladderSensor2022Submission_. While in developer mode on your Android device, you may now install the application.

# Further Information

Detailed documentation is included within each of the project files. For further information, contact [Ethan Huang](mailto:ehuang35@binghamton.edu).

Thanks to the whole team for a wonderful semester: Shrishti Gupta, Zhixuan He, Pauline Macapinlac, and Patrick Rodrigues.

Thanks to our advisor Dr. Ahyeon Koh, course professor Sebastian Freeman, and fabrication lab team.
