#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>


class Sprite
{
	SDL_Renderer * renderer;
	SDL_Texture * texture;

public:

	Sprite(SDL_Renderer * renderer, SDL_Texture * texture){
		this->renderer = renderer;
		this->texture = texture;
	};

	// Destroys the sprite instance
	void destroy();

	// Draw sprites depending on input
	void draw(int x, int y);
	void draw(int x, int y, int frames, SDL_Rect* windowRect, bool flip = false);
	void draw(SDL_Rect rect);
	void drawBackground(SDL_Rect* rect);
	void RenderHPBar(int x, int y, float Percent, SDL_Color FGColor);

	// Fetch sprite dimensions
	void getDimensions(SDL_Rect* rect);
};


class Engine
{
public:
	// Destroys the avancez library instance
	void destroy();

	// Destroys the avancez library instance and exits
	void quit();

	// Creates the main window. Returns true on success.
	bool init(int width, int height);

	// Clears the screen and draws all sprites and texts which have been drawn
	// since the last update call.
	// If update returns false, the application should terminate.
	void processInput();

	void swapBuffers();

	void clearWindow();

	// Create a sprite given a string.
	Sprite* createSprite(const char* name);

	// Draws the given text.
	void drawText(int x, int y, const char* msg, int r = 255, int g = 255, int b = 255);
	void drawGameOverText(int x, int y, const char* msg, int r, int g, int b);

	// Return the total time spent in the game, in seconds.
	float getElapsedTime();

	//Return player facing direction
	void getFaceDirection(int& direction) { direction = this->faceDirection; };

	struct KeyStatus
	{
		bool punch;
		bool kick;
		bool up; 
		bool down;
		bool left; 
		bool right; 
		bool esc; 
		bool repeat;
	};

	// Returns the keyboard status. If a flag is set, the corresponding key is being held down.
	void getKeyStatus(KeyStatus& keys);
	

private:
	SDL_Window * window;
	SDL_Renderer * renderer;

	TTF_Font* font;
	TTF_Font* gameOverFont;

	KeyStatus key;
	int faceDirection;
};

