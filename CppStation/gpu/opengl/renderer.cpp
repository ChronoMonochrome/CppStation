#include <gpu/opengl/renderer.hpp>
#include <gpu/opengl/shaderProgram.hpp>

#include "helpers.hpp"

using namespace gpu::opengl::shaderProgram;

namespace gpu {
namespace opengl {
namespace renderer {


static const int windowWidth = 1024;
static const int windowHeight = 512;
static const char* windowTitle = "OpenGL Template";
static gpu::opengl::shaderProgram::ShaderProgram basicShader;

Renderer::Renderer() :
	mVerticesNum(0)
{
}

Renderer::~Renderer()
{
	basicShader.destroy();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Cleanup VBO
	glDeleteBuffers(1, &mVbo);
	glDeleteVertexArrays(1, &mVao);

	glfwTerminate();
}

void Renderer::init()
{
	glfwInit();

	mWindow.init(windowWidth, windowHeight, windowTitle, false);
	mWindow.installMainCallbacks();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		panic("Failed to initialize GLAD");
	}

	glfwMakeContextCurrent(mWindow.mNativeWindow);
	glViewport(0, 0, windowWidth, windowHeight);

	// Create and compile our GLSL program from the shaders
	if (!basicShader.compileAndLink("assets/shaders/vertex/vertex.glsl", "assets/shaders/fragment/fragment.glsl"))
	{
		basicShader.destroy();
		println("Failed to compile the shader program, exiting early.");
	}

	// Use our shader
	basicShader.bind();

	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);

	glGenBuffers(1, &mVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mVertexBuf), mVertexBuf, GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(
		0,                              // attribute 0. No particular reason for 0, but must match the layout in the shader.
		2,                              // size
		GL_SHORT,                       // type
		sizeof(Vertex),                 // stride
		(void*)offsetof(Vertex, pos)    // array buffer offset
	);

	// 2nd attribute buffer : color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                              // attribute 1. No particular reason for 1, but must match the layout in the shader.
		3,                              // size
		GL_FLOAT,                       // type
		GL_FALSE,                       // normalized
		sizeof(Vertex),                 // stride
		(void*)offsetof(Vertex, color)  // array buffer offset
	);
}

void Renderer::pushVertex(Vertex v)
{
	if (mVerticesNum + 1 > VERTEX_BUFFER_LEN)
	{
		println("Vertex attribute buffers full, forcing draw");
		draw();
	}

    mVertexBuf[mVerticesNum] = v;
    mVerticesNum++;
}

void Renderer::pushTriangle(Vertex v1, Vertex v2, Vertex v3)
{
	if (mVerticesNum + 3 > VERTEX_BUFFER_LEN)
	{
		println("Vertex attribute buffers full, forcing draw");
		draw();
	}

	pushVertex(v1);
	pushVertex(v2);
	pushVertex(v3);
}

void Renderer::draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(Vertex) * mVerticesNum,
                    mVertexBuf);
	glDrawArraysInstanced(GL_TRIANGLES, 0, (GLsizei) mVerticesNum, 1);

	// Reset the buffers
	mVerticesNum = 0;
}

void Renderer::display()
{
	draw();

	mWindow.swapBuffers();
	glfwPollEvents();
}

} // namespace gpu
} // namespace opengl
} // namespace renderer
