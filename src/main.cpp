#include <Arduino.h>
#include <math.h>
#include <Wire.h>

const double sensorResolution = 1023.0;

unsigned long previousTimeRead = millis();
long readIntervalMillis = 1000;


const int refInput = A6;
const int chamberInput = A10;
const int probe1Input = A0;
const int probe2Input = A1;
const int probe3Input = A2;
const int probe4Input = A3;
// const int chamberPresencePin = 8;
// const int probe1PresencePin = 7;
// const int probe2PresencePin = 6;
// const int probe3PresencePin = 5;
// const int probe4PresencePin = 4;

double refVoltage;

double chamberTemp;
double probe1Temp;
double probe2Temp;
double probe3Temp;
double probe4Temp;

bool chamberPresence = false;
bool probe1Presence = false;
bool probe2Presence = false;
bool probe3Presence = false;
bool probe4Presence = false;


double convertToVoltage(int adcValue) {
  return adcValue * (5.0 / sensorResolution);
}

double getVoltage(int pin) {
  int value = analogRead(pin);
  return convertToVoltage(value);
}

double getTemperature(int pin) {
  const double reads = 10.0;

  double sumRef = 0.0;
  double sumPin = 0.0;

  for (int i = 0; i < reads; i++) {
    sumRef += getVoltage(refInput);
    sumPin += getVoltage(pin);
  }

  refVoltage = sumRef / reads;
  Serial.println((String)"refVoltage: " + refVoltage);

  double pinVoltage = sumPin / reads;
  Serial.println((String)"pinVoltage: " + pinVoltage);

  if (pinVoltage > refVoltage - 0.02 && pinVoltage < refVoltage + 0.02) {
    return -500;
  }

  const long resistor2 = 100000;
  const double aValue = 0.0007343140544;
  const double bValue = 0.0002157437229;
  const double cValue = 0.0000000951568577;

  const double resistance = resistor2 * ((refVoltage / pinVoltage) - 1);
  Serial.println((String)"resistance: " + resistance);

  const double temperatureK = 1.0 / (aValue + bValue * log(resistance) + cValue * pow(log(resistance), 3.0));
  Serial.println((String)"temperatureK: " + temperatureK);
  const double temperatureC = temperatureK - 272.15;
  Serial.println((String)"temperatureC: " + temperatureC);
  const double temperatureF = ((9.0 / 5.0) * temperatureC) + 32.0;
  Serial.println((String)"temperatureF: " + temperatureF);

  return temperatureF;
}

bool isConnectedFromTemp(double temp) {

}

void readPins() {
  chamberTemp = getTemperature(chamberInput);
  Serial.println((String)"chamberTemp: " + chamberTemp);
  probe1Temp = getTemperature(probe1Input);
  Serial.println((String)"probe1Temp: " + probe1Temp);
  probe2Temp = getTemperature(probe2Input);
  Serial.println((String)"probe2Temp: " + probe2Temp);
  probe3Temp = getTemperature(probe3Input);
  Serial.println((String)"probe3Temp: " + probe3Temp);
  probe4Temp = getTemperature(probe4Input);
  Serial.println((String)"probe4Temp: " + probe4Temp);

  String jsonOut = "{ \"0\": " + (String)chamberTemp + ", \"1\": " + (String)probe1Temp + ", \"2\": " + (String)probe2Temp + ", \"3\": " + (String)probe3Temp + ", \"4\": " + (String)probe4Temp + " }";
  char jsonOutChar[70];
  jsonOut.toCharArray(jsonOutChar, 70);

  Wire.write(jsonOutChar);
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Wire.begin(12);
  Wire.onRequest(readPins);

  pinMode(refInput, INPUT);
  pinMode(chamberInput, INPUT);
  pinMode(probe1Input, INPUT);
  pinMode(probe2Input, INPUT);
  pinMode(probe3Input, INPUT);
  pinMode(probe4Input, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();

  if (currentTime - previousTimeRead > readIntervalMillis) {
    previousTimeRead = currentTime;
  }
}