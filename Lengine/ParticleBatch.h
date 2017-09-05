#pragma once
#include <functional>
#include <glm/glm.hpp>
#include "Vertex.h"
#include "TextureCache.h"
namespace Lengine {

	class SpriteBatch;
	class Particle {
	public:
		//include x,y,width,height
		glm::vec4 posrec;

		//include direction and speed
		glm::vec2 velocity;

		ColorRGBA8 color;
		//between 0 to 1
		float life;
		bool inuse=false;
		
	};

	inline void default_update(Particle& particle, float deltatime) {
		particle.posrec.x += particle.velocity.x*deltatime;
		particle.posrec.y += particle.velocity.y*deltatime;
		particle.life -= deltatime;
		if (particle.life <= 0.0f) {
			//change status to not in use
			particle.inuse = false;
		}
	}

	class ParticleBatch
	{
	public:
		ParticleBatch(float lifetime,const GLtexture& texture,std::function<void(Particle&,float)> updatefunc=default_update);
		~ParticleBatch();
		void addparticle(const glm::vec4&posrec, const glm::vec2 &velocity
			, const ColorRGBA8& color);
		void update(float deltatime);
		void draw(SpriteBatch* spritebatch);
	private:
		int findfreeparticle();
		//store the free particle index
		//decrease the search time
		int m_particleindex;
		GLtexture m_texture;
		std::function<void(Particle&, float)> m_updatefuc;
		float m_lifetime;
		//as dynamic array's pointer
		Particle * m_batchptr;
	};

}