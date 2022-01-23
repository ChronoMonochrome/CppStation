#include <iostream>

#include <memory/bios.hpp>
#include <cpu/cpu.hpp>
#include <bus.hpp>

#include <gpu/opengl/core.hpp>
#include <gpu/opengl/shaderProgram.hpp>

#include "helpers.hpp"
#include "backtrace.hpp"

using gpu::opengl::renderer::Vertex;
using gpu::opengl::renderer::Position;
using gpu::opengl::renderer::Color;

int main()
{
	setupSigAct();

	bus::Bus bus;
	bool shouldClose = false;

	do {
		for (int i = 0; i < 1000000; i++) 
		{
			bus.mCpu.runNextInstruction();
		}
		glfwPollEvents();
		shouldClose = bus.mGpu.mRenderer.mWindow.shouldClose();
	} while(!shouldClose);

	return 0;
}
