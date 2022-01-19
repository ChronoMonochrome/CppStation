#pragma once

#include <ui/core.h>

#include <memory>

namespace CppStation
{
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
}
