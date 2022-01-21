#include <iostream>

#include <memory/bios.hpp>
#include <cpu/cpu.hpp>
#include <bus.hpp>

#include <gpu/opengl/core.hpp>
#include <gpu/opengl/shaderProgram.hpp>

#include "helpers.hpp"
#include "backtrace.hpp"

int main()
{
	setupSigAct();

	bus::Bus bus;

	bus.mGpu.mRenderer.pushTriangle({-1.0f, -1.0f}, {1.0f, -1.0f}, {0.0f, 1.0f});
	bus.mGpu.mRenderer.display();

	do {
		bus.mCpu.runNextInstruction();

		glfwPollEvents();
	} while(!bus.mGpu.mRenderer.mWindow.shouldClose());

	return 0;
}
