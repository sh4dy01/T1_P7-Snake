#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <cstdlib>
#include <string>

#include "snake.hpp"

using namespace std;

//Grille de 20 carrés de 32*32
class MainSDLWindow{

    public:
    MainSDLWindow();
    ~MainSDLWindow();
    int Init();
    void Draw(int x, int y, int length);
    void CheckForQuit();
    void AddSnake();
    void CheckBorders();
    bool CheckFruit();

    SDL_Renderer *GetRenderer();
    bool GetGameState();
    int GetFrameRate();
    int GetPixels();
    int GetSquares();

    private:
    int width, height, nbrSquare, pixels;
    bool IsGameRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect map, head;
    SDL_Rect Gfruit;
    Fruit fruit;
    Uint32 frame_rate;

};