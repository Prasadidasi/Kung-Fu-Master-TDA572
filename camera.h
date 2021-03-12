#pragma once

class Player;
class Camera {

public:
	SDL_Rect window;

	~Camera() { SDL_Log("Camera::~Camera"); }

	void Init(int width, int height) {
		SDL_Log("Camera INIT");
		this->window = { 0 , 0 , width, height };
	}

	void UpdatePlayer(Player* player) {

		double xPos = player->position.x;
		double yPos = player->position.y;

		//camera always centered on the player
		window.x = (xPos + player->player_width / 2) - SCREEN_WIDTH / 2;
		window.y = (yPos + player->player_height / 2) - SCREEN_HEIGHT / 2;

		if (window.x < 0)
			window.x = 0;

		if (window.y < 0)
			window.y = 0;

		if (window.x > LEVEL_WIDTH - window.w)		//handle edge conditions
			window.x = LEVEL_WIDTH - window.w;

		if (window.y > LEVEL_HEIGHT - window.h) {
			window.y = LEVEL_HEIGHT - window.h;
		}
	}
};

