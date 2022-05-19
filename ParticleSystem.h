#ifndef __MY_ENGINE_H__
#define __MY_ENGINE_H__

#include "../EngineCommon.h"
#include "../GameMath.h"
#include "../GraphicsEngine.h"
#include <SDL.h>
#include <vector>

/*
* Particle spawn properties used in emit() function
* Note: There is two parts for some variables begin/end,
*       Like colorBegin/colorEnd That is because
*       When it spawns it's color is colorBegin 
        and it gradually changes to colorEnd till it Dies
*/
struct ParticleProps
{
	Vector2i position; // Where to generate particle (in pixels)
	Vector2f velocity; // Velocity of particle (in pixels)
	/*
	* How random the velocity is (in pixels), 
	* lets say you set y component to 10, 
	* that means that particle will spawn with 
	* velocity.y + (random number from -10 to 10 pixels)
	* so if velocity.y is 4 and random number generated was -1
	* than particle will spawn with 3 velocity.y (4 + (-1))
	* */
	Vector2f velocityVariation;
	SDL_Color colorBegin; // Color of a particle when it spawns
	SDL_Color colorEnd; // Color of a particle when it dies
	int sizeBegin; // Size of a particle when it spawns (in pixels)
	int sizeEnd; // Size of a particle when it dies (in pixels)
	float lifeTime = 1.0f; // How long will particle live (in seconds)
};

// This is used when you want to keep generating particles in a single location for some duration 
struct ParticleSpout
{
	ParticleProps particleProperties; // Which particle to generate
	float duration = 0.0f; // How long to keep generating particles (in seconds)
	float particlesPerEmission = 1; // How much particles to produce per frame

private:
	friend class ParticleSystem;
	float passedDuration = 0.0f; //Internal variable, do not change
};


class ParticleSystem {
	friend class XCube2Engine;
	/*
	* ParticleSystem uses this representation for particles internally, 
	* it's nearly identical to ParticleProps struct, 
	* used for updating and drawing particles
	*/
	struct Particle
	{
		Vector2f position;
		Vector2f velocity; // calculated as: velocity + randomVelocityInDirection * velocityVarioation
		SDL_Color colorBegin;
		SDL_Color colorEnd;
		float rotationBegin; // Currently engine does not support rotations so this variable is unused
		float rotationEnd; // Currently engine does not support rotations so this variable is unused
		int sizeBegin;
		int sizeEnd;
		float lifeTime = 1.0f;
		float lifeRemaining = 0.0f; // How much time remaining till particle dies, used to simplify some calculations
	};
	/*
	* Max particles that can be living at the same time, 
	* There doesn't have to be a limit so this can be increased to infinity 
	* (it's lower for performance reasons)
	*/
	static constexpr size_t MAX_PARTICLES = 10000;

	private:
		std::vector<Particle> particles;
		std::vector<ParticleSpout> particleSpouts;

	public:
		void addSpout(const ParticleSpout& particleSpout); // Spawn a particle spout described with particleSpout parameter
		void emit(const ParticleProps& particleProperties); // Spawn a particle described with particleProperties parameter
		void update(); // Should be placed in update function of the engine
		void render(const std::shared_ptr<GraphicsEngine>& gfx); // Should be placed in render function of the engine
};

#endif