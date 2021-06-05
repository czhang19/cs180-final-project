#pragma once
#ifndef TARGET_H
#define TARGET_H

#include "Shape.h"
#include "MatrixStack.h"
#include "Program.h"

using namespace std;
using namespace glm;

class Target
{
public:
    Target(vec3 position, float rotation, vector<shared_ptr<Shape>> shapes, vec3 mins, vec3 maxes);
    virtual ~Target();
    void drawMe(std::shared_ptr<Program> prog); 

private:
    vec3 pos; // target position
    float rot; // target rotation
    shared_ptr<MatrixStack> Model; 
    vector<shared_ptr<Shape>> meshes;
    vec3 gMins;
    vec3 gMaxes;

    void setAndDrawModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M); 
    vec3 originTranslate(); 
    vec3 normalizedScale(); 
    void scaleToOrigin(shared_ptr<MatrixStack>& Model);
};

#endif