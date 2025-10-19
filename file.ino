#include <OneWire.h>
#include <DallasTemperature.h>

#define SOIL_MOISTURE_PIN 34  //(VP/GPIO34)
#define DS18B20_PIN 4         //(GPIO4)

// Soil value (to be adjusted)
#define DRY_VALUE 3200
#define WET_VALUE 1200

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

int soilMoistureRaw = 0;
int soilMoisturePercent = 0;
float temperatureC = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("ESP32 Soil Moisture & Temperature Monitor");
  Serial.println("==========================================");
  
  //DS18B20 Sensor
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount());
  Serial.println(" DS18B20 sensor(s)");
  
  analogSetAttenuation(ADC_11db);  // Full range: 0-3.3V
  
  Serial.println("\nStarting measurements...\n");
}

void loop() {
  soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
  
  // Lower value is more Moisture(wtf)
  soilMoisturePercent = map(soilMoistureRaw, DRY_VALUE, WET_VALUE, 0, 100);
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
  
  sensors.requestTemperatures();
  temperatureC = sensors.getTempCByIndex(0);
  
  // Display
  Serial.println("=== Sensor Readings ===");
  
  Serial.print("Soil Moisture Raw: ");
  Serial.println(soilMoistureRaw);
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisturePercent);
  Serial.print("% ");
  
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
  
  Serial.print("Temperature: ");
  if (temperatureC != DEVICE_DISCONNECTED_C) {
    Serial.print(temperatureC);
    Serial.println(" °C");
  } else {
    Serial.println("Error reading temperature!");
  }
  
  Serial.println();
  
  // Waiting 2.5 second before next reading
  delay(2500);
}
