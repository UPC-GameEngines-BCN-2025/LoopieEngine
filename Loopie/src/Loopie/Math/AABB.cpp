#include "AABB.h"
#include "Loopie/Math/OBB.h"
#include "Loopie/Math/MathUtils.h"

namespace Loopie {
    void AABB::SetNegativeInfinity() {
        MinPoint = vec3(std::numeric_limits<float>::max());
        MaxPoint = vec3(std::numeric_limits<float>::lowest());
    }

    void AABB::Enclose(const AABB& other) {
        MinPoint = min(MinPoint, other.MinPoint);
        MaxPoint = max(MaxPoint, other.MaxPoint);
    }

    void AABB::Enclose(const OBB& other) {
        auto corners = other.GetCorners();
        for (const auto& corner : corners) {
            Enclose(corner);
        }
    }

    void AABB::Enclose(const vec3& point) {
        MinPoint = min(MinPoint, point);
        MaxPoint = max(MaxPoint, point);
    }

    bool AABB::Contains(const vec3& point) const {
        bool insideX = point.x >= MinPoint.x && point.x <= MaxPoint.x;
        bool insideY = point.y >= MinPoint.y && point.y <= MaxPoint.y;
        bool insideZ = point.z >= MinPoint.z && point.z <= MaxPoint.z;

        return insideX && insideY && insideZ;
    }

    bool AABB::ContainsRay(const vec3& rayStart, const vec3& rayEnd) const
    {
        return Contains(rayStart) && Contains(rayEnd);
    }

    bool AABB::Intersects(const AABB& other) const {
        if (MaxPoint.x < other.MinPoint.x || MinPoint.x > other.MaxPoint.x) 
            return false;
        if (MaxPoint.y < other.MinPoint.y || MinPoint.y > other.MaxPoint.y) 
            return false;
        if (MaxPoint.z < other.MinPoint.z || MinPoint.z > other.MaxPoint.z) 
            return false;

        return true;
    }

    bool AABB::IntersectsSphere(const vec3& center, float radius) const {
        // Find the closest point on the AABB to the sphere center
        vec3 closestPoint;
        closestPoint.x = glm::clamp(center.x, MinPoint.x, MaxPoint.x);
        closestPoint.y = glm::clamp(center.y, MinPoint.y, MaxPoint.y);
        closestPoint.z = glm::clamp(center.z, MinPoint.z, MaxPoint.z);

        // Calculate distance from sphere center to closest point
        vec3 diff = closestPoint - center;
        float distanceSquared = glm::dot(diff, diff);

        // Check if distance is within radius
        return distanceSquared <= (radius * radius);
    }

    bool AABB::IntersectsRay(const vec3& rayStart, const vec3& rayEnd) const
    {
        vec3 dir = rayEnd - rayStart;
        float length = glm::length(dir);
        if (length < Math::EPSILON) {
            return Contains(rayStart);
        }

        vec3 invDir = 1.0f / dir;
        vec3 t1 = (MinPoint - rayStart) * invDir;
        vec3 t2 = (MaxPoint - rayStart) * invDir;

        vec3 tMin = glm::min(t1, t2);
        vec3 tMax = glm::max(t1, t2);

        float enterPoint = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
        float exitPoint = glm::min(glm::min(tMax.x, tMax.y), tMax.z);

        return enterPoint <= exitPoint && exitPoint >= 0.0f && enterPoint <= length;
    }

    bool AABB::IntersectsRay(const vec3& rayOrigin, const vec3& rayDirection, vec3& hitPoint) const
    {
        vec3 invDir = 1.0f / rayDirection;

        vec3 t1 = (MinPoint - rayOrigin) * invDir;
        vec3 t2 = (MaxPoint - rayOrigin) * invDir;

        vec3 tMin = glm::min(t1, t2);
        vec3 tMax = glm::max(t1, t2);

        float enterPoint = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
        float exitPoint = glm::min(glm::min(tMax.x, tMax.y), tMax.z);

        if (enterPoint <= exitPoint && exitPoint >= 0.0f) {
            if (enterPoint >= 0.0f) {
                hitPoint = rayOrigin + rayDirection * enterPoint;
            }
            else {
                hitPoint = rayOrigin + rayDirection * exitPoint;
            }
            return true;
        }

        return false;
    }


    vec3 AABB::GetCenter() const {
        return (MinPoint + MaxPoint) * 0.5f;
    }

    vec3 AABB::GetExtents() const {
        return (MaxPoint - MinPoint) * 0.5f;
    }

    vec3 AABB::GetSize() const {
        return MaxPoint - MinPoint;
    }

    float AABB::GetVolume() const {
        vec3 size = GetSize();
        return size.x * size.y * size.z;
    }
    OBB AABB::ToOBB() const
    {
        OBB obb;

        obb.Center = GetCenter();
        obb.Extents = GetExtents();

        obb.Axes[0] = vec3(1.0f, 0.0f, 0.0f);
        obb.Axes[1] = vec3(0.0f, 1.0f, 0.0f);  
        obb.Axes[2] = vec3(0.0f, 0.0f, 1.0f);

        return obb;
    }
    AABB AABB::Transform(const matrix4& localToWorld) const
    {
        vec3 localMin = MinPoint;
        vec3 localMax = MaxPoint;

        vec3 basisX(localToWorld[0][0], localToWorld[0][1], localToWorld[0][2]);
        vec3 basisY(localToWorld[1][0], localToWorld[1][1], localToWorld[1][2]);
        vec3 basisZ(localToWorld[2][0], localToWorld[2][1], localToWorld[2][2]);

        vec3 translation(localToWorld[3][0], localToWorld[3][1], localToWorld[3][2]);

        vec3 xa = basisX * localMin.x;
        vec3 xb = basisX * localMax.x;
        vec3 ya = basisY * localMin.y;
        vec3 yb = basisY * localMax.y;
        vec3 za = basisZ * localMin.z;
        vec3 zb = basisZ * localMax.z;

        vec3 worldMin = translation + min(xa, xb) + min(ya, yb) + min(za, zb);
        vec3 worldMax = translation + max(xa, xb) + max(ya, yb) + max(za, zb);

        AABB worldAABB;
        worldAABB.MinPoint = worldMin;
        worldAABB.MaxPoint = worldMax;
        return worldAABB;
    }
}