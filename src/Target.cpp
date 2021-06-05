#include "Target.h"


using namespace std;

Target::Target(vec3 position, float rotation, vector<shared_ptr<Shape>> shapes, vec3 mins, vec3 maxes) :
    pos(position),
    rot(rotation),
    meshes(shapes),
    gMins(mins),
    gMaxes(maxes)
{
    Model = make_shared<MatrixStack>();
}

Target::~Target()
{
}

void Target::setAndDrawModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M) {		
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    for (shared_ptr<Shape> mesh : meshes) {
        mesh->draw(prog);
    }
}

// Translate mesh to origin
vec3 Target::originTranslate() {
    float Xtrans = (gMins.x + gMaxes.x)/-2;
    float Ytrans = (gMins.y + gMaxes.y)/-2;
    float Ztrans = (gMins.z + gMaxes.z)/-2;
    return vec3(Xtrans, Ytrans, Ztrans);
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
    Model->translate(originTranslate());
}

void Target::drawMe(std::shared_ptr<Program> prog)
{
    Model->pushMatrix();
        Model->translate(pos);
        Model->rotate(rot, vec3(0, 1, 0));
        // Model->scale(vec3(0.25f, 0.25f, 0.25f));
        scaleToOrigin(Model);
        setAndDrawModel(prog, Model);
    Model->popMatrix();
}