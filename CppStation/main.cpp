#include <iostream>

#include <memory/bios.hpp>
#include <cpu/cpu.hpp>
#include <bus.hpp>

#include <gpu/opengl/core.hpp>
#include <gpu/opengl/shaderProgram.hpp>

#include "helpers.hpp"
#include "backtrace.hpp"

using gpu::opengl::renderer::Color;

int main()
{
	setupSigAct();

	bus::Bus bus;

	Color red = {int8_t(255), int8_t(0), int8_t(0)};

	bus.mGpu.mRenderer.pushTriangle({{512, 512}, red}, {{256, 256}, red}, {{0, 512}, red});

	bus.mGpu.mRenderer.display();

	do {
		bus.mCpu.runNextInstruction();

		glfwPollEvents();
	} while(!bus.mGpu.mRenderer.mWindow.shouldClose());

	return 0;
}
