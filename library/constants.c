
#include "constants.h"

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t LEADERBOARD_SIZE = {180, 100};

// TODO: Change
const vector_t PLAYER_DIMS = {40, 80};
const vector_t PLAYER_CENTER_POS = {500, 50};
const char *PLAYER_INFO = "player";

const double OUTER_RADIUS = 30;
const double INNER_RADIUS = 30;

const double MIN_REACTION_TIME_S = 1.0;

// obstacle = table (square 50 x 50) can spawn multiple obstacles in a row
const size_t OBSTACLE_HW = 50;
const size_t MAX_STACKED_OBSTACLES = 20;
const size_t MAX_N_QUEUED_OBST = 15;
const size_t OBSTACLE_CAPACITY = 1024;
const double FIRST_OBSTACLE_WAIT_TIME = 1.5;
const double MAX_TIME_UPDATE = 3.0;
const char *OBSTACLE_INFO = "obstacle";
const char *MAGNET_INFO = "magnet";
const char *COIN_INFO = "coin";
const double MAGNET_FREQUENCY = 0.25;
const double MAGNET_PERIOD_S = 20.0;
const char *LEADERBOARD_INFO = "leaderboard";

const double MAGNET_RAD = 12.5;
const double COIN_RAD = 10;

// pts of player depending on action
const size_t PLAYER_STANDING_PTS = 16;
const size_t PLAYER_RUNNING = 31;
const size_t QUESADILLA_PTS = 20;
const size_t PRICE = 10;

// text rendering dimensions
const vector_t SCORE_TL = (vector_t){.x = 0.75, .y = 0.1};
const vector_t SCORE_SIZE = (vector_t){.x = 0.5, .y = 0.3};
const vector_t HIGH_SCORE_TL = (vector_t){.x = 0.9, .y = 0.6};
const vector_t HIGH_SCORE_SIZE = (vector_t){.x = 0.8, .y = 0.3};
const vector_t COIN_TL = (vector_t){.x = 0.25, .y = 0.35};
const vector_t COIN_SIZE = (vector_t){.x = 0.5, .y = 0.3};

// player movements
const vector_t DUCK_INITIAL_VELOCITY = (vector_t){.x = 0, .y = 0};
const vector_t JUMP_INITIAL_VELOCITY = (vector_t){.x = 0, .y = 250.0};
const double DUCK_ACCELERATION_CHANGE = 0;

// Background and obstacle velocity. Background 3 is the top layer (i.e. the
// velocity of the game)
const vector_t INIT_BACKGROUND_1_SKY_VELOCITY = (vector_t){.x = 50.0, .y = 0};
const vector_t INIT_BACKGROUND_2_TREE_VELOCITY =
    (vector_t){.x = 100.0, .y = 0.0};
const vector_t INIT_BACKGROUND_3_BUILDINGS_VELOCITY =
    (vector_t){.x = 150.0, .y = 0};

// Magic ratios to make sure that jump velocity scales with x-velocity of
// obstacles Found by tweaking with fixed initial values - these ratios gave a
// nice smooth look Kinematics equations tell us that these work Points should
// be deducted from Arjun if this is problematic
const double JUMP_VEL_COMPONENT_RATIO = 5.0 / 3.0;
const double GRAV_JUMP_VEL_RATIO = 6.0 / 5.0;

const color_t SPRITE_COLOR = (color_t){0.0, 0.0, 0.0};
const color_t OBS_COLOR = (color_t){0.2, 0.2, 0.3};       // going to be tables
const color_t COIN_COLOR = (color_t){1, 1, 0};            // coin
const color_t LEADERBOARD_COLOR = (color_t){0.6, 0.3, 0}; // leaderboard
const color_t MAGNET_COLOR = (color_t){1, 0, 0};
const color_t TEXT_COLOR = (color_t){0, 0, 0};

const size_t BODY_ASSETS = 1;

const char *START_SCREEN_PATH = "assets/Start.png";
const char *GAME_OVER_PATH = "assets/Game_Over.png";
const char *SHOP_PATH = "assets/Shop.png";
const char *CARD_PATH = "assets/Card.png";
const char *PLAYER_SPRITE_AMUDHAN_PATH = "assets/Amudhan.png";
const char *PLAYER_SPRITE_ANDREA_PATH = "assets/Andrea.png";
const char *PLAYER_SPRITE_ARJUN_PATH = "assets/Arjun.png";
const char *SKY_PATH = "assets/Sky.png";
const char *BUILDING_PATH = "assets/Building.png";
const char *TREE_PATH = "assets/Tree.png";
const char *QUESADILLA_PATH = "assets/Quesadilla.png";
const char *GAME_MUSIC_PATH = "assets/GameAudio.wav";
const char *JUMP_MUSIC_PATH = "assets/Jump.wav";
const char *MENU_MUSIC_PATH = "assets/MainMenuAudio.wav";
const char *OBSTACLE_GEN_PATH_TEMPLATE = "assets/obstacles/obstacle-";
const char *OBSTACLE_IMG_EXT = ".png";

const double OBSTACLE_START_WAIT_TIME = 3.0;
const double GAME_OVER_WAIT_TIME = 3.0;

const size_t MAX_GAMES = 1024;