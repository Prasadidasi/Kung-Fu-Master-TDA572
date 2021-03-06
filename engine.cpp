#include "engine.h"

// Creates the main window. Returns true on success.
bool Engine::init(int width, int height)
{
	SDL_Log("Initializing the engine...\n");

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL failed the initialization: %s\n", SDL_GetError());
		return false;
	}
	if (IMG_Init(IMG_INIT_PNG) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Image failed the initialization: %s\n", SDL_GetError());
		return false;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Mixer failed the initialization: %s\n", Mix_GetError());
		return false;
	}

	//Create window
	window = SDL_CreateWindow("Kung Fu Master", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	//Create renderer for window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	//initialize font
	TTF_Init();
	font = TTF_OpenFont("assets/Kungfu_Master.ttf", 12); //this opens a font style and sets a size
	if (font == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "font cannot be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// initialize the keys
	key.punch = false;	key.left = false;	key.right = false, key.esc = false;
	key.kick = false;	key.down = false;	key.up = false;	   key.repeat = false;
	keyPress = SDLK_UNKNOWN;

	//initialize direction player is facing in
	faceDirection = 0;

	//Initialize renderer color
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//Clear screen
	SDL_RenderClear(renderer);

	SDL_Log("Engine up and running...\n");
	return true;
}


// Destroys the avancez library instance
void Engine::destroy()
{
	SDL_Log("Shutting down the engine\n");

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Engine::quit() {
	destroy();
	exit(0);
}


void Engine::processInput()
{
	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {

			keyPress = (event.type == SDL_KEYDOWN) ? (SDL_KeyCode)event.key.keysym.sym : SDLK_UNKNOWN;
			if (event.key.repeat == 0)
				key.repeat = true;
			else
				key.repeat = false;

			key.esc = keyboardState[SDL_SCANCODE_ESCAPE] ? true : false;
			key.esc = keyboardState[SDL_SCANCODE_Q] ? true : false;
			key.up = keyboardState[SDL_SCANCODE_UP] ? true : false;
			key.down = keyboardState[SDL_SCANCODE_DOWN] ? true : false;
			key.left = keyboardState[SDL_SCANCODE_LEFT] ? true : false;
			key.right = keyboardState[SDL_SCANCODE_RIGHT] ? true : false;
			key.kick = keyboardState[SDL_SCANCODE_A] ? true : false;
			key.punch = keyboardState[SDL_SCANCODE_D] ? true : false;

			this->faceDirection = (int)key.right - (int)key.left;  //-1 for left, 1 for right, 0 for neither or both
		}

		if (event.type == SDL_QUIT) {
			this->quit();
			return;
		}
	}
}

void Engine::getKeyStatus(KeyStatus& keys)
{
	keys.kick = key.kick;
	keys.punch = key.punch;
	keys.left = key.left;
	keys.right = key.right;
	keys.up = key.up;
	keys.down = key.down;
	keys.esc = key.esc;
	keys.repeat = key.repeat;
}

void Engine::swapBuffers() {
	//Update screen
	SDL_RenderPresent(renderer);
}

void Engine::clearWindow() {
	//Clear screen
	SDL_RenderClear(renderer);
}


Sprite * Engine::createSprite(const char * path)
{
	//SDL_Surface* surf = SDL_LoadBMP(path);
	SDL_Surface* surf = IMG_Load(path);
	if (surf == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError());
		return NULL;
	}
	//SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 255, 255, 255));  //enable transparency
	//Create texture from surface pixels
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(surf);

	Sprite *sprite = new Sprite(renderer, texture);

	return sprite;
}

void Engine::drawText(int x, int y, const char* msg, int r , int g , int b )
{
	SDL_Color color = { r, g, b };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

	SDL_Surface* surf = TTF_RenderText_Solid(font, msg, color); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

	SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(renderer, surf); //now you can convert it into a texture

	int w = 0;
	int h = 0;
	SDL_QueryTexture(msg_texture, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { x, y, w, h };

	SDL_RenderCopy(renderer, msg_texture, NULL, &dst_rect);

	SDL_DestroyTexture(msg_texture);
	SDL_FreeSurface(surf);
}

float Engine::getElapsedTime()
{
	return SDL_GetTicks() / 1000.f;
}

void Engine::getKeyPressed(SDL_KeyCode& keyPressed) {
	keyPressed = keyPress;
}

void Sprite::draw(int x, int y)
{
	SDL_Rect rect;

	rect.x = x;
	rect.y = y;

	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));

	//Render texture to screen
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void Sprite::draw(int x, int y, int frames, SDL_Rect* textureRect)
{
	SDL_Rect windowRect;
	windowRect.x = x;
	windowRect.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &(windowRect.w), &(windowRect.h));
	windowRect.w /= frames;
	
	//Animate Texture
	SDL_RenderCopy(renderer, texture, textureRect, &windowRect);
}

void Sprite::draw(SDL_Rect rect) {
	SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));
	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void Sprite::drawBackground(SDL_Rect* rect) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	
	SDL_Rect clip = {0, 56, 1792, 200};
	
	if (rect != nullptr) 
		clip.w = (*rect).w;

	//SDL_RenderCopy(renderer, texture, rect, &clip);
	SDL_RenderCopyEx(renderer, texture, rect, &clip, 0, NULL, SDL_FLIP_NONE);
}

void Sprite::getDimensions(SDL_Rect* rect) {
	SDL_QueryTexture(texture, NULL, NULL, &(rect->w), &(rect->h));
}

void Sprite::RenderHPBar(int x, int y, float Percent, SDL_Color FGColor) {
	Percent = Percent > 1.f ? 1.f : Percent < 0.f ? 0.f : Percent;
	SDL_Color old;
	SDL_GetRenderDrawColor(renderer, &old.r, &old.g, &old.g, &old.a);
	SDL_Rect bgrect = { x, y, 0, 0 };
	SDL_QueryTexture(texture, NULL, NULL, &bgrect.w, &bgrect.h);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &bgrect);
	SDL_SetRenderDrawColor(renderer, FGColor.r, FGColor.g, FGColor.b, FGColor.a);
	int pw = (int)((float)bgrect.w * Percent);
	//int px = x - (bgrect.w - pw);
	SDL_Rect fgrect = { x, y, pw, bgrect.h };
	SDL_RenderFillRect(renderer, &fgrect);
	SDL_SetRenderDrawColor(renderer, old.r, old.g, old.b, old.a);
}

void Sprite::destroy()
{
	SDL_DestroyTexture(texture);
}


