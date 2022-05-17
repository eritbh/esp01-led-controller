#include <Adafruit_NeoPixel.h>

#include "Neopixel.hpp"
#include "Routing.hpp"
#include "Secrets.hpp"


void setup() {
  Serial.begin(9600);

  // Initialize wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Connect Failed! Rebooting after 1 second...");
    delay(1000);
    ESP.restart();
  }

  // Set up LED strip
  pixels.begin();

  // Start the HTTPS server
  beginRouting();

  Serial.print("Open https://");
  Serial.print(WiFi.localIP());
  Serial.println("/ in your browser to see it working");
}

void loop() {
  // Handle web requests
  handleRouting();
  yield();

  // Handle LEDs if enough time has passed
  static unsigned long lastUpdate = millis();
  unsigned long now = millis();
  if (now > lastUpdate + DELAY) {
    lastUpdate = now;
    updateLEDs();
  }
}
