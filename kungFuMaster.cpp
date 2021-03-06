#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define ASSETS_DIR "assets/"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 256
#define LEVEL_WIDTH 1792
#define LEVEL_HEIGHT 200
#define PLAYER_HEIGHT 63
#define PLAYER_WIDTH 24
#define PLAYER_SPEED 10
#define MAX_LIVES 3
#define ENEMY_SPEED 9

const float GAME_SPEED = 10.f; //adjust speed according to levels

#include "engine.h"
#include "object_pool.h"
#include "vector2D.h"
#include "game_object.h"
#include "component.h"
#include "enemy.h"
#include "player.h"
#include "camera.h"
#include "animator.h"
#include "audiomanager.h"
#include "game.h"

int main(int argc, char** argv) {
	Engine engine;
	engine.init(SCREEN_WIDTH, SCREEN_HEIGHT);

	Game game;
	game.Create(&engine);
	game.Init();

	float lastTime = engine.getElapsedTime();
	while (true)
	{
		float newTime = engine.getElapsedTime();
		float dt = newTime - lastTime;
		dt = dt * GAME_SPEED;
		lastTime = newTime;

		engine.processInput();
		game.Update(dt);
		game.Draw();
		SDL_Delay(1000 / 60);
	}

	game.Destroy();
	engine.destroy();
	return 0;
}