#pragma once

#include <gpu/opengl/core.hpp>

namespace gpu {
namespace opengl {
namespace window {

class Window
{
public:
	Window();
	~Window();

	GLFWwindow * mNativeWindow;

	int mWidth;
	int mHeight;

	void init(int width, int height, const char* title, bool fullScreenMode);
	[[nodiscard]] bool shouldClose() const;

	void installMainCallbacks();
	void close();
	void makeCurrent();
	void swapBuffers();
};

} // namespace gpu
} // namespace opengl
} // namespace window