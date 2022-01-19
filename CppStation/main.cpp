#include <iostream>

#include <memory/bios.hpp>
#include <cpu/cpu.hpp>
#include <bus.hpp>

#include <ui/core.h>
#include <ui/Window.h>
#include <ui/Input.h>

#include "helpers.hpp"
#include "backtrace.hpp"

using namespace CppStation;

const int windowWidth = 800;
const int windowHeight = 640;
const char* windowTitle = "OpenGL Template";

int exitWithError(const char* msg)
{
	printf(msg);
	glfwTerminate();
	return -1;
}

int main()
{
	setupSigAct();
	bus::Bus bus;

	glfwInit();

	Window window(windowWidth, windowHeight, windowTitle, false);
	window.installMainCallbacks();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		println("Failed to initialize GLAD");
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.0f / 255.0f, 220.0f / 255.0f, 220.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	window.swapBuffers();
	glfwPollEvents();

	while (!window.shouldClose())
	{
		bus.mCpu.runNextInstruction();

		if (Input::isKeyDown(GLFW_KEY_E))
		{
			println("E key is being pressed.");
		}

		if (Input::isKeyDown(GLFW_KEY_ESCAPE))
		{
			window.close();
		}

		// window->swapBuffers();
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
