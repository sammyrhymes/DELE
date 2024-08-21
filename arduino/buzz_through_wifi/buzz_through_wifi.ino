#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "XRT";
const char* password = "1m@n1D10t";

WebServer server(80);

const int led = 2;  // Built-in LED is usually on GPIO 2
const int buzzer = 13;  // D7 corresponds to GPIO 13
bool ledState = false;
bool buzzerState = false;

void handleRoot() {
  Serial.println("Handling root endpoint");
  String html = "<!DOCTYPE html><html><head><title>Elephant Deterrent</title>";
  html += "<style>";
  html += "body { background-color: #111; color: #fff; font-family: 'Chivo', sans-serif; text-align: center; margin: 0; padding: 0; height: 100vh; display: flex; justify-content: center; align-items: center; }";
  html += ".container { display: flex; flex-direction: column; align-items: center; }";
  html += ".title { color: #800080; font-size: 36px; margin-bottom: 20px; }";
  html += ".button {height:100; width:100; background-color: #800080; border: none; color: white; padding: 40px 40px; text-align: center; text-decoration: none; font-size: 24px; margin: 4px 2px; cursor: pointer; border-radius: 1rem; outline: none; box-shadow: 0 5px 15px rgba(0, 0, 0, 0.3); transition: background-color 0.3s ease; }";
  html += ".button:hover { background-color: #6a006a; }";
  html += "</style></head><body>";
  html += "<div class=\"container\">";
  html += "<h1 class=\"title\">Elephant Deterrent</h1>";
  html += "<button class=\"button\" id=\"deterrentButton\">Activate Deterrent</button>";
  html += "</div>";
  html += "<script>";
  html += "var deterrentState = false;";
  html += "var button = document.getElementById('deterrentButton');";
  html += "button.addEventListener('click', function() {";
  html += "  deterrentState = !deterrentState;";
  html += "  if (deterrentState) {";
  html += "    button.textContent = \"Deactivate Deterrent\";";
  html += "    fetch('/toggleBuzzer')";
  html += "      .then(response => response.text())";
  html += "      .then(data => console.log(data))";
  html += "      .catch(error => console.error('Error:', error));";
  html += "  } else {";
  html += "    button.textContent = \"Activate Deterrent\";";
  html += "    fetch('/toggleBuzzer')";
  html += "      .then(response => response.text())";
  html += "      .then(data => console.log(data))";
  html += "      .catch(error => console.error('Error:', error));";
  html += "  }";
  html += "});";
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}



void handleToggleLed() {
  Serial.println("Toggling LED state");
  ledState = !ledState;
  digitalWrite(led, ledState ? HIGH : LOW);
  server.send(200, "text/plain", ledState ? "LED is ON" : "LED is OFF");
}

void handleToggleBuzzer() {
  Serial.println("Toggling Buzzer state");
  buzzerState = !buzzerState;
  if (buzzerState) {
    ledcAttachPin(buzzer, 0);       // Attach pin to channel 0
    ledcSetup(0, 10000, 8);         // Setup channel 0 with 25 kHz frequency and 8-bit resolution
    ledcWrite(0, 128);              // 50% duty cycle to make the buzzer sound
  } else {
    ledcWrite(0, 0);                // Turn off buzzer
    ledcDetachPin(buzzer);          // Detach pin from channel 0
  }
  server.send(200, "text/plain", buzzerState ? "Buzzer is ON" : "Buzzer is OFF");
}

void handleNotFound() {
  digitalWrite(led, HIGH);
  Serial.println("Handling not found endpoint");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, LOW);
}

void setup(void) {
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(led, LOW);
  digitalWrite(buzzer, LOW);
  Serial.begin(115200);
  Serial.println("Serial communication started");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/toggleLed", handleToggleLed);
  server.on("/toggleBuzzer", handleToggleBuzzer);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2); // Allow the CPU to switch to other tasks
}
