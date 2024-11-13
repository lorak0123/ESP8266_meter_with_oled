#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Adafruit_SSD1306.h>
#include "MenuScreen.h"

class DisplayManager {
  private:
    Adafruit_SSD1306* display;
    MenuScreen* currentScreen;
    unsigned long lastDisplayUpdate;
    unsigned int displayOnTime;
    bool displayOn;

  public:
    DisplayManager(Adafruit_SSD1306* oledDisplay, MenuScreen* initialScreen, unsigned int displayOnTime = 15000)
        : display(oledDisplay), currentScreen(initialScreen), displayOnTime(displayOnTime) {
        lastDisplayUpdate = 0;
        displayOn = true;
    }

    // Wyświetlanie aktywnego ekranu
    void updateDisplay() {
        unsigned long currentMillis = millis();
        if (isDisplayOn()){
            displayOn = true;
            display->clearDisplay();
            display->setTextColor(WHITE);
            currentScreen->displayScreen();
            display->display();
        } else {
            displayOn = false;
            display->clearDisplay();
            display->display();
        }
    }

    // Obsługa włączania i wyłączania wyświetlacza
    bool isDisplayOn() {
        unsigned long currentMillis = millis();
        if (currentMillis - lastDisplayUpdate < displayOnTime){
            displayOn = true;
        } else {
            displayOn = false;
        }
        return displayOn;
    }

    // Obsługa aktualizacji ekranu
    bool shouldUpdateDisplayAfterButtonPress() {
        bool actualDisplayOn = displayOn;
        lastDisplayUpdate = millis();

        // Jeśli ekran był wyłączony, nie aktualizuj ekranu
        if (actualDisplayOn != isDisplayOn() && !actualDisplayOn) {
            return false;
        }
        return true;

    }

    // Obsługa nawigacji w prawo i lewo - przełącza ekrany menu
    void navigateLeft() {
        if (shouldUpdateDisplayAfterButtonPress()) {
            MenuScreen* nextScreen = currentScreen->navigateLeft();
            if (nextScreen) currentScreen = nextScreen;
        }
        updateDisplay();
    }

    void navigateRight() {
        if (shouldUpdateDisplayAfterButtonPress()) {
            MenuScreen* nextScreen = currentScreen->navigateRight();
            if (nextScreen) currentScreen = nextScreen;
        }
        updateDisplay();
    }

    // Obsługa nawigacji w górę i dół - obsługiwana przez aktualny ekran
    void navigateUp() {
        if (shouldUpdateDisplayAfterButtonPress()) {
            MenuScreen* nextScreen = currentScreen->navigateUp();
            if (nextScreen) currentScreen = nextScreen;
        }
        updateDisplay();
    }

    void navigateDown() {
        if (shouldUpdateDisplayAfterButtonPress()) {
            MenuScreen* nextScreen = currentScreen->navigateDown();
            if (nextScreen) currentScreen = nextScreen;
        }
        updateDisplay();
    }

    // Obsługa wejścia do podmenu (w głąb)
    void enter() {
        if (shouldUpdateDisplayAfterButtonPress()) {
            MenuScreen* nextScreen = currentScreen->enter();
            if (nextScreen) currentScreen = nextScreen;
        }
        updateDisplay();
    }
};

#endif
