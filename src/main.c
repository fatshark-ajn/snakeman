#include "snakeman/platform.h"

int main(void) {
    GameConfig config;

    config.window_width = 1280;
    config.window_height = 720;
    config.target_fps = 60;
    config.seed = 1337;

    return sdl_run_app(&config);
}
