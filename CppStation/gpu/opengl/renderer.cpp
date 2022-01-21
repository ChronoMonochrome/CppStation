#pragma once

#include <gpu/opengl/renderer.hpp>
#include <gpu/opengl/shaderProgram.hpp>

#include "helpers.hpp"

using namespace gpu::opengl::shaderProgram;

namespace gpu {
namespace opengl {
namespace renderer {


static const int windowWidth = 800;
static const int windowHeight = 640;
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
	if (!basicShader.compileAndLink("assets/shaders/vertex/SimpleVertexShader.vertexshader", "assets/shaders/fragment/SimpleFragmentShader.fragmentshader"))
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
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glVertexAttribPointer(
		0,                              // attribute 0. No particular reason for 0, but must match the layout in the shader.
		2,                              // size
		GL_FLOAT,                       // type
		GL_FALSE,                       // normalized?
		0,                              // stride
		(void*)0                        // array buffer offset
	);
}

void Renderer::pushVertex(V2f pos)
{
	if (mVerticesNum + 1 > VERTEX_BUFFER_LEN)
	{
		println("Vertex attribute buffers full, forcing draw");
		draw();
	}

    mVertexBuf[mVerticesNum * 2]     = pos.x;
    mVertexBuf[mVerticesNum * 2 + 1] = pos.y;
    mVerticesNum++;
}

void Renderer::pushTriangle(V2f p1, V2f p2, V2f p3)
{
	if (mVerticesNum + 3 > VERTEX_BUFFER_LEN)
	{
		println("Vertex attribute buffers full, forcing draw");
		draw();
	}

	pushVertex(p1);
	pushVertex(p2);
	pushVertex(p3);
}

void Renderer::draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    mVerticesNum * 2 * sizeof(GLfloat),
                    mVertexBuf);
	glDrawArraysInstanced(GL_TRIANGLES, 0, (GLsizei) mVerticesNum, 1);

	// Reset the buffers
	mVerticesNum = 0;
}

void Renderer::display()
{
	draw();

	mWindow.swapBuffers();
}

} // namespace gpu
} // namespace opengl
} // namespace renderer