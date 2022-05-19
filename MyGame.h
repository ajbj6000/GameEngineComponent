#ifndef __TEST_GAME_H__
#define __TEST_GAME_H__

#include "../engine/AbstractGame.h"

struct GameKey {
	Point2 pos;
	bool isAlive;
};

class MyGame : public AbstractGame {
	private:
		Rect box;

		Vector2i velocity;

		std::vector<std::shared_ptr<GameKey>> gameKeys;

		/* GAMEPLAY */
		int score, numKeys, lives;
		bool gameWon;

		SDL_Texture* coinTexture;
		SDL_Texture* easterEggTextTexture;

		Mix_Chunk* coinSound = NULL;
		Mix_Chunk* victorySound = NULL;
		bool victorySoundPlayed = false;

		void handleKeyEvents();
		void update();
		void render();
		void renderUI();

	public:
        MyGame();
		~MyGame();
};

#endif