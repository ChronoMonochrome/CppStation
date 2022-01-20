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


	bus.mGpu.mRenderer.drawTriangle();
	bus.mGpu.mRenderer.mWindow.swapBuffers();

	do {
		bus.mCpu.runNextInstruction();
	
		//bus.mGpu.mRenderer.mWindow.swapBuffers();
		glfwPollEvents();
	} while(!bus.mGpu.mRenderer.mWindow.shouldClose());

	return 0;
}
