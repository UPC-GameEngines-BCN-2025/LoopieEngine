#include "Shader.h"
#include "Loopie/Core/Log.h"

#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <cstddef>

namespace Loopie {
	// If shader compilation/linking fails, GetIsValidShader() will return false.
	// The object is still valid but the shader program may not be usable.
	// SourcePath should have first [vertex] and then [fragment]
	Shader::Shader(const char* sourcePath)
	{
		if (!ParseCompileLinkShader(sourcePath, m_rendererID))
		{
			m_rendererID = 0;
			return;
		}
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_rendererID);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_rendererID);
	}

	void Shader::Unbind() const
	{	
		glUseProgram(0);
	}

	bool Shader::Reload(const char* sourcePath)
	{
		GLuint newProgram = 0;
		if (!ParseCompileLinkShader(sourcePath, newProgram))
		{
			Loopie::Log::Critical("Shader reload failed. Keeping old shader.");
			return false;
		}

		// Delete old program and swap them
		glDeleteProgram(m_rendererID);
		m_rendererID = newProgram;

		// Clear cache since uniform locations may have changed
		m_uniformLocationCache.clear();
		m_uniformsCached = false;
		m_attributesCached = false;

		return true;
	}

	void Shader::SetUniformInt(const std::string& name, int value)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniform1i(location, value);
	}

	void Shader::SetUniformFloat(const std::string& name, float value)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniform1f(location, value);
	}

	void Shader::SetUniformMat2(const std::string& name, const Loopie::matrix2& matrix)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniformMatrix2fv(location, 1, GL_FALSE, &matrix[0][0]);

	}

	void Shader::SetUniformMat3(const std::string& name, const Loopie::matrix3& matrix)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniformMatrix3fv(location, 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::SetUniformMat4(const std::string& name, const matrix4& matrix)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::SetUniformVec2(const std::string& name, const Loopie::vec2& vector)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniform2fv(location, 1, &vector[0]);
	}

	void Shader::SetUniformVec3(const std::string& name, const vec3& vector)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniform3fv(location, 1, &vector[0]);
	}

	void Shader::SetUniformVec4(const std::string& name, const Loopie::vec4& vector)
	{
		if (!CheckIfShaderIsBoundAndWarn()) return;
		GLint location = GetUniformLocation(name);
		if (location == -1)
		{
			Log::Warn("Uniform '{0}' not found in shader.", name);
			return;
		}
		glUniform4fv(location, 1, &vector[0]);
	}

	const std::string& Shader::GetShaderVersion() const
	{
		return m_shaderVersion;
	}

	GLuint Shader::GetProgramID() const
	{
		return m_rendererID;
	}

	GLint Shader::GetUniformLocation(const std::string& name)
	{
		if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
		{
			return m_uniformLocationCache[name];
		}

		GLint location = glGetUniformLocation(m_rendererID, name.c_str());
		m_uniformLocationCache[name] = location;
		return location;
	}

	bool Shader::GetIsValidShader() const
	{
		return m_isValidShader;
	}

	const std::string& Shader::GetVertexSource() const
	{ 
		return m_vertexSource; 
	}

	const std::string& Shader::GetFragmentSource() const
	{ 
		return m_fragmentSource;
	}

	const std::string& Shader::GetGeometrySource() const
	{
		return m_geometrySource;
	}

	const std::string& Shader::GetFilePath() const
	{ 
		return m_filePath; 
	}

	bool Shader::GetIsBound() const
	{
		GLint currentProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
		return currentProgram == static_cast<GLint>(m_rendererID);
	}

	const std::vector<Uniform>& Shader::GetUniforms() const
	{
		return m_uniforms;
	}

	void Shader::SetPath(const std::string& path)
	{
		m_filePath = path;
	}

	GLuint Shader::CompileShader(GLenum shaderType, const char* source)
	{
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &source, NULL);
		glCompileShader(shader);
		return shader;
	}

	bool Shader::ParseCompileLinkShader(const char* sourcePath, GLuint& programID)
	{
		if (sourcePath != nullptr && sourcePath[0] != '\0')
		{
			m_filePath = sourcePath;
		}

		if (!ParseShaderSourcePath(m_filePath))
		{
			Log::Critical("Shader file parsing failed. Aborting shader compilation.");
			return false;
		}

		m_shaderVersion = ParseGLSLVersion(m_vertexSource);

		GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, m_vertexSource.c_str());
		GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, m_fragmentSource.c_str());
		GLuint geometryShader = 0;

		if (!CheckCompileErrors(vertexShader, "VERTEX"))
		{
			Log::Critical("Vertex shader compilation failed. Aborting program link.");
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return false;
		}

		if (!CheckCompileErrors(fragmentShader, "FRAGMENT"))
		{
			Log::Critical("Fragment shader compilation failed. Aborting program link.");
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			return false;
		}

		// Create new program
		GLuint newProgram = glCreateProgram();
		glAttachShader(newProgram, vertexShader);
		glAttachShader(newProgram, fragmentShader);

		if (!m_geometrySource.empty())
		{
			geometryShader = CompileShader(GL_GEOMETRY_SHADER, m_geometrySource.c_str());
			if (!CheckCompileErrors(geometryShader, "GEOMETRY"))
			{
				Loopie::Log::Critical("Geometry shader compilation failed. Aborting program link.");
				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);
				glDeleteShader(geometryShader);
				glDeleteProgram(newProgram);
				return false;
			}
			glAttachShader(newProgram, geometryShader);
		}

		glLinkProgram(newProgram);

		if (!CheckCompileErrors(newProgram, "PROGRAM"))
		{
			Loopie::Log::Critical("Shader linking failed.");
			glDeleteProgram(newProgram);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			if (geometryShader)
			{
				glDeleteShader(geometryShader);
			}
			return false;
		}

		// Clean up shader objects (they're now in the program)
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		if (geometryShader)
		{
			glDeleteShader(geometryShader);
		}

		// Return the new program ID via output parameter
		programID = newProgram;
		GetUniformsGL();

		return true;
	}

	bool Shader::CheckCompileErrors(GLuint shader, const std::string& type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				Loopie::Log::Error("ERROR - SHADER_COMPILATION_ERROR of type {0}:\n{1}", type, infoLog);
				return false;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				Loopie::Log::Error("ERROR - PROGRAM_LINKING_ERROR of type {0}:\n{1}", type, infoLog);
				return false;
			}
		}
		return true;
	}

	std::string Shader::ParseGLSLVersion(const std::string& source)
	{
		// Regex for: "#version 460 core" or "#version 450"
		std::regex versionRE(R"(#\s*version\s+([0-9]+)(?:\s+(\w+))?)");
		std::smatch match;
		if (std::regex_search(source, match, versionRE))
		{
			std::string version = match[1].str();
			if (match.size() > 2 && match[2].matched)
				version += " " + match[2].str();
			return version;
		}
		return "unknown";
	}

	bool Shader::ParseShaderSourcePath(const std::string& filePath)
	{
		// Read the entire file
		std::ifstream file(filePath);
		if (!file.is_open())
		{
			Log::Error("ERROR - SHADER PARSER - File not found: {0}", filePath);
			m_isValidShader = false;
			return m_isValidShader;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content = buffer.str();
		file.close();

		// Helper struct
		struct ShaderMarker 
		{
			std::string tag;
			size_t position;
			std::string* destination;
		};

		// Find [vertex], [fragment] and [geometry] markers
		size_t vertexPos = content.find("[vertex]");
		size_t fragmentPos = content.find("[fragment]");
		size_t geometryPos = content.find("[geometry]");

		if (vertexPos == std::string::npos || fragmentPos == std::string::npos)
		{
			Log::Error("ERROR - SHADER PARSER - Could not find [vertex] or [fragment] markers in {0}", filePath);
			m_isValidShader = false;
			return m_isValidShader;
		}

		std::vector<ShaderMarker> markers;

		// Add found markers to the list
		markers.push_back({ "[vertex]", vertexPos, &m_vertexSource });
		markers.push_back({ "[fragment]", fragmentPos, &m_fragmentSource });
		if (geometryPos != std::string::npos)
		{
			markers.push_back({ "[geometry]", geometryPos, &m_geometrySource });
		}

		// Sort markers by position
		std::sort(markers.begin(), markers.end(), 
			[](const ShaderMarker& a, const ShaderMarker& b) 
			{
				return a.position < b.position;
			});

		// Extract shader sources
		for (size_t i = 0; i < markers.size(); ++i)
		{
			size_t start = markers[i].position + markers[i].tag.length();
			size_t end = (i + 1 < markers.size()) ? markers[i + 1].position : content.length();

			*markers[i].destination = content.substr(start, end - start);
		}

		// Trim whitespace
		auto trim = [](std::string& s)
			{
				size_t start = s.find_first_not_of(" \t\n\r");
				if (start == std::string::npos) {
					s.clear();
					return;
				}
				size_t end = s.find_last_not_of(" \t\n\r");
				s = s.substr(start, end - start + 1);
			};

		trim(m_vertexSource);
		trim(m_fragmentSource);
		if (geometryPos != std::string::npos)
		{
			trim(m_geometrySource);
		}

		m_isValidShader = !m_vertexSource.empty() && !m_fragmentSource.empty();

		return m_isValidShader;
	}

	void Shader::GetUniformsGL()
	{
		m_uniforms.clear();

		static const std::unordered_map<GLenum, UniformType> typeMap =
		{
			{GL_FLOAT, UniformType_float},
			{GL_INT, UniformType_int},
			{GL_UNSIGNED_INT, UniformType_uint},
			{GL_BOOL, UniformType_bool},
			{GL_FLOAT_VEC2, UniformType_vec2},
			{GL_FLOAT_VEC3, UniformType_vec3},
			{GL_FLOAT_VEC4, UniformType_vec4},
			{GL_FLOAT_MAT2, UniformType_mat2},
			{GL_FLOAT_MAT3, UniformType_mat3},
			{GL_FLOAT_MAT4, UniformType_mat4},
			{GL_SAMPLER_2D, UniformType_Sampler2D}, 
			{GL_SAMPLER_3D, UniformType_Sampler3D},
			{GL_SAMPLER_CUBE, UniformType_SamplerCube},
		};

		GLint numUniforms = 0;
		glGetProgramiv(m_rendererID, GL_ACTIVE_UNIFORMS, &numUniforms);

		GLint maxNameLength = 0;
		glGetProgramiv(m_rendererID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
		std::vector<char> nameBuffer(maxNameLength);

		for (GLint i = 0; i < numUniforms; ++i) 
		{
			GLsizei length = 0;
			GLint size = 0;
			GLenum type = 0;

			glGetActiveUniform(m_rendererID, i, maxNameLength, &length, &size, &type, nameBuffer.data());
			std::string name(nameBuffer.data(), length);

			// if (name._Starts_with("lp_")) // Not apt for Linux
			if (name.rfind("lp_", 0) == 0)   // C++17 portable
			{
				continue;
			}

			auto typeIt = typeMap.find(type);
			if (typeIt != typeMap.end())
			{
				m_uniforms.push_back(Uniform{ name, typeIt->second });
				GetUniformDefaultValue(m_uniforms.back());
			}
			else
			{
				Log::Warn("Unknown uniform type {0} for uniform '{1}'", type, name);
				m_uniforms.push_back(Uniform{ name, UniformType_Unknown }); 
				m_uniforms.back().defaultValue = 0;
			}
		}
	}

	bool Shader::GetUniformDefaultValue(Uniform& uniform)
	{
		GLint location = GetUniformLocation(uniform.id);
		if (location == -1)
		{
			Log::Warn("Uniform not found: {}", uniform.id);
			return false;
		}

		switch (uniform.type)
		{
		case UniformType_float:
		{
			GLfloat value;
			glGetUniformfv(m_rendererID, location, &value);
			uniform.defaultValue = value;
			break;
		}
		case UniformType_bool:
		{
			GLint value;
			glGetUniformiv(m_rendererID, location, &value);
			uniform.defaultValue = static_cast<bool>(value);
			break;
		}
		case UniformType_int:
		case UniformType_Sampler2D:
		case UniformType_Sampler3D:
		case UniformType_SamplerCube:
		{
			GLint value;
			glGetUniformiv(m_rendererID, location, &value);
			uniform.defaultValue = value;
			break;
		}
		case UniformType_uint:
		{
			GLuint value;
			glGetUniformuiv(m_rendererID, location, &value);
			uniform.defaultValue = value;
			break;
		}
		case UniformType_vec2:
		{
			glm::vec2 value;
			glGetUniformfv(m_rendererID, location, &value[0]);
			uniform.defaultValue = value;
			break;
		}
		case UniformType_vec3:
		{
			glm::vec3 value;
			glGetUniformfv(m_rendererID, location, &value[0]);
			uniform.defaultValue = value;
			break;
		}
		case UniformType_vec4:
		{
			glm::vec4 value;
			glGetUniformfv(m_rendererID, location, &value[0]);
			uniform.defaultValue = value;
			break;
		}
		case UniformType_mat2:
		{
			glm::mat2 value;
			glGetUniformfv(m_rendererID, location, &value[0][0]);
			uniform.defaultValue = value;
			break;
		}
		case UniformType_mat3:
		{
			glm::mat3 value;
			glGetUniformfv(m_rendererID, location, &value[0][0]);
			uniform.defaultValue = value;
			break;
		}
		case UniformType_mat4:
		{
			glm::mat4 value;
			glGetUniformfv(m_rendererID, location, &value[0][0]);
			uniform.defaultValue = value;
			break;
		}
		default:
			Log::Warn("Unknown uniform type in GetUniformDefaultValue: {}", uniform.id);
			return false;
		}

		return true;
	}


	bool Shader::CheckIfShaderIsBoundAndWarn() 
	{
		bool isBound = GetIsBound();
		if (!isBound)
		{
			Log::Warn("Shader is not bound. Cancelling operation.");
		}
		return isBound;
	}
}