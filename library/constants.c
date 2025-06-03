
#include "constants.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};

const vector_t PLAYER_DIMS = {10, 60};
const vector_t PLAYER_CENTER_POS = {500, 50};
const char *PLAYER_INFO = "player";

const double OUTER_RADIUS = 15;
const double INNER_RADIUS = 15;

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
const vector_t JUMP_INITIAL_VELOCITY = (vector_t){.x = 0, .y = 200.0};
const double Y_GRAV_ACCELERATION_MAG_PER_S = 200.0;
const double DUCK_ACCELERATION_CHANGE = 0;

// Background and obstacle velocity. Background 3 is the top layer (i.e. the
// velocity of the game)
const double INIT_BACKGROUND_1_SKY_VELOCITY = 50.0;
const double INIT_BACKGROUND_2_TREE_VELOCITY = 100.0;
const double INIT_BACKGROUND_3_BUILDINGS_VELOCITY = 150.0;

const color_t SPRITE_COLOR = (color_t){0.0, 0.0, 0.0};
const color_t OBS_COLOR = (color_t){0.2, 0.2, 0.3};  // going to be tables
const color_t QUESADILLA_COLOR = (color_t){1, 1, 0}; // coin

const size_t BODY_ASSETS = 1;
const char *PLAYER_SPRITE_PATH = "assets/frogger.png";
const char *BACKGROUND_PATH = "assets/background.jpg";

const double OBSTACLE_START_WAIT_TIME = 3.0;
const double GAME_OVER_WAIT_TIME = 3.0;

const size_t MAX_GAMES = 1024;