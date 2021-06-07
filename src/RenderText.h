#pragma  once

#include <iostream>
#include <memory>
#include <glad/glad.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Program.h"
#include <ft2build.h>
#include FT_FREETYPE_H  


using namespace std;

class RenderText
{
	struct Character {
	    GLuint     TextureID;  // ID handle of the glyph texture
	    glm::ivec2 Size;       // Size of glyph
	    glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	    GLuint     Advance;    // Offset to advance to next glyph
	};

	public:
		RenderText(FT_Library *ft, const shared_ptr<Program> prog);
		~RenderText() {}
		void init();
		void drawText(int alignment, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
		void drawText(int alignment, std::string text, GLfloat x, GLfloat y, GLfloat xscale, GLfloat yscale, glm::vec3 color);
	private:
		GLuint VAO, VBO;
		std::map<GLchar, Character> Characters;
		shared_ptr<Program> prog;
};