/*
 * ESP32 Soil Moisture and Temperature Monitoring System
 * 
 * Components:
 * - ESP32 NodeMCU Development Board
 * - Capacitive Soil Moisture Sensor v2.0
 * - DS18B20 Waterproof Temperature Sensor
 * - 4.7kΩ resistors (for DS18B20 pull-up)
 * 
 * Libraries needed:
 * - OneWire (for DS18B20)
 * - DallasTemperature (for DS18B20)
 */

#include <OneWire.h>
#include <DallasTemperature.h>

// Pin Definitions
#define SOIL_MOISTURE_PIN 34  // Analog pin for soil moisture sensor (VP/GPIO34)
#define DS18B20_PIN 4         // Digital pin for DS18B20 (GPIO4)

// Soil Moisture Calibration Values (adjust based on your sensor)
#define DRY_VALUE 3200    // Value when sensor is in air (dry)
#define WET_VALUE 1200    // Value when sensor is in water (wet)

// OneWire and Dallas Temperature Setup
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

// Variables
int soilMoistureRaw = 0;
int soilMoisturePercent = 0;
float temperatureC = 0.0;

void setup() {
  // Initialize Serial Communication
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ESP32 Soil Moisture & Temperature Monitor");
  Serial.println("==========================================");
  
  // Initialize DS18B20 sensor
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount());
  Serial.println(" DS18B20 sensor(s)");
  
  // Configure ADC for soil moisture reading
  analogSetAttenuation(ADC_11db);  // Full range: 0-3.3V
  
  Serial.println("\nStarting measurements...\n");
}

void loop() {
  // Read Soil Moisture
  soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
  
  // Convert to percentage (0-100%)
  // Lower value = more moisture
  soilMoisturePercent = map(soilMoistureRaw, DRY_VALUE, WET_VALUE, 0, 100);
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
  
  // Read Temperature
  sensors.requestTemperatures();
  temperatureC = sensors.getTempCByIndex(0);
  
  // Display Results
  Serial.println("=== Sensor Readings ===");
  
  // Soil Moisture
  Serial.print("Soil Moisture Raw: ");
  Serial.println(soilMoistureRaw);
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisturePercent);
  Serial.print("% ");
  
  // Moisture level indicator
  if (soilMoisturePercent >= 70) {
    Serial.println("(Very Wet)");
  } else if (soilMoisturePercent >= 50) {
    Serial.println("(Wet)");
  } else if (soilMoisturePercent >= 30) {
    Serial.println("(Moist)");
  } else if (soilMoisturePercent >= 15) {
    Serial.println("(Dry)");
  } else {
    Serial.println("(Very Dry - Water Needed!)");
  }
  
  // Temperature
  Serial.print("Temperature: ");
  if (temperatureC != DEVICE_DISCONNECTED_C) {
    Serial.print(temperatureC);
    Serial.println(" °C");
  } else {
    Serial.println("Error reading temperature!");
  }
  
  Serial.println();
  
  // Wait 5 seconds before next reading
  delay(5000);
}
