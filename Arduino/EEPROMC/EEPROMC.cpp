/*
    Includes
*/
#include "EEPROMC.h"
#include <Arduino.h>
#include <EEPROM.h>

// write 0 to all EEPROM
void EEPROMClear(void)
{
    for (int i = 0; i < EEPROM.length(); i++)
    {
        EEPROM.write(i, 0);
    }
}

/*
    Read a sequence of doubles from lower_bound to upper_bound

    @param lower_bound The first EEPROM memory location to read from
    @param upper_bound the last EEPROM memory location to read from
    @return Success or failure
*/
bool EEPROMRead(int lower_bound, int upper_bound)
{
    // check all illegal cases
    // should print error fixes, but later

    // lower bound greater than upper bound
    if (lower_bound > upper_bound)
        return false;
    // lower bound greater than mem available
    if (lower_bound > EEPROM.length() - 1)
        return false;
    // upper bound greater than mem available
    if (upper_bound > EEPROM.length() - 1)
        return false;
    // lower bound less than 0
    if (lower_bound < 0)
        return false;
    // upper bound less than 0
    if (upper_bound < 0)
        return false;

    // variable to read EEPROM values into
    double dummy;
    
    for (int i = lower_bound; i < upper_bound; i++)
    {
        EEPROM.get(i * sizeof(double), dummy);
        Serial.print(i);
        Serial.print(" : ");
        Serial.println(dummy);
    }

    return true;
}

/*
    Set EEPROM to calibrated and write data

    @param data The data to write to EEPROM
    @param int The size of data
    @param ref_resistance The reference resistance
*/
void EEPROMCalibrate(double data[], int size, int ref_resistance)
{
    // 255 at last EEPROM mem slot indicates device has been calibrated
    EEPROM.write(EEPROM.length() - 1, 255);

    for (int i = 0; i < size; i++)
    {
        EEPROM.put(i * sizeof(double), data[i]);
    }

    EEPROM.put((size + 1) * sizeof(double), ref_resistance);

}

/* 
    Checks the last EEPROM byte for calibration

    @return Success or failure
*/
bool EEPROMIsCalibrated(void)
{
    return EEPROM.read(EEPROM.length() - 1) == 255;
}

/*
    Gets the reference resistance for previous calibration

    @param size The size of the calibration set
    @return Reference resistance value
*/
int EEPROMRefResistance(int size)
{
    int mandy;
    EEPROM.get((size + 1) * sizeof(double), mandy);

    return mandy;
}