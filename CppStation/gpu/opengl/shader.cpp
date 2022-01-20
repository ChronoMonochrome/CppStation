#include <gpu/opengl/shader.hpp>

// adapted from https://github.com/codingminecraft/MinecraftCloneForYoutube/blob/master/MinecraftYoutube/src/renderer/Shader.cpp
namespace gpu {
namespace opengl {
namespace shader {

bool Shader::compile(ShaderType type, const char* shaderFilepath)
{
	// Copy the shader filepath into a string
	mFilePath = std::string(shaderFilepath);

	println("Compiling shader: {}", mFilePath.c_str());

	// Read the shader source code from the file
	std::string shaderSourceCode = readFile(mFilePath.c_str());

	GLenum shaderType = toGlShaderType(type);
	if (shaderType == GL_INVALID_ENUM)
	{
		println("ShaderType is unknown");
		return false;
	}

	// Create an empty shader handle
	mShaderId = glCreateShader(shaderType);

	// Send the shader source code to GL
	const GLchar* sourceCStr = shaderSourceCode.c_str();
	glShaderSource(mShaderId, 1, &sourceCStr, 0);

	// Compile the shader
	glCompileShader(mShaderId);

	// Check if the compilation succeeded
	GLint isCompiled = 0;
	glGetShaderiv(mShaderId, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		// If compilation failed, find out why and log the error
		GLint maxLength = 0;
		glGetShaderiv(mShaderId, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(mShaderId, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore if compilation failed
		glDeleteShader(mShaderId);

		println("Shader Compilation failed: {}", infoLog.data());

		mShaderId = UINT32_MAX;
		return false;
	}

	return true;
}

void Shader::destroy()
{
	if (mShaderId != UINT32_MAX)
	{
		glDeleteShader(mShaderId);
		mShaderId = UINT32_MAX;
	}
}


GLenum Shader::toGlShaderType(ShaderType type)
{
	switch (type)
	{
	case ShaderType::Vertex:
		return GL_VERTEX_SHADER;
	case ShaderType::Fragment:
		return GL_FRAGMENT_SHADER;
	}
	return GL_INVALID_ENUM;
}

} // namespace shader
} // namespace opengl
} // namespace gpu