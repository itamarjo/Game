//
//  game.h
//  Game
//
//  Created by Itamar Jobani on 11/27/16.
//  Copyright © 2016 Itamar Jobani. All rights reserved.
//

#ifndef game_h
#define game_h
//Using SDL, SDL_image, standard IO, and strings

#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <vector>


//Screen dimension constants
const int SCREEN_WIDTH = 1040;
const int SCREEN_HEIGHT = 680;

//forward declaration of all classes
class LTexture;
class LTimer;
class Game;
class AnimatedItem;
class Boat;
class Airplane;
class Parachutist;
class GameOver;

// to do add const to everywhere

//Texture wrapper class
class LTexture
{
public:
    //Initializes variables
    LTexture(Game *mGamePtr_);
    
    //Deallocates memory
    ~LTexture();
    
    //Loads image at specified path
    bool loadFromFile( std::string path );

    //Creates image from font string
    bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
    
    //Renders texture at given point
    void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, bool should_streach = false) const;
    
    //Gets image dimensions
    int getWidth() const;
    int getHeight() const;
    
private:
   
    // pointer to game
    Game *mGamePtr;
    //The actual hardware texture
    SDL_Texture* mTexture;
    
    //Image dimensions
    int mWidth;
    int mHeight;
    
};

class AnimatedItem
{
public:
    //Constructor: Initializes the variables
    AnimatedItem(Game *mGamePtr_, int PosX_, int PosY_, int Vel_ = 10, int VelX_ = 0, int VelY_ = 0, bool Alive_ = true);
    //Shows the item on the screen
    void render() const;
    //return position Y of animated item
    int GetPosY() const;
    //return position X of animated item
    int GetPosX() const;
    //return velocity of animated item
    int GetVel() const;
    //return width of animated item
    int GetWidth() const;
    //return height of animated item
    int GetHeight() const;
    //check is Item is alive (for parachutists and gameover)
    bool IsAlive() const;
    
    
protected:
    Game *mGamePtr; // pointer to game (in order to access renderer and window pointers)
    LTexture mTexture; 	// Item's texture
    int mPosX, mPosY; //The X and Y offsets of the item
    int mVel; //Maximum axis velocity of the Item
    int mVelX, mVelY; 	//The velocity of the item by axis
    bool mAlive = true; // set if the item is visiable and active (for parachutist and gameover)
    
private:
    AnimatedItem(AnimatedItem &other_); //disable copy constructor
};

class Boat: public AnimatedItem
{
public:
    //Constructor: Initializes the variables
    Boat(Game *mGamePtr_);
    //Takes key presses and adjusts the Boat's velocity
    void handleEvent( SDL_Event& e );
    //Moves the item
    void move();
};

class Airplane: public AnimatedItem
{
public:
    //Constructor: Initializes the variables
    Airplane(Game *mGameptr_);
    //Moves the airplane
    void move();
    
private:
    Game *mGameptr; // pointer to game object - to be able to call createParachutist method
    int mJumpXPos; // X Position of the parashutist jump
    
};

class Parachutist: public AnimatedItem
{
public:
    //Constructor: Initializes the variables
    Parachutist(Game *mGmaePtr_, int PosX_);
    //Moves the airplane
    void Move();
    //return the Parachutist Height
    int GetHeight() const;
    //Check if the Parachusist is out of the screen
    static bool IsOutOfRange(const Parachutist *parachutist_);
};

//The main class that holds all assests and renderable items
class Game
{
public:
    //Constructor: Initializes the variables
    Game();
    //Destructor: release all of the class assets
    ~Game();
    // Starting the game main loop
    void Run();
    // Render all elements in the game
    void Render();
    //Moves all animated object and check parachutist location and kill it if needed
    void Move();
    //Returns the game window pointer
    SDL_Window* GetWindow() const;
    //Returns the Game renderer pointer
    SDL_Renderer* GetRenderer() const;
    //Returns the game font pointer
    TTF_Font *GetFont() const;
    
private:
    
    Game( const Game &other_); //disable copy constructor
    
    //General assets members
    SDL_Window* mWindow; //The window we'll be rendering to
    SDL_Renderer* mRenderer; //The window renderer
    TTF_Font *mFont = NULL; //Globally used font
    
    //General game textures
    LTexture mBackgroundTexture;
    LTexture mTextTexture;
    
    
    //AnimatedItems
    Boat *mBoat; // pointer to a boat
    friend Airplane;
    Airplane *mAirplane; //pointer to airplane
    std::vector<Parachutist *> mParachutist; //vector to hold the possible multiple instances of parachutist
    GameOver *mGameOver; //animation for gameover;
    
    //Score members
    unsigned int mScore; //keeps the game score
    int mLife; //keeps how many lives have left in the game
    
    //Methods
    void createParachutist(int PosX_); // create a new Parachutist
    void removeParachutist(); // remove a new Parachutist
    bool BoatParachutistCollision(Boat *boat, Parachutist *parachutist) const;
    void TextUpdate(); //update the text texture from the current score
};



#endif /* game_h */
