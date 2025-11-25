#include "HUD.h"

HUD::HUD()
        : font_(nullptr), scoreTexture_(nullptr), lastScore_(-1),
          scoreColor_{255, 255, 0, 255}, instrColor_{255, 255, 255, 255},instrTexture_(nullptr)
{}

HUD::~HUD() {
    cleanup();
}

bool HUD::init(SDL_Renderer* renderer, const std::string& fontPath, int fontSize) {
    font_ = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font_) {
        SDL_Log("HUD: Failed to load font %s: %s", fontPath.c_str(), TTF_GetError());
        return false;
    }
    std::string combined = "W/A/S/D or Arrows: Move   "
                           "SPACE: Toggle AutoPlay   "
                           "P: Pause/Resume   "
                           "1: BFS, 2: Dijkstra (Toggle Algo)   "
                           "R: Restart (after Game Over)   "
                           "Esc/Q: Quit";
    // Erzeuge eine Texture:
    SDL_Rect rect;
    instrTexture_ = createTextTexture(renderer, combined, rect, instrColor_);
    if (instrTexture_) {
        instrRect_ = rect;
        instrRect_.x = 0;
        instrRect_.y = 570; // oder wo immer du sie anzeigen willst
    } else {
        SDL_Log("HUD: Failed to create combined instruction texture");
    }
    return true;
}

SDL_Texture* HUD::createTextTexture(SDL_Renderer* renderer, const std::string& text, SDL_Rect& outRect, SDL_Color color) {
    SDL_Surface* surf = TTF_RenderText_Blended(font_, text.c_str(), color);
    if (!surf) {
        SDL_Log("HUD: TTF_RenderText_Blended error: %s", TTF_GetError());
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (!tex) {
        SDL_Log("HUD: SDL_CreateTextureFromSurface error: %s", SDL_GetError());
        SDL_FreeSurface(surf);
        return nullptr;
    }
    outRect.w = surf->w;
    outRect.h = surf->h;
    SDL_FreeSurface(surf);
    return tex;
}

void HUD::updateScore(int newScore, SDL_Renderer* renderer, int windowWidth) {
    if (newScore == lastScore_) return;
    lastScore_ = newScore;
    // Destroy old
    if (scoreTexture_) {
        SDL_DestroyTexture(scoreTexture_);
        scoreTexture_ = nullptr;
    }
    char buf[64];
    std::snprintf(buf, sizeof(buf), "Score: %d", newScore);
    SDL_Rect rect;
    SDL_Texture* tex = createTextTexture(renderer, buf, rect, scoreColor_);
    if (tex) {
        scoreTexture_ = tex;
        scoreRect_ = rect;
        // Die X-Position wird in render() basierend auf der aktuellen windowWidth festgelegt
    }
}

void HUD::render(SDL_Renderer* renderer, int windowWidth, int windowHeight) {
    // Rendern statische instructions
    if(instrTexture_) {
        SDL_RenderCopy(renderer, instrTexture_, nullptr, &instrRect_);
    }
    // Rendern score nach oben-recht mit 10px margin
    if (scoreTexture_) {
        scoreRect_.x = windowWidth - scoreRect_.w - 10;
        scoreRect_.y = 10;
        SDL_RenderCopy(renderer, scoreTexture_, nullptr, &scoreRect_);
    }
}

void HUD::cleanup() {
    if (scoreTexture_) {
        SDL_DestroyTexture(scoreTexture_);
        scoreTexture_ = nullptr;
    }
    if (instrTexture_) {
        SDL_DestroyTexture(instrTexture_);
        instrTexture_ = nullptr;
    }
    if (font_) {
        TTF_CloseFont(font_);
        font_ = nullptr;
    }
}