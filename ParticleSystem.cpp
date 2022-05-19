#include "ParticleSystem.h"
#include <random>

void ParticleSystem::addSpout(const ParticleSpout& particleSpout)
{
	particleSpouts.emplace_back(particleSpout);
}

void ParticleSystem::emit(const ParticleProps& particleProperties)
{
	if (particles.size() >= MAX_PARTICLES)
		return;

	//Add particle to particles vector and also calculate it's modified velocity and random rotationBegin/rotationEnd
	//This 3 lines of code is just for random number generation, generally you would have some class generating these, but there was no functionality for that
	//It just generates random number from start-end
	std::random_device rd;
	std::mt19937 e2(rd()); //This is "mersenne twister" rng is better/faster than using rand()
	std::uniform_real_distribution<> dist(0.0f, 1.0f); // Random distribution from 0 to 1
	float randomAngle = dist(e2) * 6.2831853; // Random number from 0 to 2*PI, used for random velocity direction calculation
	particles.emplace_back(
		Particle{ 
			Vector2f{ (float)particleProperties.position.x, (float)particleProperties.position.y },
			Vector2f( particleProperties.velocity.x + dist(e2) * particleProperties.velocityVariation.x * cos(randomAngle), //
					  particleProperties.velocity.y + dist(e2) * particleProperties.velocityVariation.y * sin(randomAngle) ),
			particleProperties.colorBegin,
			particleProperties.colorEnd,
			float(dist(e2) * 6.2831853),
			float(dist(e2) * 6.2831853),
			particleProperties.sizeBegin,
			particleProperties.sizeEnd,
			particleProperties.lifeTime,
			particleProperties.lifeTime // This translates to: spawn particle with whole it's life time remaining
		}
	);
}

void ParticleSystem::update()
{

	float dt = 0.016f; //Hardcoded for 60 fps

	/*
	* Particle spout update:
	*   Iterates over each particle spout and generates 
	*   particles in the location for some duration,
	*   If that duration is up it kills the particle spout 
	*/
	for (size_t i = 0; i < particleSpouts.size(); ++i)
	{
		auto& ps = particleSpouts[i];

		if (ps.passedDuration > ps.duration)
		{
			/* 
			* This is faster method to remove component from the vector,
			* instead of removing component from the middle and shifting next components to left,
			* It swaps component with rightmost component and just pop's that rightmost component
			* Which is O(1) instead of O(n), this is only useful if you don't care about the order.
			*/
			std::swap(ps, particleSpouts.back());
			particleSpouts.pop_back();
			continue;
		}

		for (size_t i = 0; i < ps.particlesPerEmission; ++i)
			emit(ps.particleProperties);

		ps.passedDuration += dt;
	}

	/*
	* Particle update:
	* Updates particle's position using it's velocity, 
	* Also updates how much time it has left till death, 
	* If it's 0 particle dies and is removed from the vector
	*/
	for (size_t i = 0; i < particles.size(); ++i)
	{
		auto& p = particles[i];
		if (p.lifeRemaining <= 0)
		{
			std::swap(p, particles.back());
			particles.pop_back();
			continue;
		}
		p.lifeRemaining -= dt;
		p.position.x += p.velocity.x * dt;
		p.position.y += p.velocity.y * dt;
	}
}

void ParticleSystem::render(const std::shared_ptr<GraphicsEngine>& gfx)
{
	/*
	* Iterates over all the particles, 
	* calculates what percentage of life it has lived (which is value from 0 to 1) 
	* and that can be used to interpolate it's color/size and other begin/end variables
	* Then it renders rectangle using those variables
	*/
	for (auto& p : particles)
	{
		float life = std::min(1.0f - p.lifeRemaining / p.lifeTime, 1.0f); //Ignore the min here, had weird bug and fixed it with this
		int size = p.sizeBegin + int(float(p.sizeEnd - p.sizeBegin) * life);
		// Lerp function: a + (b - a) * t   // t is percentage value from 0 to 1 (life in this case)
		uint8_t r = p.colorBegin.r + uint8_t(float((int)p.colorEnd.r - (int)p.colorBegin.r) * life);
		uint8_t g = p.colorBegin.g + uint8_t(float((int)p.colorEnd.g - (int)p.colorBegin.g) * life);
		uint8_t b = p.colorBegin.b + uint8_t(float((int)p.colorEnd.b - (int)p.colorBegin.b) * life);
		uint8_t a = p.colorBegin.a + uint8_t(float((int)p.colorEnd.a - (int)p.colorBegin.a) * life);
		float rotation = p.rotationBegin + (p.rotationEnd - p.rotationBegin) * life;
		//TODO: rotate, but engine does not support it
		//gfx->setRotation(rotation);
		gfx->setDrawColor({ r, g, b, a });
		gfx->fillRect(p.position.x - size / 2, p.position.y - size / 2, size, size); // Draw center aligned rectangle
	}
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            