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
	int16_t x;
	int16_t y;

	static struct Position fromPacked(uint32_t val)
	{
		return {int16_t(val), int16_t(val >> 16)};
	}
};

struct Color
{
	GLfloat r;
	GLfloat g;
	GLfloat b;

	static struct Color fromPacked(uint32_t val)
	{
		return {uint8_t(val) / 255.0f, uint8_t(val >> 8) / 255.0f, uint8_t(val >> 16) / 255.0f};
	}
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
	void pushQuad(Vertex v1, Vertex v2, Vertex v3, Vertex v4);
	void draw();
	void display();
};

} // namespace gpu
} // namespace opengl
} // namespace renderer