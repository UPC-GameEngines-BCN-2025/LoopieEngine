#pragma once
#include "Loopie/Math/MathTypes.h"

namespace Loopie{
    struct OBB;

    struct AABB {
        vec3 MinPoint = vec3(0);
        vec3 MaxPoint = vec3(0);

        void SetNegativeInfinity();
        void Enclose(const AABB& other);
        void Enclose(const OBB& other);
        void Enclose(const vec3& point);

        bool Contains(const vec3& point) const;
        bool ContainsRay(const vec3& rayStart, const vec3& rayEnd) const;

        bool Intersects(const AABB& other) const;
        bool IntersectsSphere(const vec3& center, float radius) const;
        bool IntersectsRay(const vec3& rayStart, const vec3& rayEnd) const;
        bool IntersectsRay(const vec3& rayOrigin, const vec3& rayDirection, vec3& hitPoint) const;


        vec3 GetCenter() const;
        vec3 GetExtents() const;
        vec3 GetSize() const;
        float GetVolume() const;

        OBB ToOBB() const;
        AABB Transform(const matrix4& localToWorld) const;


    };
}
