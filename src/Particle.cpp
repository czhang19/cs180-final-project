//
// sueda - geometry edits Z. Wood
// 3/16
//

#include <iostream>
#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"

using namespace std; 


float randFloat(float l, float h)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * l + r * h;
}

Particle::Particle(vec3 start, vec3 col, float duration) :
	charge(1.0f),
	m(1.0f),
	d(0.0f),
	x(start),
	v(0.0f, 0.0f, 0.0f),
	lifespan(duration),
	tEnd(0.0f),
	scale(1.0f),
	color(vec4(col, 1.0f)),
	randomStart(0.0f)
{
}

Particle::~Particle()
{
}

void Particle::load(float t, vec3 start)
{
	// Random initialization
	// randomStart = t + randFloat(0.0f, 10.0f);
	// rebirth(randomStart, start);
	rebirth(t, start);
}

/* all particles born at the origin */
void Particle::rebirth(float t, vec3 start)
{
	charge = randFloat(0.0f, 1.0f) < 0.5 ? -1.0f : 1.0f;	
	m = 1.0f;
  	d = randFloat(0.0f, 0.02f);
	x = start;
	// v.x = randFloat(-0.8f, 0.8f);
	// v.y = randFloat(-0.5f, 2.5f);
	// v.z = randFloat(0.0f, 0.5f);
	v.x = randFloat(-2.0f, 2.0f);
	v.y = randFloat(-2.5f, 2.5f);
	v.z = randFloat(0.0f, 0.5f);
	// lifespan = 2.0f; // randFloat(10.0f, 20.0f); 
	tEnd = t + lifespan;
	scale = randFloat(0.2, 1.0f);
	color.a = 1.0f;
}

void Particle::update(float t, float h, const vec3 &g, const vec3 start)
{
	if(t < tEnd) {
		v += h*g;
		x += h*v; 
		color.a = (tEnd-t)/lifespan;
	} 
	// else {
	// 	rebirth(t, start);
	// }
	
}
