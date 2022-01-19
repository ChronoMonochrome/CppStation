#include <ui/Window.h>
#include <ui/Input.h>

#include "helpers.hpp"

namespace CppStation
{
	Window::Window(int width, int height, const char* title, bool fullScreenMode)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Only supply the monitor if we want to start the window in full-screen mode
		auto * primaryMonitor = fullScreenMode ? glfwGetPrimaryMonitor() : nullptr;

		mNativeWindow = glfwCreateWindow(width, height, title, primaryMonitor, nullptr);
		if (mNativeWindow == nullptr)
		{
			println("Failed to create GLFW window");
			glfwTerminate();
			return;
		}

		mWidth = width;
		mHeight = height;

		glfwMakeContextCurrent(mNativeWindow);
	}

	Window::~Window()
	{
		if (!mNativeWindow)
			return;

		glfwDestroyWindow(mNativeWindow);
	}

	bool Window::shouldClose() const
	{
		if (!mNativeWindow)
			return true;

		return glfwWindowShouldClose(mNativeWindow);
	}

	void Window::installMainCallbacks()
	{
		if (!mNativeWindow)
			return;

		glfwSetKeyCallback(mNativeWindow, Input::keyCallback);
		glfwSetCursorPosCallback(mNativeWindow, Input::mouseCallback);
		glfwSetMouseButtonCallback(mNativeWindow, Input::mouseButtonCallback);
	}

	void Window::close()
	{
		if (!mNativeWindow)
			return;

		glfwSetWindowShouldClose(mNativeWindow, GLFW_TRUE);
	}

	void Window::swapBuffers()
	{
		if (!mNativeWindow)
			return;

		glfwSwapBuffers(mNativeWindow);
	}
}
