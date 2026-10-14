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

void setup()
{
  Serial.begin(115200);
  hpdl.begin();
  hpdl.clear();

  // Each digit shows its own position, to check digit order
  hpdl.print("0123");
  delay(1500);

  hpdl.clear();
  hpdl.print("TEST");
  delay(1500);

  // All 64 characters, 4 at a time, to check every data line
  for (char c = ' '; c <= '_'; c += 4)
  {
    hpdl.clear();
    for (byte i = 0; i < 4; i++)
      hpdl.write(c + i);
    delay(600);
  }
}

void loop()
{
  // Counter to show it's alive
  static uint16_t n = 0;
  char buf[5];
  snprintf(buf, sizeof(buf), "%4u", n++ % 10000);
  hpdl.setCursor(0);
  hpdl.print(buf);
  delay(100);
}
