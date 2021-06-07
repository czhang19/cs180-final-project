/*
 * Program 4 example with diffuse and spline camera PRESS 'g'
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn (spline D. McGirr)
 */

#include <iostream>
#include <cmath>       
#include <glad/glad.h>
#include <time.h>
// #include <ft2build.h>
// #include FT_FREETYPE_H  

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "stb_image.h"
#include "Bezier.h"
#include "Spline.h"
#include "HorseBodyExt.h"
#include "Dummy.h"
#include "particleSys.h"
#include "Target.h"
#include "Arrow.h"
#include "Game.h"
#include "RenderText.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>
#define PI 3.1415927

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program - use this one for Blinn-Phong has diffuse
	std::shared_ptr<Program> prog;

	//Our shader program for textures
	std::shared_ptr<Program> texProg;

	//Our shader program for skybox
	std::shared_ptr<Program> cubeProg;

	// Our shader program for particles
	std::shared_ptr<Program> partProg;

	// Our shader program for text
	std::shared_ptr<Program> glyphProg;

	// the particle system
	// particleSys *thePartSystem;

	// Our geometry - multi-mesh shapes to be used 
	vector<vector<shared_ptr<Shape>>> meshes;
	vector<vector<shared_ptr<Texture>>> materials;
	GLint unit = 0;

	// Mesh global min and max
	vector<vec3> gMins;
	vector<vec3> gMaxes;

	// Terrain heightmap info
	vector<vector<float>> heights; 
	int canyonWidth;
	int canyonHeight;  
	float max_height = 30; 
	float g_groundSize = 500;

	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int g_GiboLen;
	// Ground VAO
	GLuint GroundVertexArrayID;
	
	unsigned int cubeMapTexture; 

	// Images to use as textures
	vector<shared_ptr<Texture>> textures;

	// Animation data
	float lightAngle = 110; 
	float gallopHeight = 0; 
	float gallopAngle = 0;
	int frontFrameCount = 0;
	int backFrameCount = 0;
	int baseFrameCount = 0;
	int cycleLength = 1;
	double baseFrameTime = 0;
	double backFrameTime = 0;
	double frontFrameTime = 0;
	double lastFrameTime;
	double delta; 
	float horseRotation = -180 * PI/180;
	bool horseIsMoving = false;

	/* Camera motion/navigation
		Mode 0 = normal WASD movement, scroll to look around
		Mode 1 = no WASD, scroll to rotate around horse
	*/
	int mode = 1; 

	// Mode 0 camera
	float gPhi = 0;
	float gTheta = 0;
	float speed = 0.01;
	vec3 g_view;
	vec3 g_strafe = vec3(1, 0, 0);
	vec3 g_eye;
	vec3 g_up = vec3(0, 1, 0);
	vec3 horse_pos; // horse_pos is horse's position on the splinepath

	// Mode 0 movement
	bool goLeft = false;
	bool goRight = false;
	bool goBack = false;
	bool goFront = false;
	bool goUp = false;
	bool goDown = false;

	// Mode 1 movement
	float RUN_SPEED = 8;
	float TURN_SPEED = 64;
	float currentSpeed = 0;
	float currentTurnSpeed = 0;

	// Mode 1 cinematic tour
	Spline splinepath[6];
	float tourLevel = 2.0f; // 1 = easy, 2 = hard
	bool goCamera = false;
	vec3 fixedPoint;
	float camRadius = 3;
	vec3 spherePos = vec3(0, 0, camRadius);

	// Gameplay
	Game* game;
	bool developerMode = 0; // 0 = off, 1 = on

	// Arrow Animation
	float h = 0.01f;
	vec3 g = vec3(0.0f, -20.0f, 0.0f);
	vector<Arrow*> arrows; 
	int quiverSize = 10; 
	int notchedArrow = 0; 

	// Targets
	vector<vec3> target_pos = {vec3(-41.5, -6, -30.7), vec3(-67.2, -6.8, -70), vec3(-57, -5.4, -144.5), vec3(-1.8, -6.7, 163),
								vec3(42.8, -6.3, -173.3), vec3(99.8, -6, -173), vec3(138.5, -6.3, -123), vec3(-24.6, -6.6, -136),
								vec3(166.5, -0.7, -73.9), vec3(167, -4.8, -39.7), vec3(131.7, -6.7, 13.9), vec3(157.8, -1.8, 49.8), 
								vec3(151.9, -2.9, 122.6), vec3(103.5, -3.4, 162.6), vec3(51, -5.8, 175), vec3(22.2, -5, 153.7), 
								vec3(-51.5, -5, 124.5), vec3(-29.7, -5.7, 78.7), vec3(149.8, -5.3, -31.2), vec3(-40.4, -5.8, 56.8)};
	vector<float> target_rot = {0.0f, 120.0f, 0.0f, -80.0f, 
								-70.0f, 30.0f, 10.0f, -70.0f,
								255.0f, 10.0f, 135.0f, 170.0f, 
								270.0f, 150.0f, 90.0f, 70.0f, 
								45.0f, 125.0f, -10.0f, 0.0f};

	// Grass, Random Scenery
	vector<vec2> clusters = {vec2(-15.2, 30), vec2(-20.3, 23.8), vec2(-15.8, 25), vec2(-15, 26.4)}; 
	vector<int> sizes = {3, 2, 2, 2};
	vector<vector<vector<vector<float>>>> randoms;

	// Text Rendering
	RenderText* textRenderer;
	FT_Library ft;
	// FT_Face face;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		// toggle move speed
		if (developerMode) {	
			if (key == GLFW_KEY_1 && action == GLFW_PRESS){
				speed = 0.01;
			}
			if (key == GLFW_KEY_2 && action == GLFW_PRESS){
				speed = 0.03;
			}
			if (key == GLFW_KEY_3 && action == GLFW_PRESS){
				speed = 0.1;
			}
			if (action == GLFW_PRESS) {
				if (key == GLFW_KEY_A) // turn left
					goLeft = true;
				if (key == GLFW_KEY_D) // turn right
					goRight = true;
				if (key == GLFW_KEY_S) // go backward
					goBack = true;
				if (key == GLFW_KEY_W) // go forward
					goFront = true;
				if (key == GLFW_KEY_Q) // go down
					goDown = true;
				if (key == GLFW_KEY_E) // go up
					goUp = true;
			} else if (action == GLFW_RELEASE) {
				if (key == GLFW_KEY_A)
					goLeft = false;
				if (key == GLFW_KEY_D)
					goRight = false;
				if (key == GLFW_KEY_S) 
					goBack = false;
				if (key == GLFW_KEY_W) 
					goFront = false;
				if (key == GLFW_KEY_Q) 
					goDown = false;
				if (key == GLFW_KEY_E) 
					goUp = false;
			}
		
			// Debugging tool to determine current eye location
			if (key == GLFW_KEY_P && action == GLFW_PRESS){
				cout << "Eye at: " << g_eye.x << ", " << g_eye.y << ", " << g_eye.z << endl;
			}

			if (key == GLFW_KEY_M && action == GLFW_PRESS) {
				if (mode == 0) {
					mode = 1;
					gPhi = 0;
					gTheta = 0;
					g_eye = vec3(horse_pos.x, horse_pos.y+1.4, horse_pos.z+3);
					fixedPoint = vec3(horse_pos.x, horse_pos.y+1.4, horse_pos.z);
					float x = camRadius*cos(gPhi)*cos(gTheta); // radius is 3
					float y = camRadius*sin(gPhi);
					float z = camRadius*cos(gPhi)*cos((glm::pi<float>()/2)-gTheta);
					spherePos = vec3(-x, -y, -z);
				} else if (mode == 1) {
					mode = 0;
					g_eye = fixedPoint + spherePos;
					vec3 direction = glm::normalize(fixedPoint - g_eye);
					gPhi = asin(direction.y);
					gTheta = atan2(direction.z, direction.x);
					g_view = glm::normalize(fixedPoint - g_eye);
				}
			}
		}
		// Update draw fill vs draw mesh
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}

		// Start and stop cinematic tour
		if (mode == 1) {
			if (key == GLFW_KEY_G && action == GLFW_RELEASE) {
				goCamera = !goCamera;
				if (goCamera) {
					horseIsMoving = true;
				} else {
					horseIsMoving = false;
				}
			}
		}

		// Shoot arrow with spacebar
		if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
			if (mode == 1) { // only allow archery in mode 1
				game->shotCount++;
				arrows[notchedArrow]->setState(LOOSE);
				arrows[notchedArrow]->setPosition(vec3(horse_pos.x, getHeightBary(horse_pos.x, horse_pos.z)+1.68, horse_pos.z));
				arrows[notchedArrow]->setVelocity((-spherePos - vec3(0.0f, -0.47f, 0.0f))*25.0f);

				notchedArrow = (notchedArrow + 1) % quiverSize; // notch next arrow
				if (arrows[notchedArrow]->getState() == LOOSE) {
					game->missed++;
					game->updateAccuracy();
				}
				arrows[notchedArrow]->setState(NOTCHED);
			}
		}	

		// Reset game, including horse_pos, targets and score
		if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
			game->restart();
			horseIsMoving = false;
			goCamera = false;

			// Reset all spline paths
			splinepath[0].reset();
			splinepath[1].reset();
			splinepath[2].reset();
			splinepath[3].reset();
			splinepath[4].reset();
			splinepath[5].reset();

			// Get initial position and orientation to place character
			vec3 last_pos = splinepath[0].getPosition();
			splinepath[0].update(0.01f);
			horse_pos = splinepath[0].getPosition();
			horse_pos.y = getHeightBary(horse_pos.x, horse_pos.z)+0.85;
			float dx = horse_pos.x - last_pos.x;
			float dz = horse_pos.z - last_pos.z; 
			if (dx != 0 || dz != 0) 
				horseRotation = atan2(dx, dz);
				cout << "in reset game" << endl;

			splinepath[0].reset(); // reset after initial small update
		}

	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY) 
	{
		if (mode == 0) {
			// constrain phi between -80 and 80 degrees
			float constraint = 80 * glm::pi<float>()/180; 
			float sensitivity = 16;
			if (gPhi + deltaY/sensitivity < constraint && 
				gPhi + deltaY/sensitivity > -constraint) {
				gPhi += deltaY/sensitivity;
			}
			gTheta -= deltaX/sensitivity; 
			float x = cos(gPhi)*cos(gTheta);
			float y = sin(gPhi);
			float z = cos(gPhi)*cos((glm::pi<float>()/2)-gTheta);
			g_view = glm::normalize(vec3(x, y, z));
		} else if (mode == 1) {
			float constraint = 30 * PI/180; // constrain phi between -30 and 30 degrees
			float sensitivity = 30; // bigger = less sensitive
			if (gPhi + deltaY/sensitivity < constraint && 
				gPhi + deltaY/sensitivity > -constraint) {
				gPhi += deltaY/sensitivity;
			}
			gTheta -= deltaX/sensitivity; 
			float x = camRadius*cos(gPhi)*cos(gTheta); // radius is 0.3
			float y = camRadius*sin(gPhi);
			float z = camRadius*cos(gPhi)*cos((glm::pi<float>()/2)-gTheta);
			spherePos = vec3(-x, -y, -z);
		}
	}

	void init(const std::string& resourceDirectory)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		// Set background color.
		CHECKED_GL_CALL(glClearColor(.72f, .84f, 1.06f, 1.0f));

		// Enable z-buffer test.
		CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
		CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		CHECKED_GL_CALL(glPointSize(24.0f));

		// Initialize the GLSL program that we will use for local shading
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("MatShine");
		prog->addUniform("light");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex"); //silence error


		// Initialize the GLSL program that we will use for texture mapping
		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		texProg->init();
		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
		texProg->addUniform("flip");
		texProg->addUniform("Texture0");
		texProg->addUniform("light");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addAttribute("vertTex");

		// Initialize the GLSL program that we will use for skybox
		cubeProg = make_shared<Program>();
		cubeProg->setVerbose(true);
		cubeProg->setShaderNames(resourceDirectory + "/cube_vert.glsl", resourceDirectory + "/cube_frag.glsl");
		cubeProg->init();
		cubeProg->addUniform("P");
		cubeProg->addUniform("V");
		cubeProg->addUniform("M");
		cubeProg->addUniform("skybox");
		cubeProg->addAttribute("vertPos");
		cubeProg->addAttribute("vertNor");
		cubeProg->addAttribute("vertTex"); //silence error

		// Initialize the GLSL program.
		partProg = make_shared<Program>();
		partProg->setVerbose(true);
		partProg->setShaderNames(
			resourceDirectory + "/billboard_vert.glsl",
			resourceDirectory + "/billboard_frag.glsl");
		if (! partProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		partProg->addUniform("P");
		partProg->addUniform("M");
		partProg->addUniform("V");
		partProg->addUniform("alphaTexture");
		partProg->addAttribute("vertPos");
		partProg->addAttribute("vertColor");
		partProg->addAttribute("vertTex"); //silence error

		// Initialize the GLSL program for text rendering
		glyphProg = make_shared<Program>();
		glyphProg->setVerbose(true);
		glyphProg->setShaderNames(resourceDirectory + "/glyph_vert.glsl", resourceDirectory + "/glyph_frag.glsl");
		if (! glyphProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		glyphProg->addUniform("P");
		glyphProg->addUniform("text");
		glyphProg->addUniform("textColor");
		glyphProg->addAttribute("vertex");

		gTheta = -glm::pi<float>()/2;
		srand(time(NULL));

		textRenderer = new RenderText(&ft, glyphProg);
	}

	void initTex(const std::string& resourceDirectory){
		vector<string> texNames = {"/canyon/diffuse.png", "/cartoonWood.jpg", "/grass/free grass.png", "/target/ARCHERY_TARGET_DIFF.png", "/alpha.bmp",
									"/bow/bow_diffuse.jpg", "/bow/clean_arrows.jpg"}; 
		vector<int> modes = {GL_CLAMP_TO_EDGE, GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE};

		for (int i = 0; i < texNames.size(); i++) {
			shared_ptr<Texture> texture = make_shared<Texture>();
			texture->setFilename(resourceDirectory + texNames[i]);
			texture->init();
			texture->setUnit(unit++);
			texture->setWrapModes(modes[i], modes[i]);
			textures.push_back(texture);
		}
	} 
	
	void initGeom(const std::string& resourceDirectory)
	{
		vector<string> objFiles = {"/stable.obj", "/cube.obj", "/horse/LD_HorseRtime02.obj", "/grass/free grass by adam127.obj", 
									"/myhorseext.obj", "/dummy.obj", "/ANIMATEDBOW.obj", "/target/ARCHERY_Target.obj",
									"/sphereWTex.obj"};
		vector<string> mtlFiles = {"", "", "/horse/", "/grass/", "", "", "/bow/", "", ""};
		vector<bool> hasTextures = {true, true, true, true, true, true, true, true, true};
		int numObj = objFiles.size();
		
		vector<vector<tinyobj::shape_t>> TOshapes(numObj);
		vector<vector<tinyobj::material_t>> objMaterials(numObj);
		vector<string> errStr(numObj);

		meshes.resize(numObj);
		gMins.resize(numObj);
		gMaxes.resize(numObj);
		materials.resize(numObj);

		// Load in the mesh and make the shape(s)
		vector<bool> rc; 
		for (int i = 0; i < numObj; i++) {
			if (mtlFiles[i].empty()) {
				rc.push_back(tinyobj::LoadObj(TOshapes[i], objMaterials[i], errStr[i], (resourceDirectory + objFiles[i]).c_str()));
			} else {
				rc.push_back(tinyobj::LoadObj(TOshapes[i], objMaterials[i], errStr[i], 
					(resourceDirectory + objFiles[i]).c_str(), (resourceDirectory + mtlFiles[i]).c_str()));
			}
		} 
		
		for (int j = 0; j < numObj; j++) {
			if (!rc[j]) {
				cerr << errStr[j] << endl;
			} else {
				for (int i = 0; i < TOshapes[j].size(); i++) {
					shared_ptr<Shape> mesh;
					if (hasTextures[j]) {
						mesh = make_shared<Shape>(true);
					} else {
						mesh = make_shared<Shape>(false);
					}
					
					mesh->createShape(TOshapes[j][i]);
					mesh->measure();
					mesh->init();
					meshes[j].push_back(mesh);
					
					// Set global min and max 
					if (i == 0 || gMins[j].x > mesh->min.x) {
						gMins[j].x = mesh->min.x;
					}
					if (i == 0 || gMaxes[j].x < mesh->max.x) {
						gMaxes[j].x = mesh->max.x;
					}
					if (i == 0 || gMins[j].y > mesh->min.y) {
						gMins[j].y = mesh->min.y;
					}
					if (i == 0 || gMaxes[j].y < mesh->max.y) {
						gMaxes[j].y = mesh->max.y;
					}
					if (i == 0 || gMins[j].z > mesh->min.z) {
						gMins[j].z = mesh->min.z;
					}
					if (i == 0 || gMaxes[j].z < mesh->max.z) {
						gMaxes[j].z = mesh->max.z;
					}
				}

				// Load diffuse materials from .mtl into textures
				for (int i = 0; i < objMaterials[j].size(); i++) {
					if (!objMaterials[j][i].diffuse_texname.empty()) {
						shared_ptr<Texture> temp = make_shared<Texture>();
						temp->setFilename(resourceDirectory + mtlFiles[j] + objMaterials[j][i].diffuse_texname);
						temp->init();
						temp->setUnit(unit++);
						temp->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
						
						materials[j].push_back(temp);
					}
				}
			}
		}

		for (int c = 0; c < clusters.size(); c++) {
			vector<vector<vector<float>>> randomClusters;
			for (int i = 0; i < sizes[c]; i++) {
				vector<vector<float>> randomSizes;
				for (int j = 0; j < sizes[c]; j++) {
					vector<float> randomPos;
					randomPos.push_back(((float) (rand() % 100))/100 - 0.5);
					randomPos.push_back(((float) (rand() % 100))/100 - 0.5);
					randomSizes.push_back(randomPos);
				}
				randomClusters.push_back(randomSizes);
			}
			randoms.push_back(randomClusters);
		}
		
		vector<std::string> faces {
			"px.jpg",
			"nx.jpg",
			"py.jpg",
			"ny.jpg",
			"pz.jpg",
			"nz.jpg"
		}; 
		cubeMapTexture = createSky(resourceDirectory + "/dawn/", faces);
		
		initGround(resourceDirectory);

		vector<Target*> targets;
		int currIndex = 7;
		for (int i = 0; i < target_pos.size(); i++) {
			Target* t = new Target(target_pos[i], target_rot[i], meshes[currIndex], gMins[currIndex], gMaxes[currIndex]);
			targets.push_back(t);
		}
		game = new Game(targets);

		for (int i = 0; i < quiverSize; i++) {
			Arrow* a = new Arrow();
			if (i == 0) // notch first arrow
				a->setState(NOTCHED); 
			arrows.push_back(a);
		}

		// Initialize spline paths
		float y = 0; // doesn't matter; use terrain height for horse_pos
		splinepath[0] = Spline(glm::vec3(-20,y,20), glm::vec3(-40,y,0), glm::vec3(-80,y,-80), glm::vec3(-60,y,-120), 20/tourLevel);
		splinepath[1] = Spline(glm::vec3(-60,y,-120), glm::vec3(-20,y,-150), glm::vec3(60,y,-175), glm::vec3(100,y,-160), 20/tourLevel);
		splinepath[2] = Spline(glm::vec3(100,y,-160), glm::vec3(160,y,-150), glm::vec3(180,y,-80), glm::vec3(150,y,-20), 20/tourLevel);
		splinepath[3] = Spline(glm::vec3(150,y,-20), glm::vec3(140,y,20), glm::vec3(160,y,80), glm::vec3(140,y,120), 15/tourLevel);
		splinepath[4] = Spline(glm::vec3(140,y,120), glm::vec3(120,y,160), glm::vec3(60,y,170), glm::vec3(0,y,160), 20/tourLevel);
		splinepath[5] = Spline(glm::vec3(0,y,160), glm::vec3(-60,y,130), glm::vec3(-45,y,60), glm::vec3(-30,y,50), 20/tourLevel);

		// Initialize horse orientation and position
		vec3 last_pos = splinepath[0].getPosition();
		splinepath[0].update(0.01f);
		horse_pos = splinepath[0].getPosition();
		horse_pos.y = getHeightBary(horse_pos.x, horse_pos.z)+0.85;
		float dx = horse_pos.x - last_pos.x;
		float dz = horse_pos.z - last_pos.z; 
		if (dx != 0 || dz != 0) 
			horseRotation = atan2(dx, dz);
			cout << "in reset game" << endl;

		splinepath[0].reset(); // reset after initial small update

		g_eye = vec3(horse_pos.x, horse_pos.y+1.4, horse_pos.z+3);
		fixedPoint = vec3(horse_pos.x, horse_pos.y+1.4, horse_pos.z);
		g_view = glm::normalize(fixedPoint - g_eye);

		lastFrameTime = glfwGetTime();
	}

	void initGround(const std::string& resourceDirectory) {
		loadHeightMap(resourceDirectory + "/canyon/height.png");
		int VERTEX_COUNT = 256; // CPU cannot handle much more vertices than 256 in the arrays/vector 
		
		int count = VERTEX_COUNT * VERTEX_COUNT;

		vector<float> vertices(count * 3);
		vector<float> normals(count * 3);
		vector<GLfloat> texcoords(count * 2);
		vector<unsigned short> indices(6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT));

		int vertexPointer = 0;
		for (int i = 0; i < VERTEX_COUNT; i++) {
			for (int j = 0; j < VERTEX_COUNT; j++) {
				vertices[vertexPointer * 3] = (float) j / ((float) VERTEX_COUNT - 1) * g_groundSize - g_groundSize/2;
				vertices[vertexPointer * 3 + 1] = getHeight(j*16, i*16);
				vertices[vertexPointer * 3 + 2] = (float) i / ((float) VERTEX_COUNT - 1) * g_groundSize - g_groundSize/2; 
				vec3 normal = calculateNormal(j, i);
				normals[vertexPointer * 3] = normal.x; 
				normals[vertexPointer * 3 + 1] = normal.y; 
				normals[vertexPointer * 3 + 2] = normal.z; 
				texcoords[vertexPointer * 2] = (float) j / ((float) VERTEX_COUNT - 1);
				texcoords[vertexPointer * 2 + 1] = (VERTEX_COUNT - 1 - (float) i) / ((float) VERTEX_COUNT - 1);
				vertexPointer++;
			}
		}
		int pointer = 0;
		for (int gz = 0; gz < VERTEX_COUNT - 1; gz++) {
			for (int gx = 0; gx < VERTEX_COUNT - 1; gx++) {
				int topL = (gz * VERTEX_COUNT) + gx;
				int topR = topL + 1;
				int botL = ((gz + 1) * VERTEX_COUNT) + gx;
				int botR = botL + 1;
				indices[pointer++] = topL;
				indices[pointer++] = botL;
				indices[pointer++] = topR;
				indices[pointer++] = topR;
				indices[pointer++] = botL;
				indices[pointer++] = botR;
			}
		}

		//generate the ground VAO
      	glGenVertexArrays(1, &GroundVertexArrayID);
      	glBindVertexArray(GroundVertexArrayID);

      	g_GiboLen = 6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT);
      	glGenBuffers(1, &GrndBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
      	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndNorBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
		glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), &normals[0], GL_STATIC_DRAW);

      	glGenBuffers(1, &GrndTexBuffObj);
      	glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
		glBufferData(GL_ARRAY_BUFFER, texcoords.size()*sizeof(GLfloat), &texcoords[0], GL_STATIC_DRAW);

      	glGenBuffers(1, &GIndxBuffObj);
     	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
      	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
	}

	// Calculate normals from terrain heightmap
	vec3 calculateNormal(int x, int z) {
		float heightL = getHeight(x*16-1, z*16);
		float heightR = getHeight(x*16+1, z*16);
		float heightD = getHeight(x*16, z*16-1);
		float heightU = getHeight(x*16, z*16+1);
		return glm::normalize(vec3(heightL-heightR, 2, heightD-heightU));
	}

	void drawGround(shared_ptr<Program> curS) {
     	curS->bind();
     	glBindVertexArray(GroundVertexArrayID);
     	textures[0]->bind(curS->getUniform("Texture0"));
		//draw the ground plane 
  		SetModel(vec3(0, -10, 0), 0, 0, 1, curS);
  		glEnableVertexAttribArray(0);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
  		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(1);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
  		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  		glEnableVertexAttribArray(2);
  		glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
  		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

   		// draw!
  		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
  		glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

  		glDisableVertexAttribArray(0);
  		glDisableVertexAttribArray(1);
  		glDisableVertexAttribArray(2);
  		curS->unbind();
     }

	void loadHeightMap(string filename) {
		int w, h, ncomps;

		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(filename.c_str(), &w, &h, &ncomps, 0);
		if(!data) {
			cerr << filename << " not found" << endl;
		}
		if(ncomps != 1) {
			cerr << filename << " must have 1 components (RGB or RGBA), instead has " << ncomps << endl;
		}
		if((w & (w - 1)) != 0 || (h & (h - 1)) != 0) {
			cerr << filename << " must be a power of 2" << endl;
		}
		canyonWidth = w;
		canyonHeight = h;

		vector<float> temp;
		for (int i = 0; i < h; i++) {
			temp.clear();
			for (int j = 0; j < w; j++) {
				float val = static_cast<int>(data[i*w+j])/255.0f;
				temp.push_back(val);
			}
			heights.push_back(temp);
		}
	}

	// Input: x and z between 0 and canyonWidth/Height - 1 (4096)
	// Output: y of terrain 
	float getHeight(float x, float z) {
		if (x < 0 || x >= canyonWidth || z < 0 || z >= canyonHeight) {
			return 0;
		}
		int h = canyonHeight - 1 - z;
		int w = x;
		float p = heights[h][w]; // pixel alpha value, normalized
		p *= max_height; 
		return p;
	}

	float barycentric(vec3 p1, vec3 p2, vec3 p3, vec2 pos) {
		float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
		float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
		float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
		float l3 = 1.0f - l1 - l2;
		return l1 * p1.y + l2 * p2.y + l3 * p3.y; 
	}
	
	float getHeightBary(float x0, float z0) {
		float w = (255) * (x0 + g_groundSize/2) / g_groundSize;
		float h = (255) * (z0 + g_groundSize/2) / g_groundSize;
		float x = w - floor(w); // should be between 0 and 1...
		float z = h - floor(h);
		float res;
		float LB = getHeight(floor(w)*16, floor(h)*16);
		float LT = getHeight(floor(w)*16, ceil(h)*16);
		float RT = getHeight(ceil(w)*16, ceil(h)*16);
		float RB = getHeight(ceil(w)*16, floor(h)*16);

		if (x < z) {
			res = barycentric(vec3(0, LB, 0), vec3(0, LT, 1), vec3(1, RT, 1), vec2(x, z));
		} else {
			res = barycentric(vec3(0, LB, 0), vec3(1, RT, 1), vec3(1, RB, 0), vec2(x, z));
		}
		return res - 10;
	}

	unsigned int createSky(string dir, vector<string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		for (GLuint i = 0; i < faces.size(); i++) {
			unsigned char *data = stbi_load((dir+faces[i]).c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
				glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
				glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			} else {
				cout << "failed to load: " << (dir+faces[i]).c_str() << endl;
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		
		cout << " creating cube map any errors : " << glGetError() << endl;
		return textureID;
	} 

	void SetMaterial(shared_ptr<Program> curS, int i) {
		float r;
		float g;
		float b;

    	switch (i) {
    		case 0: // light brown 
    			r = 175/255.0f;
				g = 147/255.0f;
				b = 101/255.0f;
				glUniform3f(curS->getUniform("MatAmb"), r/5, g/5, b/5);
    			glUniform3f(curS->getUniform("MatDif"), r, g, b);
    			glUniform3f(curS->getUniform("MatSpec"), r/4, g/4, b/4);
    			glUniform1f(curS->getUniform("MatShine"), 100.0);
    		break;
			case 1: // peach
				r = 255/255.0f;
				g = 211/255.0f;
				b = 175/255.0f;
				glUniform3f(curS->getUniform("MatAmb"), r/5, g/5, b/5);
    			glUniform3f(curS->getUniform("MatDif"), r, g, b);
    			glUniform3f(curS->getUniform("MatSpec"), r/4, g/4, b/4);
    			glUniform1f(curS->getUniform("MatShine"), 100.0);
			break;
			case 2: // dark brown
				r = 81/255.0f;
				g = 50/255.0f;
				b = 32/255.0f;
				glUniform3f(curS->getUniform("MatAmb"), r/5, g/5, b/5);
    			glUniform3f(curS->getUniform("MatDif"), r, g, b);
    			glUniform3f(curS->getUniform("MatSpec"), r/4, g/4, b/4);
    			glUniform1f(curS->getUniform("MatShine"), 100.0);
			break;
			case 3: // olive green
				r = 100/255.0f;
				g = 135/255.0f;
				b = 67/255.0f;
				glUniform3f(curS->getUniform("MatAmb"), r/10, g/10, b/10);
    			glUniform3f(curS->getUniform("MatDif"), r, g, b);
    			glUniform3f(curS->getUniform("MatSpec"), r/4, g/4, b/4);
    			glUniform1f(curS->getUniform("MatShine"), 100.0);
			break;
  		}
	}

	/* helper function to set model trasnforms */
  	void SetModel(vec3 trans, float rotY, float rotX, float sc, shared_ptr<Program> curS) {
  		mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
  		mat4 RotX = glm::rotate( glm::mat4(1.0f), rotX, vec3(1, 0, 0));
  		mat4 RotY = glm::rotate( glm::mat4(1.0f), rotY, vec3(0, 1, 0));
  		mat4 ScaleS = glm::scale(glm::mat4(1.0f), vec3(sc));
  		mat4 ctm = Trans*RotX*RotY*ScaleS;
  		glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
  	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   	}

	void setAndDrawModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M, int currIndex) {		
		setModel(prog, M);
		for (shared_ptr<Shape> mesh : meshes[currIndex]) {
			mesh->draw(prog);
		}
	}

	// Translate mesh to origin
	vec3 originTranslate(int index) {
		float Xtrans = (gMins[index].x + gMaxes[index].x)/-2;
		float Ytrans = (gMins[index].y + gMaxes[index].y)/-2;
		float Ztrans = (gMins[index].z + gMaxes[index].z)/-2;
		return vec3(Xtrans, Ytrans, Ztrans);
	}
	
	// Scale mesh to max width of 10
	vec3 normalizedScale(int index) {
		float scale = 10/std::max((gMaxes[index].x-gMins[index].x), 
			std::max((gMaxes[index].y-gMins[index].y), (gMaxes[index].z-gMins[index].z)));
		return vec3(scale, scale, scale);
	}

	// Translate mesh to origin and scale
	void scaleToOrigin(shared_ptr<MatrixStack>& Model, int currIndex) {
		Model->scale(normalizedScale(currIndex));
		Model->translate(originTranslate(currIndex));
	}


   	/* Camera controls */
	void SetView(shared_ptr<Program>  shader) {
		glm::mat4 Cam;
		if (mode == 0) {
			Cam = glm::lookAt(g_eye, g_eye + g_view, g_up);
		} else if (mode == 1) {
			Cam = glm::lookAt(fixedPoint + spherePos, fixedPoint, g_up); 
		}
  		glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(Cam));
	}

	glm::mat4 GetView() {
		glm::mat4 Cam;
		if (mode == 0) {
			Cam = glm::lookAt(g_eye, g_eye + g_view, g_up);
		} else if (mode == 1) {
			Cam = glm::lookAt(fixedPoint + spherePos, fixedPoint, g_up); 
		}
		return Cam;
	}

	void updatePosition(float frametime) {
		fixedPoint = vec3(horse_pos.x, horse_pos.y+1.4, horse_pos.z); // update for if we switch modes
		if (mode == 0) {
			if (goLeft) { // strafe left
				g_eye += speed * glm::normalize(glm::cross(g_up, g_view)); 
			} else if (goRight) { // strafe right
				g_eye -= speed * glm::normalize(glm::cross(g_up, g_view)); 
			} else if (goBack) { // dolly backward
				g_eye -= speed * g_view; 
			} else if (goFront) { // dolly forward
				g_eye += speed * g_view; 
			} else if (goUp) {
				g_eye += speed * g_up; 
			} else if (goDown) {
				g_eye -= speed * g_up; 
			}
		} else if (mode == 1) {
			if (!goCamera) {
				if (goLeft) { 
					currentTurnSpeed = TURN_SPEED;
				} else if (goRight) { 
					currentTurnSpeed = -TURN_SPEED;
				} else {
					currentTurnSpeed = 0;
				} 
				if (goBack) { 
					horseIsMoving = true;
					currentSpeed = -RUN_SPEED;
				} else if (goFront) { 
					horseIsMoving = true;
					currentSpeed = RUN_SPEED;
				} else {
					horseIsMoving = false;
					currentSpeed = 0;
				}

				horseRotation += (currentTurnSpeed * delta) * PI/180; 
				// cout << "in updatePosition" << endl;
				float distance = currentSpeed * delta;
				float dx = (float) (distance * sin(horseRotation));
				float dz = (float) (distance * cos(horseRotation));
				horse_pos.x += dx;
				horse_pos.z += dz;
				// cout << "horse_pos " << horse_pos.x << " " << horse_pos.z << endl;

				horse_pos.y = getHeightBary(horse_pos.x, horse_pos.z)+0.85;
			}
		}
	}


   	void updateUsingCameraPath(float frametime)  {

		if (goCamera) {
			vec3 last_pos = horse_pos; 
			if (!splinepath[0].isDone()){
				splinepath[0].update(frametime);
				horse_pos = splinepath[0].getPosition();
			} else if (!splinepath[1].isDone()){
				splinepath[1].update(frametime);
				horse_pos = splinepath[1].getPosition();
			} else if (!splinepath[2].isDone()){
				splinepath[2].update(frametime);
				horse_pos = splinepath[2].getPosition();
			} else if (!splinepath[3].isDone()){
				splinepath[3].update(frametime);
				horse_pos = splinepath[3].getPosition();
			} else if (!splinepath[4].isDone()){
				splinepath[4].update(frametime);
				horse_pos = splinepath[4].getPosition();
			} else if (!splinepath[5].isDone()){
				splinepath[5].update(frametime);
				horse_pos = splinepath[5].getPosition();
			} else {
				goCamera = false; // At the end of the track! 
				horseIsMoving = false;
				return;
			}
			horse_pos.y = getHeightBary(horse_pos.x, horse_pos.z)+0.85;
			float dx = horse_pos.x - last_pos.x;
			float dz = horse_pos.z - last_pos.z; 
			if (dx != 0 || dz != 0) 
				horseRotation = atan2(dx, dz);
		}
   	}
	
	// Animate hierarchically modeled horse
	void drawJillyHorse(std::shared_ptr<Program> texProg, shared_ptr<MatrixStack> Model) {
		int currIndex = 4;
		Model->pushMatrix();
			if (horseIsMoving)
				Model->translate(vec3(0, gallopHeight, 0));
			Model->translate(vec3(horse_pos.x, getHeightBary(horse_pos.x, horse_pos.z)+0.85, horse_pos.z));
			Model->scale(vec3(0.2, 0.2, 0.2));
			Model->rotate(horseRotation, vec3(0, 1, 0)); // facing direction
			scaleToOrigin(Model, currIndex);
			
			float bx; // temporary vars for rotating parts
			float by; 
			float bz;
			if (horseIsMoving) {
				// rotate around front of barrel
				bx = (meshes[currIndex][BARREL]->min.x + meshes[currIndex][BARREL]->max.x)/2; 
				by = meshes[currIndex][BARREL]->max.y;
				bz = meshes[currIndex][BARREL]->max.z;
				Model->translate(vec3(bx, by, bz));
				Model->rotate(gallopAngle*PI/180, vec3(1, 0, 0)); 
				Model->translate(vec3(-bx, -by, -bz));
			}
			
			// Draw base
			if (horseIsMoving) {
				for (int i = 0; i < BASE.size(); i++) {
					int p = BASE[i];
					Model->pushMatrix();
						if (p == 26) { // draw head
							Model->translate(vec3(0, -meshes[currIndex][p]->min.y/10, -meshes[currIndex][p]->min.z/10)); // move head closer to body
						} else if (p == 6) { // draw tail
							Model->translate(vec3(0, 0, -meshes[currIndex][p]->max.z/10)); // move tail closer to body
							Model->translate(vec3(0, meshes[currIndex][p]->max.y, meshes[currIndex][p]->max.z));
							Model->rotate(40*PI/180, vec3(1, 0, 0));
							Model->translate(vec3(0, -meshes[currIndex][p]->max.y, -meshes[currIndex][p]->max.z));
						} 
						setModel(texProg, Model);
						meshes[currIndex][p]->draw(texProg);
					Model->popMatrix();
				}

				// Draw front legs
				Model->pushMatrix();
					bx = 0; // rotate around top of right leg
					by = meshes[currIndex][UPLEG_RF]->max.y;
					bz = (meshes[currIndex][UPLEG_RF]->min.z + meshes[currIndex][UPLEG_RF]->max.z)/2;
					Model->translate(vec3(bx, by, bz));
					Model->rotate(frontFrames[frontFrameCount].upper_angle, vec3(1, 0, 0)); // Note: barrel_angle = chest_angle = rear_angle
					Model->translate(vec3(-bx, -by, -bz));
					Model->pushMatrix();
						bx = 0; // rotate around knee
						by = (meshes[currIndex][KNEE_RF]->min.y + meshes[currIndex][KNEE_RF]->max.y)/2;
						bz = (meshes[currIndex][KNEE_RF]->min.z + meshes[currIndex][KNEE_RF]->max.z)/2;
						Model->translate(vec3(bx, by, bz));
						Model->rotate(frontFrames[frontFrameCount].knee_angle, vec3(1, 0, 0)); 
						Model->translate(vec3(-bx, -by, -bz));
						Model->pushMatrix();	
							bx = 0; // rotate around ankle
							by = (meshes[currIndex][ANKLE_RF]->min.y + meshes[currIndex][ANKLE_RF]->max.y)/2;
							bz = (meshes[currIndex][ANKLE_RF]->min.z + meshes[currIndex][ANKLE_RF]->max.z)/2;
							Model->translate(vec3(bx, by, bz));
							Model->rotate(frontFrames[frontFrameCount].ankle_angle, vec3(1, 0, 0)); 
							Model->translate(vec3(-bx, -by, -bz));
							Model->pushMatrix();	
								bx = 0; // rotate hoof around pastern
								by = (meshes[currIndex][PASTERN_RF]->min.y + meshes[currIndex][PASTERN_RF]->max.y)/2;
								bz = (meshes[currIndex][PASTERN_RF]->min.z + meshes[currIndex][PASTERN_RF]->max.z)/2;
								Model->translate(vec3(bx, by, bz));
								Model->rotate(frontFrames[frontFrameCount].hoof_angle, vec3(1, 0, 0)); 
								Model->translate(vec3(-bx, -by, -bz));
								setModel(texProg, Model);
								meshes[currIndex][HOOF_RF]->draw(texProg);
							Model->popMatrix();
							setModel(texProg, Model);
							meshes[currIndex][ANKLE_RF]->draw(texProg);
							meshes[currIndex][PASTERN_RF]->draw(texProg);
						Model->popMatrix();
						setModel(texProg, Model);
						meshes[currIndex][KNEE_RF]->draw(texProg);
						meshes[currIndex][LOWLEG_RF]->draw(texProg);
					Model->popMatrix();
					setModel(texProg, Model);
					meshes[currIndex][UPLEG_RF]->draw(texProg);
				Model->popMatrix();
				// Left front leg
				int frontLeftFrameCount = (frontFrameCount - 1) % frontFrames.size(); // Right leg is 1 frame ahead of the left leg
				Model->pushMatrix();
					bx = 0; // rotate around top of left leg
					by = meshes[currIndex][UPLEG_LF]->max.y;
					bz = (meshes[currIndex][UPLEG_LF]->min.z + meshes[currIndex][UPLEG_LF]->max.z)/2;
					Model->translate(vec3(bx, by, bz));
					Model->rotate(frontFrames[frontLeftFrameCount].upper_angle, vec3(1, 0, 0)); // Note: barrel_angle = chest_angle = rear_angle
					Model->translate(vec3(-bx, -by, -bz));
					Model->pushMatrix();
						bx = 0; // rotate around knee
						by = (meshes[currIndex][KNEE_LF]->min.y + meshes[currIndex][KNEE_LF]->max.y)/2;
						bz = (meshes[currIndex][KNEE_LF]->min.z + meshes[currIndex][KNEE_LF]->max.z)/2;
						Model->translate(vec3(bx, by, bz));
						Model->rotate(frontFrames[frontLeftFrameCount].knee_angle, vec3(1, 0, 0)); 
						Model->translate(vec3(-bx, -by, -bz));
						Model->pushMatrix();	
							bx = 0; // rotate around ankle
							by = (meshes[currIndex][ANKLE_LF]->min.y + meshes[currIndex][ANKLE_LF]->max.y)/2;
							bz = (meshes[currIndex][ANKLE_LF]->min.z + meshes[currIndex][ANKLE_LF]->max.z)/2;
							Model->translate(vec3(bx, by, bz));
							Model->rotate(frontFrames[frontLeftFrameCount].ankle_angle, vec3(1, 0, 0)); 
							Model->translate(vec3(-bx, -by, -bz));
							Model->pushMatrix();	
								bx = 0; // rotate hoof around pastern
								by = (meshes[currIndex][PASTERN_LF]->min.y + meshes[currIndex][PASTERN_LF]->max.y)/2;
								bz = (meshes[currIndex][PASTERN_LF]->min.z + meshes[currIndex][PASTERN_LF]->max.z)/2;
								Model->translate(vec3(bx, by, bz));
								Model->rotate(frontFrames[frontLeftFrameCount].hoof_angle, vec3(1, 0, 0)); 
								Model->translate(vec3(-bx, -by, -bz));
								setModel(texProg, Model);
								meshes[currIndex][HOOF_LF]->draw(texProg);
							Model->popMatrix();
							setModel(texProg, Model);
							meshes[currIndex][ANKLE_LF]->draw(texProg);
							meshes[currIndex][PASTERN_LF]->draw(texProg);
						Model->popMatrix();
						setModel(texProg, Model);
						meshes[currIndex][KNEE_LF]->draw(texProg);
						meshes[currIndex][LOWLEG_LF]->draw(texProg);
					Model->popMatrix();
					setModel(texProg, Model);
					meshes[currIndex][UPLEG_LF]->draw(texProg);
				Model->popMatrix();

				// Draw back legs
				Model->pushMatrix();
					bx = 0; // rotate around top of right back leg
					by = meshes[currIndex][STIFLE_RB]->max.y;
					bz = (meshes[currIndex][STIFLE_RB]->min.z + meshes[currIndex][STIFLE_RB]->max.z)/2;
					Model->translate(vec3(bx, by, bz));
					Model->rotate(backFrames[backFrameCount].stifle_angle, vec3(1, 0, 0)); // Note: barrel_angle = chest_angle = rear_angle
					Model->translate(vec3(-bx, -by, -bz));
					Model->pushMatrix();
						bx = 0; // rotate around top of upper leg
						by = meshes[currIndex][UPLEG_RB]->max.y;
						bz = (meshes[currIndex][UPLEG_RB]->min.z + meshes[currIndex][UPLEG_RB]->max.z)/2;
						Model->translate(vec3(bx, by, bz));
						Model->rotate(backFrames[backFrameCount].upper_angle, vec3(1, 0, 0)); 
						Model->translate(vec3(-bx, -by, -bz));
						Model->pushMatrix();
							bx = 0; // rotate around knee
							by = (meshes[currIndex][KNEE_RB]->min.y + meshes[currIndex][KNEE_RB]->max.y)/2;
							bz = (meshes[currIndex][KNEE_RB]->min.z + meshes[currIndex][KNEE_RB]->max.z)/2;
							Model->translate(vec3(bx, by, bz));
							Model->rotate(backFrames[backFrameCount].knee_angle, vec3(1, 0, 0)); 
							Model->translate(vec3(-bx, -by, -bz));
							Model->pushMatrix();
								bx = 0; // rotate around ankle
								by = (meshes[currIndex][ANKLE_RB]->min.y + meshes[currIndex][ANKLE_RB]->max.y)/2;
								bz = (meshes[currIndex][ANKLE_RB]->min.z + meshes[currIndex][ANKLE_RB]->max.z)/2;
								Model->translate(vec3(bx, by, bz));
								Model->rotate(backFrames[backFrameCount].ankle_angle, vec3(1, 0, 0)); 
								Model->translate(vec3(-bx, -by, -bz));
								Model->pushMatrix();
									bx = 0; // rotate around pastern
									by = (meshes[currIndex][PASTERN_RB]->min.y + meshes[currIndex][PASTERN_RB]->max.y)/2;
									bz = (meshes[currIndex][PASTERN_RB]->min.z + meshes[currIndex][PASTERN_RB]->max.z)/2;
									Model->translate(vec3(bx, by, bz));
									Model->rotate(backFrames[backFrameCount].hoof_angle, vec3(1, 0, 0)); 
									Model->translate(vec3(-bx, -by, -bz));
									setModel(texProg, Model);
									meshes[currIndex][HOOF_RB]->draw(texProg);
								Model->popMatrix();
								setModel(texProg, Model);
								meshes[currIndex][PASTERN_RB]->draw(texProg);
								meshes[currIndex][ANKLE_RB]->draw(texProg);
							Model->popMatrix();
							setModel(texProg, Model);
							meshes[currIndex][LOWLEG_RB]->draw(texProg);
							meshes[currIndex][KNEE_RB]->draw(texProg);
						Model->popMatrix();
						setModel(texProg, Model);
						meshes[currIndex][UPLEG_RB]->draw(texProg);
					Model->popMatrix();
					setModel(texProg, Model);
					meshes[currIndex][STIFLE_RB]->draw(texProg);
				Model->popMatrix();
				// Left back leg
				int backLeftFrameCount = (backFrameCount - 1) % backFrames.size(); // Right leg is 1 frame ahead of the left leg
				Model->pushMatrix();
					bx = 0; // rotate around top of right back leg
					by = meshes[currIndex][STIFLE_LB]->max.y;
					bz = (meshes[currIndex][STIFLE_LB]->min.z + meshes[currIndex][STIFLE_LB]->max.z)/2;
					Model->translate(vec3(bx, by, bz));
					Model->rotate(backFrames[backLeftFrameCount].stifle_angle, vec3(1, 0, 0)); // Note: barrel_angle = chest_angle = rear_angle
					Model->translate(vec3(-bx, -by, -bz));
					Model->pushMatrix();
						bx = 0; // rotate around top of upper leg
						by = meshes[currIndex][UPLEG_LB]->max.y;
						bz = (meshes[currIndex][UPLEG_LB]->min.z + meshes[currIndex][UPLEG_LB]->max.z)/2;
						Model->translate(vec3(bx, by, bz));
						Model->rotate(backFrames[backLeftFrameCount].upper_angle, vec3(1, 0, 0)); 
						Model->translate(vec3(-bx, -by, -bz));
						Model->pushMatrix();
							bx = 0; // rotate around knee
							by = (meshes[currIndex][KNEE_LB]->min.y + meshes[currIndex][KNEE_LB]->max.y)/2;
							bz = (meshes[currIndex][KNEE_LB]->min.z + meshes[currIndex][KNEE_LB]->max.z)/2;
							Model->translate(vec3(bx, by, bz));
							Model->rotate(backFrames[backLeftFrameCount].knee_angle, vec3(1, 0, 0)); 
							Model->translate(vec3(-bx, -by, -bz));
							Model->pushMatrix();
								bx = 0; // rotate around ankle
								by = (meshes[currIndex][ANKLE_LB]->min.y + meshes[currIndex][ANKLE_LB]->max.y)/2;
								bz = (meshes[currIndex][ANKLE_LB]->min.z + meshes[currIndex][ANKLE_LB]->max.z)/2;
								Model->translate(vec3(bx, by, bz));
								Model->rotate(backFrames[backLeftFrameCount].ankle_angle, vec3(1, 0, 0)); 
								Model->translate(vec3(-bx, -by, -bz));
								Model->pushMatrix();
									bx = 0; // rotate around pastern
									by = (meshes[currIndex][PASTERN_LB]->min.y + meshes[currIndex][PASTERN_LB]->max.y)/2;
									bz = (meshes[currIndex][PASTERN_LB]->min.z + meshes[currIndex][PASTERN_LB]->max.z)/2;
									Model->translate(vec3(bx, by, bz));
									Model->rotate(backFrames[backLeftFrameCount].hoof_angle, vec3(1, 0, 0)); 
									Model->translate(vec3(-bx, -by, -bz));
									setModel(texProg, Model);
									meshes[currIndex][HOOF_LB]->draw(texProg);
								Model->popMatrix();
								setModel(texProg, Model);
								meshes[currIndex][PASTERN_LB]->draw(texProg);
								meshes[currIndex][ANKLE_LB]->draw(texProg);
							Model->popMatrix();
							setModel(texProg, Model);
							meshes[currIndex][LOWLEG_LB]->draw(texProg);
							meshes[currIndex][KNEE_LB]->draw(texProg);
						Model->popMatrix();
						setModel(texProg, Model);
						meshes[currIndex][UPLEG_LB]->draw(texProg);
					Model->popMatrix();
					setModel(texProg, Model);
					meshes[currIndex][STIFLE_LB]->draw(texProg);
				Model->popMatrix();
			} else {
				setModel(texProg, Model);
				for (int i = 0; i < BASE.size(); i++) {
					int p = BASE[i];
					if (p == HEAD_H) { // draw head
						Model->pushMatrix();
							Model->translate(vec3(0, -meshes[currIndex][p]->min.y/10, -meshes[currIndex][p]->min.z/10)); // move head closer to body
							setModel(texProg, Model);
							meshes[currIndex][p]->draw(texProg);
						Model->popMatrix();
					} else if (p == TAIL) {
						Model->pushMatrix();
							Model->translate(vec3(0, 0, -meshes[currIndex][p]->max.z/10)); // move tail closer to body
							Model->translate(vec3(0, meshes[currIndex][p]->max.y, meshes[currIndex][p]->max.z));
							Model->rotate(20*PI/180, vec3(1, 0, 0));
							Model->translate(vec3(0, -meshes[currIndex][p]->max.y, -meshes[currIndex][p]->max.z));
							setModel(texProg, Model);
							meshes[currIndex][p]->draw(texProg);
						Model->popMatrix();
					} else {
						meshes[currIndex][p]->draw(texProg);
					}
				}
				for (int i = 0; i < LEFTBACKLEG.size(); i++) {
					int p = LEFTBACKLEG[i];
					meshes[currIndex][p]->draw(texProg);
				}
				for (int i = 0; i < LEFTFRONTLEG.size(); i++) {
					int p = LEFTFRONTLEG[i];
					meshes[currIndex][p]->draw(texProg);
				}
				for (int i = 0; i < RIGHTBACKLEG.size(); i++) {
					int p = RIGHTBACKLEG[i];
					meshes[currIndex][p]->draw(texProg);
				}
				for (int i = 0; i < RIGHTFRONTLEG.size(); i++) {
					int p = RIGHTFRONTLEG[i];
					meshes[currIndex][p]->draw(texProg);
				}
			}
		Model->popMatrix();
	}

	vec3 getMidpoint(int currIndex, int bodyPart) {
		return vec3((meshes[currIndex][bodyPart]->min.x + meshes[currIndex][bodyPart]->max.x)/2,
					(meshes[currIndex][bodyPart]->min.y + meshes[currIndex][bodyPart]->max.y)/2,
					(meshes[currIndex][bodyPart]->min.z + meshes[currIndex][bodyPart]->max.z)/2);
	}

	void drawDummy(std::shared_ptr<Program> prog, shared_ptr<MatrixStack> Model) {
		int currIndex = 5;
		Model->pushMatrix();
			if (horseIsMoving)
				Model->translate(vec3(0, gallopHeight, 0));
			Model->translate(vec3(horse_pos.x, getHeightBary(horse_pos.x, horse_pos.z)+1.35, horse_pos.z));
			Model->scale(vec3(0.1, 0.1, 0.1));
			Model->rotate(horseRotation-90*PI/180, vec3(0, 1, 0)); // facing direction
			Model->rotate(-90*PI/180, vec3(1, 0, 0));
			scaleToOrigin(Model, currIndex);

			float bx; // temporary vars for rotating parts
			float by; 
			float bz;
			vec3 temp; 

			// draw upper body
			Model->pushMatrix();
				temp = getMidpoint(currIndex, BELLY); // rotate around belly
				Model->translate(temp);
				Model->rotate(-75*PI/180, vec3(0, 0, 1)); // TODO: Rotate according to g_eye/g_lookAt
				Model->translate(-temp);
				// draw tilted head facing almost forward 
				Model->pushMatrix();
					temp = getMidpoint(currIndex, NECK); // rotate around neck
					Model->translate(temp);
					Model->rotate(10*PI/180, vec3(1, 0, 0)); 
					Model->rotate(65*PI/180, vec3(0, 0, 1)); 
					Model->translate(-temp);
					setModel(prog, Model);
					SetMaterial(prog, 1);
					meshes[currIndex][NECK]->draw(prog);
					meshes[currIndex][HEAD_D]->draw(prog);
				Model->popMatrix();
				setModel(prog, Model);
				SetMaterial(prog, 3);
				meshes[currIndex][BELLY]->draw(prog);
				meshes[currIndex][TORSO]->draw(prog);
				// draw right arm
				Model->pushMatrix();
					temp = getMidpoint(currIndex, SHOULDER_R); // rotate around right shoulder
					Model->translate(temp);
					Model->rotate(20*PI/180, vec3(0, 0, 1)); 
					Model->rotate(-20*PI/180, vec3(1, 0, 0)); 
					Model->translate(-temp);
					Model->pushMatrix();
						temp = getMidpoint(currIndex, ELBOW_R); // rotate around right elbow
						Model->translate(temp);
						Model->rotate(145*PI/180, vec3(0, 0, 1)); 
						Model->rotate(-20*PI/180, vec3(1, 0, 0)); 
						Model->translate(-temp);
						Model->pushMatrix();
							temp = getMidpoint(currIndex, WRIST_R); // rotate around right wrist
							Model->translate(temp);
							Model->rotate(-90*PI/180, vec3(0, 1, 0)); 
							Model->translate(-temp);
							setModel(prog, Model);
							SetMaterial(prog, 1);
							meshes[currIndex][WRIST_R]->draw(prog);
							meshes[currIndex][HAND_R]->draw(prog);
						Model->popMatrix();
						setModel(prog, Model);
						SetMaterial(prog, 3);
						meshes[currIndex][ELBOW_R]->draw(prog);
						meshes[currIndex][FOREARM_R]->draw(prog);
					Model->popMatrix();
					setModel(prog, Model);
					meshes[currIndex][SHOULDER_R]->draw(prog);
					meshes[currIndex][BICEP_R]->draw(prog);
				Model->popMatrix();
				// draw left arm
				Model->pushMatrix();
					temp = getMidpoint(currIndex, SHOULDER_L); // rotate around left shoulder
					Model->translate(temp);
					Model->rotate(-20*PI/180, vec3(0, 0, 1)); 
					Model->translate(-temp);
					Model->pushMatrix();
						temp = getMidpoint(currIndex, ELBOW_L); // rotate around left elbow
						Model->translate(temp);
						Model->rotate(-20*PI/180, vec3(0, 0, 1)); 
						Model->translate(-temp);
						Model->pushMatrix();
							temp = getMidpoint(currIndex, WRIST_L); // rotate around left wrist
							Model->translate(temp);
							Model->rotate(-90*PI/180, vec3(0, 1, 0)); 
							Model->translate(-temp);
							setModel(prog, Model);
							SetMaterial(prog, 1);
							meshes[currIndex][WRIST_L]->draw(prog);
							meshes[currIndex][HAND_L]->draw(prog);
						Model->popMatrix();
						setModel(prog, Model);
						SetMaterial(prog, 3);
						meshes[currIndex][ELBOW_L]->draw(prog);
						meshes[currIndex][FOREARM_L]->draw(prog);
					Model->popMatrix();
					setModel(prog, Model);
					meshes[currIndex][SHOULDER_L]->draw(prog);
					meshes[currIndex][BICEP_L]->draw(prog);
				Model->popMatrix();
			Model->popMatrix();
			setModel(prog, Model);
			meshes[currIndex][HIPS]->draw(prog);
			
			// draw right leg
			Model->pushMatrix();
				temp = getMidpoint(currIndex, PELVIS_R); // rotate around right pelvis
				Model->translate(temp);
				Model->rotate(-45*PI/180, vec3(1, 0, 0)); 
				Model->rotate(-30*PI/180, vec3(0, 1, 0)); 
				Model->translate(-temp);
				Model->pushMatrix();
					temp = getMidpoint(currIndex, KNEE_R); // rotate around right knee
					Model->translate(temp);
					Model->rotate(50*PI/180, vec3(1, 0, 0)); 
					Model->rotate(45*PI/180, vec3(0, 1, 0)); 
					Model->translate(-temp);
					Model->pushMatrix();
						temp = getMidpoint(currIndex, ANKLE_R); // rotate around right ankle
						Model->translate(temp);
						Model->rotate(-30*PI/180, vec3(0, 0, 1)); 
						Model->translate(-temp);
						setModel(prog, Model);
						SetMaterial(prog, 2);
						meshes[currIndex][ANKLE_R]->draw(prog);
						meshes[currIndex][FOOT_R]->draw(prog);
					Model->popMatrix();
					setModel(prog, Model);
					meshes[currIndex][LOWLEG_R]->draw(prog);
					SetMaterial(prog, 0);
					meshes[currIndex][KNEE_R]->draw(prog);
				Model->popMatrix();
				setModel(prog, Model);
				meshes[currIndex][PELVIS_R]->draw(prog);
				meshes[currIndex][UPLEG_R]->draw(prog);
			Model->popMatrix();

			// draw left leg
			Model->pushMatrix();
				temp = getMidpoint(currIndex, PELVIS_L); // rotate around left pelvis
				Model->translate(temp);
				Model->rotate(45*PI/180, vec3(1, 0, 0)); 
				Model->rotate(-30*PI/180, vec3(0, 1, 0)); 
				Model->translate(-temp);
				Model->pushMatrix();
					temp = getMidpoint(currIndex, KNEE_L); // rotate around left knee
					Model->translate(temp);
					Model->rotate(-50*PI/180, vec3(1, 0, 0)); 
					Model->rotate(45*PI/180, vec3(0, 1, 0)); 
					Model->translate(-temp);
					Model->pushMatrix();
						temp = getMidpoint(currIndex, ANKLE_L); // rotate around left ankle
						Model->translate(temp);
						Model->rotate(30*PI/180, vec3(0, 0, 1)); 
						Model->translate(-temp);
						setModel(prog, Model);
						SetMaterial(prog, 2);
						meshes[currIndex][ANKLE_L]->draw(prog);
						meshes[currIndex][FOOT_L]->draw(prog);
					Model->popMatrix();
					setModel(prog, Model);
					meshes[currIndex][LOWLEG_L]->draw(prog);
					SetMaterial(prog, 0);
					meshes[currIndex][KNEE_L]->draw(prog);
				Model->popMatrix();
				setModel(prog, Model);
				meshes[currIndex][PELVIS_L]->draw(prog);
				meshes[currIndex][UPLEG_L]->draw(prog);
			Model->popMatrix();
		Model->popMatrix();
	}

	void render(float frametime) {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float aspect = width/(float)height;

		// Create the matrix stacks 
		auto Projection = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		// Update the camera position
		updateUsingCameraPath(frametime);
		updatePosition(frametime);

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 600.0f);

		int currIndex; // current obj mesh index
		glm::mat4 cam = GetView();
		game->setCamera(cam);

		cubeProg->bind();
		// Draw skybox
		currIndex = 1;
		glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glDepthFunc(GL_LEQUAL); // set the depth function to always draw the box!
		SetView(cubeProg); // set up view matrix to include your view transforms
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture); // bind the cube map texture
		Model->pushMatrix();
			Model->scale(vec3(g_groundSize, g_groundSize, g_groundSize));
			// glUniformMatrix4fv(cubeProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			setAndDrawModel(cubeProg, Model, currIndex);
		Model->popMatrix();
		glDepthFunc(GL_LESS); // set the depth test back to normal!
		cubeProg->unbind(); 
		
		texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(texProg);
		glUniform3f(texProg->getUniform("light"), cos(lightAngle * glm::pi<float>()/180), sin(lightAngle * glm::pi<float>()/180), 0);
		glUniform1i(texProg->getUniform("flip"), 1);
		textures[1]->bind(texProg->getUniform("Texture0"));
		// Draw stable
		Model->pushMatrix();
			currIndex = 0;
			Model->translate(vec3(-20, getHeightBary(-20, 28)+1.8, 28));
			// Model->scale(vec3(0.1, 0.1, 0.1));
			Model->rotate(120 * glm::pi<float>()/180, vec3(0, 1, 0));
			Model->rotate(-glm::pi<float>()/2, vec3(1, 0, 0));
			scaleToOrigin(Model, currIndex);
			setAndDrawModel(texProg, Model, currIndex);
		Model->popMatrix();
		// Draw horse in the stable
		currIndex = 2;
		float horse_x;
		float horse_y;
		float horse_z;
		Model->pushMatrix();
			horse_x = -23;
			horse_z = 31;
			horse_y = getHeightBary(horse_x, horse_z)+0.7;
			Model->translate(vec3(horse_x, horse_y, horse_z));
			Model->scale(vec3(0.2, 0.2, 0.2));
			// Model->rotate(180 * glm::pi<float>()/180, vec3(0, 1, 0));
			Model->rotate(-60*PI/180, vec3(0, 1, 0));
			scaleToOrigin(Model, currIndex);
			setModel(texProg, Model);
			for (int i = 0; i < materials[currIndex].size(); i++) {
				if (i == 1 || i == 4) {
					materials[currIndex][0]->bind(texProg->getUniform("Texture0"));
				} else if (i == 2 || i == 3) {
					materials[currIndex][1]->bind(texProg->getUniform("Texture0"));
				} else if (i == 0) {
					materials[currIndex][2]->bind(texProg->getUniform("Texture0"));
				}
				meshes[currIndex][i]->draw(texProg);
			}
		Model->popMatrix();

		drawJillyHorse(texProg, Model); 

		// Draw grass
		currIndex = 3;
		textures[2]->bind(texProg->getUniform("Texture0"));
		for (int c = 0; c < clusters.size(); c++) {
			for (int i = 0; i < sizes[c]; i++) {
				for (int j = 0; j < sizes[c]; j++) {
					Model->pushMatrix();	
						float r1 = randoms[c][i][j][0];
						float r2 = randoms[c][i][j][1];
						float x = clusters[c].x + (i - sizes[c]/2 + r1) * 0.8; 
						float z = clusters[c].y + (j - sizes[c]/2 + r2) * 0.8;
						Model->translate(vec3(x, getHeightBary(x, z) + 0.6, z));
						Model->scale(vec3(0.1, 0.1, 0.1));
						scaleToOrigin(Model, currIndex);
						setAndDrawModel(texProg, Model, currIndex);
					Model->popMatrix();
				}
			}
		}

		// draw bow
		currIndex = 6;
		vec3 temp;
		// materials[currIndex][1]->bind(texProg->getUniform("Texture0")); // TODO: add bow texture
		textures[5]->bind(texProg->getUniform("Texture0"));
		Model->pushMatrix();
			if (horseIsMoving)
				Model->translate(vec3(0, gallopHeight, 0));
			Model->translate(vec3(horse_pos.x, getHeightBary(horse_pos.x, horse_pos.z)+1.65, horse_pos.z));
			temp = getMidpoint(currIndex, 1);
			Model->rotate(horseRotation-90*PI/180, vec3(0, 1, 0)); // facing direction
			Model->translate(vec3(0.3f, 0, 0.1f)); // move a little forward
			Model->rotate(5*PI/180, vec3(0, 1, 0));
			Model->rotate(90*PI/180, vec3(1, 0, 0));
			Model->scale(vec3(0.25f, 0.25f, 0.25f));
			Model->translate(-temp); // move to origin
			setModel(texProg, Model);
			meshes[currIndex][1]->draw(texProg); // bow
		Model->popMatrix();

		// draw arrows
		for (Arrow* a : arrows) {
			State s = a->getState(); 
			if (s == LOOSE) {
				currIndex = 8;
				textures[1]->bind(texProg->getUniform("Texture0"));
				Model->pushMatrix();
					Model->translate(a->getPosition());
					Model->scale(vec3(0.05f, 0.05f, 0.05f));
					scaleToOrigin(Model, currIndex);
					setAndDrawModel(texProg, Model, currIndex);
				Model->popMatrix();
				// currIndex = 6;
				// textures[6]->bind(texProg->getUniform("Texture0"));
				// Model->pushMatrix();
				// 	Model->translate(a->getPosition());
				// 	temp = getMidpoint(currIndex, 0);
				// 	Model->rotate(a->getRotation()-90*PI/180, vec3(0, 1, 0)); 
				// 	Model->rotate(90*PI/180, vec3(1, 0, 0));
				// 	Model->scale(vec3(0.25f, 0.25f, 0.25f));
				// 	Model->translate(-temp); // move to origin
				// 	setModel(texProg, Model);
				// 	meshes[currIndex][0]->draw(texProg); // arrow
				// 	Model->scale(vec3(0.05f, 0.05f, 0.05f));
				// 	scaleToOrigin(Model, currIndex);
				// 	setAndDrawModel(texProg, Model, currIndex);
				// Model->popMatrix();
			} else if (s == NOTCHED) {
				currIndex = 6;
				textures[6]->bind(texProg->getUniform("Texture0"));
				Model->pushMatrix();
					if (horseIsMoving)
						Model->translate(vec3(0, gallopHeight, 0));
					Model->translate(vec3(horse_pos.x, getHeightBary(horse_pos.x, horse_pos.z)+1.68, horse_pos.z));
					temp = getMidpoint(currIndex, 0);
					Model->rotate(horseRotation-90*PI/180, vec3(0, 1, 0)); // facing direction
					Model->translate(vec3(0.2f, 0, 0.095f)); // move a little forward
					Model->rotate(5*PI/180, vec3(0, 1, 0));
					Model->rotate(90*PI/180, vec3(1, 0, 0));
					Model->scale(vec3(0.25f, 0.25f, 0.25f));
					Model->translate(-temp); // move to origin
					setModel(texProg, Model);
					meshes[currIndex][0]->draw(texProg); // index 0 = arrow
				Model->popMatrix();
			}
		}

		// draw targets
		currIndex = 7;
		textures[3]->bind(texProg->getUniform("Texture0"));
		game->drawTargets(texProg);

		texProg->unbind();

		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(prog);
		glUniform3f(prog->getUniform("light"), cos(lightAngle * glm::pi<float>()/180), sin(lightAngle * glm::pi<float>()/180), 0);
		drawDummy(prog, Model);
		prog->unbind();

		drawGround(texProg);

		// draw particle system LAST
		partProg->bind();
		textures[4]->bind(partProg->getUniform("alphaTexture"));
		CHECKED_GL_CALL(glUniformMatrix4fv(partProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix())));
		SetView(partProg);
		Model->pushMatrix();
			Model->loadIdentity();
			glUniformMatrix4fv(partProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			CHECKED_GL_CALL(glEnable(GL_BLEND)); 
			game->drawPartSystems(partProg);
			CHECKED_GL_CALL(glDisable(GL_BLEND)); 
		Model->popMatrix();
		partProg->unbind();

		// print score
		CHECKED_GL_CALL(glEnable(GL_BLEND)); 
		glyphProg->bind();
		glm::mat4 proj = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
		glUniformMatrix4fv(glyphProg->getUniform("P"), 1, GL_FALSE, value_ptr(proj));
		GameState s = game->getState();
		if (s == FREEPLAY) {
			// display instructions and press R to start
		} else if (s == LOBBY) {
			// display instructions and prompt when ready
		} else if (s == INGAME) {
			// display targets and instructions
		} else if (s == GAMEOVER) {
			// display score and "restart button"
		}

		string score = "Targets hit: " + to_string(game->getScore()) + "/20"; 
		string shotCount = "Balls fired: " + to_string(game->shotCount); 
		string accuracy = "Accuracy: " + to_string(game->getAccuracy()) + "%"; 
		string crosshair = "+";
		textRenderer->drawText(1, score, width / 2, height * 9 / 10, 1.5f, vec3(0.0f, 0.0f, 0.0f)); // 1 = CENTER
		textRenderer->drawText(1, shotCount, width / 2, height * 17 / 20, 1.0f, vec3(0.0f, 0.0f, 0.0f)); // 1 = CENTER
		textRenderer->drawText(1, accuracy, width / 2, height * 16 / 20, 1.0f, vec3(0.0f, 0.0f, 0.0f)); // 1 = CENTER
		textRenderer->drawText(1, crosshair, width / 2, height / 2, 2.0f, vec3(0.0f, 0.0f, 0.0f)); // 1 = CENTER
		glyphProg->unbind();
		CHECKED_GL_CALL(glDisable(GL_BLEND));


		// Pop matrix stacks.
		Projection->popMatrix();
		
		// Update horse animation variables 
		double curr = glfwGetTime();
		if (curr - baseFrameTime > (double) (baseFrameCount + 1) * cycleLength / baseFrames.size()) {
			// cout << curr - frameTime << " " << baseFrameCount << endl;
			baseFrameCount++;
			if (curr - baseFrameTime > cycleLength) {
				baseFrameTime = curr;
				baseFrameCount = 0;
			}	
		}
		if (curr - frontFrameTime > (double) (frontFrameCount + 1) * cycleLength / frontFrames.size()) {
			frontFrameCount++;
			if (curr - frontFrameTime > cycleLength) {
				frontFrameTime = curr;
				frontFrameCount = 0;
			}	
		}
		if (curr - backFrameTime > (double) (backFrameCount + 1) * cycleLength / backFrames.size()) {
			// cout << curr - frameTime << " " << baseFrameCount << endl;
			backFrameCount++;
			if (curr - backFrameTime > cycleLength) {
				backFrameTime = curr;
				backFrameCount = 0;
			}	
		}

		gallopHeight = sin(2*PI * (curr - baseFrameTime)/cycleLength - PI)*0.1;
		gallopAngle = sin(2*PI * (curr - baseFrameTime)/cycleLength)*2; // angle: 0-5 deg

		delta = curr - lastFrameTime;
		lastFrameTime = curr;

		// Check if arrow hit any targets, or the ground, or is out of bounds
		for (Arrow* a : arrows) {
			State s = a->getState();
			if (s == LOOSE) {
				a->update(h, g); 
				vec3 arrow_pos = a->getPosition();
				State newS = game->update(s, arrow_pos, 0.5f);
				a->setState(newS); 
				if ((newS == LOOSE && arrow_pos.y < getHeightBary(arrow_pos.x, arrow_pos.z)) ||
					(newS == LOOSE && (	abs(arrow_pos.x) > g_groundSize / 2 || 
										abs(arrow_pos.y) > g_groundSize / 2 || 
										abs(arrow_pos.z) > g_groundSize / 2 ))) { 
					a->setState(INQUIVER); // if arrow hit the ground, stop drawing this arrow
					game->missed++;
					game->updateAccuracy();
				}
			} 
		}
		
		// arrowRotationY = atan2(h*v.x, h*v.z);
	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	// windowManager->init(640, 480);
	windowManager->init(960, 720);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initTex(resourceDir);
	application->initGeom(resourceDir);

	auto lastTime = chrono::high_resolution_clock::now();
	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// save current time for next frame
		auto nextLastTime = chrono::high_resolution_clock::now();

		// get time since last frame
		float deltaTime =
			chrono::duration_cast<std::chrono::microseconds>(
				chrono::high_resolution_clock::now() - lastTime)
				.count();
		// convert microseconds (weird) to seconds (less weird)
		deltaTime *= 0.000001;

		// reset lastTime so that we can calculate the deltaTime
		// on the next frame
		lastTime = nextLastTime;

		// Render scene.
		application->render(deltaTime);
		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();

	return 0;
}
