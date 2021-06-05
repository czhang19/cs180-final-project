#include "Target.h"


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
    // Model->scale(vec3(0.25f, 0.25f, 0.25f));
    scaleToOrigin(Model);
    gTransform = Model->topMatrix(); 

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

void Target::explodeOnContact(vec3 pos, float radius) {
    
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