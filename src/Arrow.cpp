#include "Arrow.h"

Arrow::Arrow() :
    pos(vec3(0.0f, 0.0f, 0.0f)),
    state(INQUIVER),
    v(vec3(0.0f, 0.0f, 0.0f))
{
}

Arrow::~Arrow() 
{
}

void Arrow::update(float h, vec3 g) 
{
    v += h*g;
	pos += h*v;
}