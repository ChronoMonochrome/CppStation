#pragma once

#include <gpu/opengl/core.hpp>

#include "helpers.hpp"

namespace gpu {
namespace opengl {
namespace shader {

enum class ShaderType
{
	Vertex,
	Fragment,
};

struct Shader
{
	std::string mFilePath;
	uint32_t mShaderId;
	ShaderType mType;

	bool compile(ShaderType type, const char* shaderFilepath);
	void destroy();

	static GLenum toGlShaderType(ShaderType type);
};

} // namespace shader
} // namespace opengl
} // namespace gpu