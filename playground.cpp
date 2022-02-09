#include "playground.hpp"
#include "snake.hpp"

#include <iostream>

MainSDLWindow::MainSDLWindow(){
    this->window = NULL;
    this->renderer = NULL;
    this->highScore = 0;
    this->ballCount = -1;
    this->IsGameRunning = true;
    this->IsGameWon = false;

    this->map.h = SCREEN_SIZE - PIXELS * 2;
    this->map.w = map.h;
    this->map.x = PIXELS;
    this->map.y = map.x;

    this->scoreArea.x = 0;
    this->scoreArea.y = 0;

    this->square.w = PIXELS;
    this->square.h = square.w;
    this->headTexture = this->ballTexture = this->bodyTexture = this->tailTexture = this->mBallTexture = this->scoreTexture = NULL;
}

MainSDLWindow::~MainSDLWindow(){
    SDL_DestroyTexture(headTexture);
    SDL_DestroyTexture(bodyTexture);
    SDL_DestroyTexture(tailTexture);
    SDL_DestroyTexture(ballTexture);
    SDL_DestroyTexture(mBallTexture);
    SDL_DestroyTexture(scoreTexture);
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

int MainSDLWindow::Init(){
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // --- Create the window --- //
    window = SDL_CreateWindow(NAME_OF_THE_GAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_SIZE, SCREEN_SIZE, SDL_WINDOW_RESIZABLE);
    if (window == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    // --- Create the renderer --- //
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // --- Create the textures --- //
    headTexture = CreateTextureFromImage("textures/head.bmp");
    bodyTexture = CreateTextureFromImage("textures/body.bmp");
    tailTexture = CreateTextureFromImage("textures/tail.bmp");
    mBallTexture = CreateTextureFromImage("textures/mBall.bmp");

    if(TTF_Init() == -1){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return EXIT_FAILURE;
    };
    font = TTF_OpenFont("ARIAL.TTF", 32);
    if (font == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return EXIT_FAILURE;
    }

    DrawText(BASE_HIGHSCORE_TEXT, highScore);

    return EXIT_SUCCESS;
}

// Draw everything
void MainSDLWindow::Draw(Segment *head, Fruit *fruits, bool ballStars[SET], int length, int score){
    int angle = 0;

    SetBallTexture(fruits->GetStar());
    
    if (score >= highScore) { highScore = score; }
    
    // --- White background & draw the playground color --- //
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 79, 124, 187, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &map);

    // --- Draw Texts --- //
    DrawText(BASE_SCORE_TEXT, score);
    DrawText(BASE_HIGHSCORE_TEXT, highScore);

    // --- Draw Fruits --- //
    Fruit *temp = fruits;
    while (temp != NULL)
    {
        square.x = temp->GetX() * PIXELS;
        square.y = temp->GetY() * PIXELS;
        if (temp->GetMalus())
        {
            SDL_SetRenderTarget(renderer, mBallTexture); 
            SDL_RenderCopy(renderer, mBallTexture, NULL, &square);
        }else{
            
            SDL_SetRenderTarget(renderer, ballTexture); 
            SDL_RenderCopy(renderer, ballTexture, NULL, &square);
        }
        temp = temp->GetNext();
    }

    square.x = SCREEN_SIZE / 2 - (PIXELS * ceil(SET/2));
    square.y = SCREEN_SIZE - PIXELS;
    for (int i = 0; i < SET; i++)
    {   
        square.x +=  PIXELS;
        if (ballStars[i])
        {
            SetBallTexture(i);
            SDL_SetRenderTarget(renderer, ballTexture); 
            SDL_RenderCopy(renderer, ballTexture, NULL, &square);
        }
        
    }

    // --- Draw every part of the snake --- //
    Segment *body = head;
    for (int i = 0; i < length; i++)
    {
        square.x = body->GetX();
        square.y = body->GetY();

        switch (body->GetDirection()) // Rotate the texture
            {
            case UP:
                angle = 180;
                break;
            case RIGHT:
                angle = -90;
                break;
            case LEFT:
                angle = 90;
                break;
            case DOWN:
                angle = 0;
                break;
            case PAUSE:
                angle = 0;
                break;
            default:
                break;
            }

        if (i == 0) // Head part
        {
            SDL_SetRenderTarget(renderer, headTexture); 
            SDL_RenderCopyEx(renderer, headTexture, NULL, &square, angle, NULL, SDL_FLIP_NONE);
        }
        else if (i == length - 1){ // Tail part
            SDL_SetRenderTarget(renderer, tailTexture);
            SDL_RenderCopyEx(renderer, tailTexture, NULL, &square, angle, NULL, SDL_FLIP_NONE);
        }else{ // Body part
            SDL_SetRenderTarget(renderer, bodyTexture);
            SDL_RenderCopyEx(renderer, bodyTexture, NULL, &square, angle, NULL, SDL_FLIP_NONE);
        }
        body = body->GetNext(); // Take the next part
    }

    SDL_RenderPresent(renderer); // Refresh the renderer
}

SDL_Texture *MainSDLWindow::CreateTextureFromImage(const char path[]){
    SDL_Surface *tmp = NULL; 
    SDL_Texture *texture = NULL;
    tmp = SDL_LoadBMP(path);

    if(tmp == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return NULL;
    }
    texture = SDL_CreateTextureFromSurface(renderer, tmp);
    SDL_FreeSurface(tmp);
    if(texture == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return NULL;
    }
    return texture;
}

void MainSDLWindow::SetBallTexture(int star){
    ballString = "textures/ball" + to_string(star) + ".bmp";
    const char *ballStar = ballString.c_str();
    ballTexture = CreateTextureFromImage(ballStar);
}

void MainSDLWindow::DrawText(const char name[]){

    scoreSurface = TTF_RenderText_Solid(font, name, {0, 0, 0});
    scoreArea.w = scoreSurface->w;
    scoreArea.h = scoreSurface->h;
    scoreArea.x = SCREEN_SIZE / 2 - scoreArea.w;
    scoreArea.y = SCREEN_SIZE - PIXELS;
    
    scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreArea);
}

void MainSDLWindow::DrawText(const char name[], int score){

    scoreString = name + to_string(score);
    scoreText = scoreString.c_str();
    scoreSurface = TTF_RenderText_Solid(font, scoreText, {0, 0, 0});
    scoreArea.w = scoreSurface->w;
    scoreArea.h = scoreSurface->h;
    if (name == BASE_HIGHSCORE_TEXT)
    {
        scoreArea.x = SCREEN_SIZE - scoreArea.w;
        scoreArea.y = 0;
    }else{
        scoreArea.x = 0;
        scoreArea.y = 0;
    }
    
    scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreArea);
}

// Check if the user clicked on the X to close the game
void MainSDLWindow::CheckForQuit(){
    SDL_Event event;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);

    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            IsGameRunning = false;
        }
    }
}

void MainSDLWindow::CheckIfWon(int length, int turn){
    if (length != MAX_LENGTH - turn)
    {
        IsGameWon = true;
    }
}

SDL_Renderer *MainSDLWindow::GetRenderer(){return renderer;}
bool MainSDLWindow::GetGameState(){return IsGameRunning;}
bool MainSDLWindow::GetIfWon(){return IsGameWon;}