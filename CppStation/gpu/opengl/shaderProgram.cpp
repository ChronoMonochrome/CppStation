#include <unordered_set>

#include <gpu/opengl/shaderProgram.hpp>
#include <gpu/opengl/shader.hpp>

using namespace gpu::opengl::shader;

// This code was adapted from https://github.com/codingminecraft/MinecraftCloneForYoutube/blob/master/MinecraftYoutube/src/renderer/ShaderProgram.cpp
namespace gpu {
namespace opengl {
namespace shaderProgram {

bool ShaderProgram::compileAndLink(const char* vertexShaderFile, const char* fragmentShaderFile)
{
	// Create the shader program
	GLuint program = glCreateProgram();

	Shader vertexShader;
	if (!vertexShader.compile(ShaderType::Vertex, vertexShaderFile))
	{
		vertexShader.destroy();
		println("Failed to compile vertex shader.");
		return false;
	}

	Shader fragmentShader;
	if (!fragmentShader.compile(ShaderType::Fragment, fragmentShaderFile))
	{
		fragmentShader.destroy();
		println("Failed to compile fragment shader.");
		return false;
	}

	// Attach the vertex/fragment shaders and try to link them together
	glAttachShader(program, vertexShader.mShaderId);
	glAttachShader(program, fragmentShader.mShaderId);

	// Try to link our program
	glLinkProgram(program);

	// Log errors if the linking failed
	GLint isLinked = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore if linking failed
		glDeleteProgram(program);
		vertexShader.destroy();
		fragmentShader.destroy();

		println("Shader linking failed:\n{}", infoLog.data());
		mProgramId = UINT32_MAX;
		return false;
	}

	// Always detach shaders after a successful link and destroy them since we don't need them anymore
	glDetachShader(program, vertexShader.mShaderId);
	glDetachShader(program, fragmentShader.mShaderId);
	vertexShader.destroy();
	fragmentShader.destroy();

	// If linking succeeded, get all the active uniforms and store them in our map of uniform variable locations
	int numUniforms;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

	int maxCharLength;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxCharLength);
	if (numUniforms > 0 && maxCharLength > 0)
	{
		std::string charBuffer;
		charBuffer.resize(maxCharLength);

		for (int i = 0; i < numUniforms; i++)
		{
			int length, size;
			GLenum dataType;
			glGetActiveUniform(program, i, maxCharLength, &length, &size, &dataType, &charBuffer[0]);
			GLint varLocation = glGetUniformLocation(program, &charBuffer[0]);
		}
	}

	mProgramId = program;
	println("Shader compilation and linking succeeded <Vertex:{}>:<Fragment:{}>", vertexShaderFile, fragmentShaderFile);
	return true;
}

void ShaderProgram::destroy()
{
	if (mProgramId != UINT32_MAX)
	{
		glDeleteProgram(mProgramId);
		mProgramId = UINT32_MAX;
	}
}

void ShaderProgram::bind() const
{
	glUseProgram(mProgramId);
}

void ShaderProgram::unbind() const
{
	glUseProgram(0);
}

} // namespace shaderProgram
} // namespace opengl
} // namespace gpu