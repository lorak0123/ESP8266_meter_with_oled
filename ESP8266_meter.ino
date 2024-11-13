#include "SensorData.h"
#include "Secrets.h"
#include "Config.h"
#include "IntervalJob.h"
#include <Adafruit_SSD1306.h>
#include "DisplayManager.h"
#include "AirScreen.h"
#include "LightScreen.h"
#include "InfoScreen.h"
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include "ScioSense_ENS160.h"
#include <ESP8266WiFi.h>
#include <include/WiFiState.h>
#include <MQTT.h>

ScioSense_ENS160 ens160(ENS160_I2CADDR_1);
Adafruit_AHTX0 aht;
WiFiClient wifiClient;
WiFiState wifiState;
MQTTClient mqttClient;

// Create a SensorData object for the air sensor
SensorData<float> airDataReadings(20, 4);
// Create a SensorData object for the light sensor
SensorData<int> lightDataReadings(5, 1);

// Create a Adafruit_SSD1306 object for the display
Adafruit_SSD1306 display(128, 64, &Wire, -1);


AirScreen airScreen(&display, &airDataReadings);
LightScreen lightScreen(&display, &lightDataReadings);
InfoScreen infoScreen(&display, &WiFi);

DisplayManager displayManager(&display, &airScreen);

IntervalJob screenUpdateJob(DISPLAY_UPDATE_TIME, []() {
  displayManager.updateDisplay();
});

IntervalJob lightDataCollectionJob(LIGHT_DATA_COLLECTION_TIME, []() {
  int lightValue[] = {analogRead(INPUT_LIGHT)};
  lightDataReadings.addReadings(lightValue);
});

IntervalJob mqttClientLoopJob(1000, []() {
  mqttClient.loop();
});


bool publishData() {
  if (WiFi.getMode() == WIFI_OFF) {
    WiFi.forceSleepWake();
  }

  if (!mqttClient.connected()){
    return false;
  }

  float* values = airDataReadings.getAverage();
  if (values) {
    mqttClient.publish("home/" DEVICE_NAME "/temperature", String(values[0]));
    mqttClient.publish("home/" DEVICE_NAME "/humidity",  String(values[1]));
    mqttClient.publish("home/" DEVICE_NAME "/eCO2",  String(values[2]));
    mqttClient.publish("home/" DEVICE_NAME "/TVOC",  String(values[3]));
  }

  int* value = lightDataReadings.getAverage();
  if (value) {
    mqttClient.publish("home/" DEVICE_NAME "/light", String(value[0]));
  }

  // delay to allow the data to be sent
  new IntervalJob(3000, []() {
    WiFi.forceSleepBegin();
  }, 0, true);

  return true;
}

IntervalJob publishDataJob(MQTT_DATA_UPDATE_TIME, publishData, 1000);

IntervalJob airDataCollectionJob(AIR_DATA_COLLECTION_TIME, []() {
  sensors_event_t humidity1, temp;
  aht.getEvent(&humidity1, &temp);
  float tempC = (temp.temperature);
  float humidity = (humidity1.relative_humidity);

  ens160.set_envdata(tempC, humidity);

  ens160.measure(true);
  ens160.measureRaw(true);

  float TVOC = ens160.getTVOC();
  float eCO2 = ens160.geteCO2();

  float airValues[] = {tempC, humidity, eCO2, TVOC};
  airDataReadings.addReadings(airValues);
});


void set_connection() {
    if (WiFi.status() == WL_CONNECTED) {
        if (!mqttClient.connected()){
            mqttClient.connect(DEVICE_NAME, MQTT_USER, MQTT_PASS);
        }
    }
}

IntervalJob connectionJob(1000, set_connection);


void setup() {
  pinMode(INPUT_BUTTON_LEFT, INPUT);
  pinMode(INPUT_BUTTON_RIGHT, INPUT);
  pinMode(INPUT_BUTTON_UP, INPUT);
  pinMode(INPUT_BUTTON_DOWN, INPUT);
  pinMode(INPUT_BUTTON_ENTER, INPUT);

  airScreen.nextScreen = &lightScreen;
  airScreen.prevScreen = &infoScreen;
  lightScreen.nextScreen = &infoScreen;
  lightScreen.prevScreen = &airScreen;
  infoScreen.nextScreen = &airScreen;
  infoScreen.prevScreen = &lightScreen;

  Wire.begin(14, 12); // SDA, SCL

  Serial.begin(9600);

  // setup display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();

  ens160.begin();
  Serial.println(ens160.available() ? "done." : "failed!");
  if (ens160.available()) {
    Serial.println(ens160.setMode(ENS160_OPMODE_STD) ? "done." : "failed!");
  }

  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  mqttClient.begin(MQTT_BROKER, MQTT_PORT, wifiClient);
  WiFi.forceSleepBegin();
}

void checkButtons() {
  if (digitalRead(INPUT_BUTTON_LEFT) == HIGH) {
    displayManager.navigateLeft();
    delay(200);
  }
  if (digitalRead(INPUT_BUTTON_RIGHT) == HIGH) {
    displayManager.navigateRight();
    delay(200);
  }
  if (digitalRead(INPUT_BUTTON_UP) == HIGH) {
    displayManager.navigateUp();
    delay(200);
  }
  if (digitalRead(INPUT_BUTTON_DOWN) == HIGH) {
    displayManager.navigateDown();
    delay(200);
  }
  if (digitalRead(INPUT_BUTTON_ENTER) == HIGH) {
    displayManager.enter();
    delay(200);
  }
}

void loop() {
    checkButtons();
    IntervalJob::runAll();
}
