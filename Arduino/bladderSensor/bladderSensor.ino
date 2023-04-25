#include <Wire.h>
#include <string.h>
#include "AD5933.h"
#include "EEPROMC.h"
#include "EEPROM.h"

#define START_FREQ  (80000)
#define FREQ_INCR   (1000)
#define NUM_INCR    (40)
#define REF_RESIST  (1800)
#define DELAY       (1000)
#define TEST_CALIBRATION 0

double gain[NUM_INCR];
int phase[NUM_INCR+1];
int count;

void setup(void)
{

  // Begin I2C
  Wire.begin();

  // Begin serial at 9600 baud for output
  Serial.begin(9600);
  // Wait for Serial to begin
  while (!Serial);

  // Perform initial configuration. Fail if any one of these fail.
  if (!(AD5933::reset() &&
        AD5933::setInternalClock(true) &&
        AD5933::setStartFrequency(START_FREQ) &&
        AD5933::setIncrementFrequency(FREQ_INCR) &&
        AD5933::setNumberIncrements(NUM_INCR) &&
        AD5933::setPGAGain(PGA_GAIN_X1)))
        {
            Serial.println("Failed in initialization");
            while (true);
        }

  Serial.print("Calibrated: ");
  if (!EEPROMIsCalibrated() || TEST_CALIBRATION)
  {
    Serial.println("No");
    if (AD5933::calibrate(gain, phase, REF_RESIST, NUM_INCR+1))
    {
      EEPROMCalibrate(gain, NUM_INCR, REF_RESIST);
    }
  }
  else
  {
    Serial.println("Yes");
    // load EEPROM values into gain[]
    double mandy;

    for (int i = 0;i < NUM_INCR;i++)
    {
      EEPROM.get(i * sizeof(double), mandy);
      gain[i] = mandy;
    }
  }

  Serial.print("Start freq: ");
  Serial.println(START_FREQ);

  Serial.print("Increment Frequency: ");
  Serial.println(FREQ_INCR);

  Serial.print("Increment #: ");
  Serial.println(NUM_INCR);

  Serial.print("Previous reference resistance: ");
  Serial.println(EEPROMRefResistance(NUM_INCR));

  Serial.print("Delay: ");
  Serial.println(DELAY);

  Serial.println("EEPROM calibration data:");
  for (int i = 0;i < NUM_INCR;i++)
  {
    Serial.print("\tEEPROM[");
    Serial.print(i);
    Serial.print("]: ");
    printDouble(gain[i], 10);
    Serial.println();
  }
  
  Serial.println("Device calibrated");

  delay(1000);
}


void loop(void)
{
  // Easy to use method for frequency sweep
  frequencySweepEasy();

  // Delay
  delay(DELAY);

}

// Easy way to do a frequency sweep. Does an entire frequency sweep at once and
// stores the data into arrays for processing afterwards. This is easy-to-use,
// but doesn't allow you to process data in real time.
void frequencySweepEasy() {
    // Create arrays to hold the data
    int real[NUM_INCR + 1], imag[NUM_INCR + 1];

    // Perform the frequency sweep
    if (AD5933::frequencySweep(real, imag, NUM_INCR+1)) {
      // Print the frequency data
      int cfreq = START_FREQ/1000;
      double avg_impedance = 0;
      for (int i = 0; i < NUM_INCR; i++, cfreq += FREQ_INCR/1000) {
        // Compute impedance
        // equation for amplitude
        double magnitude = sqrt(pow(real[i], 2) + pow(imag[i], 2));
        double impedance = 1/(magnitude * gain[i]);

        avg_impedance += impedance;
      }

      // uh I have to test this code
      // Serial.print(count++);
      // Serial.print(" : Average impedance = ");
      // printDouble((avg_impedance / NUM_INCR), 4);
      // Serial.println(" Ohms");

      printDouble((avg_impedance / NUM_INCR), 4);
      Serial.print("\n");
      // Serial.print();
    }
    else Serial.println("Frequency sweep failed...");
}


// Removes the frequencySweep abstraction from above. This saves memory and
// allows for data to be processed in real time. However, it's more complex.
void frequencySweepRaw() {
    // Create variables to hold the impedance data and track frequency
    int real, imag, i = 0, cfreq = START_FREQ/1000;

    // Initialize the frequency sweep
    if (!(AD5933::setPowerMode(POWER_STANDBY) &&          // place in standby
          AD5933::setControlMode(CTRL_INIT_START_FREQ) && // init start freq
          AD5933::setControlMode(CTRL_START_FREQ_SWEEP))) // begin frequency sweep
         {
             Serial.println("Could not initialize frequency sweep...");
         }

    // Perform the actual sweep
    while ((AD5933::readStatusRegister() & STATUS_SWEEP_DONE) != STATUS_SWEEP_DONE) {
        // Get the frequency data for this frequency point
        if (!AD5933::getComplexData(&real, &imag)) {
            Serial.println("Could not get raw frequency data...");
        }

        // Print out the frequency data
        Serial.print(cfreq);
        Serial.print(": R=");
        Serial.print(real);
        Serial.print("/I=");
        Serial.print(imag);

        // Compute impedance
        double magnitude = sqrt(pow(real, 2) + pow(imag, 2));
        double impedance = 1/(magnitude*gain[i]); //replace with EEPROM data
        Serial.print("  |Z|=");
        Serial.println(impedance);

        // Increment the frequency
        i++;
        cfreq += FREQ_INCR/1000;
        AD5933::setControlMode(CTRL_INCREMENT_FREQ);
    }

    Serial.println("Frequency sweep complete!");

    // Set AD5933 power mode to standby when finished
    if (!AD5933::setPowerMode(POWER_STANDBY))
        Serial.println("Could not set to standby...");
}


// cite this code
void printDouble( double val, byte precision){
  // prints val with number of decimal places determine by precision
  // precision is a number from 0 to 6 indicating the desired decimial places
  // example: printDouble( 3.1415, 2); // prints 3.14 (two decimal places)

  Serial.print (int(val));  //prints the int part
  if( precision > 0) {
    Serial.print("."); // print the decimal point
    unsigned long frac;
    unsigned long mult = 1;
    byte padding = precision -1;
    while(precision--)
       mult *=10;
       
    if(val >= 0)
      frac = (val - int(val)) * mult;
    else
      frac = (int(val)- val ) * mult;
    unsigned long frac1 = frac;
    while( frac1 /= 10 )
      padding--;
    while(  padding--)
      Serial.print("0");
    Serial.print(frac,DEC) ;
  }
}

