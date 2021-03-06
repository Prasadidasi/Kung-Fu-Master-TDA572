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

	// Draw the sprite at the given position.
	// Valid coordinates are between (0,0) (upper left) and (width-32, height-32) (lower right).
	// (All sprites are 32*32 pixels, clipping is not supported)
	void draw(int x, int y);
	void draw(int x, int y, int frames, SDL_Rect* windowRect);
	void draw(SDL_Rect rect);
	void drawBackground(SDL_Rect* rect);
	void RenderHPBar(int x, int y, float Percent, SDL_Color FGColor);
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

	// Return the total time spent in the game, in seconds.
	float getElapsedTime();
	void getFaceDirection(int& direction) { direction = this->faceDirection; };

	struct KeyStatus
	{
		bool punch;
		bool kick;
		bool up; // space
		bool down;
		bool left; // left arrow
		bool right; // right arrow
		bool esc; // escape button
		bool repeat;
	};

	// Returns the keyboard status. If a flag is set, the corresponding key is being held down.
	void getKeyStatus(KeyStatus& keys);
	void getKeyPressed(SDL_KeyCode& keyPressed);
	

private:
	SDL_Window * window;
	SDL_Renderer * renderer;

	TTF_Font* font;

	KeyStatus key;
	SDL_KeyCode keyPress;
	int faceDirection;
};

