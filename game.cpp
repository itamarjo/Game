/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdexcept>
#include <stdlib.h>//use of rand
#include <sstream>
#include <algorithm> // use of for_each

#include "game.hpp"

LTexture::LTexture(Game *mGamePtr_): mTexture(NULL), mWidth(0), mHeight(0), mGamePtr(mGamePtr_)
{}

LTexture::~LTexture()
{
     SDL_DestroyTexture( mTexture );
}

bool LTexture::loadFromFile( std::string path )
{
    std::string errormsg;
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
        throw std::runtime_error("Unable to load image! SDL_image Error");
    }
   
    //Color key image
    SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface( mGamePtr->GetRenderer(), loadedSurface );
    if( newTexture == NULL )
    {
        errormsg = "Unable to create texture from %s! SDL Error: %s\n";
        errormsg.append(SDL_GetError());
        throw std::runtime_error(errormsg.c_str());
    }
		
    //Get image dimensions
    mWidth = loadedSurface->w;
    mHeight = loadedSurface->h;
		

    //Get rid of old loaded surface
    SDL_FreeSurface( loadedSurface );
	

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}


bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
    std::string errormsg;

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( mGamePtr->GetFont(), textureText.c_str(), textColor );
	if( textSurface == NULL )
	{
        errormsg = "Unable to render text surface! SDL_ttf Error: %s\n";
        errormsg.append(TTF_GetError());
        throw std::runtime_error(errormsg.c_str());
    }
		
    //Create texture from surface pixels
    mTexture = SDL_CreateTextureFromSurface( mGamePtr->GetRenderer(), textSurface );
    if( mTexture == NULL )
    {
        errormsg = "Unable to create texture from rendered text! SDL Error: %s\n";
        errormsg.append(SDL_GetError());
        throw std::runtime_error(errormsg.c_str());
    }
	
    //Get image dimensions
    mWidth = textSurface->w;
    mHeight = textSurface->h;
    
    //Get rid of old surface
    SDL_FreeSurface( textSurface );
	
	//Return success
	return mTexture != NULL;
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip, bool should_streach) const 
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };	

	//Set rendering space and render to screen
	if (should_streach)
	{
		renderQuad.w = SCREEN_WIDTH; 
		renderQuad.h = SCREEN_HEIGHT;
	}

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		if (should_streach)
		{

			renderQuad.w = SCREEN_WIDTH;
			renderQuad.h = SCREEN_HEIGHT;
		}
		else
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}
	}

	//Render to screen
	SDL_RenderCopyEx( mGamePtr->GetRenderer(), mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth() const
{
	return mWidth;
}

int LTexture::getHeight() const
{
	return mHeight;
}


AnimatedItem::AnimatedItem(Game *mGamePtr_, int PosX_, int PosY_, int Vel_, int VelX_, int VelY_, bool Alive_) :
                                            mGamePtr(mGamePtr_),
                                            mTexture(mGamePtr_),
                                            mPosX(PosX_), //Initialize the offsets
								  			mPosY(PosY_), 
											mVel(Vel_), //Initialize the Max Velocity
											mVelX(VelX_), //Initialize the velocity
											mVelY(VelY_), //Initialize the velocity
                                            mAlive(Alive_)
{}


//return Position Y of AnimatedItem
int AnimatedItem::GetPosY() const
{
	return(mPosY);
}

//return Position X of AnimatedItem
int AnimatedItem::GetPosX() const
{
	return(mPosX);
}

//return velocity of AnimatedItem
int AnimatedItem::GetVel() const
{
	return(mVel);
}

//return width of AnimatedItem
int AnimatedItem::GetWidth() const
{
	return(mTexture.getWidth());
}

//return height of AnimatedItem
int AnimatedItem::GetHeight() const
{
	return(mTexture.getHeight());
}


void AnimatedItem::render() const 
{
    //Show the item on the screen
	mTexture.render( mPosX, mPosY );
}

bool AnimatedItem::IsAlive() const
{
    return(mAlive);
}

Boat::Boat(Game *mGamePtr_): AnimatedItem(mGamePtr_, 0, 400, 10)
{
 
	//Load Boat texture
	if( !mTexture.loadFromFile( "boatF.png" ) )
	{
        throw std::runtime_error("Failed to load Boat texture!\n");
	}
}

void Boat::move()
{
    //Move the Boat left or right
    mPosX += mVelX;

    //If the Boat went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + mTexture.getWidth() > SCREEN_WIDTH ) )
    {
        //Move back
        mPosX -= mVelX;
    }

}

void Boat::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_LEFT: mVelX -= mVel; break;
            case SDLK_RIGHT: mVelX += mVel; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_LEFT: mVelX += mVel; break;
            case SDLK_RIGHT: mVelX -= mVel; break;
        }
    }
}

//Airplane constructor 
Airplane::Airplane(Game *mGamePtr_): AnimatedItem(mGamePtr_,240, 10, 10), mGameptr(mGamePtr_)
{
	//Load Boat texture
	if( !mTexture.loadFromFile( "airplaneF.png" ) )
	{
        throw std::runtime_error("Failed to load airplane texture!\n");
	}

	mJumpXPos = rand() %  SCREEN_WIDTH; // save a random location when the parashutist will jump
}

void Airplane::move()
{
    //Move the airplane to the left 
    mPosX -= mVel;
	
    //if airplane reached to the location of the parachutist jump
    if((mPosX >= mJumpXPos) && (mPosX < (mJumpXPos + mVel)) ) // taking into account position move in  mVel jumps
    {
    	mGameptr->createParachutist(mJumpXPos); // make the parachutist jump
    }

    //If the airplane exit the screen from the left
    if( (mPosX + mTexture.getWidth() ) < 0 ) 
    {
        //Move back to the right of the screen
        mPosX = SCREEN_WIDTH;	
        mJumpXPos = rand() %  SCREEN_WIDTH; // save a random location when the parashutist will jump
    }
}

//
Parachutist::Parachutist(Game *mGamePtr_, int PosX_): AnimatedItem(mGamePtr_, PosX_, 0, 4)
{
	//Load parachutist texture
	if( !mTexture.loadFromFile( "parachutistF.png" ) )
	{
         throw std::runtime_error("Failed to load parachutist texture!\n");
	}
}

void Parachutist::Move()
{
    //Move the parachutist down and to the left 
    mPosY += mVel;
    mPosX -= (mVel / 4);
}


//return Height of Parachutist
int Parachutist::GetHeight() const
{
	return(mTexture.getHeight());
}

class GameOver: public AnimatedItem
{
public:
    //Constructor: Initializes the variables
    GameOver(Game *mGameptr_);
    //Moves the airplane
    void Move();
    void Run();
};

GameOver::GameOver(Game *mGamePtr_): AnimatedItem(mGamePtr_, SCREEN_WIDTH + 150,  (SCREEN_HEIGHT / 2) - 200 , 40, 0, 0, false)
{

    //Load Boat texture
    if( !mTexture.loadFromFile( "gameover.png" ) )
    {
       throw std::runtime_error("Failed to load gameover texture!\n");
    }
}

void GameOver::Move()
{
    
    if(mPosX >= ((SCREEN_WIDTH / 2) - (GetWidth() / 2 ) + 25)) // Check if the gameover reach the center of the screen
    {
        //Move the gameover to the left
        mPosX -= mVel;
    }
}

void GameOver::Run()
{
    mAlive = true;
}

//Constructor for class Game
Game::Game():   mBackgroundTexture(this),
                mTextTexture(this),
                mScore(0),
                mLife(3)

{
    std::string errormsg;
    
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        errormsg = "SDL could not initialize! SDL Error: %s\n";
        errormsg.append(SDL_GetError());
        throw std::runtime_error(errormsg.c_str());
    }
    
    //Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
        throw std::runtime_error("Warning: Linear texture filtering not enabled!");
    }
    
    //Create window
    mWindow = SDL_CreateWindow( "Matific Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( mWindow == NULL )
    {
        errormsg = "Window could not be created! SDL Error: %s\n";
        errormsg.append(SDL_GetError());
        throw std::runtime_error(errormsg.c_str());
    }
    //Create vsynced renderer for window
    mRenderer = SDL_CreateRenderer( mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if( mRenderer == NULL )
    {
        errormsg = "Renderer could not be created! SDL Error: %s\n";
        errormsg.append(SDL_GetError());
        throw std::runtime_error(errormsg.c_str());
    }
    
    //Initialize renderer color
    SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
            
    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        errormsg = "SDL_image could not initialize! SDL_image Error: %s\n";
        errormsg.append(SDL_GetError());
        throw std::runtime_error(errormsg.c_str());
    }
        
    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        errormsg = "SDL_ttf could not initialize! SDL_ttf Error: %s\n";
        errormsg.append(SDL_GetError());
        throw std::runtime_error(errormsg.c_str());
    }
    
    //Load background texture
    if( !mBackgroundTexture.loadFromFile( "background.bmp" ) )
    {
        throw std::runtime_error("Failed to load background texture image!\n" );
    }
            
    //Open the font
    mFont = TTF_OpenFont( "hand.ttf", 22 );
    if( mFont == NULL )
    {
        errormsg =  "Failed to load font! SDL_ttf Error: %s\n";
        errormsg.append(SDL_GetError());
        throw std::runtime_error(errormsg.c_str());
    }
    
    //Create the animated items
    mBoat = new Boat(this);
    mAirplane = new Airplane(this);
    mGameOver = new GameOver(this);
}


void Game::Run()
{
	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

    //While application is running
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            //Handle input for the Boat
                mBoat->handleEvent( e );
        }
            
        //check if the game is over
        if(mGameOver->IsAlive())
        {
            mGameOver->Move(); //display game over animation
        }
        else
        {
            Move(); //Move all game elements
        }
        //Render all game elements
        Render();
    }
}


bool Parachutist::IsOutOfRange(const Parachutist *parachutist_)
{
	//If the parachutist reach the bottom
	return (parachutist_->mPosY - parachutist_->GetHeight() ) > SCREEN_HEIGHT ;
}

//check if the parachutist collides with the boat
inline bool Game::BoatParachutistCollision(Boat *boat, Parachutist *parachutist) const 
{
    return ((   //Checking if the parachutist reach in at the range of the Y position of the boat floor
                (boat->GetPosY() + boat->GetHeight() * 0.9)   >= // calculating boat floor Y offset
                (parachutist->GetPosY()+ parachutist->GetHeight()) ) && // parachutist's feet Y offset
				// Same check but this time checking the Veocity "jumps" into account
                ((boat->GetPosY() + boat->GetHeight() * 0.9) <  ( parachutist->GetPosY()+ parachutist->GetHeight() + parachutist->GetVel() ) ) &&
                // checking the X position of the the parachutist legs is within the range of the boat floor
				( boat->GetPosX() <= parachutist->GetPosX() + (parachutist->GetWidth() *  0.625 )) &&
				( (boat->GetPosX() + boat->GetWidth()) >= (parachutist->GetPosX() + (parachutist->GetWidth() *  0.375 ) ))
				);

}
void Game::Move()
{
	//Move the animated items
	mBoat->move();
	mAirplane->move();

	//Go through all parachutists that alive currently
	for (std::vector<Parachutist *>::iterator it = mParachutist.begin(); it != mParachutist.end(); ++it)
	{
        //Move each instance of the parachutists
		(*it)->Move();
		
        if( ( ((*it)->GetPosY() + (*it)->GetHeight()) > (mBoat->GetPosY() + mBoat->GetHeight() * 0.9) ) &&
           
           ( ((*it)->GetPosY() + (*it)->GetHeight())  <= (mBoat->GetPosY() + mBoat->GetHeight() * 0.9) +(*it)->GetVel())
           ) // check if Parachutist passed the boat
        {

            if (mLife == 0)
            {
                mGameOver->Run();
            }
            else
            {
                mLife -= 1;
            }
        }
        
        // delete Parachutist from vector if the parachutist pass the bottom of the screen or touch the boat
		if(((*it)->GetPosY() - (*it)->GetHeight()) > SCREEN_HEIGHT) // check if Parachutist passed the bottom of the screen
		{
			it = mParachutist.erase(it) - 1;
            		}
		else if (BoatParachutistCollision(mBoat, *it) )		{
			it = mParachutist.erase(it) - 1;
			mScore += 10;
		}

	}
}
void Game::Render() 
{
	//Clear screen
	SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( mRenderer );

	//Render background texture to screen
	mBackgroundTexture.render( 0, 0 , 0 , 0 , 0 , SDL_FLIP_NONE, true);

	//Render objects
	for (std::vector<Parachutist *>::iterator it = mParachutist.begin(); it != mParachutist.end(); ++it)
		{
			(*it)->render();
		}

	mBoat->render();
	mAirplane->render();
	TextUpdate();
	//Render Text
	mTextTexture.render( 20, 40 );
    mGameOver->render();

	//Update screen - please note that Vsync take care of stablizing the frame rate and syncing it to screen refresh rate.
	SDL_RenderPresent( mRenderer );
}

void Game::createParachutist(int PosX_)
{
	mParachutist.push_back(new Parachutist(this, PosX_));
}

void Game::TextUpdate()
{
	std::stringstream scoretext;
    std::string errormsg;
	scoretext << "Score: " << mScore << " Life: " <<mLife;

	//Render text
	SDL_Color textColor = { 0, 0, 0 };
	if( !mTextTexture.loadFromRenderedText( scoretext.str() , textColor ) )
	{
        errormsg =  "Failed to render text texture!\n";
        errormsg.append(SDL_GetError());
        throw std::runtime_error(errormsg.c_str());
	}
}
Game::~Game()
{

	//Destroy window	
	SDL_DestroyRenderer( mRenderer );
	SDL_DestroyWindow( mWindow );

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

//Returns the game window pointer
SDL_Window* Game::GetWindow() const
{
    return(mWindow);
}

//Returns the Game renderer pointer
SDL_Renderer* Game::GetRenderer() const
{
    return(mRenderer);
}

//Returns the game font pointer
TTF_Font* Game::GetFont() const 
{
    return(mFont);
}


