#include <chrono>
#include <cstdio>
#include <iostream>
#include <SDL3/SDL.h>

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

int direction_user_should_move_y() {
    const bool *key_states = SDL_GetKeyboardState(NULL);
    int direction = 0;

    /* (We're writing our code such that it sees both keys are pressed and cancels each other out!) */
    if (key_states[SDL_SCANCODE_W]) {
        direction += -1; /* pressed what would be "W" on a US QWERTY keyboard. Move forward! */
    }

    if (key_states[SDL_SCANCODE_S]) {
        direction += 1; /* pressed what would be "S" on a US QWERTY keyboard. Move backward! */
    }

    /* (In practice it's likely you'd be doing full directional input in here, but for simplicity, we're just showing forward and backward) */

    return direction; /* wasn't key in W or S location, don't move. */
}

int direction_user_should_move_x() {
    const bool *key_states = SDL_GetKeyboardState(NULL);
    int direction = 0;

    if (key_states[SDL_SCANCODE_D]) {
        direction += 1; /* pressed what would be "W" on a US QWERTY keyboard. Move forward! */
    }

    if (key_states[SDL_SCANCODE_A]) {
        direction += -1; /* pressed what would be "S" on a US QWERTY keyboard. Move backward! */
    }

    /* (In practice it's likely you'd be doing full directional input in here, but for simplicity, we're just showing forward and backward) */

    return direction; /* wasn't key in W or S location, don't move. */
}



struct Sprite {
    SDL_Texture *characterTexture;
    SDL_FRect source;
    SDL_FRect postion;
    float direction_x = 0;
    float direction_y = 0;
    int animation_step = -1;
    int animation_total = 4;
    int animation_sync = 0;
        int frameWidth = 200;

    Sprite() {

    }

    Sprite(SDL_Renderer *mRenderer) {
        direction_x = 0;
        direction_y = 0;

        postion = SDL_FRect{0, 0, 200, 245};
        source = SDL_FRect{0, 0, 200, 245};

        SDL_Surface *sdlSurface1 = SDL_LoadBMP("..\\dude.bmp");
        characterTexture = SDL_CreateTextureFromSurface(mRenderer, sdlSurface1);
        SDL_DestroySurface(sdlSurface1);
    }

    ~Sprite() {
        SDL_DestroyTexture(characterTexture);
    }

    void SetPosition(float x, float y) {

    }

    void Update() {
        animation_step = (animation_step + 1) % animation_total;
        direction_x = direction_x + direction_user_should_move_x();
        direction_y = direction_y + direction_user_should_move_y();
        animation_sync = (animation_sync + 1) % 10;
        if (postion.x != direction_x) {
            postion.x = direction_x;
            if (animation_sync == 0) {
                source.x = animation_step * frameWidth;
            }
        } else if (postion.y != direction_y) {
            postion.y = direction_y;
            if (animation_sync == 0) {
                source.x = animation_step * frameWidth;
            }
        } else {
            source.x = 0;
        }
    }

    void Render(SDL_Renderer *mRenderer) {
        SDL_RenderTexture(mRenderer, characterTexture, &source, &postion);
    }
};

struct SDLApp {
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    bool mDone = false;
    Sprite* mainCharacter;

    void create_scene() {
        mainCharacter = new Sprite(mRenderer);
    }

    SDLApp() {
        SDL_Init(SDL_INIT_VIDEO);
        std::tm tm = {};
        std::stringstream ss("Dec 13 2025 12:35:34");
        ss >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
        const auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        SDL_CreateWindowAndRenderer("Steal A Brainrot", 800, 600, 0, &mWindow, &mRenderer);
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
        SDL_Delay(10);
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
