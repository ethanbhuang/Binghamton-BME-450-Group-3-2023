#ifndef EEPROMC
#define EEPROMC

// The EEPROMCalibrate library works exclusively with doubles
// extending the library is out of the scope,
// but future work may be done to improve the library's flexibility


// Write 0 to all EEPROM memory locations
void EEPROMClear(void);
/*
    read a sequence of doubles from lower_bound to upper_bound
    from EEPROM
*/
bool EEPROMRead(int lower_bound, int upper_bound);
// Set EEPROM to calibrated and write data
void EEPROMCalibrate(double data[], int size, int ref_resistance);
// Check EEPROM for calibration
bool EEPROMIsCalibrated(void);
// Get the reference resistance for previous calibration
int EEPROMRefResistance(int size);

#endif