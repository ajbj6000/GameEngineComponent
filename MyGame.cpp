#include "MyGame.h"


MyGame::MyGame() : AbstractGame(), score(0), lives(3), numKeys(5), gameWon(false), box(5, 5, 30, 30) {
	TTF_Font * font = ResourceManager::loadFont("res/fonts/arial.ttf", 64);
	gfx->useFont(font);
	gfx->setVerticalSync(true);

    for (int i = 0; i < numKeys; i++) {
        std::shared_ptr<GameKey> k = std::make_shared<GameKey>();
        k->isAlive = true;
        k->pos = Point2(getRandom(0, 750), getRandom(0, 550));
        gameKeys.push_back(k);
    }

	coinSound = Mix_LoadWAV("res/Coin.wav");
	if (coinSound == NULL)
		throw EngineException("Couldn't load coin sound");

	victorySound = Mix_LoadWAV("res/victory.wav");
	if (victorySound == NULL)
		throw EngineException("Couldn't load victory sound");

	coinTexture = gfx->createTexture("res/Coin.png");
	easterEggTextTexture = gfx->createTexture("res/EasterEggText.png");
}

MyGame::~MyGame() {
	Mix_FreeChunk(coinSound);
	coinSound = NULL;
	Mix_FreeChunk(victorySound);
	victorySound = NULL;
}

void MyGame::handleKeyEvents() {
	int speed = 3;

	if (eventSystem->isPressed(Key::W))
		velocity.y = -speed;

	if (eventSystem->isPressed(Key::S))
		velocity.y = speed;

	if (eventSystem->isPressed(Key::A))
		velocity.x = -speed;

	if (eventSystem->isPressed(Key::D))
		velocity.x = speed;
}

void MyGame::update() {
	box.x += velocity.x;
	box.y += velocity.y;

	// SO trails is red now slowly shifting to white as it dies
	ParticleProps trailProps{};
	trailProps.position = { box.x + box.w / 2, box.y + box.h / 2 };
	trailProps.colorBegin = { 255, 0, 0, 48 }; //Red
	trailProps.colorEnd = { 255, 255, 255, 32 }; // This is white in RGB, 
	trailProps.lifeTime = 1.0f; // In seconds
	trailProps.sizeBegin = box.w ; // Particle is as big as box at start
	trailProps.sizeEnd = 0;
	trailProps.velocity = { (float)-velocity.x, (float)-velocity.y }; // This makes particle move in the opposite direction to where player is moving
	trailProps.velocityVariation = { 32.0f, 32.0f }; // Particle can go velocity + 32 pixels in random direction								
	// Do not apply velocity if box is out of bounds
	if ((box.x + box.w) > gfx->getCurrentWindowSize().w || box.x < 0)
		box.x -= velocity.x;
	if ((box.y + box.h) > gfx->getCurrentWindowSize().h || box.y < 0)
		box.y -= velocity.y;

	// Emit particle trail if box is moving
	if (velocity.x != 0 || velocity.y != 0)
		particleSystem->emit(trailProps); //Trail is emmited here, since it is in update() function it is emmited every frame (which is 60 fps here)

	for (size_t i = 0; i < gameKeys.size(); ++i)
	{
		auto& key = gameKeys[i];

		if (key->isAlive && box.intersects(Rectangle2(key->pos.x - 8, key->pos.y - 8, 16, 16))) {
			// Emit yellow particle 8 times when coin is picked up (8 just looked good)
			ParticleProps props{};
			props.position = { key->pos.x, key->pos.y };
			props.colorBegin = { 255, 255, 0, 255 };
			props.colorEnd = { 255, 255, 255, 0 };
			props.lifeTime = 0.8f;
			props.sizeBegin = 8;
			props.sizeEnd = 0;
			props.velocityVariation = { 100.0f, 100.0f };
			for (size_t i = 0; i < 8; ++i)
				particleSystem->emit(props); // Here are some particles emmited when player picks up coin with different props (which stands for properties btw)

			score += 200;
			key->isAlive = false;
			numKeys--;
			gameKeys.erase(gameKeys.begin() + i);

			sfx->playSound(coinSound);
		}
	}

	if (numKeys == 0)
		gameWon = true;
	
	particleSystem->update();

	velocity.x = 0;
	velocity.y = 0;
}

void MyGame::render() {
	gfx->setDrawColor(SDL_COLOR_RED);
	gfx->fillRect(box.x, box.y, box.w, box.h);

	gfx->setDrawColor(SDL_COLOR_YELLOW);
	for (auto key : gameKeys)
	{
		if (key->isAlive)
		{
			SDL_Rect rect{ key->pos.x - 8, key->pos.y - 8, 16, 16 };
			gfx->drawTexture(coinTexture, &rect);
		}
	}

	particleSystem->render(gfx);
}

void MyGame::renderUI() {
	gfx->setDrawColor(SDL_COLOR_AQUA);
	std::string scoreStr = std::to_string(score);
	gfx->drawText(scoreStr, 780 - scoreStr.length() * 50, 25);

	if (gameWon)
	{
		gfx->drawText("YOU WON!", 250, 400);
		SDL_Rect rect{ 250, 300, 330, 32 };
		gfx->drawTexture(easterEggTextTexture, &rect);
		if (!victorySoundPlayed)
		{
			sfx->playSound(victorySound);
			victorySoundPlayed = true;
		}
	}
}