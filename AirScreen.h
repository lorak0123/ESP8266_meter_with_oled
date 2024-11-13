#ifndef AIRSCREEN_H
#define AIRSCREEN_H

#include "MenuScreen.h"

class AirScreen : public MenuScreen {
  private:
    SensorData<float>* airDataReadings;

  public:
    MenuScreen* nextScreen = nullptr;
    MenuScreen* prevScreen = nullptr;

    AirScreen(Adafruit_SSD1306* display, SensorData<float>* airDataReadings) : MenuScreen(display) {
        this->airDataReadings = airDataReadings;
    }

    void displayScreen() override {
        display->setTextSize(2);
        display->setCursor(45, 0);
        display->print("AIR");
        display->drawLine(0, 16, 128, 16, WHITE);

        float* values = airDataReadings->getLastReadings();

        if (!values) {
            display->setCursor(20, 30);
            display->print("NO DATA!");
            return;
        }
        display->setTextSize(1);
        display->setCursor(0, 20);
        display->print("Temp: ");
        display->print(values[0]);
        display->print(" C");
        display->setCursor(0, 30);
        display->print("Humidity: ");
        display->print(values[1]);
        display->print(" %");
        display->setCursor(0, 40);
        display->print("eCO2: ");
        display->print(values[2]);
        display->print(" ppm");
        display->setCursor(0, 50);
        display->print("TVOC: ");
        display->print(values[3]);
        display->print(" ppb");
    }

    MenuScreen* navigateLeft() override {
      return prevScreen;
    }
    MenuScreen* navigateRight() override {
      return nextScreen;
    }

    MenuScreen* navigateUp() override {
        return nullptr;
    }

    MenuScreen* navigateDown() override {
        return nullptr;
    }

    MenuScreen* enter() override {
        return nullptr;
    }
};

#endif
