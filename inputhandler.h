#pragma once

class InputHandler {
	char* spriteId;
public:

	 virtual void Update(Engine* engine, Player* player, float dt) {
		 if (dt <= 0)
			 return;

		 Engine::KeyStatus keys;
		 engine->getKeyStatus(keys);
		 spriteId = "PLAYER_IDLE";

		 if ((keys.right && !keys.left) || (keys.left && !keys.right))
			 spriteId = "PLAYER_LEFT";

		 if (keys.punch)
			 spriteId = "PLAYER_PUNCH";

		 if (keys.kick)
			 spriteId = "PLAYER_KICK";

		 if (keys.down && !keys.up) {
			 spriteId = "PLAYER_DOWN";
			 if (keys.punch)
				 spriteId = "PLAYER_DOWN_PUNCH";
			 else if (keys.kick)
				 spriteId = "PLAYER_DOWN_KICK";
		 }

		 if (keys.up && !keys.down) 
			 spriteId = "PLAYER_UP";

		 if (player->grabbed)
			 spriteId = "PLAYER_GRABBED";

		 if (player->isLevelOver)
			 spriteId = "PLAYER_LEFT";

		 player->setPlayerSprite(spriteId);
	 }
};