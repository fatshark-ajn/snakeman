#include "snakeman/platform.h"

#include <SDL2/SDL.h>

static InputState input_from_frame(int *should_exit) {
    SDL_Event event;
    InputState input;

    input.direction = INPUT_DIR_NONE;
    input.start_pressed = 0;
    input.pause_pressed = 0;
    input.resume_pressed = 0;
    input.restart_pressed = 0;
    input.quit_pressed = 0;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            input.quit_pressed = 1;
            *should_exit = 1;
        }

        if (event.type != SDL_KEYDOWN) {
            continue;
        }

        switch (event.key.keysym.sym) {
            case SDLK_UP:
            case SDLK_w:
                input.direction = INPUT_DIR_UP;
                break;
            case SDLK_DOWN:
            case SDLK_s:
                input.direction = INPUT_DIR_DOWN;
                break;
            case SDLK_LEFT:
            case SDLK_a:
                input.direction = INPUT_DIR_LEFT;
                break;
            case SDLK_RIGHT:
            case SDLK_d:
                input.direction = INPUT_DIR_RIGHT;
                break;
            case SDLK_RETURN:
            case SDLK_SPACE:
                input.start_pressed = 1;
                break;
            case SDLK_r:
                input.restart_pressed = 1;
                break;
            case SDLK_p:
                input.pause_pressed = 1;
                input.resume_pressed = 1;
                break;
            case SDLK_q:
            case SDLK_ESCAPE:
                input.quit_pressed = 1;
                break;
            default:
                break;
        }
    }

    return input;
}

int sdl_run_app(const GameConfig *config) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int should_exit;
    uint64_t perf_freq;
    uint64_t previous_counter;
    float accumulator;
    float fixed_dt;
    Game game;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
        return 1;
    }

    window = SDL_CreateWindow(
        "Snakeman",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config->window_width,
        config->window_height,
        SDL_WINDOW_SHOWN
    );
    if (window == NULL) {
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    game_init(&game, config);

    should_exit = 0;
    perf_freq = SDL_GetPerformanceFrequency();
    previous_counter = SDL_GetPerformanceCounter();
    accumulator = 0.0f;
    fixed_dt = 1.0f / (float)config->target_fps;

    while (!should_exit && game.state != GAME_STATE_QUIT) {
        uint64_t current_counter = SDL_GetPerformanceCounter();
        uint64_t delta_counter = current_counter - previous_counter;
        float frame_dt = (float)delta_counter / (float)perf_freq;
        InputState input = input_from_frame(&should_exit);

        previous_counter = current_counter;
        accumulator += frame_dt;

        while (accumulator >= fixed_dt) {
            game_update(&game, &input, fixed_dt);
            accumulator -= fixed_dt;
        }

        if (game.state == GAME_STATE_TITLE) {
            SDL_SetRenderDrawColor(renderer, 20, 28, 44, 255);
        } else if (game.state == GAME_STATE_RUNNING) {
            SDL_SetRenderDrawColor(renderer, 16, 48, 24, 255);
        } else if (game.state == GAME_STATE_HIGHSCORES) {
            SDL_SetRenderDrawColor(renderer, 34, 16, 44, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 24, 24, 24, 255);
        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
