#include <SD.h>

unsigned long TimeBetweenReadings = 1000;
float Temp;
int rawAnalogReading;

unsigned long thermistor;
String dataString;

unsigned long previousTime = 0;
unsigned long currentTime;

const int chipSelect = 8; // Replace with the appropriate CS pin number

void setup() {
  Serial.begin(9600);
  while (!Serial)
    continue; // Wait for Serial Monitor to open
  Serial.println("Initializing SD card...");
  
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Error opening datalog.csv");
  } else {
    Serial.println("Time (s),Temperature (C),Salinity Raw Voltage (V),ppt");
    dataFile.println("Time (s),Temperature (C),Salinity Raw Voltage (V),ppt");
    dataFile.close();
  }
}

void loop() {
  currentTime = millis();
  if (currentTime - previousTime >= TimeBetweenReadings) {
    previousTime = currentTime;

    float Salinity_Count;
    float Salinity_Voltage;
    float ppt;
    float Temp_Voltage;
    float Temp_Count;
    float Temp_Resistance;
    unsigned long Time = currentTime; // Store the current milliseconds value

    Salinity_Count = analogRead(A0);
    Salinity_Voltage = Salinity_Count / 1023.0 * 5.0;
    ppt = 16.3 * Salinity_Voltage;
    rawAnalogReading = analogRead(A2);
    thermistor = resistance(rawAnalogReading);
    Temp = steinharthart(thermistor);

    dataString = String(Time / 1000) + "," + String(Temp, 1) + "," + String(Salinity_Voltage, 2) + "," + String(ppt, 3);
    Serial.println(dataString);

    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println(dataString);
      dataFile.close();
    } else {
      Serial.println("Error opening datalog.csv");
    }
  }
}

unsigned long resistance(unsigned long rawAnalogInput) {
  unsigned long temp;
  temp = (rawAnalogInput * 15000) / (1023 - rawAnalogInput);
  return temp;
}

float steinharthart(unsigned long resistance) {
  float temp;
  float logRes = log(resistance);
  float k0 = 0.00102119;
  float k1 = 0.000222468;
  float k2 = 0.000000133342;

  temp = 1 / (k0 + k1 * logRes + k2 * logRes * logRes * logRes);
  temp = temp - 273.15;
  return temp;
}
