#pragma once
#include "MathTypes.h"

namespace Loopie
{
    namespace Math
    {
        static constexpr float PI = 3.14159265358979323846f;
        static constexpr float DEG2RAD = PI / 180.0f;
        static constexpr float RAD2DEG = 180.0f / PI;
        static constexpr float EPSILON = 1e-6f;

        inline matrix4 ToMatrix4(const quaternion& q) {
            return toMat4(q);
        }

        inline quaternion ToQuaternion(const matrix3& m) {
            return toQuat(m);
        }

        inline bool Approximately(float a, float b, float epsilon = EPSILON) {
            return abs(a - b) < epsilon;
        }

        inline void DecomposeMatrix(const matrix4& matrix, vec3& position, quaternion& rotation, glm::vec3& scale)
        {
            vec3 skew;
            vec4 perspective;
            decompose(matrix, scale, rotation, position, skew, perspective);
        }

    }
}