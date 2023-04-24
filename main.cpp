#include <Arduino.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include<ACS712.h>
#define channel_id 2034656
#define channel_api "KMHY7QQ3KPKAESXN"
/*
  Arduino DC Voltage Demo 2
  dc-voltage-demo-vref.ino
  Use Arduino A/D converter to measure voltage
  Use external voltage reference
  Use external voltage divider with 30k & 7.5k resistors
  Results displayed on Serial Monitor

  DroneBot Workshop 2021
  https://dronebotworkshop.com
*/

// Define analog input
#define ANALOG_IN_PIN A0

// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;

// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0;
// int privious =0;
// Float for Reference Voltage
float ref_voltage = 3.34;
bool prev =true;
// Integer for ADC value
int adc_value = 0;
WiFiClient client;
ACS712 sensor(ACS712_05B, A0);
float I =0;
void setup()
{
  // Use external voltage reference
  // Setup Serial Monitor
  Serial.begin(9600);
  WiFi.disconnect();
  WiFi.begin("CHIKUMIKU", "123211234");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print("..");
  }
  // Serial.println();
  Serial.println("NodeMCU is connected!");
  // Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);
  // Serial.println("DC Voltage Test");
}

void loop()
{
  // Read the Analog Input
  adc_value = analogRead(ANALOG_IN_PIN);

  I = sensor.getCurrentAC();
  // Determine voltage at ADC input
  adc_voltage = (adc_value * ref_voltage) / 1024.0;

  // Calculate voltage at divider input
  in_voltage = adc_voltage / (R2 / (R1 + R2));
  if (in_voltage > 1.0)
  {
    in_voltage = in_voltage - .84;
  }
  float bat_percentage = map(in_voltage, 8, 14.5, 0, 100); // 2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage

  if (bat_percentage >= 100)
  {
    bat_percentage = 100;
  }
  if (bat_percentage <= 0)
  {
    bat_percentage = 1;
  }
    ThingSpeak.setField(1, String(in_voltage));
    ThingSpeak.setField(2, String(bat_percentage));
  // Serial.println(String("I = ") + I + " A");
    // ThingSpeak.setField(1,I);
    ThingSpeak.writeFields(channel_id, channel_api);
}