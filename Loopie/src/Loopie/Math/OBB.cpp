#include "OBB.h"
#include "Loopie/Math/AABB.h"
#include "Loopie/Math/MathUtils.h"

namespace Loopie {
	void OBB::ApplyTransform(const matrix4& transform)
    {
        Center = vec3(transform * vec4(Center, 1.0f));

        matrix3 rotationMatrix = matrix3(transform);
        for (int i = 0; i < 3; i++)
            Axes[i] = normalize(rotationMatrix * Axes[i]);

        vec3 scale(length(vec3(transform[0])), length(vec3(transform[1])), length(vec3(transform[2])));
        Extents *= scale;

        SetCornersDirty();
    }

    AABB OBB::ToAABB() const {
        AABB aabb;
        aabb.SetNegativeInfinity();

        auto corners = GetCorners();
        for (const auto& corner : corners) {
            aabb.Enclose(corner);
        }

        return aabb;
    }

    bool OBB::Contains(const vec3& point) const {
        vec3 localPoint = point - Center;

        for (int i = 0; i < 3; i++) {
            float projection = dot(localPoint, Axes[i]);
            if (abs(projection) > Extents[i]) {
                return false;
            }
        }
        return true;
    }

    bool OBB::ContainsRay(const vec3& rayStart, const vec3& rayEnd) const {
        vec3 localStart = rayStart - Center;
        vec3 localEnd = rayEnd - Center;

        for (int i = 0; i < 3; i++) {
            float startProj = dot(localStart, Axes[i]);
            float endProj = dot(localEnd, Axes[i]);

            if (startProj < -Extents[i] || startProj > Extents[i] ||
                endProj < -Extents[i] || endProj > Extents[i]) {
                return false;
            }
        }
        return true;
    }

    bool OBB::IntersectsRay(const vec3& rayStart, const vec3& rayEnd) const {
        vec3 localStart = rayStart - Center;
        vec3 localEnd = rayEnd - Center;

        float startLocal[3], endLocal[3];
        for (int i = 0; i < 3; i++) {
            startLocal[i] = dot(localStart, Axes[i]);
            endLocal[i] = dot(localEnd, Axes[i]);
        }

        vec3 localMin(-Extents.x, -Extents.y, -Extents.z);
        vec3 localMax(Extents.x, Extents.y, Extents.z);

        vec3 localDir = localEnd - localStart;
        float length = glm::length(localDir);
        if (length < Math::EPSILON) {
            return (startLocal[0] >= -Extents.x && startLocal[0] <= Extents.x && startLocal[1] >= -Extents.y && startLocal[1] <= Extents.y && startLocal[2] >= -Extents.z && startLocal[2] <= Extents.z);
        }

        vec3 invLocalDir = 1.0f / localDir;
        vec3 t1 = (localMin - vec3(startLocal[0], startLocal[1], startLocal[2])) * invLocalDir;
        vec3 t2 = (localMax - vec3(startLocal[0], startLocal[1], startLocal[2])) * invLocalDir;

        vec3 tMin = min(t1, t2);
        vec3 tMax = max(t1, t2);

        float tEnter = max(max(tMin.x, tMin.y), tMin.z);
        float exitPoint = min(min(tMax.x, tMax.y), tMax.z);

        return tEnter <= exitPoint && exitPoint >= 0.0f && tEnter <= length;
    }


    bool OBB::IntersectsRay(const vec3& rayOrigin, const vec3& rayDirection, vec3& hitPoint) const {
        vec3 localOrigin = rayOrigin - Center;

        float originLocal[3], dirLocal[3];
        for (int i = 0; i < 3; i++) {
            originLocal[i] = dot(localOrigin, Axes[i]);
            dirLocal[i] = dot(rayDirection, Axes[i]);
        }

        vec3 localMin(-Extents.x, -Extents.y, -Extents.z);
        vec3 localMax(Extents.x, Extents.y, Extents.z);

        vec3 invDirLocal = 1.0f / vec3(dirLocal[0], dirLocal[1], dirLocal[2]);

        vec3 t1 = (localMin - vec3(originLocal[0], originLocal[1], originLocal[2])) * invDirLocal;
        vec3 t2 = (localMax - vec3(originLocal[0], originLocal[1], originLocal[2])) * invDirLocal;

        vec3 tMin = min(t1, t2);
        vec3 tMax = max(t1, t2);

        float entryPoint = max(max(tMin.x, tMin.y), tMin.z);
        float exitPoint = min(min(tMax.x, tMax.y), tMax.z);

        if (entryPoint <= exitPoint && exitPoint >= 0.0f) {
            if (entryPoint >= 0.0f) {
                hitPoint = rayOrigin + rayDirection * entryPoint;
            }
            else {
                hitPoint = rayOrigin + rayDirection * exitPoint;
            }
            return true;
        }

        return false;
    }

    const std::array<vec3, 8>& OBB::GetCorners() const
    {
        if (_cornersDirty) {
            vec3 xAxis = Axes[0] * Extents.x;
            vec3 yAxis = Axes[1] * Extents.y;
            vec3 zAxis = Axes[2] * Extents.z;

            _cachedCorners[0] = Center - xAxis - yAxis - zAxis;
            _cachedCorners[1] = Center + xAxis - yAxis - zAxis;
            _cachedCorners[2] = Center + xAxis + yAxis - zAxis;
            _cachedCorners[3] = Center - xAxis + yAxis - zAxis;
            _cachedCorners[4] = Center - xAxis - yAxis + zAxis;
            _cachedCorners[5] = Center + xAxis - yAxis + zAxis;
            _cachedCorners[6] = Center + xAxis + yAxis + zAxis;
            _cachedCorners[7] = Center - xAxis + yAxis + zAxis;

            _cornersDirty = false;
        }
        return _cachedCorners;
    }
}
