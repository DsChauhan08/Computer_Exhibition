#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "YOUR_WIFI_SSID";      
const char* password = "YOUR_WIFI_PASSWORD";

#define SOIL_MOISTURE_PIN 34 //(VP/GPIO34)
#define DS18B20_PIN 4         //(GPIO4)

#define DRY_VALUE 3200    
#define WET_VALUE 1200    

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

// Localhost port 80 : https://localhost:80 
WebServer server(80);

int soilMoistureRaw = 0;
int soilMoisturePercent = 0;
float temperatureC = 0.0;
String moistureStatus = "";

void readSensors() {
  soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN)
  soilMoisturePercent = map(soilMoistureRaw, DRY_VALUE, WET_VALUE, 0, 100);
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);
  
  if (soilMoisturePercent >= 70) {
    moistureStatus = "Very Wet";
  } else if (soilMoisturePercent >= 50) {
    moistureStatus = "Wet";
  } else if (soilMoisturePercent >= 30) {
    moistureStatus = "Moist";
  } else if (soilMoisturePercent >= 15) {
    moistureStatus = "Dry";
  } else {
    moistureStatus = "Very Dry - Water Needed!";
  }
  
  sensors.requestTemperatures();
  temperatureC = sensors.getTempCByIndex(0);
}

// HTML
String getHTML() {
  String html = "<!DOCTYPE html><html lang='en'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP32 Plant Monitor</title>";
  html += "<style>";
  html += "* { margin: 0; padding: 0; box-sizing: border-box; }";
  html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; padding: 20px; }";
  html += ".container { max-width: 600px; margin: 0 auto; }";
  html += ".header { text-align: center; color: white; margin-bottom: 30px; }";
  html += ".header h1 { font-size: 2.5em; margin-bottom: 10px; text-shadow: 2px 2px 4px rgba(0,0,0,0.3); }";
  html += ".header p { font-size: 1.1em; opacity: 0.9; }";
  html += ".card { background: white; border-radius: 20px; padding: 30px; margin-bottom: 20px; box-shadow: 0 10px 30px rgba(0,0,0,0.3); }";
  html += ".sensor-reading { margin-bottom: 30px; }";
  html += ".sensor-reading:last-child { margin-bottom: 0; }";
  html += ".sensor-label { font-size: 0.9em; color: #666; text-transform: uppercase; letter-spacing: 1px; margin-bottom: 10px; }";
  html += ".sensor-value { font-size: 3em; font-weight: bold; color: #667eea; margin-bottom: 5px; }";
  html += ".sensor-unit { font-size: 1.2em; color: #999; }";
  html += ".status-badge { display: inline-block; padding: 8px 16px; border-radius: 20px; font-size: 0.9em; font-weight: 600; margin-top: 10px; }";
  html += ".status-very-wet { background: #e3f2fd; color: #1976d2; }";
  html += ".status-wet { background: #e8f5e9; color: #388e3c; }";
  html += ".status-moist { background: #fff3e0; color: #f57c00; }";
  html += ".status-dry { background: #fbe9e7; color: #d84315; }";
  html += ".status-very-dry { background: #ffebee; color: #c62828; }";
  html += ".progress-bar { width: 100%; height: 20px; background: #f0f0f0; border-radius: 10px; overflow: hidden; margin-top: 15px; }";
  html += ".progress-fill { height: 100%; background: linear-gradient(90deg, #667eea, #764ba2); transition: width 0.5s ease; border-radius: 10px; }";
  html += ".divider { height: 2px; background: linear-gradient(90deg, transparent, #e0e0e0, transparent); margin: 25px 0; }";
  html += ".refresh-info { text-align: center; color: #999; font-size: 0.85em; margin-top: 20px; }";
  html += ".refresh-btn { background: linear-gradient(135deg, #667eea, #764ba2); color: white; border: none; padding: 12px 30px; border-radius: 25px; font-size: 1em; cursor: pointer; box-shadow: 0 4px 15px rgba(102,126,234,0.4); margin-top: 15px; }";
  html += ".refresh-btn:active { transform: scale(0.98); }";
  html += "@media (max-width: 480px) { .sensor-value { font-size: 2.5em; } .header h1 { font-size: 2em; } }";
  html += "</style>";
  html += "<script>";
  html += "function autoRefresh() { setTimeout(function(){ location.reload(); }, 5000); }";
  html += "window.onload = autoRefresh;";
  html += "</script>";
  html += "</head>";
  html += "<body>";
  html += "<div class='container'>";
  
  html += "<div class='header'>";
  html += "<h1>🌱 Plant Monitor</h1>";
  html += "<p>Real-time environmental monitoring</p>";
  html += "</div>";
  
  html += "<div class='card'>";
  
  // Soil Moisture Section
  html += "<div class='sensor-reading'>";
  html += "<div class='sensor-label'>Soil Moisture</div>";
  html += "<div class='sensor-value'>" + String(soilMoisturePercent) + "<span class='sensor-unit'>%</span></div>";
  
  // Status Badge
  String statusClass = "status-moist";
  if (soilMoisturePercent >= 70) statusClass = "status-very-wet";
  else if (soilMoisturePercent >= 50) statusClass = "status-wet";
  else if (soilMoisturePercent >= 30) statusClass = "status-moist";
  else if (soilMoisturePercent >= 15) statusClass = "status-dry";
  else statusClass = "status-very-dry";
  
  html += "<div class='status-badge " + statusClass + "'>" + moistureStatus + "</div>";
  
  // Progress Bar
  html += "<div class='progress-bar'>";
  html += "<div class='progress-fill' style='width: " + String(soilMoisturePercent) + "%'></div>";
  html += "</div>";
  html += "</div>";
  
  
  html += "<div class='divider'></div>";
  
  // Temperature Section
  html += "<div class='sensor-reading'>";
  html += "<div class='sensor-label'>Temperature</div>";
  if (temperatureC != DEVICE_DISCONNECTED_C) {
    html += "<div class='sensor-value'>" + String(temperatureC, 1) + "<span class='sensor-unit'>°C</span></div>";
  } else {
    html += "<div class='sensor-value' style='color: #e74c3c;'>Error</div>";
    html += "<div class='sensor-unit'>Sensor disconnected</div>";
  }
  html += "</div>";
  
  // Refresh Button and Info
  html += "<div class='refresh-info'>";
  html += "<p style='margin-top: 10px;'>Auto-refresh every 5 seconds</p>";
  html += "<p style='margin-top: 5px;'>Raw sensor value: " + String(soilMoistureRaw) + "</p>";
  html += "</div>";
  
  html += "</div>"; // Close card
  html += "</div>"; // Close container
  html += "</body></html>";
  
  return html;
}

// URL root 
void handleRoot() {
  readSensors();
  server.send(200, "text/html", getHTML());
}

// JSON data endpoint
void handleJSON() {
  readSensors();
  String json = "{";
  json += "\"soilMoisture\":" + String(soilMoisturePercent) + ",";
  json += "\"soilMoistureRaw\":" + String(soilMoistureRaw) + ",";
  json += "\"moistureStatus\":\"" + moistureStatus + "\",";
  json += "\"temperature\":" + String(temperatureC) + ",";
  json += "\"temperatureValid\":" + String(temperatureC != DEVICE_DISCONNECTED_C ? "true" : "false");
  json += "}";
  server.send(200, "application/json", json);
}

// Handle 404
void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}

void setup() {
  //Serial Communication
  Serial.begin(115200);
  delay(1200);
  
  Serial.println("\n\n Soil Monitor - WiFi Web Server");
  Serial.println("======================================");
  
  //  DS18B20 sensor
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount());
  Serial.println(" DS18B20 sensor(s)");
  
  analogSetAttenuation(ADC_11db);
  
  // Connecting to WiFi
  Serial.println("\nConnecting to WiFi...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n\n✓ WiFi Connected!");
    Serial.println("================================");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("================================");
    Serial.println("\nOpen this IP address in your mobile browser");
    Serial.println("to view sensor readings!");
    Serial.println();
  } else {
    Serial.println("\n\n✗ WiFi Connection Failed!");
    Serial.println("Please check your credentials and try again.");
  }
  
  //web server routes
  server.on("/", handleRoot);
  server.on("/data", handleJSON);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("Web server started!\n");
}

void
loop() {
    server.handleClient();
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 5000) {
    readSensors();
    Serial.println("=== Current Readings ===");
    Serial.print("Moisture: ");
    Serial.print(soilMoisturePercent);
    Serial.print("% (");
    Serial.print(moistureStatus);
    Serial.println(")");
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" °C");
    Serial.println();
    lastPrint = millis();
  }
}
