#include "Frustum.h"

namespace Loopie {

	float Plane::DistanceToPoint(const vec3& point) const{
		return dot(Normal, point) + Distance;
	}

	void Plane::Normalize() {
		float len = length(Normal);
		if (len > 0.0f) {
			Normal /= len;
			Distance /= len;
		}
	}

	bool Frustum::Intersects(const vec3& point) const{
        for (int i = 0; i < 6; i++) {
            if (Planes[i].DistanceToPoint(point) < 0.0f)
                return false;
        }
        return true;
	}

	bool Frustum::Intersects(const AABB& box) const{
        vec3 min = box.MinPoint;
        vec3 max = box.MaxPoint;

        for (int i = 0; i < 6; i++) {
            const Plane& plane = Planes[i];

            vec3 positive;
            positive.x = (plane.Normal.x >= 0) ? max.x : min.x;
            positive.y = (plane.Normal.y >= 0) ? max.y : min.y;
            positive.z = (plane.Normal.z >= 0) ? max.z : min.z;

            if (plane.DistanceToPoint(positive) < 0.0f)
                return false;
        }
        return true;
	}

	bool Frustum::Intersects(const OBB& box) const{
        const auto& corners = box.GetCorners();

        for (int i = 0; i < 6; i++) {
            const Plane& p = Planes[i];
            int out = 0;
            for (int c = 0; c < 8; c++) {
                if (p.DistanceToPoint(corners[c]) < 0.0f)
                    out++;
            }
            if (out == 8)
                return false;
        }

        return true;
	}

	void Frustum::FromMatrix(const matrix4& viewMatrix){
        Planes[LEFT].Normal.x = viewMatrix[0][3] + viewMatrix[0][0];
        Planes[LEFT].Normal.y = viewMatrix[1][3] + viewMatrix[1][0];
        Planes[LEFT].Normal.z = viewMatrix[2][3] + viewMatrix[2][0];
        Planes[LEFT].Distance = viewMatrix[3][3] + viewMatrix[3][0];

        Planes[RIGHT].Normal.x = viewMatrix[0][3] - viewMatrix[0][0];
        Planes[RIGHT].Normal.y = viewMatrix[1][3] - viewMatrix[1][0];
        Planes[RIGHT].Normal.z = viewMatrix[2][3] - viewMatrix[2][0];
        Planes[RIGHT].Distance = viewMatrix[3][3] - viewMatrix[3][0];

        Planes[BOTTOM].Normal.x = viewMatrix[0][3] + viewMatrix[0][1];
        Planes[BOTTOM].Normal.y = viewMatrix[1][3] + viewMatrix[1][1];
        Planes[BOTTOM].Normal.z = viewMatrix[2][3] + viewMatrix[2][1];
        Planes[BOTTOM].Distance = viewMatrix[3][3] + viewMatrix[3][1];

        Planes[TOP].Normal.x = viewMatrix[0][3] - viewMatrix[0][1];
        Planes[TOP].Normal.y = viewMatrix[1][3] - viewMatrix[1][1];
        Planes[TOP].Normal.z = viewMatrix[2][3] - viewMatrix[2][1];
        Planes[TOP].Distance = viewMatrix[3][3] - viewMatrix[3][1];

        Planes[NEAR].Normal.x = viewMatrix[0][3] + viewMatrix[0][2];
        Planes[NEAR].Normal.y = viewMatrix[1][3] + viewMatrix[1][2];
        Planes[NEAR].Normal.z = viewMatrix[2][3] + viewMatrix[2][2];
        Planes[NEAR].Distance = viewMatrix[3][3] + viewMatrix[3][2];

        Planes[FAR].Normal.x = viewMatrix[0][3] - viewMatrix[0][2];
        Planes[FAR].Normal.y = viewMatrix[1][3] - viewMatrix[1][2];
        Planes[FAR].Normal.z = viewMatrix[2][3] - viewMatrix[2][2];
        Planes[FAR].Distance = viewMatrix[3][3] - viewMatrix[3][2];

        for (int i = 0; i < 6; i++)
            Planes[i].Normalize();

        SetCornersDirty();
	}

    vec3 Frustum::IntersectPlanes(const Plane& p1, const Plane& p2, const Plane& p3) const
    {
        const vec3& n1 = p1.Normal;
        const vec3& n2 = p2.Normal;
        const vec3& n3 = p3.Normal;

        vec3 n2xn3 = cross(n2, n3);
        vec3 n3xn1 = cross(n3, n1);
        vec3 n1xn2 = cross(n1, n2);

        float denom = dot(n1, n2xn3);

        return (-p1.Distance * n2xn3 - p2.Distance * n3xn1 - p3.Distance * n1xn2) / denom;
    }

    const std::array<vec3, 8>& Frustum::GetCorners() const
    {
        if (_cornersDirty) {
            const Plane& left = Planes[LEFT];
            const Plane& right = Planes[RIGHT];
            const Plane& top = Planes[TOP];
            const Plane& bottom = Planes[BOTTOM];
            const Plane& near = Planes[NEAR];
            const Plane& far = Planes[FAR];

            _cachedCorners[0] = IntersectPlanes(near, top, left);
            _cachedCorners[1] = IntersectPlanes(near, top, right);
            _cachedCorners[2] = IntersectPlanes(near, bottom, right);
            _cachedCorners[3] = IntersectPlanes(near, bottom, left);

            _cachedCorners[4] = IntersectPlanes(far, top, left);
            _cachedCorners[5] = IntersectPlanes(far, top, right);
            _cachedCorners[6] = IntersectPlanes(far, bottom, right);
            _cachedCorners[7] = IntersectPlanes(far, bottom, left);

            _cornersDirty = false;
        }
        return _cachedCorners;
    }
}