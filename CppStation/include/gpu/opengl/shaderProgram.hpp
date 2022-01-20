#include <gpu/opengl/core.hpp>

namespace gpu {
namespace opengl {
namespace shaderProgram {

struct ShaderProgram
{
	uint32_t mProgramId;

	bool compileAndLink(const char* vertexShaderFile, const char* fragmentShaderFile);
	void bind() const;
	void unbind() const;
	void destroy();
};

} // namespace shaderProgram
} // namespace opengl
} // namespace gpu