
#include "constants.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t LEADERBOARD_SIZE = {100, 50};

// TODO: Change
const vector_t PLAYER_DIMS = {20, 40};
const vector_t PLAYER_CENTER_POS = {500, 50};
const char *PLAYER_INFO = "player";

const double OUTER_RADIUS = 30;
const double INNER_RADIUS = 30;

const double MIN_REACTION_TIME_S = 1.5;

// obstacle = table (square 50 x 50) can spawn multiple obstacles in a row
const size_t OBSTACLE_HW = 50;
const size_t MAX_STACKED_OBSTACLES = 5;
const double FIRST_OBSTACLE_WAIT_TIME = 3.0;
const size_t OBSTACLE_CAPACITY = 1024;
const double AVG_TIME_OBSTACLES = 5.0;
const char *OBSTACLE_INFO = "obstacle";

// pts of player depending on action
const size_t PLAYER_STANDING_PTS = 16;
const size_t PLAYER_RUNNING = 31;
const size_t QUESADILLA_PTS = 20;

// player movements
const vector_t DUCK_INITIAL_VELOCITY = (vector_t){.x = 0, .y = 0};
const vector_t JUMP_INITIAL_VELOCITY = (vector_t){.x = 0, .y = 250.0};
const double Y_GRAV_ACCELERATION_MAG_PER_S = 300.0;
const double DUCK_ACCELERATION_CHANGE = 0;

// Background and obstacle velocity. Background 3 is the top layer (i.e. the
// velocity of the game)
const vector_t INIT_BACKGROUND_1_SKY_VELOCITY = (vector_t){.x = 50.0, .y = 0};
const vector_t INIT_BACKGROUND_2_TREE_VELOCITY =
    (vector_t){.x = 100.0, .y = 0.0};
const vector_t INIT_BACKGROUND_3_BUILDINGS_VELOCITY =
    (vector_t){.x = 150.0, .y = 0};

const color_t SPRITE_COLOR = (color_t){0.0, 0.0, 0.0};
const color_t OBS_COLOR = (color_t){0.2, 0.2, 0.3};       // going to be tables
const color_t QUESADILLA_COLOR = (color_t){1, 1, 0};      // coin
const color_t LEADERBOARD_COLOR = (color_t){0.6, 0.3, 0}; // leaderboard
const color_t TEXT_COLOR = (color_t){1,1,1};

const size_t BODY_ASSETS = 1;
const char *PLAYER_SPRITE_PATH = "assets/Amudhan.png";
const char *PLAYER_SPRITE_ANDREA_PATH = "assets/Andrea.png";
const char *PLAYER_SPRITE_ARJUN_PATH = "assets/Arjun.png";
const char *SKY_PATH = "assets/Sky.png";
const char *BUILDING_PATH = "assets/Building.png";
const char *TREE_PATH = "assets/Tree.png";
const char *OBSTACLE_SPRITE_PATH = "assets/Obstacle.png";
const char *QUESADILLA_PATH = "assets/Quesadilla.png";
const char *MUSIC_PATH = "assets/memphis-trap-wav-349366.wav";

const double OBSTACLE_START_WAIT_TIME = 3.0;
const double GAME_OVER_WAIT_TIME = 3.0;

const size_t MAX_GAMES = 1024;