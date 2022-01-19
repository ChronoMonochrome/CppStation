#pragma once

#include <gpu/opengl/core.hpp>

namespace gpu {
namespace opengl {
namespace window {

class Window
{
public:
	Window(int width, int height, const char* title, bool fullScreenMode);
	~Window();

	GLFWwindow * mNativeWindow;

	int mWidth;
	int mHeight;

	[[nodiscard]] bool shouldClose() const;

	void installMainCallbacks();

	void close();

	void swapBuffers();
};

} // namespace gpu
} // namespace opengl
} // namespace window