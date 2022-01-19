#include <iostream>

#include <memory/bios.hpp>
#include <cpu/cpu.hpp>
#include <bus.hpp>

#include <gpu/opengl/core.hpp>

#include "helpers.hpp"
#include "backtrace.hpp"

int main()
{
	setupSigAct();

	bus::Bus bus;

	while (!bus.mGpu.mRenderer.mWindow.shouldClose())
	{
		bus.mCpu.runNextInstruction();
		glfwPollEvents();
	}

	return 0;
}
