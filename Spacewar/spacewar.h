
#ifndef _SPACEWAR_H             // Prevent multiple definitions if this 
#define _SPACEWAR_H             // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include <string>
#include "game.h"
#include "textureManager.h"
#include "image.h"
#include "dashboard.h"
#include "planet.h"
#include "ship.h"
#include "torpedo.h"

namespace spacewarNS
{
    const char FONT[] = "Arial Bold";  // font
    const int FONT_BIG_SIZE = 256;     // font height
    const int FONT_SCORE_SIZE = 48;
    const COLOR_ARGB FONT_COLOR = graphicsNS::YELLOW;
    const COLOR_ARGB SHIP1_COLOR = graphicsNS::BLUE;
    const COLOR_ARGB SHIP2_COLOR = graphicsNS::YELLOW;
    const int SCORE_Y = 10;
    const int SCORE1_X = 60;
    const int SCORE2_X = GAME_WIDTH-80;
    const int HEALTHBAR_Y = 30;
    const int SHIP1_HEALTHBAR_X = 40;
    const int SHIP2_HEALTHBAR_X = GAME_WIDTH-100;
    const int COUNT_DOWN_X = GAME_WIDTH/2 - FONT_BIG_SIZE/4;
    const int COUNT_DOWN_Y = GAME_HEIGHT/2 - FONT_BIG_SIZE/2;
    const int COUNT_DOWN = 5;           // count down from 5
    const int BUF_SIZE = 20;
    const int ROUND_TIME = 5;           // time until new round starts
}

//=============================================================================
// This class is the core of the game
//=============================================================================
class Spacewar : public Game
{
private:
    // game items
    TextureManager menuTexture, nebulaTexture, gameTextures;   // textures
    Ship    ship1, ship2;       // spaceships
    Torpedo torpedo1, torpedo2; // torpedoes
    Planet  planet;             // the planet
    Image   nebula;             // backdrop image
    Image   menu;               // menu image
    Bar     healthBar;          // health bar for ships
    TextDX  fontBig;            // DirectX font for game banners
    TextDX  fontScore;
    bool    menuOn;
    bool    countDownOn;        // true when count down is displayed
    float   countDownTimer;
    char buffer[spacewarNS::BUF_SIZE];
    bool    ship1Scored, ship2Scored;   // true if ship scored during round
    bool    roundOver;          // true when round is over
    float   roundTimer;         // time until new round starts
    int     ship1Score, ship2Score; // scores
    bool    musicOff;           // true to turn music off

public:
    // Constructor
    Spacewar();
    // Destructor
    virtual ~Spacewar();
    // Initialize the game
    void initialize(HWND hwnd);
    void update();      // must override pure virtual from Game
    void ai();          // "
    void collisions();  // "
    void render();      // "
    void consoleCommand(); // process console command
    void roundStart();  // start a new round of play
    void releaseAll();
    void resetAll();
};

#endif
