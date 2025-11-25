// SDL Snake Spiel - main.cpp

// Wenn Sie die Hauptbehandlung von SDL umgehen möchten, heben Sie die Kommentierung der nächsten Zeile auf:
// #define SDL_MAIN_HANDLED

#include <SDL.h>
#include "game.h"
#include <iostream>

int main(int argc, char* argv[]) {
#ifdef SDL_MAIN_HANDLED
    SDL_SetMainReady();
#endif

    // Spiel grid Dimensionen
    const int GRID_BREITE = 40;
    const int GRID_HOHE = 30;

    // Fenster Dimensionen
    const int FENSTER_BREITE = 800;
    const int FENSTER_HOHE = 600;

    try {
        Game spiel(GRID_BREITE, GRID_HOHE, FENSTER_BREITE, FENSTER_HOHE);

        std::cout << "Starte SDL Schlangen Spiel..." << std::endl;
        std::cout << "Steuerung:" << std::endl;
        std::cout << "  W/Pfeil Hoch  - Nach Oben" << std::endl;
        std::cout << "  S/Pfeil Runter- Nach Unten" << std::endl;
        std::cout << "  A/Pfeil Links - Nach Links" << std::endl;
        std::cout << "  D/Pfeil Rechts- Nach Rechts" << std::endl;
        std::cout << "  P             - Pause/Fortsetzen" << std::endl;
        std::cout << "  R             - Neustart (nach Game Over)" << std::endl;
        std::cout << "  Q/Escape      - Beenden" << std::endl;
        std::cout << std::endl;

        spiel.run();

    } catch (const std::exception& e) {
        std::cerr << "Fehler: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Spiel beendet. Danke fuers Spielen!" << std::endl;
    return 0;
}
