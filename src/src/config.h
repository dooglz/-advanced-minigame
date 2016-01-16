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

#define GAME_NAME "Astral Insanity"

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
  Explosion,
  TEX_COUNT
};

extern string filepath;
static const string textureNames[TEX_COUNT]{"img/spaceship1.png",
                                            "img/spaceship2.png",
                                            "img/spaceship3.png",
                                            "img/spaceship-drakir1.png",
                                            "img/spaceship-drakir2.png",
                                            "img/spaceship-drakir3.png",
                                            "img/spaceship-drakir4.png",
                                            "img/spaceship-drakir5.png",
                                            "img/spaceship-drakir6.png",
                                            "img/spaceship-drakir7.png",
                                            "img/bullet.png",
                                            "img/background.png",
                                            "img/life.png",
                                            "img/shield.png",
                                            "img/missile.png",
                                            "img/explosion.png"};

enum SOUND { MISSILE, EXPLOSION, LASER, SHOOT, SFX_COUNT };

static const string soundNames[SFX_COUNT]{"sounds/missile.wav", "sounds/explosion.wav",
                                            "sounds/laser.wav","sounds/shoot.wav" };


enum MUSIC { AMBIENTBGM, MAINBGM, MAINBGM2, BOSSBGM, MUSIC_COUNT };

static const string musicNames[]{
  "music/orbitals.ogg", "music/robotix.ogg", "music/starflake.ogg", "music/synth5.ogg"
};


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