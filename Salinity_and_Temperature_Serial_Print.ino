/*
VernierAnalogVoltage (v 2015.06)
Reads a Vernier analog (BTA) sensor connected to pin A0 of the Arduino. This
sketch displays the raw voltage reading on the Serial Monitor. As written, the
reading will be displayed every half second. Change the variable TimeBetweenReadings
to change the rate.

See www.vernier.com/arduino for more information.
*/
int TimeBetweenReadings = 500;
float Temp;
int rawAnalogReading;

// Variables used in the code for calculations
unsigned long timeRef;    // reference for starting time

unsigned long timeInterval;
unsigned long ndx;        // index for data counter
unsigned long thermistor;


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println("Time (s)    Voltage Readings from A0 (N)");
}

void loop() {
  while (true) {
    float Salinity_Count;
    float Salinity_Voltage;
    float ppt;
    float Temp_Voltage;
    float Temp_Count;
    float Temp_Resistance;
    float Time; // the print below does the division first to avoid overflows
    Serial.print(millis() / 1000.0); // using millis() for time calculation
    Salinity_Count = analogRead(A0);
    Salinity_Voltage = Salinity_Count / 1023 * 5.0; // convert from count to raw voltage
    Serial.print("     ");
    Serial.print("Salinity Voltage ");
    Serial.print(Salinity_Voltage);
    ppt = 16.3 * Salinity_Voltage;
    Serial.print("     ");
    Serial.print("ppt ");
    Serial.print(ppt);
    rawAnalogReading = analogRead(A2);  // reads raw analog value from Arduino
    thermistor = resistance(rawAnalogReading);     // converts raw analog value to a resistance
    Temp = steinharthart(thermistor);              // Applies the Steinhart-hart equation
    Serial.print("     Temperature (C) ");
    Serial.print(Temp,1);   // display temperature to one digit 
    Serial.println(" ");
    delay(TimeBetweenReadings); // delay in between reads for stability
  }
}
 
                 

unsigned long resistance(unsigned long rawAnalogInput)
/* function to convert the raw Analog Input reading to a resistance value    
 * Schematic:
 *   [Ground] -- [thermistor] -------- | -- [15,000 ohm bridge resistor] --[Vcc (5v)]
 *                                     |
 *                                Analog Pin 0
 *
 * For the circuit above:
 * Resistance = ((rawAnalogInput*15000) /(1023 - rawAnalogInput))
 */
{
  unsigned long temp;  // temporary variable to store calculations in
  temp = (rawAnalogInput * 15000) / (1023 - rawAnalogInput);
  return temp; // returns the value calculated to the calling function.
}

float steinharthart(unsigned long resistance)
// function users steinhart-hart equation to return a temperature in degrees celsius. 
/* Inputs ADC count from Thermistor and outputs Temperature in Celsius
 * There is a huge amount of information on the web about using thermistors with the Arduino.
 * Here we are concerned about using the Vernier Stainless Steel Temperature Probe TMP-BTA and the 
 * Vernier Surface Temperature Probe STS-BTA, but the general principles are easy to extend to other
 * thermistors.
 * This version utilizes the Steinhart-Hart Thermistor Equation:
 *    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]^3}
 *   for the themistor in the Vernier TMP-BTA probe:
 *    A =0.00102119 , B = 0.000222468 and C = 1.33342E-7
 *    Using these values should get agreement within 1 degree C to the same probe used with one
 *    of the Vernier interfaces
 * 
 */
{
  float temp; // temporary variable to store calculations in
  float logRes = log(resistance); 
  // calculating logirithms is time consuming for a microcontroller - so we just
  // do this once and store it to a variable.
  float k0 = 0.00102119;
  float k1 = 0.000222468;
  float k2 = 0.000000133342; 

  temp = 1 / (k0 + k1 * logRes + k2 * logRes*logRes*logRes);
  temp = temp - 273.15;  // convert from Kelvin to Celsius 
  return temp;
}
