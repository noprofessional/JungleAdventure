#include "ParticleEngine.h"
#include"SpriteBatch.h"

namespace Lengine {

	ParticleEngine::ParticleEngine() {

	}
	
	ParticleEngine::~ParticleEngine() {
		for (int i = 0;i < m_batches.size();i++) {
			//call ParticleBatches destructor
			//free all memory
			delete m_batches[i];
		}
	}

	void ParticleEngine::addbatch(ParticleBatch* batchptr){
		m_batches.push_back(batchptr);
	}

	void ParticleEngine::update(float deltatime) {
		for (int i = 0;i < m_batches.size();i++) {
			m_batches[i]->update(deltatime);
		}
	}

	//render when called
	void ParticleEngine::draw(SpriteBatch* spritebatch) {
		spritebatch->begin();
		for (int i = 0;i < m_batches.size();i++) {
			m_batches[i]->draw(spritebatch);
		}
		spritebatch->end();
		spritebatch->renderBatch();
	}



}