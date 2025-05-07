#include <PZEM004Tv30.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Display settings
#define DISPLAY_ADDRESS   0x3C
#define DISPLAY_SCL_PIN   5  // D1 on NodeMCU
#define DISPLAY_SDA_PIN   4  // D2 on NodeMCU
#define DISPLAY_WIDTH     128
#define DISPLAY_HEIGHT    64
#define DISPLAY_RESET_PIN -1

// Relay pin
#define RELAY_1 13  // GPIO13 (D7)

ESP8266WebServer server(80);

Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, DISPLAY_RESET_PIN);

PZEM004Tv30 pzem1(14, 12); // GPIO14(D5) to Tx PZEM004; GPIO12(D6) to Rx PZEM004

float voltage1 = 0.0;
float current1 = 0.0;
float power1 = 0.0;
float energy1 = 0.0;
float frequency1 = 0.0;
float pf1 = 0.0;
float va1 = 0.0;
float VAR1 = 0.0;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP8266 Power Monitor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css">
  <style>
    body { 
      font-family: Arial, sans-serif; 
      margin: 0;
      padding: 20px;
      background-color: #f0f0f0;
    }
    .grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
      gap: 20px;
      max-width: 1200px;
      margin: 0 auto;
    }
    .card {
      background: white;
      border-radius: 15px;
      padding: 25px;
      box-shadow: 0 4px 8px rgba(0,0,0,0.1);
      display: flex;
      align-items: center;
      text-align: left;
    }
    .icon {
      font-size: 40px;
      margin-right: 25px;
      min-width: 50px;
      text-align: center;
    }
    .content {
      display: flex;
      flex-direction: column;
    }
    .label {
      font-size: 16px;
      color: #7f8c8d;
      margin-bottom: 5px;
      font-weight: bold;
    }
    .value {
      font-size: 24px;
      color: #2c3e50;
      display: flex;
      align-items: baseline;
    }
    .unit {
      font-size: 16px;
      color: #95a5a6;
      margin-left: 5px;
    }
    .fa-bolt { color: #f1c40f; }
    .fa-exchange-alt { color: #3498db; }
    .fa-plug { color: #e74c3c; }
    .fa-chart-line { color: #2ecc71; }
    .fa-wave-square { color: #9b59b6; }
    .fa-percent { color: #e67e22; }
    .fa-lightbulb { color: #e74c3c; }
    .fa-charging-station { color: #3498db; }
    h1 {
      text-align: center;
      margin: 30px 0;
      color: #2c3e50;
    }
    .status {
      margin-top: 30px;
      text-align: center;
    }
    .relay-status {
      display: inline-block;
      padding: 10px 20px;
      border-radius: 10px;
      font-weight: bold;
      font-size: 18px;
      margin-top: 10px;
    }
    .on {
      background-color: #e74c3c;
      color: white;
    }
    .off {
      background-color: #2ecc71;
      color: white;
    }
  </style>
  <script>
    function updateData() {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var data = JSON.parse(this.responseText);
          document.getElementById('voltage').innerHTML = data.voltage + '<span class="unit">V</span>';
          document.getElementById('current').innerHTML = data.current + '<span class="unit">A</span>';
          document.getElementById('power').innerHTML = data.power + '<span class="unit">W</span>';
          document.getElementById('energy').innerHTML = data.energy + '<span class="unit">kWh</span>';
          document.getElementById('frequency').innerHTML = data.frequency + '<span class="unit">Hz</span>';
          document.getElementById('pf').innerHTML = data.pf;
          document.getElementById('va').innerHTML = data.va + '<span class="unit">VA</span>';
          document.getElementById('var').innerHTML = data.var + '<span class="unit">VAR</span>';
          
          // Update relay status
          var relayStatus = document.getElementById('relay-status');
          if (data.relay == "ON") {
            relayStatus.className = "relay-status on";
            relayStatus.innerHTML = "RELAY: ON";
          } else {
            relayStatus.className = "relay-status off";
            relayStatus.innerHTML = "RELAY: OFF";
          }
        }
      };
      xhttp.open("GET", "/data", true);
      xhttp.send();
    }
    setInterval(updateData, 2000);
    window.onload = updateData;
  </script>
</head>
<body>
  <h1><i class="fas fa-plug"></i> ESP8266 Power Monitor</h1>
  <div class="grid">
    <div class="card">
      <i class="fas fa-bolt icon"></i>
      <div class="content">
        <div class="label">VOLTAGE</div>
        <div class="value" id="voltage">%VOLTAGE%<span class="unit">V</span></div>
      </div>
    </div>
    <div class="card">
      <i class="fas fa-exchange-alt icon"></i>
      <div class="content">
        <div class="label">CURRENT</div>
        <div class="value" id="current">%CURRENT%<span class="unit">A</span></div>
      </div>
    </div>
    <div class="card">
      <i class="fas fa-plug icon"></i>
      <div class="content">
        <div class="label">ACTIVE POWER</div>
        <div class="value" id="power">%POWER%<span class="unit">W</span></div>
      </div>
    </div>
    <div class="card">
      <i class="fas fa-charging-station icon"></i>
      <div class="content">
        <div class="label">APPARENT POWER</div>
        <div class="value" id="va">%VA%<span class="unit">VA</span></div>
      </div>
    </div>
    <div class="card">
      <i class="fas fa-lightbulb icon"></i>
      <div class="content">
        <div class="label">REACTIVE POWER</div>
        <div class="value" id="var">%VAR%<span class="unit">VAR</span></div>
      </div>
    </div>
    <div class="card">
      <i class="fas fa-chart-line icon"></i>
      <div class="content">
        <div class="label">ENERGY</div>
        <div class="value" id="energy">%ENERGY%<span class="unit">kWh</span></div>
      </div>
    </div>
    <div class="card">
      <i class="fas fa-wave-square icon"></i>
      <div class="content">
        <div class="label">FREQUENCY</div>
        <div class="value" id="frequency">%FREQUENCY%<span class="unit">Hz</span></div>
      </div>
    </div>
    <div class="card">
      <i class="fas fa-percent icon"></i>
      <div class="content">
        <div class="label">POWER FACTOR</div>
        <div class="value" id="pf">%PF%</div>
      </div>
    </div>
  </div>
  <div class="status">
    <h2>Current Relay Status</h2>
    <div class="relay-status %RELAY_CLASS%" id="relay-status">RELAY: %RELAY_STATUS%</div>
  </div>
</body>
</html>
)rawliteral";

// Function prototypes - must be declared before they're used
float zeroIfNan(float v);
void setupDisplay();
void setupWiFi();
void setupWebServer();
void readPZEMValues();
void controlRelay();
void updateDisplay();
String processor(const String& var);

// Convert NaN values to zero
float zeroIfNan(float v) {
  return isnan(v) ? 0 : v;
}

String processor(const String& var) {
  if(var == "VOLTAGE") return isnan(voltage1) ? "Error" : String(voltage1, 2);
  else if(var == "CURRENT") return isnan(current1) ? "Error" : String(current1, 2);
  else if(var == "POWER") return isnan(power1) ? "Error" : String(power1, 2);
  else if(var == "ENERGY") return isnan(energy1) ? "Error" : String(energy1, 3);
  else if(var == "FREQUENCY") return isnan(frequency1) ? "Error" : String(frequency1, 1);
  else if(var == "PF") return isnan(pf1) ? "Error" : String(pf1, 2);
  else if(var == "VA") return isnan(va1) ? "Error" : String(va1, 2);
  else if(var == "VAR") return isnan(VAR1) ? "Error" : String(VAR1, 2);
  else if(var == "RELAY_STATUS") return digitalRead(RELAY_1) == LOW ? "ON" : "OFF";
  else if(var == "RELAY_CLASS") return digitalRead(RELAY_1) == LOW ? "on" : "off";
  return String();
}

void setupDisplay() {
  Wire.begin(DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
  display.clearDisplay();
  display.setCursor(10, 0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println("PZEM-004T");
  display.setCursor(10, 36);
  display.setTextSize(1);
  display.println("Starting...");
  display.display();
  delay(1000);
}

void setupWiFi() {
  // Connect to WiFi
  WiFi.begin(ssid, password);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Connecting to WiFi");
  display.display();
  
  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi Connected!");
    display.println("IP Address:");
    display.println(WiFi.localIP());
    display.display();
    delay(2000);
  } else {
    Serial.println("\nFailed to connect to WiFi");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi Failed!");
    display.println("Check credentials");
    display.display();
    delay(2000);
  }
}

// Set up web server routes
void setupWebServer() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](){
    server.send(200, "text/html", index_html);
  });

  // Route for JSON data
  server.on("/data", HTTP_GET, [](){
    String json = "{";
    json += "\"voltage\":\"" + String(isnan(voltage1) ? "0.00" : String(voltage1, 2)) + "\",";
    json += "\"current\":\"" + String(isnan(current1) ? "0.00" : String(current1, 2)) + "\",";
    json += "\"power\":\"" + String(isnan(power1) ? "0.00" : String(power1, 2)) + "\",";
    json += "\"energy\":\"" + String(isnan(energy1) ? "0.000" : String(energy1, 3)) + "\",";
    json += "\"frequency\":\"" + String(isnan(frequency1) ? "0.0" : String(frequency1, 1)) + "\",";
    json += "\"pf\":\"" + String(isnan(pf1) ? "0.00" : String(pf1, 2)) + "\",";
    json += "\"va\":\"" + String(isnan(va1) ? "0.00" : String(va1, 2)) + "\",";
    json += "\"var\":\"" + String(isnan(VAR1) ? "0.00" : String(VAR1, 2)) + "\",";
    json += "\"relay\":\"" + String(digitalRead(RELAY_1) == LOW ? "ON" : "OFF") + "\"";
    json += "}";
    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void readPZEMValues() {
  voltage1 = zeroIfNan(pzem1.voltage());
  current1 = zeroIfNan(pzem1.current());
  power1 = zeroIfNan(pzem1.power());
  energy1 = zeroIfNan(pzem1.energy()) / 1000;
  frequency1 = zeroIfNan(pzem1.frequency());
  pf1 = zeroIfNan(pzem1.pf());

  if (pf1 == 0) {
    va1 = 0;
    VAR1 = 0;
  } else {
    va1 = power1 / pf1;
    VAR1 = va1 * sqrt(1 - sq(pf1));
  }

  Serial.println("");
  Serial.printf("Voltage        : %.2f V\n", voltage1);
  Serial.printf("Current        : %.2f A\n", current1);
  Serial.printf("Power Active   : %.2f W\n", power1);
  Serial.printf("Energy         : %.2f kWh\n", energy1);
  Serial.printf("Frequency      : %.2f Hz\n", frequency1);
  Serial.printf("Cosine Phi     : %.2f PF\n", pf1);
  Serial.printf("Apparent Power : %.2f VA\n", va1);
  Serial.printf("Reactive Power : %.2f VAR\n", VAR1);
  Serial.printf("---------- END ----------\n");
}

void controlRelay() {
  if (current1 >= 1.0) {
    digitalWrite(RELAY_1, LOW);
    Serial.println("⚠️ Arus tinggi! Relay 1 ON.");
  } else {
    digitalWrite(RELAY_1, HIGH);
    Serial.println("Relay 1 OFF");
  }
}


void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.printf("Voltage  : %.2f V\n", voltage1);
  display.printf("Current  : %.2f A\n", current1);
  display.printf("Power RMS: %.2f W\n", power1);
  display.printf("Energy   : %.1f kWh\n", energy1);
  display.printf("Apprn Pwr: %.1f VA\n", va1);
  display.printf("Cos Phi  : %.2f PF\n", pf1);
  display.printf("React Pwr: %.1f VAR\n", VAR1);
  display.printf("Frequency: %.1f Hz\n", frequency1);
  display.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_1, OUTPUT);
  digitalWrite(RELAY_1, HIGH); // Relay default NONAKTIF (OFF)
  
  setupDisplay();
  setupWiFi();
  setupWebServer();
}

void loop() {
  // Handle web client requests
  server.handleClient();
  
  // Read PZEM-004T values
  readPZEMValues();
  
  // Control relay based on current
  controlRelay();
  
  // Update OLED display
  updateDisplay();
  
  delay(1000); // Main loop delay
}
