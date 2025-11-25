#ifndef SNAKEGAME_HUD_H
#define SNAKEGAME_HUD_H
#include <SDL_log.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

class HUD {
private:
    TTF_Font* font_;
    SDL_Texture* scoreTexture_;
    SDL_Rect scoreRect_;
    int lastScore_;

    // Statische instruction textures
    SDL_Texture* instrTexture_;
    SDL_Rect instrRect_;

    SDL_Color scoreColor_;
    SDL_Color instrColor_;

    // Helfer zum Erstellen einer Textur aus Text, outRect.w/h wird aus der Oberfläche festgelegt
    SDL_Texture* createTextTexture(SDL_Renderer* renderer, const std::string& text, SDL_Rect& outRect, SDL_Color color);

public:
    HUD();
    ~HUD();

    // Aufruf nach Initialisierung von SDL und TTF, Renderer ist gültig
    bool init(SDL_Renderer* renderer, const std::string& fontPath, int fontSize);

    // Rufen Sie an, wenn sich der score ändert
    void updateScore(int newScore, SDL_Renderer* renderer, int windowWidth);


    void render(SDL_Renderer* renderer, int windowWidth, int windowHeight);

    // Cleanup Ressourcen (optional, wenn der Destruktor dies übernimmt)
    void cleanup();


};
#endif //SNAKEGAME_HUD_H
