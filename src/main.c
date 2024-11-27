#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <math.h>
#include <stdio.h>
#define WIDTH 1400
#define HEIGHT 800
#define DELTA_TIME 1.0f/60.0f;

typedef struct {
    float x1, y1, x2, y2;
} Wall;

// Rotate and draw function
void rotate_and_draw(SDL_Renderer* renderer, float cx, float cy, int x, int y, float rad) {
    float xr = cos(rad) * (x) - sin(rad) * (y) + cx;
    float yr = sin(rad) * (x) + cos(rad) * (y) + cy;
    SDL_RenderDrawPoint(renderer, (int)xr, (int)yr);
}

// Draw a circle with rotation
void draw_circle(SDL_Renderer* renderer, float cx, float cy, float r, float angle) {
    float rad = angle * M_PI / 180.0;
    int x = 0, y = r;
    int p = 1 - r;

    // Initial octants
    rotate_and_draw(renderer, cx, cy, x, y, rad);
    rotate_and_draw(renderer, cx, cy, -x, y, rad);
    rotate_and_draw(renderer, cx, cy, x, -y, rad);
    rotate_and_draw(renderer, cx, cy, -x, -y, rad);
    rotate_and_draw(renderer, cx, cy, y, x, rad);
    rotate_and_draw(renderer, cx, cy, -y, x, rad);
    rotate_and_draw(renderer, cx, cy, y, -x, rad);
    rotate_and_draw(renderer, cx, cy, -y, -x, rad);

    while (x < y) {
        if (p < 0) {
            p = p + 2 * x + 3;
        } else {
            p = p + 2 * (x - y) + 5;
            y--;
        }
        x++;

        rotate_and_draw(renderer, cx, cy, x, y, rad);
        rotate_and_draw(renderer, cx, cy, -x, y, rad);
        rotate_and_draw(renderer, cx, cy, x, -y, rad);
        rotate_and_draw(renderer, cx, cy, -x, -y, rad);
        rotate_and_draw(renderer, cx, cy, y, x, rad);
        rotate_and_draw(renderer, cx, cy, -y, x, rad);
        rotate_and_draw(renderer, cx, cy, y, -x, rad);
        rotate_and_draw(renderer, cx, cy, -y, -x, rad);
    }
}

// Multi-wall raycasting function
void raycast(SDL_Renderer* renderer, float cx, float cy, float angle, float ray_length, Wall* walls, int num_walls) {
    float rad = angle * M_PI / 180.0;
    float rayDirX = cos(rad);
    float rayDirY = sin(rad);
    float closest_t = ray_length;
    float closest_x = cx + ray_length * rayDirX;
    float closest_y = cy + ray_length * rayDirY;

    for (int i = 0; i < num_walls; i++) {
        Wall wall = walls[i];
        float x1 = wall.x1, y1 = wall.y1, x2 = wall.x2, y2 = wall.y2;
        float denom = (x2 - x1) * rayDirY - (y2 - y1) * rayDirX;

        if (denom == 0) continue; // Parallel lines

        float t_num = (cx - x1) * (y2 - y1) - (cy - y1) * (x2 - x1);
        float s_num = (cx - x1) * rayDirY - (cy - y1) * rayDirX;
        float t = t_num / denom;
        float s = s_num / denom;

        if (t >= 0 && t <= closest_t && s >= 0 && s <= 1) {
            closest_t = t;
            closest_x = cx + t * rayDirX;
            closest_y = cy + t * rayDirY;
        }
    }

    SDL_RenderDrawLine(renderer, cx, cy, closest_x, closest_y);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed initializing SDL lib: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("2D Raycasting", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int quit = 0;
    SDL_Event e;
    float cx = 500, cy = 500, angle = 0.0f, radius = 50.0f, rotation_speed = 2.0f, ray_length = 1000.0f;
    int direction = 1;

    Wall walls[] = {
        {600, 100, 800, 600},
        {100, 300, 900, 300},
        {400, 700, 1000, 700},
    };
    int num_walls = sizeof(walls) / sizeof(walls[0]);

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        if (keystate[SDL_SCANCODE_A]) cx--;
        if (keystate[SDL_SCANCODE_D]) cx++;
        if (keystate[SDL_SCANCODE_W]) cy--;
        if (keystate[SDL_SCANCODE_S]) cy++;
        if (keystate[SDL_SCANCODE_Q]) angle -= rotation_speed;
        if (keystate[SDL_SCANCODE_R]) angle += rotation_speed;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        draw_circle(renderer, cx, cy, radius, angle);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < num_walls; i++) {
            SDL_RenderDrawLine(renderer, walls[i].x1, walls[i].y1, walls[i].x2, walls[i].y2);
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        int num_rays = 120;
        for (int i = 0; i < num_rays; i++) {
            float ray_angle = angle + i * (60.0 / num_rays);
            raycast(renderer, cx, cy, ray_angle, ray_length, walls, num_walls);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
