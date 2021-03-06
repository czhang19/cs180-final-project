#include <iostream>

#include "Game.h"
#include "Target.h"
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

Game::Game(vector<Target*> t) :
    score(0),
    targets(t),
    shotCount(0),
    missed(0),
    accuracy(100.0f), 
    state(LOBBY)
{ 
}

Game::~Game() {} 

void Game::restart() {
    score = 0;
    shotCount = 0;
    missed = 0;
    accuracy = 100.0f;
    for (int i = 0; i < targets.size(); i++) {
        targets[i]->reset();
    }
}

void Game::drawTargets(std::shared_ptr<Program> prog) {
    for (int i = 0; i < targets.size(); i++) {
        if (!targets[i]->exploded) {
            targets[i]->drawMe(prog);
        }
    }
}

void Game::drawPartSystems(std::shared_ptr<Program> prog) {
    for (int i = 0; i < targets.size(); i++) {
        if (targets[i]->exploded) {
            targets[i]->drawParticles(prog);
            targets[i]->update();
        }
    }
}

void Game::setCamera(mat4 inC) {
    for (int i = 0; i < targets.size(); i++) {
        targets[i]->setCamera(inC);
    }
}

State Game::update(State s, vec3 arrow_pos, float r) {
    for (int i = 0; i < targets.size(); i++) { // check if arrow contacted any target
        if (s == LOOSE && !targets[i]->exploded) {
            bool b = targets[i]->explodeOnContact(arrow_pos, r);
            if (b) 
            {
                score++;
                updateAccuracy();
                // cout << "score = " << score << endl;
                return INQUIVER; // if target exploded, stop drawing this arrow
            }
        }
    }
    return s;
}

void Game::updateAccuracy() {
    accuracy = 100 * score / (missed + score);
    // cout << "accuracy = " << accuracy << ", score = " << score << ", missed = " << missed << endl;; 
}