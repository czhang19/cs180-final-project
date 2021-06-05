#pragma once
#ifndef TARGET_H
#define TARGET_H

#include "Shape.h"
#include "MatrixStack.h"
#include "Program.h"
#include "particleSys.h"

using namespace std;
using namespace glm;

class Target
{
public:
    bool exploded; 

    Target(vec3 position, float rotation, vector<shared_ptr<Shape>> shapes, vec3 mins, vec3 maxes);
    virtual ~Target();
    void drawMe(std::shared_ptr<Program> prog); 
    void drawParticles(std::shared_ptr<Program> prog) {thePartSystem->drawMe(prog);}
    void setCamera(mat4 inC) {thePartSystem->setCamera(inC);}
    void update() {thePartSystem->update();}
    void reset(); 
    bool explodeOnContact(vec3 point, float radius); 

private:
    vec3 pos; // target position
    float rot; // target rotation
    shared_ptr<MatrixStack> Model; 
    vector<shared_ptr<Shape>> meshes;
    vec3 gMins; // bounding box in object space
    vec3 gMaxes;
    vec3 aabbMins; // axis-aligned bounding box in world space
    vec3 aabbMaxes;
    mat4 gTransform;
    particleSys *thePartSystem;

    void setAndDrawModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M); 
    vec3 normalizedScale(); 
    void scaleToOrigin(shared_ptr<MatrixStack>& Model);
    void explode();
};

#endif