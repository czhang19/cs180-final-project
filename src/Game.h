#pragma once
#ifndef GAME_H
#define GAME_H

#include "Arrow.h"
#include "Target.h"

enum GameState {
    FREEPLAY,
    LOBBY, // displaying
    INGAME,
    GAMEOVER
};

class Game {
public: 
    int shotCount; 
    int missed; 

    Game(vector<Target*> t); 
    virtual ~Game(); 
    void restart();
    void drawTargets(std::shared_ptr<Program> prog);
    void drawPartSystems(std::shared_ptr<Program> prog);
    void setCamera(mat4 inC); 
    State update(State s, vec3 arrow_pos, float r);
    void updateAccuracy();
    int getScore() {return score;}
    int getAccuracy() {return accuracy;}
    GameState getState() {return state;}

private:
    int score; 
    int accuracy; // percentage
    vector<Target*> targets;
    GameState state; 
};

#endif