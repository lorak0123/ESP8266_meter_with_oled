#ifndef LIGHTSCREEN_H
#define LIGHTSCREEN_H

#include "MenuScreen.h"

class LightScreen : public MenuScreen {
  private:
    SensorData<int>* lightDataReadings;

  public:
    MenuScreen* nextScreen = nullptr;
    MenuScreen* prevScreen = nullptr;

    LightScreen(Adafruit_SSD1306* display, SensorData<int>* lightDataReadings) : MenuScreen(display) {
        this->lightDataReadings = lightDataReadings;
    }

    void displayScreen() override {
        display->setTextSize(2);
        display->setCursor(35, 0);
        display->print("LIGHT");
        display->drawLine(0, 16, 128, 16, WHITE);
        display->setCursor(20, 30);
        int* value = lightDataReadings->getLastReadings();
        if (value){
          display->print(value[0]);
          display->print(" lux");
        }
        else{
          display->print("NO DATA!");
        }
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
