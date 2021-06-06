#pragma once
#ifndef GAME_H
#define GAME_H

#include "Arrow.h"
#include "Target.h"

class Game {
public: 
    Game(vector<Target*> t); 
    virtual ~Game(); 
    void restart();
    void drawTargets(std::shared_ptr<Program> prog);
    void drawPartSystems(std::shared_ptr<Program> prog);
    void setCamera(mat4 inC); 
    State update(State s, vec3 arrow_pos);

private:
    int score; 
    vector<Target*> targets;
};

#endif