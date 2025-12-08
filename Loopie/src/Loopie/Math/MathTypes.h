#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp> // Seems like it is experimental - Will cause error if uncommented
#include <glm/gtx/matrix_decompose.hpp>

// Might want to use wrappers / inliners to ensure glm can be swapped if necessary
namespace Loopie
{
	// Vectors
	typedef glm::vec2 vec2;
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;

	// Integer Vectors
	typedef glm::ivec2 ivec2;
	typedef glm::ivec3 ivec3;
	typedef glm::ivec4 ivec4;

    // Unsigned Vectors
    typedef glm::uvec2 uvec2;
    typedef glm::uvec3 uvec3;
    typedef glm::uvec4 uvec4;

	// Matrices
    typedef glm::mat2 matrix2;
	typedef glm::mat3 matrix3;
	typedef glm::mat4 matrix4;

	// Quaternion
	typedef glm::quat quaternion;

    // Vector operations
    using glm::normalize;

    using glm::cross;
    using glm::dot;
    using glm::length;

    // Matrix transforms
    using glm::translate;
    using glm::rotate;
    using glm::scale;
    using glm::decompose;

    // Angles
    using glm::radians;
    using glm::degrees;

    // Camera / projection
    using glm::perspective;
    using glm::ortho;
    using glm::lookAt;

    // Quaternion operations
    using glm::slerp;
    using glm::angleAxis;
    using glm::toMat4;
    using glm::toQuat;

    // Utilities
    using glm::clamp;
    using glm::mix;
    using glm::min;
    using glm::max;
    using glm::abs;

}

