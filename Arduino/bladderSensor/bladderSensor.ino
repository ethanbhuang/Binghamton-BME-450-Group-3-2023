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
#define WINDOW_SIZE (10)
#define WINDOW_TOLERANCE (5)
#define DIFF (15.0)
#define TEST_CALIBRATION (0)

// Array to store calibration gain values
double gain[NUM_INCR];
// Array to store calibration phase values
// Unused, but required by AD5933.h 
int phase[NUM_INCR+1];
double start_med_arr[WINDOW_SIZE];
double start_med;
double target;
double window_med_arr[WINDOW_SIZE];
double window_med;
int window_med_count;

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
  double avg_impedance = frequencySweepAvg();

  for (int i = WINDOW_SIZE - 1;i > 0;i--)
  {
    window_med_arr[i - 1] = window_med_arr[i];
  }

  window_med_arr[WINDOW_SIZE - 1] = avg_impedance;

  window_med = median(window_med_arr, WINDOW_SIZE);

  if (window_med <= target) window_med_count += 1;
  else window_med_count = 0;
  
  if (window_med_count >= WINDOW_TOLERANCE)
  {
    Serial.println("You've reached critical capacity.");
    while (true);
  }

  printDouble(avg_impedance, 4);
  Serial.print("\n");

  // Delay
  delay(DELAY);

}

// Easy way to do a frequency sweep. Does an entire frequency sweep at once and
// stores the data into arrays for processing afterwards. This is easy-to-use,
// but doesn't allow you to process data in real time.
double frequencySweepAvg() {
   // Create arrays to hold the data
    int real[NUM_INCR + 1], imag[NUM_INCR + 1];
    double avg_impedance = 0;

    // Perform the frequency sweep
    if (AD5933::frequencySweep(real, imag, NUM_INCR+1)) {
      // Print the frequency data
      int cfreq = START_FREQ/1000;
      for (int i = 0; i < NUM_INCR; i++, cfreq += FREQ_INCR/1000) {
        // Compute impedance
        // equation for amplitude
        double magnitude = sqrt(pow(real[i], 2) + pow(imag[i], 2));
        double impedance = 1/(magnitude * gain[i]);

        avg_impedance += impedance;
      }

      avg_impedance /= NUM_INCR;
    }
    else Serial.println("Frequency sweep failed...");

  return avg_impedance;
}

// Code sourced from
// https://forum.arduino.cc/t/printing-a-double-variable/44327/17
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

double median(double arr[], int size)
{
  double *temp = arr;
  double median;

  qsort(temp, size, sizeof(arr[0]), sort_desc);

  if (WINDOW_SIZE % 2 == 0)
  {
    median = (arr[size / 2] + arr[(size / 2) + 1]) / 2; 
  } 
  else
  {
    median = arr[size / 2];
  }

  return median;
}

// sort function for qsort
int sort_desc(const void *cmp1, const void *cmp2)
{
  // Need to cast the void * to int *
  double a = *((double *) cmp1);
  double b = *((double *) cmp2);
  // The comparison
  return a > b ? -1 : (a < b ? 1 : 0);
}