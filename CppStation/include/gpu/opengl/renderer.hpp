#pragma once

#include <thread>

#include <gpu/opengl/core.hpp>
#include <gpu/opengl/window.hpp>
#include <ui/input.hpp>

using namespace gpu::opengl::window;

namespace gpu {
namespace opengl {
namespace renderer {

class Renderer
{
public:
	Renderer();
	~Renderer();

	Window mWindow;

	void init();
};

} // namespace gpu
} // namespace opengl
} // namespace renderer