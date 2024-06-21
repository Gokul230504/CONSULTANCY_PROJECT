#include <WiFi.h>
#include <WebServer.h>

#define Sensor9 12

float t, t1, t2;
float windSpeed;

bool state = true;
bool count = true;

int S9 = 0;

const char* ssid     = "Goks Iqoo z7 pro";
const char* password = "123@Gokul#";

WebServer server(80);

void setup() {
  Serial.begin(115200);  // start serial for output
  pinMode(Sensor9, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Define server endpoints
  server.on("/", HTTP_GET, handleRoot);
  server.on("/windSpeed", HTTP_GET, handleWindSpeed);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
  Serial.println(WiFi.localIP());
}

void loop() {
  readSensor();
  
  if (state == 0) {
    if (count == 0) {
      if (S9 == 1) {
        t2 = millis();
        t = t2 - t1;
        t = t / 1000;
        windSpeed = (2 * 3.1416 * 0.00001 * 3600) / t ;
        count = true;
      }
    } else {
      if (S9 == 0) {
        t1 = millis();
        count = false;
      }
    }
  } else {
    if (S9 == 1) {
      state = 0;
    }
  }

  // Handle incoming client requests
  server.handleClient();
}

void readSensor() {
  S9 = digitalRead(Sensor9);
}

void handleRoot() {
  String content = "<html><head><style>body { font-size: 30px; font-family: 'Arial Black', Gadget, sans-serif; } .navbar { background-color: #333; overflow: hidden; } .navbar a { float: left; display: block; color: #f2f2f2; text-align: center; padding: 14px 20px; text-decoration: none; font-size: 36px; } .navbar a:hover { background-color: #ddd; color: black; } .content { padding: 16px; } </style></head><body>";
  content += "<div class='navbar'><a href='#'>V-Guard Industries Wind Parameter Monitor</a></div>";
  content += "<div class='content'><h1>Welcome to ESP32 Wind Speed Monitor</h1>";
  content += "<div style='width: 200px; height: 100px; background-color: #FFA500; text-align: center; padding-top: 30px; margin-top: 20px;'>";
  content += "<span id='windSpeed'>Loading...</span> m/s</div>";
  content += "<script>setInterval(function() { fetch('/windSpeed').then(response => response.text()).then(data => document.getElementById('windSpeed').innerText = data); }, 1000);</script>";
  content += "</div></body></html>";
  
  server.send(200, "text/html", content);
}


void handleWindSpeed() {
  char buffer[10];
  dtostrf(windSpeed, 4, 2, buffer); // Convert windSpeed to string
  server.send(200, "text/plain", buffer);
}
