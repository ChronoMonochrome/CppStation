#include <gpu/opengl/window.hpp>
#include <ui/input.hpp>

#include "helpers.hpp"

namespace gpu {
namespace opengl {
namespace window {

Window::Window()
{
}

Window::~Window()
{
	if (!mNativeWindow)
		return;

	glfwDestroyWindow(mNativeWindow);
}

void Window::init(int width, int height, const char* title, bool fullScreenMode)
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

	makeCurrent();
	glfwSetWindowUserPointer(mNativeWindow, (void*)this);
}

bool Window::shouldClose() const
{
	if (!mNativeWindow)
		return true;

	return glfwWindowShouldClose(mNativeWindow);
}

void resizeCallback(GLFWwindow* nativeWindow, int newWidth, int newHeight)
{
	Window* window = (Window*)glfwGetWindowUserPointer(nativeWindow);
	window->mWidth = newWidth;
	window->mHeight = newHeight;
	glViewport(0, 0, newWidth, newHeight);
}

void Window::installMainCallbacks()
{
	if (!mNativeWindow)
		return;

	glfwSetKeyCallback(mNativeWindow, ui::input::keyCallback);
	glfwSetCursorPosCallback(mNativeWindow, ui::input::mouseCallback);
	glfwSetMouseButtonCallback(mNativeWindow, ui::input::mouseButtonCallback);
	glfwSetWindowSizeCallback(mNativeWindow, resizeCallback);
}

void Window::close()
{
	if (!mNativeWindow)
		return;

	glfwSetWindowShouldClose(mNativeWindow, GLFW_TRUE);
}

void Window::makeCurrent()
{
	glfwMakeContextCurrent(mNativeWindow);
}

void Window::swapBuffers()
{
	if (!mNativeWindow)
		return;

	glfwSwapBuffers(mNativeWindow);
}

} // namespace gpu
} // namespace opengl
} // namespace window