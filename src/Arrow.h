#pragma once
#ifndef ARROW_H
#define ARROW_H

#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm; 


enum State {
    INQUIVER,
    NOTCHED,
    LOOSE
};

class Arrow {
public:
    Arrow();
    virtual ~Arrow();
    void setState(State s) {state = s;}
    State getState() {return state;}
    void setPosition(vec3 p) {pos = p;}
    vec3 getPosition() {return pos;}
    void setVelocity(vec3 vel) {v = vel;}
    void update(float h, vec3 g); // update pos + velocity when LOOSE

private:
    State state; 
    vec3 pos; // use when LOOSE
    vec3 v; // velocity when LOOSE
};

#endif
