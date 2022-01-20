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
GLuint VertexArrayID;
GLuint vertexbuffer;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	basicShader.destroy();

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
}

void Renderer::drawTriangle()
{
	const GLfloat vertexBufferData[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

	glDisableVertexAttribArray(0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
}

} // namespace gpu
} // namespace opengl
} // namespace renderer