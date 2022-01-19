#include <iostream>
#include <thread>

#include <memory/bios.hpp>
#include <cpu/cpu.hpp>
#include <bus.hpp>

#include <gpu/opengl/core.hpp>
#include <gpu/opengl/window.hpp>

#include "helpers.hpp"
#include "backtrace.hpp"

static const int windowWidth = 800;
static const int windowHeight = 640;
static const char* windowTitle = "OpenGL Template";

static bool shouldClose = false;

using namespace gpu::opengl::window;

void renderLoop()
{
	Window window(windowWidth, windowHeight, windowTitle, false);
	window.installMainCallbacks();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		panic("Failed to initialize GLAD");
	}

	glfwMakeContextCurrent(window.mNativeWindow);
	glViewport(0, 0, windowWidth, windowHeight);

	while (!window.shouldClose())
	{
		glClearColor(0.0f / 255.0f, 220.0f / 255.0f, 220.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		window.swapBuffers();
		glfwPollEvents();
	}

	shouldClose = true;
}

int main()
{
	setupSigAct();

	glfwInit();

	std::thread t1(renderLoop);

	bus::Bus bus;
	while (!shouldClose)
	{
		bus.mCpu.runNextInstruction();
	}

	t1.join();
	glfwTerminate();

	return 0;
}
