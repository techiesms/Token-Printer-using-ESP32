#include <WiFi.h>
#include <WebServer.h>
#include "Adafruit_Thermal.h"


// WiFi credentials
const char* ssid = "SSID";
const char* password = "PASS";


Adafruit_Thermal printer(&Serial2);

// Web Server on port 80
WebServer server(80);

void handleRoot() {
  // HTML for the webpage
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>Token Printer</title>
    </head>
    <body>
      <h2>Enter Token Number</h2>
      <form action="/print" method="GET">
        <label for="token">Token Number:</label>
        <input type="number" id="token" name="token" required>
        <input type="submit" value="Print">
      </form>
    </body>
    </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handlePrint() {
  // Get the token number from the GET request
  String token = server.arg("token");
  
  // Print the token number to the thermal printer
  printer.wake(); // Wake up the printer
  printer.println("Token Number: " + token);
  printer.println("");
  printer.feed(2); // Feed paper for 2 lines
  printer.sleep(); // Put printer to sleep

  // Send back the response with the input form still available
  server.send(200, "text/html",
    "<h1>Token Printed!</h1>"
    "<form action=\"/print\" method=\"get\">"
    "Token Number: <input type=\"text\" name=\"token\"><br><br>"
    "<input type=\"submit\" value=\"Print Token\">"
    "</form>");
}


void setup() {
  Serial.begin(115200);

  // Initialize Thermal Printer
  Serial2.begin(9600);
  printer.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // Print IP address on the serial monitor

  // Start Web Server
  server.on("/", handleRoot);
  server.on("/print", handlePrint);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
