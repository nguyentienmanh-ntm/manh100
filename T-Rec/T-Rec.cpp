#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 550;

bool init(); //khởi tạo cửa sổ
void close(); //đóng cửa sổ
bool load();
bool collision(SDL_Rect a, SDL_Rect b); //sử lý va chạm
SDL_Texture* loadTexture(char* path);
SDL_Renderer* gRenderer = NULL;
SDL_Window* gWindow = NULL;
TTF_Font* Font = NULL;

SDL_Event e; // sử lý sự kiện
// load ảnh
SDL_Texture* walk[10]; SDL_Texture* run[8]; SDL_Texture* dead[8]; SDL_Texture* jump[12];
SDL_Texture* zFmWalk[8]; SDL_Texture* zFmAttack[8]; SDL_Texture* zMWalk[8]; SDL_Texture* zMAttack[8];
SDL_Texture* desFire = NULL; SDL_Texture* omap[10];
SDL_Texture* coin = NULL; SDL_Texture* bg = NULL;
SDL_Texture* menuGame = NULL;
Mix_Music* Music = NULL;
SDL_Color black = { 0,0,0 }; SDL_Color red = { 255,0,0 }; SDL_Color white = { 255,255,255 };
bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }

        //Create window
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            //Create vsynced renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
                    success = false;
                }
                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}
SDL_Texture* loadTexture(char* path)
{
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface != NULL)
    {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 255, 255));// made nền trong suốt
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}
// load phông chữ
SDL_Texture* loadFromRenderedText(std::string textureText, SDL_Color textColor, SDL_Rect& rect)
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(Font, textureText.c_str(), textColor); 
    SDL_Texture* mTexture = NULL;
    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if (mTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        rect.w = textSurface->w;
        rect.h = textSurface->h;
        SDL_FreeSurface(textSurface);
    }
    return mTexture;
}
void close()
{
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    SDL_QUIT;
    TTF_Quit;
    Mix_Quit;
}
bool load()
{
    char s[] = "3.png"; char b[] = "2.png"; char mnG[] = "Menu.png";
    char nFire[] = "fire.png";  char nCoin[] = "coin (2).png"; char nBg[] = "backg.png";
    coin = loadTexture(nCoin);
    desFire = loadTexture(nFire); bg = loadTexture(nBg);
    menuGame = loadTexture(mnG);
    omap[0] = loadTexture(s); omap[1] = loadTexture(b); 
    for (int i = 0; i < 12; i++) {
        char a[20];
        sprintf_s(a, "Jump (%d).png", i + 1);
        jump[i] = loadTexture(a); if (jump[i] == NULL) return false;
        if (i < 10) {
            sprintf_s(a, "Walk (%d).png", i + 1);
            walk[i] = loadTexture(a); if (walk[i] == NULL) return false;
            if (i < 8) {
                sprintf_s(a, "Run (%d).png", i + 1);
                run[i] = loadTexture(a); if (run[i] == NULL) return false;
                sprintf_s(a, "Dead (%d).png", i + 1);
                dead[i] = loadTexture(a); if (dead[i] == NULL) return false;
                sprintf_s(a, "Attack (%d).png", i + 1);
                zFmAttack[i] = loadTexture(a);
                sprintf_s(a, "zWalk (%d).png", i + 1);
                zFmWalk[i] = loadTexture(a);
                sprintf_s(a, "zAttack (%d).png", i + 1);
                zMAttack[i] = loadTexture(a);
                sprintf_s(a, "zMWalk (%d).png", i + 1);
                zMWalk[i] = loadTexture(a);
            }
        }
    }

    Font = TTF_OpenFont("ARCADE.ttf", 70);
    Music = Mix_LoadMUS("music.wav");

    return true;
}
void eventD(SDL_Event e, bool& bwalk, bool& brun, bool& bjump, bool& bdead, int& state) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_w: if (bjump) {
            bwalk = false, brun = false, bdead = false;
        }
                   else {
            bwalk = true, brun = false, bdead = false, bjump = false;
        }
                   state = 0;  
                      break;
        case SDLK_UP: bjump = true, brun = false, bdead = false, bwalk = false;
            break;
        case SDLK_e:  if (bjump) {
            bwalk = false, brun = false, bdead = false;
        }
                   else { bwalk = false, brun = true, bdead = false, bjump = false; }
                   state = 2;   break;
        case SDLK_m: Mix_PlayMusic(Music, -1); break;
        case SDLK_h: Mix_HaltMusic(); break;;
        }
    }
}
// load ảnh
void renderD(int& state, bool& bwalk, bool& brun, bool& bjump, bool& bdead, int& indexW, int& indexR, int& indexJ, int& indexD, SDL_Rect& rDiasourous, int &die, int STEP_WALK, int STEP_RUN, int STEP_JUMP) {
    if (die == 1) {
        rDiasourous.w = 115, rDiasourous.h = 125, rDiasourous.y = 355;
        SDL_RenderCopy(gRenderer, dead[indexD], NULL, &rDiasourous);
        indexD++;
        if (indexD > 7) die = 2;
   }
    else {
        if (bwalk) {
            SDL_RenderCopy(gRenderer, walk[indexW], NULL, &rDiasourous);
            indexR = 0, indexW++, indexJ = 0, indexD = 0;
            rDiasourous.x = (rDiasourous.x + STEP_WALK) % SCREEN_WIDTH;
        }
        if (brun) {
            SDL_RenderCopy(gRenderer, run[indexR], NULL, &rDiasourous);
            indexR++, indexW = 0, indexJ = 0, indexD = 0;
            rDiasourous.x = (rDiasourous.x + STEP_RUN) % SCREEN_WIDTH;
        }
        if (bjump) {
            SDL_RenderCopy(gRenderer, jump[indexJ], NULL, &rDiasourous);
            indexR = 0, indexW = 0, indexJ++, indexD = 0;
            if (state == 0)  rDiasourous.x = (rDiasourous.x + STEP_WALK );
            else if (state == 2) rDiasourous.x = (rDiasourous.x + STEP_RUN);
            if (indexJ > 2 && indexJ < 7) rDiasourous.y -= STEP_JUMP;
            else if (indexJ > 7 && indexJ < 12) rDiasourous.y += STEP_JUMP;
            if (indexJ == 11) {
                bjump = false;
                switch (state) {
                case 0: bwalk = true; break;
                case 2: brun = true; break;
                default: break;
                }
            }
        }
    }
    if (indexW > 9) indexW = 0;
    if (indexJ > 11) indexJ = 0;
    if (indexR > 7) indexR = 0;
}
//random chướng ngại vật
void renderMap(vector< int> cellMap, SDL_Rect r, SDL_Rect c, int posMap, int& lose, SDL_Rect rsrcFire[5], int indexF, int &over, int indexZ, int DISTANCE) {
    r.x = posMap; bool w = false;
    for (int i = 0; i < cellMap.size(); i++) {
            switch (cellMap[i]) {
            case 1: SDL_RenderCopy(gRenderer, omap[0], NULL, &r); break;
            case 2: r.h = 130, r.y = 325; SDL_RenderCopy(gRenderer, desFire, &rsrcFire[indexF], &r); break;
            case 3: r.h = 130, r.y = 325; SDL_RenderCopy(gRenderer, zFmAttack[indexZ], NULL, &r); break;
            case 4: r.h = 130, r.y = 325; SDL_RenderCopy(gRenderer, zFmWalk[indexZ], NULL, &r); break;
            case 5: r.h = 130, r.y = 325; SDL_RenderCopy(gRenderer, zMAttack[indexZ], NULL, &r); break;
            case 6: r.h = 130, r.y = 325; SDL_RenderCopy(gRenderer, zMWalk[indexZ], NULL, &r); break;
            default: break;
            }
            if (collision(c, r)) lose = 1;
            r.h = 100; r.y = 355;
            r.x += DISTANCE;
            if (i == cellMap.size() - 2) over = r.x;
        }
}
// kiểm tra va chạm
bool collision(SDL_Rect a, SDL_Rect b) {
    if (a.x + a.w > b.x + 15 && a.x < b.x + 50 && a.y + a.h > b.y) return true;
    return false;
}
bool Menu(SDL_Event e,SDL_Texture* menuText[3], SDL_Rect rMenuText[3], int& timeDelay, bool Bwin, int RECORD) {
    if (Bwin) return true;
    else {
        string sRecord; stringstream ssRecord;
        ssRecord << RECORD; ssRecord >> sRecord;
        bool b = false; bool record = false;
        SDL_Rect constMenu[3];
        while (!b) {
            rMenuText[0].x = 50; rMenuText[0].y = 300;
            rMenuText[1].x = 50; rMenuText[1].y = rMenuText[0].y + rMenuText[0].h;
            rMenuText[2].x = 50; rMenuText[2].y = rMenuText[1].y + rMenuText[1].h;
            constMenu[0] = rMenuText[0]; constMenu[1] = rMenuText[1]; constMenu[2] = rMenuText[2];
            while (SDL_PollEvent(&e)) {
                menuText[0] = loadFromRenderedText("Play", white, rMenuText[0]);
                if (!record)  menuText[1] = loadFromRenderedText("Record", white, rMenuText[1]);
                else menuText[1] = loadFromRenderedText(sRecord, white, rMenuText[1]);
                menuText[2] = loadFromRenderedText("Exit", white, rMenuText[2]);
                if (e.type == SDL_QUIT) {
                    b = true;
                    return false;
                }
                if (e.type == SDL_MOUSEMOTION) {
                    if (e.button.x > rMenuText[0].x && e.button.x < rMenuText[0].x + rMenuText[0].w && e.button.y > rMenuText[0].y && e.button.y < rMenuText[0].y + rMenuText[0].h) {
                        menuText[0] = loadFromRenderedText("Play", red, rMenuText[0]);

                    }
                    if (e.button.x > rMenuText[1].x && e.button.x < rMenuText[1].x + rMenuText[1].w && e.button.y > rMenuText[1].y && e.button.y < rMenuText[1].y + rMenuText[1].h) {
                       if(!record) menuText[1] = loadFromRenderedText("Record", red, rMenuText[1]);
                     
                    }
                    if (e.button.x > rMenuText[2].x && e.button.x < rMenuText[2].x + rMenuText[2].w && e.button.y > rMenuText[2].y && e.button.y < rMenuText[2].y + rMenuText[2].h) {
                        menuText[2] = loadFromRenderedText("Exit", red, rMenuText[2]);
                    }
                }
                else   if (e.type = SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                    if (e.button.x > rMenuText[0].x && e.button.x < rMenuText[0].x + rMenuText[0].w && e.button.y > rMenuText[0].y && e.button.y < rMenuText[0].y + rMenuText[0].h)  return true;
                    else if (e.button.x > rMenuText[2].x && e.button.x < rMenuText[2].x + rMenuText[2].w && e.button.y > rMenuText[2].y && e.button.y < rMenuText[2].y + rMenuText[2].h) return false;
                    else if (e.button.x > rMenuText[1].x && e.button.x < rMenuText[1].x + rMenuText[1].w && e.button.y > rMenuText[1].y && e.button.y < rMenuText[1].y + rMenuText[1].h) record = true;
                }
                 if (e.type == SDL_KEYDOWN) {
                    switch (e.key.keysym.sym) {
                    case SDLK_m: Mix_PlayMusic(Music, -1); break;
                    case SDLK_h: Mix_HaltMusic(); break;
                    default: break;
                    }
                }
            }
            SDL_RenderClear(gRenderer);
            SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
            SDL_RenderCopy(gRenderer, menuGame, NULL, NULL);
            for (int i = 0; i < 3; i++) {
                SDL_RenderCopy(gRenderer, menuText[i], NULL, &constMenu[i]);
            }
            SDL_RenderPresent(gRenderer);
            timeDelay = SDL_GetTicks() / 1000;
        }
    }
    return false;
}
int main(int argv, char* argc[]) {
    int RECORD = 0;
    do {
        int STEP_WALK = 25;
        int STEP_RUN = 30;
        const int STEP_JUMP = 60;
        int DISTANCE = 700; 
        const int CELL = 50;
        vector< int> cellMap; cellMap.push_back(1), cellMap.push_back(2), cellMap.push_back(4);
        bool quit = false, Bwin = false;
        int indexR = 0, indexW = 0, indexJ = 0, indexD = 0, lose = 0;
        bool bwalk = true, brun = false, bdead = false, bjump = false;
        int state = 0, indexF = 0, indexZ = 0, posMap = 500, over = 0, TIME_PLAY = 0, hideTime = 0;
        SDL_Rect rDiasourous = { 0,355,90,100 };  SDL_Rect srcCoin[10]; SDL_Rect rCoin = { 100,100,50,50 };
        SDL_Rect rsrcFire[5];  SDL_Rect rMap = { 0,355,65,100 };
        rsrcFire[0] = { 0,0,80,120 }, rsrcFire[1] = { 80,0,80,120 }, rsrcFire[2] = { 160,0,80,120 }, rsrcFire[3] = { 240,0,80,120 }, rsrcFire[4] = { 320,0,80,120 };

        SDL_Texture* menuText[3]; SDL_Rect rMenuText[3]; int timeDelay = 0;

        if (!init()) return 0;
        else {
            if (!load()) return 0;
            else {
                if (!Menu(e, menuText, rMenuText, timeDelay, Bwin,RECORD)) return 0;
                while (!quit) {
                    while (SDL_PollEvent(&e)) {
                        if (e.type == SDL_QUIT) {
                            quit = true;
                        }
                        eventD(e, bwalk, brun, bjump, bdead, state);
                    }

                    SDL_RenderClear(gRenderer);
                    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
                    SDL_RenderCopy(gRenderer, bg, NULL, NULL);
                  
                    if (rDiasourous.x > over) {
                        int random = 0;
                        random = rand() % 6 + 1;
                        cellMap.push_back(random);
                    }
                    TIME_PLAY = SDL_GetTicks() / 1000 - timeDelay;
                    int virtualTime = SDL_GetTicks() / 1000 - timeDelay - hideTime;
                    Font = TTF_OpenFont("lazy.ttf", 30);
                    string sTime = "Score :"; string nTime; stringstream ssTime;
                    ssTime << TIME_PLAY; ssTime >> nTime;
                    sTime = sTime + " " + nTime;
                    SDL_Rect rTime = { SCREEN_WIDTH / 2, 0, 0 ,0 };
                    SDL_Texture* Time; Time = loadFromRenderedText(sTime, white, rTime);
                    SDL_RenderCopy(gRenderer, Time, NULL, &rTime);

                   
                    renderMap(cellMap, rMap, rDiasourous, posMap, lose, rsrcFire, indexF, over, indexZ, DISTANCE);
                    renderD(state, bwalk, brun, bjump, bdead, indexW, indexR, indexJ, indexD, rDiasourous, lose, STEP_WALK, STEP_RUN, STEP_JUMP);
                    indexF++; if (indexF > 4) indexF = 0; indexZ++; if (indexZ > 7) indexZ = 0;
                    if (rDiasourous.x > SCREEN_WIDTH / 2) {
                        if (state == 0) { rDiasourous.x -= STEP_WALK ;   posMap -= STEP_WALK ; }
                        if (state == 2) { rDiasourous.x -= STEP_RUN+15;   posMap -= STEP_RUN ; }
                    }
                    if (lose == 2) quit = true;
                    if (TIME_PLAY > RECORD) RECORD = TIME_PLAY;
                    SDL_Delay(100);
                    SDL_RenderPresent(gRenderer);
                    if (quit) SDL_Delay(3000);
                }
            }
        }
        close();
    } while (true);
    return 0;
}
