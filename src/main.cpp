#include <Arduino.h>
#include <HPDL1414.h>

// HPDL-1414 pin -> ESP32 GPIO
//  1 D5 -> 3     12 D6 -> 12
//  2 D4 -> 4     11 D3 -> 11
//  3 WR -> 5     10 D2 -> 10
//  4 A1 -> 6      9 D1 -> 9
//  5 A0 -> 7      8 D0 -> 8
//  6 VDD -> 5V    7 GND -> GND
const byte dataPins[7] = {8, 9, 10, 11, 4, 3, 12}; // D0 - D6
const byte addrPins[2] = {7, 6};                   // A0, A1
const byte wrenPins[] = {5};                       // WR

HPDL1414 hpdl(dataPins, addrPins, wrenPins, sizeof(wrenPins));

// Voltage divider: VSUPPLY -- R_REF -- SENSE_PIN -- R_x -- GND
//   R_x = R_REF * V / (VSUPPLY - V)
const byte SENSE_PIN = 1;        // GPIO1 = ADC1_CH0
const float R_REF = 100000.0;    // reference resistor, ohms
const float VSUPPLY_MV = 5236.0; // measure your actual 5V rail and put it here
const float ADC_MAX_MV = 3100.0; // ADC saturates around here at 11dB attenuation
const float OPEN_MV = min(VSUPPLY_MV, ADC_MAX_MV) - 50.0;
const int SAMPLES = 64;

float readMilliVolts()
{
  uint32_t sum = 0;
  for (int i = 0; i < SAMPLES; i++)
    sum += analogReadMilliVolts(SENSE_PIN);
  return (float)sum / SAMPLES;
}

// Formats ohms into exactly 4 characters, e.g. " 470", "4.7K", " 47K", "1.2M"
void formatOhms(float r, char *buf)
{
  long ohms = lround(r);
  if (ohms < 1000)
  {
    snprintf(buf, 5, "%4ld", ohms);
    return;
  }
  long tenthsK = lround(r / 100.0);
  if (tenthsK < 100)
  {
    snprintf(buf, 5, "%ld.%ldK", tenthsK / 10, tenthsK % 10);
    return;
  }
  long k = lround(r / 1000.0);
  if (k < 1000)
  {
    snprintf(buf, 5, "%3ldK", k);
    return;
  }
  long tenthsM = lround(r / 100000.0);
  if (tenthsM < 100)
    snprintf(buf, 5, "%ld.%ldM", tenthsM / 10, tenthsM % 10);
  else
    snprintf(buf, 5, "%3ldM", min(lround(r / 1000000.0), 999L));
}

void show(const char *text)
{
  hpdl.setCursor(0);
  hpdl.print(text);
}

void setup()
{
  Serial.begin(115200);
  hpdl.begin();
  hpdl.clear();

  analogReadResolution(12);
  analogSetPinAttenuation(SENSE_PIN, ADC_11db);

  show("OHMS");
  delay(800);
}

void loop()
{
  float mv = readMilliVolts();
  char buf[5];

  if (mv >= OPEN_MV)
  {
    strcpy(buf, "OPEN");
    Serial.printf("%.0f mV  open\n", mv);
  }
  else
  {
    float r = R_REF * mv / (VSUPPLY_MV - mv);
    formatOhms(r, buf);
    Serial.printf("%.0f mV  %.0f ohms\n", mv, r);
  }

  show(buf);
  delay(200);
}
