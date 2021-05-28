/*
 * Program 4 example with diffuse and spline camera PRESS 'g'
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn (spline D. McGirr)
 */

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "stb_image.h"
#include "Bezier.h"
#include "Spline.h"
#include <time.h>       

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

	// Our geometry - multi-meesh shapes to be used 
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
	float max_height = 3; 
	float g_groundSize = 50;

	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int g_GiboLen;
	// Ground VAO
	GLuint GroundVertexArrayID;
	
	unsigned int cubeMapTexture; 

	// Images to use as a textures
	shared_ptr<Texture> texture1;	
	shared_ptr<Texture> texture2;
	shared_ptr<Texture> texture3;

	// Animation data
	float lightAngle = 110; 
	float gallopHeight = 0; 
	float gallopAngle = 0;

	/* Camera motion/navigation
		Mode 0 = normal WASD movement, scroll to look around
		Mode 1 = no WASD, scroll to rotate around horse
	*/
	int mode = 0; 

	// Mode 0 camera
	float gPhi = 0;
	float gTheta = 0;
	float speed = 0.001; // 0.1;
	// vec3 g_view = vec3(0, 0, -1);
	vec3 g_view;
	vec3 g_strafe = vec3(1, 0, 0);
	// vec3 g_eye = vec3(-2.8,-0.8,3.5);
	vec3 g_eye;
	vec3 g_up = vec3(0, 1, 0);
	// vec3 horse_pos = vec3(-2.8,-0.8,3.5);
	// vec3 horse_pos = vec3(-2.8,-0.8,3.8); // assume y is the ground height
	vec3 horse_pos; // horse_pos is horse's position on the splinepath
	vec2 horse_start = vec2(-2.8,3.8); // start pos x, z

	// Mode 0 movement
	bool goLeft = false;
	bool goRight = false;
	bool goBack = false;
	bool goFront = false;
	bool goUp = false;
	bool goDown = false;

	// Mode 1 cinematic tour
	Spline splinepath[1];
	bool goCamera = false;
	// bool enabledFixedTour = false;
	// bool fixLookAt = false;
	// vec3 fixedPoint = vec3(-2, -0.7, 2.8);
	vec3 fixedPoint;
	float camRadius = 0.3;
	vec3 spherePos = vec3(0, 0, camRadius);

	// Random scenery
	time_t rseed; 

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (mode == 0) {
			// toggle move speed
			if (key == GLFW_KEY_1 && action == GLFW_PRESS){
				speed = 0.001;
			}
			if (key == GLFW_KEY_2 && action == GLFW_PRESS){
				speed = 0.01;
			}
			if (key == GLFW_KEY_3 && action == GLFW_PRESS){
				speed = 0.05;
			}
			if (action == GLFW_PRESS) {
				if (key == GLFW_KEY_A) // strafe left
					goLeft = true;
				if (key == GLFW_KEY_D) // strafe right
					goRight = true;
				if (key == GLFW_KEY_S) // dolly backward
					goBack = true;
				if (key == GLFW_KEY_W) // dolly forward
					goFront = true;
				if (key == GLFW_KEY_Q) // go up
					goUp = true;
				if (key == GLFW_KEY_E) // go down
					goDown = true;
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
					goUp = false;
				if (key == GLFW_KEY_E) 
					goDown = false;
			}
			// if (key == GLFW_KEY_A) { // strafe left
			// 	if (action == GLFW_PRESS) {
			// 		goLeft = true;
			// 	} else if (action == GLFW_RELEASE) {
			// 		goLeft = false;
			// 	}
			// }
			// if (key == GLFW_KEY_D) { // strafe right
			// 	if (action == GLFW_PRESS) {
			// 		goRight = true;
			// 	} else if (action == GLFW_RELEASE) {
			// 		goRight = false;
			// 	}
			// }
			// if (key == GLFW_KEY_S) { // dolly backward
			// 	if (action == GLFW_PRESS) {
			// 		goBack = true;
			// 	} else if (action == GLFW_RELEASE) {
			// 		goBack = false;
			// 	}
			// }
			// if (key == GLFW_KEY_W) { // dolly forward
			// 	if (action == GLFW_PRESS) {
			// 		goFront = true;
			// 	} else if (action == GLFW_RELEASE) {
			// 		goFront = false;
			// 	}
			// }
			// if (key == GLFW_KEY_Q && action == GLFW_PRESS){ // go up
			// 	if (action == GLFW_PRESS) {
			// 		goUp = true;
			// 	} else if (action == GLFW_RELEASE) {
			// 		goUp = false;
			// 	}
			// }
			// if (key == GLFW_KEY_E && action == GLFW_PRESS){ // go down
			// 	if (action == GLFW_PRESS) {
			// 		goDown = true;
			// 	} else if (action == GLFW_RELEASE) {
			// 		goDown = false;
			// 	}
			// }
		}
		// User first-person movement
		// if (key == GLFW_KEY_A && action == GLFW_PRESS) { // strafe left
		// 	g_eye += speed * glm::normalize(glm::cross(g_up, g_view)); 
		// }
		// if (key == GLFW_KEY_D && action == GLFW_PRESS) { // strafe right
		// 	g_eye -= speed * glm::normalize(glm::cross(g_up, g_view)); 
		// }
		// if (key == GLFW_KEY_S && action == GLFW_PRESS) { // dolly backward
		// 	g_eye -= speed * g_view; 
		// }
		// if (key == GLFW_KEY_W && action == GLFW_PRESS) { // dolly forward
		// 	g_eye += speed * g_view; 
		// }
		// Update positional light direction, clamping light angle
		// if (key == GLFW_KEY_Q && action == GLFW_PRESS){
		// 	if (lightAngle < 140) 
		// 		lightAngle += 10; 
		// }
		// if (key == GLFW_KEY_E && action == GLFW_PRESS){
		// 	if (lightAngle > 40) 
		// 		lightAngle -= 10; 
		// }
		// Toggle fixed tour (either look at fixed point or allow look around during animation)
		// if (key == GLFW_KEY_L && action == GLFW_PRESS){
		// 	// Only allow toggling if not currently in tour
		// 	if (!goCamera) 
		// 		enabledFixedTour = !enabledFixedTour; 
		// }
		// Debugging tool to determine current eye location
		if (key == GLFW_KEY_P && action == GLFW_PRESS){
			cout << "Eye at: " << g_eye.x << ", " << g_eye.y << ", " << g_eye.z << endl;
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
				// if (enabledFixedTour) { 
				// 	if (goCamera) { 
				// 		// Start tour
				// 		fixLookAt = true;
				// 	} else {
				// 		// Pause tour
				// 		fixLookAt = false;
				// 		// Update camera variables to match current view
				// 		vec3 direction = glm::normalize(fixedPoint - g_eye);
				// 		gPhi = asin(direction.y);
				// 		gTheta = atan2(direction.z, direction.x);
				// 		g_view = glm::normalize(fixedPoint - g_eye);
				// 	}
				// }	
			}
		}
		if (key == GLFW_KEY_M && action == GLFW_PRESS) {
			if (mode == 0) {
				mode = 1;
				gPhi = 0;
				gTheta = 0;
				g_eye = vec3(horse_pos.x, horse_pos.y+0.12, horse_pos.z+0.3);
				fixedPoint = vec3(horse_pos.x, horse_pos.y+0.12, horse_pos.z);
				float x = camRadius*cos(gPhi)*cos(gTheta); // radius is 0.3
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
		GLSL::checkVersion();

		// Set background color.
		glClearColor(.72f, .84f, 1.06f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

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
		prog->addUniform("lightPos");
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

		initTex(resourceDirectory);

  		// Initialize spline paths
		float y = -0.85;
		splinepath[0] = Spline(glm::vec3(-2,y,2), glm::vec3(-4,y,0), glm::vec3(-11.5,y,-10), glm::vec3(-6,y,-12), 15);
		// splinepath[1] = Spline(glm::vec3(-2,y,-16), glm::vec3(5,y,-18), glm::vec3(20,y,-17), glm::vec3(17,y,-8), 25);

		gTheta = -glm::pi<float>()/2;
		rseed = time(NULL); 
	}

	void initTex(const std::string& resourceDirectory){
		//read in a load the texture
		texture1 = make_shared<Texture>();
  		texture1->setFilename(resourceDirectory + "/canyon/diffuse.png");
  		texture1->init();
  		texture1->setUnit(unit++);
  		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

  		texture2 = make_shared<Texture>();
  		texture2->setFilename(resourceDirectory + "/cartoonWood.jpg");
  		texture2->init();
  		texture2->setUnit(unit++);
		texture2->setWrapModes(GL_REPEAT, GL_REPEAT);

		texture3 = make_shared<Texture>();
  		texture3->setFilename(resourceDirectory + "/grass/free grass.png");
  		texture3->init();
  		texture3->setUnit(unit++);
  		texture3->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	} 
	
	void initGeom(const std::string& resourceDirectory)
	{
		vector<string> objFiles = {"/stable.obj", "/cube.obj", "/horse/LD_HorseRtime02.obj", "/grass/free grass by adam127.obj", "/horse2/horse 1.obj"};
		vector<string> mtlFiles = {"", "", "/horse/", "/grass/", "/horse2/"};
		vector<bool> hasTextures = {true, true, true, true, true};
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

		horse_pos = vec3(horse_start.x, getHeightW(horse_start.x, horse_start.y) + 0.08, horse_start.y);
		g_eye = vec3(horse_pos.x, horse_pos.y+0.12, horse_pos.z+0.3);
		fixedPoint = vec3(horse_pos.x, horse_pos.y+0.12, horse_pos.z);
		g_view = glm::normalize(fixedPoint - g_eye);
	}

	void initGround(const std::string& resourceDirectory) {
		loadHeightMap(resourceDirectory + "/canyon/height.png");
		int VERTEX_COUNT = 256; // CPU cannot handle much more vertices than 256 in the arrays/vector 
		
		int count = VERTEX_COUNT * VERTEX_COUNT;
		float g_groundY = -0.25;

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
     	texture1->bind(curS->getUniform("Texture0"));
		//draw the ground plane 
  		SetModel(vec3(0, -1, 0), 0, 0, 1, curS);
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

	// Input: x and z between 0 and canyonWidth/Height (4096)
	// Output: y of terrain 
	float getHeight(float x, float z) {
		if (x < 0 || x >= canyonHeight || z < 0 || z >= canyonHeight) {
			return 0;
		}
		int h = canyonHeight - 1 - z;
		int w = x;
		float p = heights[h][w]; // pixel alpha value, normalized
		p *= max_height; 
		return p;
	}

	// Input: x and z in world space
	// Output: y of terrain in world space
	float getHeightW(float x, float z) {
		int w = (canyonWidth - 1) * (x + g_groundSize/2) / g_groundSize;
		int h = (canyonHeight - 1) * (z + g_groundSize/2) / g_groundSize;
		return getHeight(w, h) - 1;
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
		// if (enabledFixedTour && fixLookAt) {
		// 	Cam = glm::lookAt(g_eye, fixedPoint, g_up); // During the tour, keep lookAt at fixedPoint
		// } else {
		// 	//Cam  = glm::lookAt(g_eye, g_eye + g_view, g_up); // In general, calculate lookAt with with g_view and g_eye
		// 	Cam = glm::lookAt(g_eye, fixedPoint, g_up); 
		// }
		if (mode == 0) {
			Cam = glm::lookAt(g_eye, g_eye + g_view, g_up);
		} else if (mode == 1) {
			Cam = glm::lookAt(fixedPoint + spherePos, fixedPoint, g_up); 
		}
  		glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(Cam));
	}

	void updatePosition(float frametime) {
		fixedPoint = vec3(horse_pos.x, horse_pos.y+0.12, horse_pos.z);
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
	}


   	void updateUsingCameraPath(float frametime)  {

		if (goCamera) {
			if (!splinepath[0].isDone()){
				splinepath[0].update(frametime);
				horse_pos = splinepath[0].getPosition();
				// g_eye = splinepath[0].getPosition();
			// } else if (!splinepath[1].isDone()) {
			// 	splinepath[1].update(frametime);
			// 	g_eye = splinepath[1].getPosition();
			} else {
				splinepath[0].reset();
				// splinepath[1].reset();
				// // Once the tour is over, update variables and view
				// if (enabledFixedTour) {
				// 	fixLookAt = false;
				// 	goCamera = false;
				// 	// Update camera variables to match current view
				// 	vec3 direction = glm::normalize(fixedPoint - g_eye);
				// 	gPhi = asin(direction.y);
				// 	gTheta = atan2(direction.z, direction.x);
				// 	g_view = glm::normalize(fixedPoint - g_eye);
				// }
			}
		}
   	}

	void render(float frametime) {
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

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
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		int currIndex; // current obj mesh index

		cubeProg->bind();
		// Draw skybox
		currIndex = 1;
		glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glDepthFunc(GL_LEQUAL); // set the depth function to always draw the box!
		SetView(cubeProg); // set up view matrix to include your view transforms
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture); // bind the cube map texture
		Model->pushMatrix();
			Model->scale(vec3(50, 50, 50));
			glUniformMatrix4fv(cubeProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			setAndDrawModel(cubeProg, Model, currIndex);
		Model->popMatrix();
		glDepthFunc(GL_LESS); // set the depth test back to normal!
		cubeProg->unbind(); 
		
		texProg->bind();
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		SetView(texProg);
		glUniform3f(texProg->getUniform("light"), cos(lightAngle * glm::pi<float>()/180), sin(lightAngle * glm::pi<float>()/180), 0);
		glUniform1i(texProg->getUniform("flip"), 1);
		texture2->bind(texProg->getUniform("Texture0"));
		// Draw stable
		Model->pushMatrix();
			currIndex = 0;
			Model->translate(vec3(-2, getHeightW(-2, 2.8)+0.18, 2.8));
			Model->scale(vec3(0.1, 0.1, 0.1));
			Model->rotate(120 * glm::pi<float>()/180, vec3(0, 1, 0));
			Model->rotate(-glm::pi<float>()/2, vec3(1, 0, 0));
			scaleToOrigin(Model, currIndex);
			setAndDrawModel(texProg, Model, currIndex);
		Model->popMatrix();
		// Draw horses
		currIndex = 2;
		float horse_x;
		float horse_y;
		float horse_z;
		// Model->pushMatrix();
		// 	horse_x = g_eye.x;
		// 	horse_z = g_eye.z;
		// 	horse_y = getHeightW(horse_x, horse_z)+0.085+gallopHeight; 
		// 	// horse_y = std::min(getHeightW(horse_x, horse_z)+0.08, g_eye.y - 0.1); // horse must stay on ground
		// 	// Model->translate(horse_pos); 
		// 	Model->translate(vec3(horse_x, horse_y, horse_z)); 
		// 	Model->scale(vec3(0.02, 0.02, 0.02));
		// 	Model->rotate(180 * glm::pi<float>()/180, vec3(0, 1, 0));
		// 	Model->rotate(gallopAngle*PI/180, vec3(1, 0, 0));
		// 	scaleToOrigin(Model, currIndex);
		// 	setModel(texProg, Model);
		// 	for (int i = 0; i < materials[currIndex].size(); i++) {
		// 		if (i == 1 || i == 4) {
		// 			materials[currIndex][0]->bind(texProg->getUniform("Texture0"));
		// 		} else if (i == 2 || i == 3) {
		// 			materials[currIndex][1]->bind(texProg->getUniform("Texture0"));
		// 		} else if (i == 0) {
		// 			materials[currIndex][2]->bind(texProg->getUniform("Texture0"));
		// 		}
		// 		meshes[currIndex][i]->draw(texProg);
		// 	}
		// Model->popMatrix();
		Model->pushMatrix();
			// horse_x = -2.3;
			// horse_z = 3.1;
			// horse_y = getHeightW(horse_x, horse_z)+0.08;
			// Model->translate(vec3(horse_x, horse_y, horse_z));
			Model->translate(horse_pos);
			// Model->translate(vec3(horse_pos.x, getHeightW(horse_pos.x, horse_pos.z)+0.08, horse_pos.z));
			Model->scale(vec3(0.02, 0.02, 0.02));
			Model->rotate(-60 * glm::pi<float>()/180, vec3(0, 1, 0));
			// Model->rotate(gallopAngle*PI/180, vec3(1, 0, 0)); // TODO: remove
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
		// Draw horse2
		// currIndex = 4;
		// Model->pushMatrix();
		// 	// horse_x = -2.3;
		// 	// horse_z = 3.1;
		// 	// horse_y = getHeightW(horse_x, horse_z)+0.08;
		// 	// Model->translate(vec3(-2.8,-0.8,3.5));
		// 	Model->translate(fixedPoint);
		// 	Model->scale(vec3(0.015, 0.015, 0.015));
		// 	scaleToOrigin(Model, currIndex);
		// 	setModel(texProg, Model);
		// 	// setAndDrawModel(texProg, Model, currIndex);
		// Model->popMatrix();
		// Draw grass
		currIndex = 3;
		texture3->bind(texProg->getUniform("Texture0"));
		srand(rseed); // Set random seed, so each render generates same values
		vector<vec2> clusters = {vec2(-1.52, 3), vec2(-2.03, 2.38), vec2(-1.58, 2.5), vec2(-1.5, 2.64)}; 
		vector<int> sizes = {3, 2, 2, 2};
		for (int c = 0; c < clusters.size(); c++) {
			for (int i = 0; i < sizes[c]; i++) {
				for (int j = 0; j < sizes[c]; j++) {
					Model->pushMatrix();	
						float r1 = ((float) (rand() % 100))/100 - 0.5; 
						float r2 = ((float) (rand() % 100))/100 - 0.5; 
						float x = clusters[c].x + (i - sizes[c]/2 + r1) * 0.08; 
						float z = clusters[c].y + (j - sizes[c]/2 + r2) * 0.08;
						Model->translate(vec3(x, getHeightW(x, z) + 0.06, z));
						Model->scale(vec3(0.01, 0.01, 0.01));
						scaleToOrigin(Model, currIndex);
						setAndDrawModel(texProg, Model, currIndex);
					Model->popMatrix();
				}
			}
		}
		texProg->unbind();

		drawGround(texProg);

		// Pop matrix stacks.
		Projection->popMatrix();

		gallopHeight = sin(glfwGetTime()*3)*0.005;
		gallopAngle = sin(glfwGetTime()*3)*5; // angle: 0-5 deg
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
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
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
