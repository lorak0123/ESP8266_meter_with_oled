#ifndef INFOSCREEN_H
#define INFOSCREEN_H

#include "MenuScreen.h"
#include <ESP8266WiFi.h>

#define NUMBER_OF_OPTIONS 7
#define ON_SCREEN_OPTIONS 3

class InfoScreen : public MenuScreen {
  private:
    ESP8266WiFiClass *wifi;
    unsigned short screen_slider = 5;

  bool setCursorForOption(unsigned short option, unsigned short screen_shift = 0) {
    if (option < screen_slider || option >= screen_slider + ON_SCREEN_OPTIONS) {
      return false;
    }
    display->setCursor(screen_shift, 25 + 12 * (option - screen_slider));
    return true;
  }

  public:
    MenuScreen* nextScreen = nullptr;
    MenuScreen* prevScreen = nullptr;

    InfoScreen(Adafruit_SSD1306* display, ESP8266WiFiClass* wifi) : MenuScreen(display) {
        this->wifi = wifi;
    }

    void displayScreen() override {
        display->setTextSize(2);
        display->setCursor(35, 0);
        display->print("INFO");
        display->drawLine(0, 16, 128, 16, WHITE);

        display->setTextSize(1);
        if (setCursorForOption(0)) {
          display->print("WIFI status");
        }
        if (setCursorForOption(1, 10)) {
          display->print(WiFi.localIP());
        }
        if (setCursorForOption(2, 10)) {
          display->print(wifi->SSID());
        }
        if (setCursorForOption(3, 10)) {
          display->print(wifi->RSSI());
          display->print(" dBm");
        }
        if (setCursorForOption(4, 10)) {
          display->print(wifi->BSSIDstr());
        }

        if (setCursorForOption(5)) {
          display->print("Connected: ");
          display->print(wifi->isConnected() ? "yes" : "no");
        }

        if (setCursorForOption(6)) {
          display->print("WiFi mode: ");
          WiFiMode_t mode = wifi->getMode();
          if (mode == WIFI_OFF) {
            display->print("Sleep");
          } else if (mode == WIFI_STA) {
            display->print("STA");
          } else if (mode == WIFI_AP) {
            display->print("AP");
          } else if (mode == WIFI_AP_STA) {
            display->print("AP_STA");
          }
        }

        if (setCursorForOption(7)) {
          display->print("Working time: ");
          display->print(int(millis() / 60000));
          display->print(" min");
        }
    }

    MenuScreen* navigateLeft() override {
      return prevScreen;
    }
    MenuScreen* navigateRight() override {
      return nextScreen;
    }

    MenuScreen* navigateUp() override {
        screen_slider = (screen_slider + NUMBER_OF_OPTIONS - 1) % NUMBER_OF_OPTIONS;
        return nullptr;
    }

    MenuScreen* navigateDown() override {
        screen_slider = (screen_slider + 1) % NUMBER_OF_OPTIONS;
        return nullptr;
    }

    MenuScreen* enter() override {
        return nullptr;
    }
};

#endif
