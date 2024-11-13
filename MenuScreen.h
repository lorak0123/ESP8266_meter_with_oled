#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <Adafruit_SSD1306.h>

class MenuScreen {
  public:
    // Konstruktor, aby powiązać ekran OLED z ekranem menu
    MenuScreen(Adafruit_SSD1306* display) : display(display) {}

    // Metoda wirtualna do wyświetlania ekranu - implementowana przez klasy podrzędne
    virtual void displayScreen() = 0;

    // Metoda nawigacji - prawo, lewo są obsługiwane na poziomie DisplayManager
    virtual MenuScreen* navigateLeft() = 0;
    virtual MenuScreen* navigateRight() = 0;

    // Metody nawigacji obsługiwane przez klasę podrzędną
    virtual MenuScreen* navigateUp() = 0;
    virtual MenuScreen* navigateDown() = 0;
    virtual MenuScreen* enter() = 0; // Wchodzi głębiej w ekran

  protected:
    Adafruit_SSD1306* display; // Wskaźnik do obiektu wyświetlacza OLED
};

#endif
