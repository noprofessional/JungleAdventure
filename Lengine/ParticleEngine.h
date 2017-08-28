#pragma once
#include<vector>
#include "ParticleBatch.h"

namespace Lengine {
	class SpriteBatch;
	class ParticleEngine
	{
	public:
		ParticleEngine();
		~ParticleEngine();
		void addbatch(ParticleBatch* batchptr);
		void update(float deltatime);
		void draw(SpriteBatch* spritebatch);
	private:
		std::vector<ParticleBatch*> m_batches;
		int a;
	};

}