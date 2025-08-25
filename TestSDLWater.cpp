#include <iostream>
#include <cmath>
#include <vector>
#include <deque>
#include <SDL3/SDL.h>
#include "WaterEffect.h"


const int GRID_SIZE = 200;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* texture = nullptr;


int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL Initialization Failed: %s", SDL_GetError());
        return -1;
    }
    int windowWidth = 1600;
    int windowHeight = 900;
    if (!SDL_CreateWindowAndRenderer("Water Effect", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("SDL Window and Renderer Creation Failed: %s", SDL_GetError());
        return -1;
    }

    SDL_SetRenderVSync(renderer, 1);

    SDL_Surface* surface = SDL_LoadBMP("water_effect.bmp");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    auto waterTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, windowWidth, windowHeight);

    WaterEffect waterEffect(window, renderer);

    waterEffect.applyPresetParams();
    waterEffect.initGrid(windowWidth, windowHeight);

    bool is_active = true;
    while (is_active) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_active = false;
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                int w, h;
                SDL_GetWindowSizeInPixels(window, &w, &h);

                waterEffect.addDefaultClickRipple(event.button.x * windowWidth / w, event.button.y * windowHeight / h, static_cast<float>(SDL_GetTicks()) / 1000.0f);
            }
        }
        SDL_SetRenderTarget(renderer, waterTexture);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        waterEffect.setupEffectCanvas();

        SDL_RenderTexture(renderer, texture, nullptr, nullptr);

        waterEffect.renderEffect(static_cast<float>(SDL_GetTicks()) / 1000.0f);

        SDL_SetRenderTarget(renderer, nullptr);

        SDL_RenderTexture(renderer, waterTexture, nullptr, nullptr);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}