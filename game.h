#pragma once


bool change_direction = false;

class Game : public GameObject
{
	std::set<GameObject*> game_objects;	
	
	ObjectPool<Enemy> grapplerPool;
	
	Engine* engine;
	Player* player;
	Enemy* enemy;
	Camera* camera;
	Animator* animator;
	AudioManager* audioManager;

	Sprite *background;
	Sprite* background_header;
	Sprite* enemy_health;
	Sprite* player_health;
	Sprite *life_sprite;

	unsigned int score = 0;
	int timer = 1400;
	int lives = MAX_LIVES;
	float enemyHealth = 1.f;
	float playerHealth = 1.f;
	bool game_over = false;
	SDL_Color playerColor = {255,0,0,255};
	SDL_Color enemyColor = {255,0,255,255};

public:

	virtual void Create(Engine* engine)
	{
		SDL_Log("Game::Create");
		camera = new Camera();
		this->engine = engine;

		background_header = engine->createSprite(ASSETS_DIR "background-header.jpg");
		background = engine->createSprite(ASSETS_DIR "background-level1.jpg");
		enemy_health = engine->createSprite(ASSETS_DIR "enemy_health.png");
		player_health = engine->createSprite(ASSETS_DIR "player_health.png");
		
		player = new Player();
		player->playerSprite = engine->createSprite(ASSETS_DIR "player_single.png");
		PlayerBehaviourComponent* player_behaviour = new PlayerBehaviourComponent();
		player_behaviour->Create(engine, player, &game_objects);
		CollideComponent* playerGrapplerCollision = new CollideComponent();
		playerGrapplerCollision->Create(engine, player, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&grapplerPool));

		player->Create();
		player->AddComponent(player_behaviour);
		player->AddComponent(playerGrapplerCollision);
		player->AddReceiver(this);
		game_objects.insert(player);

		grapplerPool.Create(25);
		for (auto enemy = grapplerPool.pool.begin(); enemy != grapplerPool.pool.end(); enemy++) {
			(*enemy)->animIds = { "GRAPWALK", "GRAPATTACK", "GRAPHIT", "GRAPDEATH", "GRAPGRAB"};
			
			EnemyBehaviorComponent* enemyBehavior = new EnemyBehaviorComponent();
			enemyBehavior->Create(engine, *enemy, &game_objects);
			RigidBodyComponent* enemyBody = new RigidBodyComponent();
			enemyBody->Create(engine, *enemy, &game_objects);
			
			(*enemy)->Create();
			(*enemy)->AddComponent(enemyBehavior);
			(*enemy)->AddComponent(enemyBody);
			(*enemy)->AddReceiver(this);
			game_objects.insert(*enemy);
		}

		animator = new Animator();
		animator->Create(engine,camera,ASSETS_DIR "player_single.png");
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_single.png", 1, SDLK_UNKNOWN);
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_left.png", 4, SDLK_LEFT);
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_right.png", 4, SDLK_RIGHT);
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_down.png", 1, SDLK_DOWN);
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_jump.png", 5, SDLK_UP);
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_standing_kick.png", 3, SDLK_a);
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_standing_punch.png", 4, SDLK_d);

		animator->AddSpriteSheet(engine, ASSETS_DIR "grappler_attack.png", 4, "GRAPATTACK");
		animator->AddSpriteSheet(engine, ASSETS_DIR "grappler_walk.png", 4, "GRAPWALK");
		animator->AddSpriteSheet(engine, ASSETS_DIR "grappler_hit.png", 1, "GRAPHIT");
		animator->AddSpriteSheet(engine, ASSETS_DIR "grappler_death.png", 2, "GRAPDEATH");
		animator->AddSpriteSheet(engine, ASSETS_DIR "grappler_grab.png", 1, "GRAPGRAB");

		audioManager = new AudioManager();
		audioManager->createMusicFx(ASSETS_DIR "kungfu_backgroundMusic.mp3","BGM");
		audioManager->createMusicFx(ASSETS_DIR "kungfu_gameOver.mp3", "GAMEOVER");
		audioManager->createMusicFx(ASSETS_DIR "kungfu_gameStart.mp3", "GAMESTART");

		audioManager->createSoundFx(ASSETS_DIR "kungfu_kick.mp3", "KICK");
		audioManager->createSoundFx(ASSETS_DIR "kungfu_punch.mp3", "PUNCH");
		audioManager->createSoundFx(ASSETS_DIR "grab.wav", "GRAB");
		audioManager->createSoundFx(ASSETS_DIR "enemyDeath.wav", "ENEMYDEATH");

		score = 0;
	}

	virtual void Init()
	{
		camera->Init(SCREEN_WIDTH, SCREEN_HEIGHT); 
		player->Init(Vector2D(1470,210), camera);

		int offset = 0;
		for (int j = 0; j < 5; j++) {
			for (int i = 0; i < 5; i++) {
				grapplerPool.pool[i*5 + j]->Init(Vector2D(1320 + offset - 50 * (i + j), 211), "GRAPPLER");
				grapplerPool.pool[i*5 + j]->GetComponent<RigidBodyComponent*>()->velocity.x = ENEMY_SPEED;
			}
			offset += 50;
		}

		enabled = true;
		if (Mix_PlayMusic(audioManager->getMusic("BGM"), -1) == -1) {
			SDL_Log("Can't play music: %s", Mix_GetError());
		}
	}

	virtual void Update(float dt)
	{
		Engine::KeyStatus keys;
		engine->getKeyStatus(keys);
		if (keys.esc) {
			Destroy();
			engine->quit();
		}

		if (playerHealth <= 0.f) {
			animator->handleDeath(camera, player);
			dt = 0;
		}
		animator->dt = dt;

		camera->UpdatePlayer(player);
		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Update(dt);

		audioManager->Update(engine, dt);
		//animator->Update();
	}

	virtual void Draw()
	{
		engine->clearWindow();
		
		background->drawBackground(&camera->window);
		background_header->draw(0, 0);
		//player_health->draw(115,10);
		enemy_health->RenderHPBar(115, 30, enemyHealth, enemyColor);
		player_health->RenderHPBar(115, 10, playerHealth, playerColor);

		SDL_KeyCode keyPressed;
		engine->getKeyPressed(keyPressed);
		
		animator->animateGo(player, camera, &grapplerPool, keyPressed);
		for (auto enemy : grapplerPool.pool) {
			animator->animateGo(enemy, camera, &grapplerPool, SDLK_UNKNOWN, animator->getEnemyState(enemy, player));
		}
		engine->drawText(420, 10, "LIVES");
		engine->drawText(340, 10, "SCORE");
		engine->drawText(15, 10, "PLAYER", 255, 0, 0);
		engine->drawText(15, 30, "ENEMY", 255, 1, 255);
		engine->drawText(560, 10, "TIMER");
		
		char msg[1024];
		sprintf(msg, "%d", Timer());
		engine->drawText(564, 30, msg);

		sprintf(msg, "%4d", lives);
		engine->drawText(425, 30, msg);

		sprintf(msg, "%4d", Score());
		engine->drawText(344, 30, msg);
		
		
		//life_sprite->draw(1470, 240);
		
		engine->swapBuffers();
	}

	virtual void Receive(Message m, GameObject* go)
	{
		if (m == ENEMY_HIT) {
			Enemy* enemy = (Enemy*)go;
			animator->enemyHit(camera, enemy, enemy->animIds[2]);
			audioManager->playSound("ENEMYDEATH");
			score += 200;
		}

		if (m == PLAYER_GRABBED) {
			Enemy* enemy = (Enemy*)go;
			enemy->grabPlayer = true;
			playerHealth -= 0.005;
		}
	}

	unsigned int Score()
	{
		return score;
	}

	unsigned int Timer()
	{
		return timer--;
	}

	virtual void Destroy()
	{
		SDL_Log("Game::Destroy");

		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Destroy();

		animator->Destroy();
		background->destroy();
		background_header->destroy();
		player->Destroy();
		grapplerPool.Destroy();
	}
};
