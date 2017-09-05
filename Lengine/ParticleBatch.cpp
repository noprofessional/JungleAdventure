#include "ParticleBatch.h"
#include "SpriteBatch.h"

namespace Lengine {


	//updatefunc has default value default_update function
	ParticleBatch::ParticleBatch(float lifetime,const GLtexture& texture, std::function<void(Particle&, float)> updatefunc)
	{
		m_batchptr = new Particle[1000];
		m_particleindex = 0;
		m_texture = texture;
		m_lifetime = lifetime;
		m_updatefuc = updatefunc;
	}


	ParticleBatch::~ParticleBatch()
	{
		delete[] m_batchptr;
	}

	void ParticleBatch::addparticle(const glm::vec4&posrec,const glm::vec2 &velocity
		,const ColorRGBA8& color) {
		//find a free particle index
		int particleindex = findfreeparticle();
		//use alias to decrease code amount
		Particle & p = m_batchptr[particleindex];
		p.posrec = posrec;
		p.velocity = velocity;
		p.color = color;
		//just been born
		p.life = 1.0f;
		//set status to using,in case of being overwrite
		p.inuse = true;
		//set free index to next one
		m_particleindex = particleindex + 1;

	}

	void ParticleBatch::update(float deltatime) {
		for (int i = 0;i < 1000;i++) {
			if (m_batchptr[i].inuse) {
				m_updatefuc(m_batchptr[i], deltatime/m_lifetime);
			}
		}
	}

	//actual render not just add one to the spritebatch
	void ParticleBatch::draw(SpriteBatch* spritebatch) {
		const glm::vec4 uvrec(0.0f, 0.0f, 1.0f, 1.0f);
		for (int i = 0;i < 1000;i++) {
			Particle &p = m_batchptr[i];
			if (p.inuse) {
				//only draw particle that are in use
				spritebatch->draw(p.posrec, uvrec, m_texture.ids[0], 1.0f, p.color);
			}
		}
	}

	int ParticleBatch::findfreeparticle() {
		//give the size of the array
		//usually the next one should be free
		for (int i = m_particleindex;i < 1000;i++) {
			if (m_batchptr[i].inuse != true) {
				return i;
			}
		}
		//can't find then back from the begining
		for (int i = 0;i < m_particleindex;i++) {
			if (m_batchptr[i].inuse != true) {
				return i;
			}
		}

		//there is no free particle
		//overwrite the first one
		return 0;
	}

}