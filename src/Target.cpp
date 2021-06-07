#include "Target.h"
#include <iostream>

using namespace std;

Target::Target(vec3 position, float rotation, vector<shared_ptr<Shape>> shapes, vec3 mins, vec3 maxes) :
    pos(position),
    rot(rotation),
    meshes(shapes),
    gMins(mins),
    gMaxes(maxes),
    exploded(false)
{
    Model = make_shared<MatrixStack>();
    Model->translate(pos);
    Model->rotate(rot, vec3(0, 1, 0));
    Model->scale(vec3(0.2f, 0.2f, 0.2f));
    scaleToOrigin(Model);
    gTransform = Model->topMatrix(); 

    aabbMins = gTransform * vec4(gMins, 1.0f); 
    aabbMaxes = gTransform * vec4(gMaxes, 1.0f);
    float temp; 
    for (int i = 0; i < 3; i++) // if min became max and vice versa, swap
    { 
        if (aabbMins[i] > aabbMaxes[i]) {
            temp = aabbMins[i];
            aabbMins[i] = aabbMaxes[i];
            aabbMaxes[i] = temp;
        }
    }

    thePartSystem = new particleSys(position);
    thePartSystem->gpuSetup();
}

Target::~Target()
{
}

void Target::reset() {
    exploded = false; 
}

void Target::explode() {
    thePartSystem->reSet();
    exploded = true; 
}

// If contacted, explode target and return true
bool Target::explodeOnContact(vec3 point, float radius) {
    // point vs AABB
    // if ((point.x >= aabbMins.x && point.x <= aabbMaxes.x) &&
    //     (point.y >= aabbMins.y && point.y <= aabbMaxes.y) &&
    //     (point.z >= aabbMins.z && point.z <= aabbMaxes.z))
    // { 
        // cout << pos.x << " " << pos.y << " " << pos.z << endl;
        // explode(); 
        // return true;
    // }
    // sphere vs AABB
    float x = std::max(aabbMins.x, std::min(point.x, aabbMaxes.x));
    float y = std::max(aabbMins.y, std::min(point.y, aabbMaxes.y));
    float z = std::max(aabbMins.z, std::min(point.z, aabbMaxes.z));

    float distance = std::sqrt((x - point.x) * (x - point.x) +
                           (y - point.y) * (y - point.y) +
                           (z - point.z) * (z - point.z));

    if (distance < radius) {
        // cout << pos.x << " " << pos.y << " " << pos.z << endl;
        explode(); 
        return true;
    }

    return false;
}

void Target::setAndDrawModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M) {		
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    for (shared_ptr<Shape> mesh : meshes) {
        mesh->draw(prog);
    }
}

// Scale mesh to max width of 100
vec3 Target::normalizedScale() {
    float scale = 10/std::max((gMaxes.x-gMins.x), 
        std::max((gMaxes.y-gMins.y), (gMaxes.z-gMins.z)));
    return vec3(scale, scale, scale);
}

// Translate mesh to origin and scale
void Target::scaleToOrigin(shared_ptr<MatrixStack>& Model) {
    Model->scale(normalizedScale());
    Model->translate((gMins + gMaxes)/-2.0f);
}

void Target::drawMe(std::shared_ptr<Program> prog)
{
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(gTransform));
    for (shared_ptr<Shape> mesh : meshes) {
        mesh->draw(prog);
    }
}