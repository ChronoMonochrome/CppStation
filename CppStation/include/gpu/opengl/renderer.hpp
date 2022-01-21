#pragma once

#include <thread>

#include <gpu/opengl/core.hpp>
#include <gpu/opengl/window.hpp>
#include <ui/input.hpp>

using namespace gpu::opengl::window;

namespace gpu {
namespace opengl {
namespace renderer {

// Maximum number of vertex that can be stored in an attribute buffers
static const uint32_t VERTEX_BUFFER_LEN = 16 * 1024;

struct Position
{
	GLfloat x;
	GLfloat y;
};

struct Color
{
	GLfloat r;
	GLfloat g;
	GLfloat b;
};

struct Vertex
{
	Position pos;
	Color color;
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	Window mWindow;
	Vertex mVertexBuf[VERTEX_BUFFER_LEN];
	uint32_t mVerticesNum;
	GLuint mVao;
	GLuint mVbo;

	void init();
	void pushVertex(Vertex v);
	void pushTriangle(Vertex v1, Vertex v2, Vertex v3);
	void draw();
	void display();
};

} // namespace gpu
} // namespace opengl
} // namespace renderer