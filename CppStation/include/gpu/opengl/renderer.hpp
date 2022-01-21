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
static const uint32_t VERTEX_BUFFER_LEN = 64 * 1024;

struct V2f
{
	GLfloat x;
	GLfloat y;
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	Window mWindow;
	GLfloat mVertexBuf[VERTEX_BUFFER_LEN];
	uint32_t mVerticesNum;
	GLuint mVao;
	GLuint mVbo;

	void init();
	void pushVertex(V2f pos);
	void pushTriangle(V2f p1, V2f p2, V2f p3);
	void draw();
	void display();
};

} // namespace gpu
} // namespace opengl
} // namespace renderer