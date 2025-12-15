#include <chrono>
#include <cstdio>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef enum BRAINROT_TYPE {
    COMMON,
    EPIC,
    RARE,
    OG
} BRAINROT_TYPE;

typedef struct Brainrot {
    int price;
    int earns_per_second;
    std::chrono::system_clock::time_point created_at;
    char name[100];
    BRAINROT_TYPE brainrot_type;
} Brainrot;


struct Sprite {
    SDL_Texture *characterTexture;
    SDL_FRect source;
    SDL_FRect postion;
    float deltaX = 0;
    float deltaY = 0;
    bool isSprinting = false;
    int animation_step = -1;
    int animation_total = 4;
    int animation_sync = 0;
    int frameWidth = 200;
    bool isMoving = true;
    float money = 0;

    std::stringstream debug_text;

    Sprite() {
    }

    Sprite(SDL_Renderer *mRenderer) {
        deltaX = 0;
        deltaY = 0;

        postion = SDL_FRect{0, 0, 200, 245};
        source = SDL_FRect{0, 0, 200, 245};

        SDL_Surface *sdlSurface1 = SDL_LoadBMP("..\\crosanini.bmp");
        characterTexture = SDL_CreateTextureFromSurface(mRenderer, sdlSurface1);
        SDL_DestroySurface(sdlSurface1);
    }

    ~Sprite() {
        SDL_DestroyTexture(characterTexture);
    }

    void UpdatePosition() {
        const bool *key_states = SDL_GetKeyboardState(NULL);
        isSprinting = key_states[SDL_SCANCODE_LSHIFT] || key_states[SDL_SCANCODE_RSHIFT];
        float stepLength = 10;
        if (isSprinting) {
            stepLength = 25;
        }
        deltaX = 0;
        deltaY = 0;
        if (key_states[SDL_SCANCODE_D] || key_states[SDL_SCANCODE_RIGHT]) {
            deltaX = stepLength;
        }
        if (key_states[SDL_SCANCODE_A] || key_states[SDL_SCANCODE_LEFT]) {
            deltaX = -stepLength;
        }
        if (key_states[SDL_SCANCODE_W] || key_states[SDL_SCANCODE_UP]) {
            deltaY = -stepLength;
        }
        if (key_states[SDL_SCANCODE_S] || key_states[SDL_SCANCODE_DOWN]) {
            deltaY = stepLength;
        }

        postion.x += deltaX;
        postion.y += deltaY;
    }

    void UpdateAnimation() {
        if (deltaY || deltaX) {
            animation_step = (animation_step + 1) % animation_total;
            if (postion.x != deltaX) {
                source.x = animation_step * frameWidth;
            } else if (postion.y != deltaY) {
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

    void Render(SDL_Renderer *mRenderer) {
        SDL_RenderTexture(mRenderer, characterTexture, &source, &postion);
        SDL_SetRenderDrawColor(mRenderer, 0xff,0xff,0xff,0xff);

        SDL_RenderDebugText(mRenderer, postion.x + 40, postion.y + 20, "crosanini maslonini");
        SDL_SetRenderDrawColor(mRenderer, 0xff,0x33,0x33,0xff);
        SDL_RenderDebugText(mRenderer, postion.x + 40, postion.y + 40, "smartbrain god");
        SDL_RenderDebugText(mRenderer, postion.x + 40, postion.y + 60, "3k 900$.s");

        SDL_SetRenderDrawColor(mRenderer, 0x00,0xff,0xff,0xff);
        SDL_RenderDebugText(mRenderer, 10,  10, debug_text.str().c_str());
    }
};

struct SDLApp {
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    bool mDone = false;
    Sprite *mainCharacter;

    void create_scene() {
        mainCharacter = new Sprite(mRenderer);
    }

    SDLApp() {
        SDL_Init(SDL_INIT_VIDEO);
        std::tm tm = {};
        std::stringstream ss("Dec 13 2025 12:35:34");
        ss >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
        const auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        SDL_CreateWindowAndRenderer("Steal A Brainrot", 1280, 1024, 0, &mWindow, &mRenderer);
        if (mRenderer) {
            SDL_Log("renderer %s", SDL_GetRendererName(mRenderer));
        }
        create_scene();
        Brainrot brainrot_six_seven = {
            10000, 67,
            tp,
            "SixSeven \0", COMMON
        };
    }

    ~SDLApp() {
        SDL_DestroyWindow(mWindow);
        SDL_DestroyRenderer(mRenderer);
        delete mainCharacter;
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

    void Update() {
        mainCharacter->Update();
    }

    void Render() {
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(mRenderer);
        mainCharacter->Render(mRenderer);
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
    app.MainLoop();
    return 0;
}
