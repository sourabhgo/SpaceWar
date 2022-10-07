// This class is the core of the game

#include "spaceWar.h"

//=============================================================================
// Constructor
//=============================================================================
Spacewar::Spacewar()
{
    menuOn = true;
    countDownOn = false;
    roundOver = false;
    ship1Score = 0;
    ship2Score = 0;
    ship1Scored = false;
    ship2Scored = false;
    initialized = false;
    musicOff = false;
}

//=============================================================================
// Destructor
//=============================================================================
Spacewar::~Spacewar()
{
    releaseAll();           // call onLostDevice() for every graphics item
}

//=============================================================================
// Initializes the game
// Throws GameError on error
//=============================================================================
void Spacewar::initialize(HWND hwnd)
{
    Game::initialize(hwnd); // throws GameError

    // initialize DirectX fonts
    fontBig.initialize(graphics, spacewarNS::FONT_BIG_SIZE, false, false, spacewarNS::FONT);
    fontBig.setFontColor(spacewarNS::FONT_COLOR);
    fontScore.initialize(graphics, spacewarNS::FONT_SCORE_SIZE, false, false, spacewarNS::FONT);

    // menu texture
    if (!menuTexture.initialize(graphics,MENU_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu texture"));

    // nebula texture
    if (!nebulaTexture.initialize(graphics,NEBULA_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing nebula texture"));

    // main game textures
    if (!gameTextures.initialize(graphics,TEXTURES_IMAGE))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing game textures"));

    // menu image
    if (!menu.initialize(graphics,0,0,0,&menuTexture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menu"));

    // nebula image
    if (!nebula.initialize(graphics,0,0,0,&nebulaTexture))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing nebula"));

    // planet
    if (!planet.initialize(this, planetNS::WIDTH, planetNS::HEIGHT, 2, &gameTextures))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing planet"));

    // ship1
    if (!ship1.initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &gameTextures))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing ship1"));

    ship1.setFrames(shipNS::SHIP1_START_FRAME, shipNS::SHIP1_END_FRAME);
    ship1.setCurrentFrame(shipNS::SHIP1_START_FRAME);
    ship1.setColorFilter(SETCOLOR_ARGB(255,230,230,255));   // light blue, used for shield and torpedo
    ship1.setMass(shipNS::MASS);

    // ship2
    if (!ship2.initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &gameTextures))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing ship2"));

    ship2.setFrames(shipNS::SHIP2_START_FRAME, shipNS::SHIP2_END_FRAME);
    ship2.setCurrentFrame(shipNS::SHIP2_START_FRAME);
    ship2.setColorFilter(SETCOLOR_ARGB(255,255,255,64));    // light yellow, used for shield
    ship2.setMass(shipNS::MASS);

    // torpedo1
    if (!torpedo1.initialize(this, torpedoNS::WIDTH, torpedoNS::HEIGHT, torpedoNS::TEXTURE_COLS, &gameTextures))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing torpedo1"));

    torpedo1.setFrames(torpedoNS::START_FRAME, torpedoNS::END_FRAME);
    torpedo1.setCurrentFrame(torpedoNS::START_FRAME);
    torpedo1.setColorFilter(SETCOLOR_ARGB(255,128,128,255));   // light blue

    // torpedo2
    if (!torpedo2.initialize(this, torpedoNS::WIDTH, torpedoNS::HEIGHT, torpedoNS::TEXTURE_COLS, &gameTextures))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing torpedo2"));

    torpedo2.setFrames(torpedoNS::START_FRAME, torpedoNS::END_FRAME);
    torpedo2.setCurrentFrame(torpedoNS::START_FRAME);
    torpedo2.setColorFilter(SETCOLOR_ARGB(255,255,255,64));     // light yellow

    // health bar
    healthBar.initialize(graphics, &gameTextures, 0, spacewarNS::HEALTHBAR_Y, 2.0f, graphicsNS::WHITE);

    // Start ships on opposite sides of planet in stable clockwise orbit
    ship1.setX(GAME_WIDTH/4 - shipNS::WIDTH);
    ship2.setX(GAME_WIDTH - GAME_WIDTH/4);
    ship1.setY(GAME_HEIGHT/2 - shipNS::HEIGHT);
    ship2.setY(GAME_HEIGHT/2);
    ship1.setVelocity(VECTOR2(0,-shipNS::SPEED));
    ship2.setVelocity(VECTOR2(0,shipNS::SPEED));

    audio->playCue(ACTION_THEME);

    return;
}

//=============================================================================
// Update all game items
//=============================================================================
void Spacewar::update()
{
    if (menuOn)
    {
        if (input->anyKeyPressed())
        {
            menuOn = false;
            input->clearAll();
            roundStart();
        }
    } 
    else if(countDownOn)
    {
        countDownTimer -= frameTime;
        if(countDownTimer <= 0)
            countDownOn = false;
    } 
    else 
    {
        if (ship1.getActive())
        {
            if (input->isKeyDown(SHIP1_FORWARD_KEY) || input->getGamepadDPadUp(0))   // if engine on
            {
                ship1.setEngineOn(true);
                audio->playCue(ENGINE1);
            }
            else
            {
                ship1.setEngineOn(false);
                audio->stopCue(ENGINE1);
            }
            ship1.rotate(shipNS::NONE);
            if (input->isKeyDown(SHIP1_LEFT_KEY) || input->getGamepadDPadLeft(0))   // if turn ship1 left
                ship1.rotate(shipNS::LEFT);
            if (input->isKeyDown(SHIP1_RIGHT_KEY) || input->getGamepadDPadRight(0)) // if turn ship1 right
                ship1.rotate(shipNS::RIGHT);
            if (input->isKeyDown(SHIP1_FIRE_KEY) || input->getGamepadA(0))          // if ship1 fire
                torpedo1.fire(&ship1);                  // fire torpedo1
        }
        if (ship2.getActive())
        {
            if (input->isKeyDown(SHIP2_FORWARD_KEY)  || input->getGamepadDPadUp(1)) // if engine on
            {
                ship2.setEngineOn(true);
                audio->playCue(ENGINE2);
            }
            else
            {
                ship2.setEngineOn(false);
                audio->stopCue(ENGINE2);
            }
            ship2.rotate(shipNS::NONE);
            if (input->isKeyDown(VK_LEFT) || input->getGamepadDPadLeft(1))      // if turn ship2 left
                ship2.rotate(shipNS::LEFT);
            if (input->isKeyDown(VK_RIGHT) || input->getGamepadDPadRight(1))    // if turn ship2 right
                ship2.rotate(shipNS::RIGHT);
            if (input->isKeyDown(SHIP2_FIRE_KEY) || input->getGamepadA(1))      // if ship2 fire
                torpedo2.fire(&ship2);                  // fire torpedo2
        }
        if(roundOver)
        {
            roundTimer -= frameTime;
            if(roundTimer <= 0)
                roundStart();
        }
    }
    ship1.gravityForce(&planet, frameTime);
    ship2.gravityForce(&planet, frameTime);
    torpedo1.gravityForce(&planet, frameTime);
    torpedo2.gravityForce(&planet, frameTime);

    // Update the entities
    planet.update(frameTime);
    ship1.update(frameTime);
    ship2.update(frameTime);
    torpedo1.update(frameTime);
    torpedo2.update(frameTime);
}

//=============================================================================
// Start a new round of play
//=============================================================================
void Spacewar::roundStart()
{
    // Start ships on opposite sides of planet in stable clockwise orbit
    ship1.setX(GAME_WIDTH/4 - shipNS::WIDTH);
    ship2.setX(GAME_WIDTH - GAME_WIDTH/4);
    ship1.setY(GAME_HEIGHT/2 - shipNS::HEIGHT);
    ship2.setY(GAME_HEIGHT/2);
    ship1.setVelocity(VECTOR2(0,-shipNS::SPEED));
    ship2.setVelocity(VECTOR2(0,shipNS::SPEED));

    ship1.setDegrees(0);
    ship2.setDegrees(180);
    ship1.repair();
    ship2.repair();
    countDownTimer = spacewarNS::COUNT_DOWN;
    countDownOn = true;
    roundOver = false;
    ship1Scored = false;
    ship2Scored = false;
}

//=============================================================================
// Artificial Intelligence
//=============================================================================
void Spacewar::ai()
{}

//=============================================================================
// Handle collisions
//=============================================================================
void Spacewar::collisions()
{
    VECTOR2 collisionVector;
    // if collision between ship1 and planet
    if(ship1.collidesWith(planet, collisionVector))
    {
        ship1.toOldPosition();      // move ship out of collision
        ship1.damage(PLANET);
        input->gamePadVibrateLeft(0,65535,1.0);  // vibrate controller 0, 100%, 1.0 sec
    }
    // if collision between ship2 and planet
    if(ship2.collidesWith(planet, collisionVector))
    {
        ship2.toOldPosition();  // move ship out of collision
        ship2.damage(PLANET);
        input->gamePadVibrateLeft(1,65535,1.0);
    }
    // if collision between ship1 and ship2
    if(ship1.collidesWith(ship2, collisionVector))
    {
        // bounce off other ship
        ship1.bounce(collisionVector, ship2);
        ship2.bounce(collisionVector*-1, ship1);
        ship1.damage(SHIP);
        ship2.damage(SHIP);
        input->gamePadVibrateRight(0,30000,0.5);
        input->gamePadVibrateRight(1,30000,0.5);
    }
    // if collision between torpedos and ships
    if(torpedo1.collidesWith(ship2, collisionVector))
    {
        ship2.damage(TORPEDO);
        torpedo1.setVisible(false);
        torpedo1.setActive(false);
        input->gamePadVibrateRight(1,20000,0.5);
    }
    if(torpedo2.collidesWith(ship1, collisionVector))
    {
        ship1.damage(TORPEDO);
        torpedo2.setVisible(false);
        torpedo2.setActive(false);
        input->gamePadVibrateRight(0,20000,0.5);
    }
    // if collision between torpedos and planet
    if(torpedo1.collidesWith(planet, collisionVector))
    {
        torpedo1.setVisible(false);
        torpedo1.setActive(false);
        audio->playCue(TORPEDO_CRASH);
    }
    if(torpedo2.collidesWith(planet, collisionVector))
    {
        torpedo2.setVisible(false);
        torpedo2.setActive(false);
        audio->playCue(TORPEDO_CRASH);
    }

    // check for scores
    if(ship1.getActive() == false && ship2Scored == false)
    {
        ship2Score++;
        ship2Scored = true;
        if(roundOver == false)
        {
            roundTimer = spacewarNS::ROUND_TIME;
            roundOver = true;
        }
    }
    if(ship2.getActive() == false && ship1Scored == false)
    {
        ship1Score++;
        ship1Scored = true;
        if(roundOver == false)
        {
            roundTimer = spacewarNS::ROUND_TIME;
            roundOver = true;
        }
    }
}

//=============================================================================
// Render game items
//=============================================================================
void Spacewar::render()
{
    graphics->spriteBegin();                // begin drawing sprites

    nebula.draw();                          // display Orion nebula
    planet.draw();                          // draw the planet

    // display scores
    fontScore.setFontColor(spacewarNS::SHIP1_COLOR);
    _snprintf_s(buffer, spacewarNS::BUF_SIZE, "%d", (int)ship1Score);
    fontScore.print(buffer,spacewarNS::SCORE1_X,spacewarNS::SCORE_Y);
    fontScore.setFontColor(spacewarNS::SHIP2_COLOR);
    _snprintf_s(buffer, spacewarNS::BUF_SIZE, "%d", (int)ship2Score);
    fontScore.print(buffer,spacewarNS::SCORE2_X,spacewarNS::SCORE_Y);

    // display health bars
    healthBar.setX((float)spacewarNS::SHIP1_HEALTHBAR_X);
    healthBar.set(ship1.getHealth());
    healthBar.draw(spacewarNS::SHIP1_COLOR);
    healthBar.setX((float)spacewarNS::SHIP2_HEALTHBAR_X);
    healthBar.set(ship2.getHealth());
    healthBar.draw(spacewarNS::SHIP2_COLOR);

    ship1.draw();                           // draw the spaceships
    ship2.draw();

    torpedo1.draw(graphicsNS::FILTER);      // draw the torpedoes using colorFilter
    torpedo2.draw(graphicsNS::FILTER);

    if(menuOn)
        menu.draw();
    if(countDownOn)
    {
        _snprintf_s(buffer, spacewarNS::BUF_SIZE, "%d", (int)(ceil(countDownTimer)));
        fontBig.print(buffer,spacewarNS::COUNT_DOWN_X,spacewarNS::COUNT_DOWN_Y);
    }

    graphics->spriteEnd();                  // end drawing sprites
}

//=============================================================================
// process console commands
//=============================================================================
void Spacewar::consoleCommand()
{
    // The music control is located here because consoleCommand is always called,
    // even if the game is paused.
    if(paused || musicOff)
        audio->pauseCategory("Music");
    else
        audio->resumeCategory("Music");

    command = console->getCommand();    // get command from console
    if(command == "")                   // if no command
        return;

    if (command == "help")              // if "help" command
    {
        console->print("Console Commands:");
        console->print("fps - toggle display of frames per second");
        console->print("gravity off - turns off planet gravity");
        console->print("gravity on - turns on planet gravity");
        console->print("planet off - disables planet");
        console->print("planet on - enables planet");
        console->print("music on - plays music");
        console->print("music off - no music");
        return;
    }
    if (command == "fps")
    {
        fpsOn = !fpsOn;                 // toggle display of fps
        if(fpsOn)
            console->print("fps On");
        else
            console->print("fps Off");
    }

    if (command == "gravity off")
    {
        planet.setMass(0);
        console->print("Gravity Off");
    }
    else if (command == "gravity on")
    {
        planet.setMass(planetNS::MASS);
        console->print("Gravity On");
    }
    else if (command == "planet off")
    {
        planet.disable();
        console->print("Planet Off");
    }
    else if (command == "planet on")
    {
        planet.enable();
        console->print("Planet On");
    }
    else if (command == "music off")
    {
        musicOff = true;
        console->print("Music Off");
    }
    else if (command == "music on")
    {
        musicOff = false;
        console->print("Music On");
    }
}

//=============================================================================
// The graphics device was lost.
// Release all reserved video memory so graphics device may be reset.
//=============================================================================
void Spacewar::releaseAll()
{
    menuTexture.onLostDevice();
    nebulaTexture.onLostDevice();
    gameTextures.onLostDevice();
    fontScore.onLostDevice();
    fontBig.onLostDevice();

    Game::releaseAll();
    return;
}

//=============================================================================
// The grahics device has been reset.
// Recreate all surfaces.
//=============================================================================
void Spacewar::resetAll()
{
    fontBig.onResetDevice();
    fontScore.onResetDevice();
    gameTextures.onResetDevice();
    nebulaTexture.onResetDevice();
    menuTexture.onResetDevice();

    Game::resetAll();
    return;
}
