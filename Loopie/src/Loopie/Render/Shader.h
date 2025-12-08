#pragma once
#include "Loopie/Math/MathTypes.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <variant>

typedef unsigned int GLuint;
typedef int GLint;
typedef unsigned int GLenum;

// Things left to work on for this module: 
// - The shader file may have more than the vertex, fragment and geometry information. 
//   If others are added, that information should be able to be parsed and compiled as well.
// - Verify if we actually need to cache the attributes, version and uniforms.
// - Verify if we need array support for uniformTypes.

namespace Loopie {

	enum UniformType
	{
		UniformType_float,
		UniformType_int,
		UniformType_uint,
		UniformType_bool,
		UniformType_vec2,
		UniformType_vec3,
		UniformType_vec4,
		UniformType_mat2,
		UniformType_mat3,
		UniformType_mat4,
		UniformType_Sampler2D, // treated as int
		UniformType_Sampler3D, // treated as int
		UniformType_SamplerCube, // treated as int?

		UniformType_Unknown
	};

	struct Uniform
	{
		std::string id;
		UniformType type;
		std::variant<int, float, bool, unsigned int, vec2, vec3, vec4, matrix2, matrix3, matrix4> defaultValue;
	};

	class Shader
	{
	public:
		// Shaders might need geometry information, which would be included in the constructor & Reload
		Shader(const char* sourcePath);
		~Shader();

		void Bind() const;
		void Unbind() const;
		// Constructor and Reload do similar things, might be a good thing to 
		// put them together in a helper function 
		bool Reload(const char* sourcePath = nullptr); // Recompiles from the source path currently set

		// Add more if needed: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glUniform.xhtml
		void SetUniformInt(const std::string& name, int value);
		void SetUniformFloat(const std::string& name, float value);
		void SetUniformMat2(const std::string& name, const Loopie::matrix2& matrix);
		void SetUniformMat3(const std::string& name, const Loopie::matrix3& matrix);
		void SetUniformMat4(const std::string& name, const Loopie::matrix4& matrix);
		void SetUniformVec2(const std::string& name, const Loopie::vec2& vector);
		void SetUniformVec3(const std::string& name, const Loopie::vec3& vector);
		void SetUniformVec4(const std::string& name, const Loopie::vec4& vector);

		// Getters
		GLuint GetProgramID() const;
		GLint GetUniformLocation(const std::string& name);
		bool GetIsValidShader() const;
		bool GetIsBound() const;
		// Vertex and Fragment shader version be different, although they will generally match.
		// https://stackoverflow.com/questions/30943346/should-vertex-and-fragment-shader-versions-always-match
		const std::string& GetShaderVersion() const;
		const std::string& GetVertexSource() const;
		const std::string& GetFragmentSource() const;
		const std::string& GetGeometrySource() const;
		const std::string& GetFilePath() const;
		const std::vector<Uniform>& GetUniforms() const;

		// Setters
		void SetPath(const std::string& path);

	private:
		// Helper function to avoid redundant code
		bool ParseCompileLinkShader(const char* sourcePath, GLuint& programID);
		GLuint CompileShader(GLenum shaderType, const char* sourcePath);

		// OpenGL is silent about shaders failing, which is why I created this function
		bool CheckCompileErrors(GLuint shader, const std::string& type);

		// Parse GLSL's version of shader string file
		std::string ParseGLSLVersion(const std::string& source);
		bool ParseShaderSourcePath(const std::string& filePath);
		void GetUniformsGL();
		bool GetUniformDefaultValue(Uniform& uniform);
		//void ExtractUniforms(const std::string& parsedShader, const std::unordered_map<std::string, 
							 //UniformType>& typeMap, const std::regex& uniformRegex);
		bool CheckIfShaderIsBoundAndWarn();



	private:
		GLuint m_rendererID = 0;
		std::vector<Uniform> m_uniforms;
		std::unordered_map<std::string, GLint> m_uniformLocationCache;
		mutable bool m_uniformsCached = false;
		mutable bool m_attributesCached = false;

		std::string m_filePath;
		std::string m_shaderVersion; 

		// Keeping sources in case we might want to inspect them later 
		// delete them once we find everything works as expected
		std::string m_vertexSource;
		std::string m_fragmentSource;
		std::string m_geometrySource;

		bool m_isValidShader = true;
	};
}