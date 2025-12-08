#pragma once
#include "Loopie/Math/MathTypes.h"
#include "Loopie/Math/AABB.h"
#include "Loopie/Math/OBB.h"

namespace Loopie {

    struct Plane {
        vec3 Normal;
        float Distance;

        float DistanceToPoint(const vec3& point) const;
        void Normalize();
    };

    struct Frustum {
        Plane Planes[6];

        enum {
            LEFT = 0,
            RIGHT = 1,
            TOP = 2,
            BOTTOM = 3,
            NEAR = 4,
            FAR = 5
        };

        mutable std::array<vec3, 8> _cachedCorners;
        mutable bool _cornersDirty = true;

        bool Intersects(const vec3& point) const;
        bool Intersects(const AABB& box) const;
        bool Intersects(const OBB& box) const;

        void FromMatrix(const matrix4& viewProjectionMatrix);

        vec3 IntersectPlanes(const Plane& p1, const Plane& p2, const Plane& p3) const;
        const std::array<vec3, 8>& GetCorners() const;
        void SetCornersDirty() { _cornersDirty = true; }
    };
}
