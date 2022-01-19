#pragma once

#include <gpu/opengl/renderer.hpp>

#include "helpers.hpp"

namespace gpu {
namespace opengl {
namespace renderer {


static const int windowWidth = 800;
static const int windowHeight = 640;
static const char* windowTitle = "OpenGL Template";

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	mWindow.swapBuffers();
}

} // namespace gpu
} // namespace opengl
} // namespace renderer