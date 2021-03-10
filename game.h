#pragma once


bool change_direction = false;

class Game : public GameObject
{
	std::set<GameObject*> game_objects;

	ObjectPool<Enemy> grapplerPool;
	ObjectPool<Enemy> throwerPool;
	ObjectPool<Knife> knifePool;

	Engine* engine;
	Player* player;
	Camera* camera;
	Animator* animator;
	AudioManager* audioManager;
	SpawnHandler* spawnHandler;

	Sprite* background;
	Sprite* background_header;
	Sprite* enemy_health;
	Sprite* player_health;

	unsigned int score = 0;
	int timer = 1900;
	int lives = MAX_LIVES;
	float enemyHealth = 1.f;
	float playerHealth = 1.f;
	bool game_over = false;
	bool level_win = false;
	bool haltTimer = false;
	SDL_Color playerColor = { 255,0,0,255 };
	SDL_Color enemyColor = { 255,0,255,255 };

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
		CollideComponent* playerThrowerCollision = new CollideComponent();
		playerThrowerCollision->Create(engine, player, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&throwerPool));
		CollideComponent* playerKnifeCollision = new CollideComponent();
		playerKnifeCollision->Create(engine, player, &game_objects, reinterpret_cast<ObjectPool<GameObject>*>(&knifePool));

		player->Create();
		player->AddComponent(player_behaviour);
		player->AddComponent(playerGrapplerCollision);
		player->AddComponent(playerThrowerCollision);
		player->AddComponent(playerKnifeCollision);
		player->AddReceiver(this);
		game_objects.insert(player);

		grapplerPool.Create(15);
		for (auto enemy = grapplerPool.pool.begin(); enemy != grapplerPool.pool.end(); enemy++) {
			(*enemy)->animIds = { "GRAPWALK", "GRAPATTACK", "GRAPHIT", "GRAPDEATH", "GRAPGRAB" };

			EnemyBehaviorComponent* enemyBehavior = new EnemyBehaviorComponent();
			enemyBehavior->Create(engine, *enemy, player, &game_objects, &knifePool);
			RigidBodyComponent* enemyBody = new RigidBodyComponent();
			enemyBody->Create(engine, *enemy, &game_objects);

			(*enemy)->Create();
			(*enemy)->AddComponent(enemyBehavior);
			(*enemy)->AddComponent(enemyBody);
			(*enemy)->AddReceiver(this);
			game_objects.insert(*enemy);
		}

		throwerPool.Create(2);
		for (auto enemy = throwerPool.pool.begin(); enemy != throwerPool.pool.end(); enemy++) {
			(*enemy)->animIds = { "THROWALK", "THROWATTACK", "THROWHIT", "THROWDEATH", "THROWIDLE" };

			EnemyBehaviorComponent* enemyBehavior = new EnemyBehaviorComponent();
			enemyBehavior->Create(engine, *enemy, player, &game_objects, &knifePool);
			RigidBodyComponent* enemyBody = new RigidBodyComponent();
			enemyBody->Create(engine, *enemy, &game_objects);

			(*enemy)->Create();
			(*enemy)->AddComponent(enemyBehavior);
			(*enemy)->AddComponent(enemyBody);
			(*enemy)->AddReceiver(this);
			game_objects.insert(*enemy);
		}

		knifePool.Create(4);
		for (auto knife = knifePool.pool.begin(); knife != knifePool.pool.end(); knife++)
		{
			KnifeBehaviourComponent* behaviour = new KnifeBehaviourComponent();
			behaviour->Create(engine, *knife, &game_objects);
			RigidBodyComponent* knifeBody = new RigidBodyComponent();
			knifeBody->Create(engine, *knife, &game_objects);
			(*knife)->Create();
			(*knife)->AddComponent(behaviour);
			(*knife)->AddComponent(knifeBody);
			(*knife)->AddReceiver(this);
			game_objects.insert(*knife);
		}

		animator = new Animator();
		animator->Create(engine, camera, ASSETS_DIR "player_single.png"); //IDLE Player animation
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_single.png", 1, "PLAYER_IDLE");
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_left.png", 4, "PLAYER_LEFT");
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_right.png", 4, "PLAYER_RIGHT");
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_down.png", 1, "PLAYER_DOWN");
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_jump.png", 5, "PLAYER_UP");
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_standing_kick.png", 3, "PLAYER_KICK");
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_standing_punch.png", 4, "PLAYER_PUNCH");
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_down_punch.png", 4, "PLAYER_DOWN_PUNCH");
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_down_kick.png", 3, "PLAYER_DOWN_KICK");
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_death.png", 2, "PLAYER_DEATH");
		animator->AddSpriteSheet(engine, ASSETS_DIR "player_grabbed.png", 1, "PLAYER_GRABBED");

		//Enemy spritehseets
		animator->AddSpriteSheet(engine, ASSETS_DIR "grappler_attack.png", 4, "GRAPATTACK");
		animator->AddSpriteSheet(engine, ASSETS_DIR "grappler_walk.png", 4, "GRAPWALK");
		animator->AddSpriteSheet(engine, ASSETS_DIR "grappler_hit.png", 1, "GRAPHIT");
		animator->AddSpriteSheet(engine, ASSETS_DIR "grappler_death.png", 2, "GRAPDEATH");
		animator->AddSpriteSheet(engine, ASSETS_DIR "grappler_grab.png", 1, "GRAPGRAB");
		animator->AddSpriteSheet(engine, ASSETS_DIR "thrower_walk.png", 4, "THROWALK");
		animator->AddSpriteSheet(engine, ASSETS_DIR "thrower_attack.png", 2, "THROWATTACK");
		animator->AddSpriteSheet(engine, ASSETS_DIR "thrower_hit.png", 1, "THROWHIT");
		animator->AddSpriteSheet(engine, ASSETS_DIR "thrower_death.png", 2, "THROWDEATH");
		animator->AddSpriteSheet(engine, ASSETS_DIR "thrower_idle.png", 1, "THROWIDLE");
		animator->AddSpriteSheet(engine, ASSETS_DIR "knife.png", 1, "KNIFE");

		audioManager = new AudioManager();
		audioManager->createMusicFx(ASSETS_DIR "kungfu_backgroundMusic.mp3", "BGM");
		audioManager->createMusicFx(ASSETS_DIR "kungfu_gameOver.mp3", "GAMEOVER");
		audioManager->createMusicFx(ASSETS_DIR "kungfu_gameStart.mp3", "GAMESTART");
		audioManager->createMusicFx(ASSETS_DIR "kungfu_level_win.mp3", "LEVELOVER");
		audioManager->createSoundFx(ASSETS_DIR "kungfu_kick.mp3", "KICK");
		audioManager->createSoundFx(ASSETS_DIR "kungfu_punch.mp3", "PUNCH");
		audioManager->createSoundFx(ASSETS_DIR "knife_throw.wav", "KNIFETHROW");
		audioManager->createSoundFx(ASSETS_DIR "grab.wav", "GRAB");
		audioManager->createSoundFx(ASSETS_DIR "enemyDeath.wav", "ENEMYDEATH");
		audioManager->createSoundFx(ASSETS_DIR "playerDeath.wav", "PLAYERDEATH");

		spawnHandler = new SpawnHandler();
		spawnHandler->Create(player);

		score = 0;
	}

	virtual void Init()
	{
		camera->Init(SCREEN_WIDTH, SCREEN_HEIGHT);
		player->Init(Vector2D(1470, 210), camera);

		for (int i = 0; i < 5; i++) {
			Enemy* enemy = grapplerPool.FirstAvailable();
			enemy->Init(Vector2D(1220 - 50 * i, 211), "GRAPPLER");
			enemy->GetComponent<RigidBodyComponent*>()->velocity.x = ENEMY_SPEED;
		}

		for (int i = 0; i < 1; i++) {
			Enemy* enemy = throwerPool.FirstAvailable();
			enemy->Init(Vector2D(650 - 50 * i, 205), "THROWER");
			enemy->GetComponent<RigidBodyComponent*>()->velocity.x = ENEMY_SPEED;
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

		if (game_over)
			dt = 0;

		animator->dt = dt;
		camera->UpdatePlayer(player);
		for (auto go = game_objects.begin(); go != game_objects.end(); go++)
			(*go)->Update(dt);

		spawnHandler->Update(0, &grapplerPool, dt, 1100, 1130, "GRAPPLER", true);
		spawnHandler->Update(1, &grapplerPool, dt, 1000, 1030, "GRAPPLER", false);
		spawnHandler->Update(2, &grapplerPool, dt, 920, 930, "GRAPPLER", true);
		spawnHandler->Update(3, &grapplerPool, dt, 730, 750, "GRAPPLER", false);
		spawnHandler->Update(4, &grapplerPool, dt, 630, 650, "GRAPPLER", true);
		spawnHandler->Update(5, &grapplerPool, dt, 450, 500, "GRAPPLER", true);
		spawnHandler->Update(6, &grapplerPool, dt, 450, 500, "GRAPPLER", false);
		spawnHandler->Update(6, &grapplerPool, dt, 570, 590, "GRAPPLER", false);
		spawnHandler->Update(7, &throwerPool, dt, 1050, 1090, "THROWER", false);
		spawnHandler->Update(8, &throwerPool, dt, 600, 620, "THROWER", true);
		spawnHandler->Update(9, &throwerPool, dt, 820, 850, "THROWER", false);
		spawnHandler->Update(10, &throwerPool, dt, 420, 440, "THROWER", false);

		animator->setFaceDirection(dt);
		audioManager->Update(engine, dt);

	}

	virtual void Draw()
	{
		engine->clearWindow();

		background->drawBackground(&camera->window);
		background_header->draw(0, 0);;
		enemy_health->RenderHPBar(115, 30, enemyHealth, enemyColor);
		player_health->RenderHPBar(115, 10, playerHealth, playerColor);

		animator->animatePlayer(player, camera);
		if (!game_over) {
			for (auto enemy : grapplerPool.pool)
				animator->animateGo(enemy, camera, &grapplerPool, animator->getEnemyState(enemy, player));
			for (auto enemy : throwerPool.pool)
				animator->animateGo(enemy, camera, &throwerPool, animator->getEnemyState(enemy, player));
			for (auto knife : knifePool.pool)
				animator->animateGo(knife, camera, &throwerPool, "KNIFE");
		}

		engine->drawText(420, 10, "LIVES");
		engine->drawText(340, 10, "SCORE");
		engine->drawText(15, 10, "PLAYER", 255, 0, 0);
		engine->drawText(15, 30, "ENEMY", 255, 1, 255);
		engine->drawText(560, 10, "TIMER");

		char msg[1024];
		sprintf(msg, "%d", Timer(haltTimer));
		engine->drawText(564, 30, msg);

		sprintf(msg, "%4d", lives);
		engine->drawText(425, 30, msg);

		sprintf(msg, "%4d", Score());
		engine->drawText(344, 30, msg);

		if (game_over && !level_win)
			engine->drawGameOverText(100, 90, "GAME OVER", 0, 0, 0);

		if (level_win)
			engine->drawGameOverText(100, 90, "LEVEL WIN", 255, 255, 32);

		engine->swapBuffers();
	}

	virtual void Receive(Message m, GameObject* go)
	{
		if (m == ENEMY_HIT) {
			Enemy* enemy = (Enemy*)go;
			animator->enemyHit(camera, enemy, enemy->animIds[2]);
			audioManager->playSound("ENEMYDEATH");
			score += (enemy->enemyType == "GRAPPLER" ) ? 200 : 500;
			enemyHealth -= (enemyHealth / 35);
		}

		if (m == THROW_KNIFE) {
			Enemy* enemy = (Enemy*)go;
			spawnHandler->spawnKnife(enemy, camera, &knifePool);
		}

		if (m == KNIFE_HIT) {
			playerHealth -= 0.05;
			score -= 50;
		}

		if (m == PLAYER_GRABBED) {
			Enemy* enemy = (Enemy*)go;
			enemy->grabPlayer = true;
			playerHealth -= 0.005;
			if (playerHealth <= 0.f)
				player->Send(GAME_OVER);
		}

		if (m == LEVEL_WIN) {
			level_win = true;
			haltTimer = true;
			game_over = true;
			Mix_HaltMusic();
			Mix_PlayMusic(audioManager->getMusic("LEVELOVER"), 1);
		}

		if (m == GAME_OVER) {
			game_over = true;
			lives = (lives == 0) ? 0 : lives--;
			player->enabled = false;
			Mix_HaltMusic();
			Mix_PlayMusic(audioManager->getMusic("GAMEOVER"), 1);
			Mix_PlayChannel(-1, audioManager->getSound("PLAYERDEATH"), 0);
		}
	}

	unsigned int Score()
	{
		return score;
	}

	unsigned int Timer(bool stop = false)
	{
		if (timer == 0 && game_over == false) {
			player->Send(GAME_OVER);
			return timer;
		}
		if (stop == true)
			return timer;

		return game_over ? timer : timer--;
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
