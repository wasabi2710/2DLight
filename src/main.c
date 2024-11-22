#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#define WIDTH 1400
#define HEIGHT 800
#define DELTA_TIME 1.0f/60.0f;

void rotate_and_draw(SDL_Renderer* renderer, float cx, float cy, int x, int y, float rad) {
    // Rotate the point (x, y) around (cx, cy) by angle rad (in radians)
    float xr = cos(rad) * (x) - sin(rad) * (y) + cx;
    float yr = sin(rad) * (x) + cos(rad) * (y) + cy;
    // Draw the rotated point
    SDL_RenderDrawPoint(renderer, (int)xr, (int)yr);
}
void draw_circle(SDL_Renderer* renderer, float cx, float cy, float r, float angle) {
    // Convert angle to radians
    float rad = angle * M_PI / 180.0;
    // Inits
    int x = 0;
    int y = r;
    int p = 1 - r; // Decision parameter
    // Draw the initial 8 octants
    rotate_and_draw(renderer, cx, cy, x, y, rad);
    rotate_and_draw(renderer, cx, cy, -x, y, rad);
    rotate_and_draw(renderer, cx, cy, x, -y, rad);
    rotate_and_draw(renderer, cx, cy, -x, -y, rad);
    rotate_and_draw(renderer, cx, cy, y, x, rad);
    rotate_and_draw(renderer, cx, cy, -y, x, rad);
    rotate_and_draw(renderer, cx, cy, y, -x, rad);
    rotate_and_draw(renderer, cx, cy, -y, -x, rad);
    // Loop to rasterize the rest
    while (x < y) {
        if (p < 0) {
            p = p + 2 * x + 3;
        } else {
            p = p + 2 * (x - y) + 5;
            y = y - 1;
        }
        x++;

        // Draw the rotated points
        rotate_and_draw(renderer, cx, cy, x, y, rad);
        rotate_and_draw(renderer, cx, cy, -x, y, rad);
        rotate_and_draw(renderer, cx, cy, x, -y, rad);
        rotate_and_draw(renderer, cx, cy, -x, -y, rad);
        rotate_and_draw(renderer, cx, cy, y, x, rad);
        rotate_and_draw(renderer, cx, cy, -y, x, rad);
        rotate_and_draw(renderer, cx, cy, y, -x, rad);
        rotate_and_draw(renderer, cx, cy, -y, -x, rad);
    }
    // Draw a line from the center to the edge of the circle
    SDL_RenderDrawLine(renderer, cx + (r / 1.5)*cos(rad), cy + (r / 1.5)*sin(rad), cx + r * cos(rad), cy + r * sin(rad));
    SDL_RenderDrawLine(renderer, cx + (r / 1.5) * cos(rad + M_PI / 4), cy + (r / 1.5) * sin(rad + M_PI / 4), cx + r * cos(rad + M_PI / 4), cy + r * sin(rad + M_PI / 4));
    SDL_RenderDrawLine(renderer, cx + (r / 1.5) * cos(rad + M_PI / 2), cy + (r / 1.5) * sin(rad + M_PI / 2), cx + r * cos(rad + M_PI / 2), cy + r * sin(rad + M_PI / 2));
    SDL_RenderDrawLine(renderer, cx + (r / 1.5) * cos(rad + 3 * M_PI / 4), cy + (r / 1.5) * sin(rad + 3 * M_PI / 4), cx + r * cos(rad + 3 * M_PI / 4), cy + r * sin(rad + 3 * M_PI / 4));
    SDL_RenderDrawLine(renderer, cx + (r / 1.5) * cos(rad + M_PI), cy + (r / 1.5) * sin(rad + M_PI), cx + r * cos(rad + M_PI), cy + r * sin(rad + M_PI));
    SDL_RenderDrawLine(renderer, cx + (r / 1.5) * cos(rad + 5 * M_PI / 4), cy + (r / 1.5) * sin(rad + 5 * M_PI / 4), cx + r * cos(rad + 5 * M_PI / 4), cy + r * sin(rad + 5 * M_PI / 4));
    SDL_RenderDrawLine(renderer, cx + (r / 1.5) * cos(rad + 3 * M_PI / 2), cy + (r / 1.5) * sin(rad + 3 * M_PI / 2), cx + r * cos(rad + 3 * M_PI / 2), cy + r * sin(rad + 3 * M_PI / 2));
    SDL_RenderDrawLine(renderer, cx + (r / 1.5) * cos(rad + 7 * M_PI / 4), cy + (r / 1.5) * sin(rad + 7 * M_PI / 4), cx + r * cos(rad + 7 * M_PI / 4), cy + r * sin(rad + 7 * M_PI / 4));
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) { // init video
        printf("Failed initializing SDL lib: %s\n", SDL_GetError());
        return 1;
    }
    // window creation
    SDL_Window* window = SDL_CreateWindow("2D Ray", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    // create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        return 1;
    }

    // render properties
    int direction = 1;
    float radius = 50.0f;
    float rotation_speed = 2.0f;
    float angle = 0.0f;

    // start event loop
    int quit = 0;
    SDL_Event e;
    // continuous key state
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    while (!quit) {
        if (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            // key hold event
            if (keystate[SDL_SCANCODE_A]) {
                direction = -1;
            } else if (keystate[SDL_SCANCODE_D]) {
                direction = 1;
            } else {
                direction = 0;
            }
        }

        // background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // render logics
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        draw_circle(renderer, 200, 200, radius, angle);
        angle += rotation_speed * direction;

        // present the subsequent renderer
        SDL_RenderPresent(renderer);
    }

    // destroy
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
