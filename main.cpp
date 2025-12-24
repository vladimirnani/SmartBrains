#include <chrono>
#include <cstdio>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define WINDOW_HEIGHT 1024
#define WINDOW_WIDTH 1920

struct SDLApp;

struct SmartBrain {
    const char * name;
    const char * path;
    const char * scarcity;
    const int playerNumber;
    const char * earn;
    const char * buy;
    const char * sell;
};

struct Background {
    SDL_Renderer *mRenderer;
    SDL_Texture *bgTexture;

    Background() {
    }

    Background(SDL_Renderer *renderer) {
        mRenderer = renderer;
        SDL_Surface *sdlSurface1 = SDL_LoadBMP("..\\bg.bmp");
        bgTexture = SDL_CreateTextureFromSurface(mRenderer, sdlSurface1);
        SDL_DestroySurface(sdlSurface1);
    }

    void Render(SDL_Renderer *mRenderer) {
        SDL_RenderTexture(mRenderer, bgTexture, NULL, NULL);
    }
};


struct Sprite {
    SDL_FRect source;
    SDL_FRect position;

    SDL_Texture *texture;
    float deltaX = 0;
    float deltaY = 0;
    float spriteHeight = 245;
    float spriteWidth = 200;
    int animation_step = -1;
    int animation_total = 4;
    int animation_sync = 0;
    int frameWidth = 200;
    bool renderFlipped = false;
    std::stringstream debug_text;


    Sprite(SDL_Renderer* mRenderer, const std::string &path) {
        SDL_Surface *sdlSurface1 = SDL_LoadBMP(path.c_str());
        texture = SDL_CreateTextureFromSurface(mRenderer, sdlSurface1);
        SDL_DestroySurface(sdlSurface1);
    }

    ~Sprite() {
        SDL_DestroyTexture(texture);
    }

    virtual void Render(SDL_Renderer* mRenderer) {
        if (deltaX < 0) {
            renderFlipped = true;
        }
        else if (deltaX> 0) {
            renderFlipped = false;
        }

        if (renderFlipped) {
            SDL_RenderTextureRotated(mRenderer, texture, &source, &position, 0, nullptr, SDL_FLIP_HORIZONTAL);
        }
        else {
            SDL_RenderTexture(mRenderer, texture, &source, &position);
        }
    }
};

struct Player: Sprite {
    bool isSprinting = false;
    double money = 0;
    int playerNumber;

    Player(SDL_Renderer *mRenderer, const int number, const std::string& path): Sprite(mRenderer, path){
        deltaX = 0;
        deltaY = 0;
        playerNumber = number;
        float spriteHeight = 245;

        position = SDL_FRect{0, 0, 200, spriteHeight};
        source = SDL_FRect{0, 0, 200, spriteHeight};
    }


    void UpdatePosition() {
        deltaX = 0;
        deltaY = 0;

        const bool *key_states = SDL_GetKeyboardState(NULL);

        if (playerNumber == 1) {
            isSprinting = key_states[SDL_SCANCODE_RSHIFT];
        }
        else {
            isSprinting = key_states[SDL_SCANCODE_LSHIFT];
        }

        float stepLength = 10;
        if (isSprinting) {
            stepLength = 30;
        }
        if (playerNumber == 1) {
            if (key_states[SDL_SCANCODE_RIGHT]) {
                deltaX = stepLength;
            }
            if (key_states[SDL_SCANCODE_LEFT]) {
                deltaX = -stepLength;
            }
            if (key_states[SDL_SCANCODE_UP]) {
                deltaY = -stepLength;
            }
            if (key_states[SDL_SCANCODE_DOWN]) {
                deltaY = stepLength;
            }
        }
        else {
            if (key_states[SDL_SCANCODE_D]) {
                deltaX = stepLength;
            }
            if (key_states[SDL_SCANCODE_A]) {
                deltaX = -stepLength;
            }
            if (key_states[SDL_SCANCODE_W]) {
                deltaY = -stepLength;
            }
            if (key_states[SDL_SCANCODE_S]) {
                deltaY = stepLength;
            }
        }


        position.x += deltaX;
        if (position.x > WINDOW_WIDTH) {
            position.x =0;
        }
        if (position.x < 0) {
            position.x = WINDOW_WIDTH;
        }
        position.y += deltaY;
        if (position.y > WINDOW_HEIGHT) {
            position.y = 0;
        }

        if (position.y < 0) {
            position.y = WINDOW_HEIGHT;
        }
    }

    void UpdateAnimation() {
        if (deltaY || deltaX) {
            animation_step = (animation_step + 1) % animation_total;
            if (position.x != deltaX) {
                source.x = animation_step * frameWidth;
            } else if (position.y != deltaY) {
                source.x = animation_step * frameWidth;
            } else {
                source.x = 0;
            }
        }

    }

    void UpdateDebugText() {
        debug_text.str("");
        debug_text << "Money: " << money << " $\n" << std::endl;

        if (isSprinting) {
            debug_text << "Sprinting" << std::endl;
        }
    }

    void Update() {
        UpdatePosition();
        UpdateAnimation();
        UpdateDebugText();
        money++;
    }
};

struct SmartBrainCharacter: Sprite {
    double money = 0;
    const SmartBrain* mData;
    bool renderFlipped = false;
    std::stringstream debug_text;

    SmartBrainCharacter(SDL_Renderer *mRenderer, const SmartBrain* data): Sprite(mRenderer, data->path) {
        mData = data;
        float xPos = data->playerNumber * 400;
        float yPos =  WINDOW_HEIGHT / 2 - spriteHeight / 2;
        position = SDL_FRect{xPos, yPos, 200, spriteHeight};
        source = SDL_FRect{0, 0, 200, spriteHeight};
    }

    ~SmartBrainCharacter() {
        SDL_DestroyTexture(texture);
    }

    void UpdatePosition() {
        float stepLength = 10;
        deltaX = 0;
        deltaX = stepLength;
        position.x += deltaX;
        if (position.x > WINDOW_WIDTH) {
            position.x = -spriteWidth;
        }
    }

    void UpdateAnimation() {
        if (deltaY || deltaX) {
            animation_step = (animation_step + 1) % animation_total;
            if (position.x != deltaX) {
                source.x = animation_step * frameWidth;
            } else if (position.y != deltaY) {
                source.x = animation_step * frameWidth;
            } else {
                source.x = 0;
            }
        }

    }

    void UpdateDebugText() {
        debug_text.str("");
        debug_text << "Money: " << money << " $\n" << std::endl;
    }

    void Update() {
        UpdatePosition();
        UpdateAnimation();
        UpdateDebugText();
        money++;
    }

    void RenderText(SDL_Renderer *mRenderer) const {
        SDL_SetRenderDrawColor(mRenderer, 0xff,0xff,0xff,0xff);

        float yPosition = position.y - 100;

        SDL_RenderDebugText(mRenderer, position.x + 40, yPosition + 20, mData->name);
        SDL_SetRenderDrawColor(mRenderer, 0xff,0x33,0x33,0xff);
        SDL_RenderDebugText(mRenderer, position.x + 40, yPosition + 40, mData->scarcity);
        std::string_view sv1("earn: ");
        std::string_view sv2(mData->earn);
        SDL_RenderDebugText(mRenderer, position.x + 40, yPosition + 60, (std::string(sv1) + std::string(sv2)).c_str());
        std::string_view buy1("buy: ");
        std::string_view buy2(mData->buy);
        SDL_RenderDebugText(mRenderer, position.x + 40, yPosition + 80,  (std::string(buy1) + std::string(buy2)).c_str());
        std::string_view sell1("sell: ");
        std::string_view sell2(mData->sell);
        SDL_RenderDebugText(mRenderer, position.x + 40, yPosition + 100, (std::string(sell1) + std::string(sell2)).c_str());

        SDL_SetRenderDrawColor(mRenderer, 0x00,0xff,0xff,0xff);
        SDL_RenderDebugText(mRenderer, 10,  10, debug_text.str().c_str());
    }

    void Render(SDL_Renderer *mRenderer) override {
        Sprite::Render(mRenderer);
        RenderText(mRenderer);
    }
};


struct Portal: Sprite {

    Portal(SDL_Renderer *mRenderer, const std::string& path, float x, float y): Sprite(mRenderer, path) {
        deltaX = 0;
        deltaY = 0;
        float spriteHeight = 92;
        float spriteWidth = 204;

        position = SDL_FRect{x, y, spriteWidth, spriteHeight};
        source = SDL_FRect{0, 0, spriteWidth, spriteHeight};
    }
};

struct SDLApp {
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    bool mDone = false;
    SmartBrainCharacter *crosaniniSprite;
    SmartBrainCharacter *lasFantasSprite;
    Player *PlayerLeft;
    Player *PlayerRight;
    Background * bg;
    SmartBrain * crosanini;
    SmartBrain * lasfantas;
    Portal * portal1;
    Portal * portal2;

    void create_scene() {
        bg = new Background(mRenderer);
        crosaniniSprite = new SmartBrainCharacter(mRenderer, crosanini);
        lasFantasSprite = new SmartBrainCharacter(mRenderer, lasfantas);
        PlayerLeft = new Player(mRenderer, 1, "..\\max.bmp");
        PlayerRight = new Player(mRenderer, 2, "..\\dude.bmp");
        portal1 = new Portal(mRenderer,  "..\\portal.bmp", 200, 200);
        portal2 = new Portal(mRenderer,  "..\\portal.bmp", 1000, 800);
    }

    SDLApp() {
        SDL_Init(SDL_INIT_VIDEO);
        std::tm tm = {};
        std::stringstream ss("Dec 13 2025 12:35:34");
        ss >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
        const auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        SDL_CreateWindowAndRenderer("Steal A Brainrot", 1904, 1024, 0, &mWindow, &mRenderer);
        if (mRenderer) {
            SDL_Log("renderer %s", SDL_GetRendererName(mRenderer));
        }

        crosanini = new SmartBrain{
            "crosanini maslonini",
            "..\\smartbrains\\crosanini.bmp",
            "smartbrain god",
            1,
            "900$",
            "3k",
            "3k"
        };

        lasfantas = new SmartBrain{
            "las fantas",
            "..\\smartbrains\\lasfantas.bmp",
            "smartbrain god",
            2,
            "1.2k$",
            "50m",
            "100m"
        };
        create_scene();

    }

    ~SDLApp() {
        SDL_DestroyWindow(mWindow);
        SDL_DestroyRenderer(mRenderer);
        delete crosaniniSprite;
        delete lasFantasSprite;
        delete bg;
        SDL_Quit();
    }

    void Input() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                mDone = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                // SDL_Log("Key presseed %d %d", event.key.key, event.key.mod);
            }
        }
    }

    int teleport(Player *player, Portal *source, Portal *target) const {
        auto x = player->position.x + player->spriteWidth / 2;
        auto y = player->position.y +  player->spriteHeight;

        if (
            x > source->position.x && x < source->position.x + portal1->spriteWidth  &&
            y > source->position.y && y < source->position.y + portal1->spriteHeight) {

            player->position.x = target->position.x ;
            player->position.y = target->position.y;
            return 1;
        }
        return 0;
    }

    void Update() {
        const bool *key_states = SDL_GetKeyboardState(NULL);

        PlayerLeft->Update();
        PlayerRight->Update();
        crosaniniSprite->Update();
        lasFantasSprite->Update();

        if (key_states[SDL_SCANCODE_SPACE]) {
            teleport(PlayerLeft, portal1, portal2);
            teleport(PlayerLeft, portal2, portal1);
            teleport(PlayerRight, portal1, portal2);
            teleport(PlayerRight, portal2, portal1);
        }

    }

    void Render() {
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(mRenderer);
        bg->Render(mRenderer);
        portal1->Render(mRenderer);
        portal2->Render(mRenderer);
        crosaniniSprite->Render(mRenderer);
        lasFantasSprite->Render(mRenderer);
        PlayerLeft->Render(mRenderer);
        PlayerRight->Render(mRenderer);
        SDL_RenderPresent(mRenderer);
    }

    void Tick() {
        Input();
        Update();
        Render();
        SDL_Delay(50);
    }

    void MainLoop() {
        Uint64 lastTime = 0;
        Uint64 fps = 0;
        while (!mDone) {
            Tick();
            auto currentTime = SDL_GetTicks();
            fps++;
            if (currentTime > lastTime + 1000) {
                lastTime = currentTime;
                std::string title;
                title += "Brainrot FPS:" + std::to_string(fps);
                SDL_SetWindowTitle(mWindow, title.c_str());
                fps = 0;
            }
        }

        printf("Hello, World!\n");
    }
};

int main() {
    auto app = SDLApp();
    FILE * settings;

    settings = fopen("settings.txt", "r");
    double stored_money = 0.0f;
    fscanf_s(settings, "%lf", &stored_money);
    fclose(settings);

    if (stored_money) {
        app.crosaniniSprite->money = stored_money;
    }

    app.MainLoop();
    settings = fopen("settings.txt", "w+");
    fprintf(settings, "%lf", app.crosaniniSprite->money);
    fclose(settings);
    return 0;
}