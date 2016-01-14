#include "Particles.h"  
#include <sstream>

ParticleSystem::ParticleSystem(int width, int height)
{
	m_transparent = sf::Color(0, 0, 0, 0);
	m_image.create(width, height, m_transparent);

	m_position.x = 0.5f * width;
	m_position.y = 0.5f * height;
	m_particleSpeed = 20.0f;
	m_dissolve = false;
	m_dissolutionRate = 4;
	m_shape = Shape::CIRCLE;
}

ParticleSystem::~ParticleSystem()
{
	for (ParticleIterator it = m_particles.begin(); it != m_particles.end(); it++)
	{
		delete *it;
	}
}

void ParticleSystem::fuel(int particles)
{
	float angle;
	Particle* particle;
	for (int i = 0; i < particles; i++)
	{
		particle = new Particle();
		particle->pos.x = m_position.x;
		particle->pos.y = m_position.y;

		switch (m_shape)
		{
		case Shape::CIRCLE:
			angle = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 6.232f));
			particle->vel.x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / cos(angle)));
			particle->vel.y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / sin(angle)));
			break;
		case Shape::SQUARE:
			particle->vel.x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));
			particle->vel.y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));
			break;
		default:
			particle->vel.x = 0.5f; // Easily detected
			particle->vel.y = 0.5f; // Easily detected
		}

		if (particle->vel.x == 0.0f && particle->vel.y == 0.0f)
		{
			delete particle;
			continue;
		}

		particle->color.r = rand() % 255;
		particle->color.g = rand() % 255;
		particle->color.b = rand() % 255;
		particle->color.a = 255;
		m_particles.push_back(particle);
	}
}

void ParticleSystem::update()
{

	m_clock.restart();

	for (ParticleIterator it = m_particles.begin(); it != m_particles.end(); it++)
	{
		(*it)->vel.x += m_gravity.x;
		(*it)->vel.y += m_gravity.y;

		(*it)->pos.x += (*it)->vel.x * m_particleSpeed;
		(*it)->pos.y += (*it)->vel.y  * m_particleSpeed;

		if (m_dissolve) {
			(*it)->color.a -= m_dissolutionRate;
		}

		if ((*it)->pos.x > m_image.getSize().x || (*it)->pos.x < 0 || (*it)->pos.y > m_image.getSize().y || (*it)->pos.y < 0 || (*it)->color.a < 10)
		{
			delete (*it);
			it = m_particles.erase(it);
			if (it == m_particles.end()) return;
		}
	}
}

void ParticleSystem::render()
{
	for (ParticleIterator it = m_particles.begin(); it != m_particles.end(); it++)
	{
		m_image.setPixel((int)(*it)->pos.x, (int)(*it)->pos.y, (*it)->color);
	}
}

void ParticleSystem::remove()
{
	for (ParticleIterator it = m_particles.begin(); it != m_particles.end(); it++)
	{
		m_image.setPixel((int)(*it)->pos.x, (int)(*it)->pos.y, m_transparent);
	}
}

std::string ParticleSystem::getNumberOfParticlesString()
{
	std::ostringstream oss;
	oss << m_particles.size();
	return oss.str();
}