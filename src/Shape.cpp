
#include "Shape.h"
#include <iostream>
#include <cassert>

#include "GLSL.h"
#include "Program.h"

using namespace std;

Shape::Shape(bool textured) :
	eleBufID(0),
	posBufID(0),
	norBufID(0),
	texBufID(0), 
   vaoID(0)
{
	min = glm::vec3(0);
	max = glm::vec3(0);
	texOff = !textured;
}

Shape::~Shape()
{
}

// copy the data from the shape to this object
void Shape::createShape(tinyobj::shape_t & shape)
{
	posBuf = shape.mesh.positions;
	norBuf = shape.mesh.normals;
	texBuf = shape.mesh.texcoords;
	eleBuf = shape.mesh.indices;
}

void Shape::measure()
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;

	minX = minY = minZ = std::numeric_limits<float>::max();
	maxX = maxY = maxZ = -std::numeric_limits<float>::max();

	//Go through all vertices to determine min and max of each dimension
	for (size_t v = 0; v < posBuf.size() / 3; v++)
	{
		if (posBuf[3*v+0] < minX) minX = posBuf[3 * v + 0];
		if (posBuf[3*v+0] > maxX) maxX = posBuf[3 * v + 0];

		if (posBuf[3*v+1] < minY) minY = posBuf[3 * v + 1];
		if (posBuf[3*v+1] > maxY) maxY = posBuf[3 * v + 1];

		if (posBuf[3*v+2] < minZ) minZ = posBuf[3 * v + 2];
		if (posBuf[3*v+2] > maxZ) maxZ = posBuf[3 * v + 2];
	}

	min.x = minX;
	min.y = minY;
	min.z = minZ;
	max.x = maxX;
	max.y = maxY;
	max.z = maxZ;
}

void Shape::init()
{
	// Initialize the vertex array object
	CHECKED_GL_CALL(glGenVertexArrays(1, &vaoID));
	CHECKED_GL_CALL(glBindVertexArray(vaoID));

	// Send the position array to the GPU
	CHECKED_GL_CALL(glGenBuffers(1, &posBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posBufID));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW));

	// If no vertex normals are given, calculate ourselves
	if (norBuf.empty())
	{
		// Allocate normal per vertex
		norBuf = vector<float>(posBuf.size(), 0);

		// For each face, add its face normal to its vertices 
		for (size_t v = 0; v < eleBuf.size() / 3; v++) {
			unsigned int i0 = eleBuf[3*v+0]; // index of vertex 0
			unsigned int i1 = eleBuf[3*v+1]; // index of vertex 1
			unsigned int i2 = eleBuf[3*v+2]; // index of vertex 2

			float e1x = posBuf[3*i1+0] - posBuf[3*i0+0];
			float e1y = posBuf[3*i1+1] - posBuf[3*i0+1];
			float e1z = posBuf[3*i1+2] - posBuf[3*i0+2];

			float e2x = posBuf[3*i2+0] - posBuf[3*i0+0];
			float e2y = posBuf[3*i2+1] - posBuf[3*i0+1];
			float e2z = posBuf[3*i2+2] - posBuf[3*i0+2];

			float nx = e1y*e2z - e1z*e2y;
			float ny = -(e1x*e2z - e1z*e2x);
			float nz = e1x*e2y - e1y*e2x;

			norBuf[3*i0+0] += nx;
			norBuf[3*i1+0] += nx;
			norBuf[3*i2+0] += nx;

			norBuf[3*i0+1] += ny;
			norBuf[3*i1+1] += ny;
			norBuf[3*i2+1] += ny;

			norBuf[3*i0+2] += nz;
			norBuf[3*i1+2] += nz;
			norBuf[3*i2+2] += nz;
		}

		// Normalize each vertex
		for (size_t v = 0; v < norBuf.size() / 3; v++) {
			float magnitude = std::sqrt(std::pow(norBuf[3*v+0], 2) + std::pow(norBuf[3*v+1], 2) + std::pow(norBuf[3*v+2], 2));
			norBuf[3*v+0] /= magnitude;
			norBuf[3*v+1] /= magnitude;
			norBuf[3*v+2] /= magnitude;
		}
	}
	// Send the normal array to the GPU
	CHECKED_GL_CALL(glGenBuffers(1, &norBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, norBufID));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW));
	

	// Send the texture array to the GPU
	if (texBuf.empty() || texOff)
	{
		texBufID = 0;
	}
	else
	{
		CHECKED_GL_CALL(glGenBuffers(1, &texBufID));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, texBufID));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW));
	}

	// Send the element array to the GPU
	CHECKED_GL_CALL(glGenBuffers(1, &eleBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID));
	CHECKED_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW));

	// Unbind the arrays
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Shape::draw(const shared_ptr<Program> prog) const
{
	int h_pos, h_nor, h_tex;
	h_pos = h_nor = h_tex = -1;

	CHECKED_GL_CALL(glBindVertexArray(vaoID));

	// Bind position buffer
	h_pos = prog->getAttribute("vertPos");
	GLSL::enableVertexAttribArray(h_pos);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posBufID));
	CHECKED_GL_CALL(glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));

	// Bind normal buffer
	h_nor = prog->getAttribute("vertNor");
	if (h_nor != -1 && norBufID != 0)
	{
		GLSL::enableVertexAttribArray(h_nor);
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, norBufID));
		CHECKED_GL_CALL(glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));
	}

	if (texBufID != 0)
	{
		// Bind texcoords buffer
		h_tex = prog->getAttribute("vertTex");

		if (h_tex != -1 && texBufID != 0)
		{
			GLSL::enableVertexAttribArray(h_tex);
			CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, texBufID));
			CHECKED_GL_CALL(glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0));
		}
	}

	// Bind element buffer
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID));

	// Draw
	CHECKED_GL_CALL(glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0));

	// Disable and unbind
	if (h_tex != -1)
	{
		GLSL::disableVertexAttribArray(h_tex);
	}
	if (h_nor != -1)
	{
		GLSL::disableVertexAttribArray(h_nor);
	}
	GLSL::disableVertexAttribArray(h_pos);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
