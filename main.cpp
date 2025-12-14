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

struct SDLApp {
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    bool mDone = false;
    SDL_Surface *sdlSurface1;
    SDL_Surface *winSurface;
    SDL_Rect charRect;
    SDL_Rect dstrect;
    int direction_x = 0;
    int direction_y = 0;

    SDLApp() {
        SDL_Init(SDL_INIT_VIDEO);
        std::tm tm = {};
        std::stringstream ss("Dec 13 2025 12:35:34");
        ss >> std::get_time(&tm, "%b %d %Y %H:%M:%S");
        const auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        direction_x = 0;
        direction_y = 0;

        charRect = {50, 50, 50, 50};
        dstrect = {0, 0, 100, 100};

        Brainrot brainrot_six_seven = {
            10000, 67,
            tp,
            "SixSeven \0", COMMON
        };
    }

    ~SDLApp() {
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }

    void Tick() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                mDone = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Log("Key presseed %d %d", event.key.key, event.key.mod);
            }
        }
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderClear(mRenderer);
        direction_x = direction_x + direction_user_should_move_x();
        direction_y = direction_y + direction_user_should_move_y();
        dstrect.x = direction_x;
        dstrect.y = direction_y;

        winSurface = SDL_GetWindowSurface(mWindow);


        SDL_BlitSurface(sdlSurface1, NULL, winSurface, &dstrect);
        SDL_SetRenderDrawColor(mRenderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_UpdateWindowSurface(mWindow);

        SDL_Delay(16);
    }

    void MainLoop() {
        SDL_CreateWindowAndRenderer("Steal A Brainrot", 800, 600, 0, &mWindow, &mRenderer);

        sdlSurface1 = SDL_LoadBMP("C:\\Users\\Vladimir\\CLionProjects\\brainrot\\sahur.bmp");

        while (!mDone) {
            Tick();
        }

        printf("Hello, World!\n");
    }
};

int main() {
    auto app = SDLApp();
    app.MainLoop();
    return 0;
}
