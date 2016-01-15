#pragma once
#include <string>
using namespace std;

#define MAX_ENEMIES 255
#define DEFAULT_WINDOW_RESOULUTION_X 600  // 461
#define DEFAULT_WINDOW_RESOULUTION_Y 1000 // 768
#define DEFAULT_WINDOW_RESOULUTION DEFAULT_WINDOW_RESOULUTION_X, DEFAULT_WINDOW_RESOULUTION_Y
#define GAME_WORLD_X 600
#define GAME_WORLD_Y 1000
#define GAME_RESOULUTION GAME_WORLD_X, GAME_WORLD_Y

#define GAME_NAME "SPACE WARS"

#define VSYNC true

enum TEX {
  spaceship1,
  spaceship2,
  spaceship3,
  enemy1,
  enemy2,
  enemy3,
  enemy4,
  enemy5,
  enemy6,
  enemy7,
  bullet,
  Background,
  Life,
  Shield,
  Missile,
  TEX_COUNT
};

const string filepath = "..\\res/";
static const string textureNames[TEX_COUNT]{"img/spaceship1.png",
                                            "img/spaceship2.png",
                                            "img/spaceship3.png",
                                            "img/Spaceship-Drakir1.png",
                                            "img/Spaceship-Drakir2.png",
                                            "img/Spaceship-Drakir3.png",
                                            "img/Spaceship-Drakir4.png",
                                            "img/Spaceship-Drakir5.png",
                                            "img/Spaceship-Drakir6.png",
                                            "img/Spaceship-Drakir7.png",
                                            "img/bullet.png",
                                            "img/Background.png",
                                            "img/Life.png",
                                            "img/Shield.png",
                                            "img/Missile.png"};

#ifdef _DEBUG
#if defined(_WIN32) || defined(_WIN64)
#define BREAKPOINT __debugbreak();
#elif __APPLE__
#define BREAKPOINT __builtin_trap();
#elif __linux__
#define BREAKPOINT __builtin_trap();
#endif
#else
#define BREAKPOINT
#endif