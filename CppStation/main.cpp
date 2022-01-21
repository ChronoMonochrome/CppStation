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


	do {
		bus.mCpu.runNextInstruction();

		glfwPollEvents();
	} while(!bus.mGpu.mRenderer.mWindow.shouldClose());

	return 0;
}
