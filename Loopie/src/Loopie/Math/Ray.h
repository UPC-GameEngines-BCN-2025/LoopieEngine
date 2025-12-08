#include "Loopie/Math/MathTypes.h"

namespace Loopie
{
	class Ray
	{
	public:
		Ray(vec3 startPoint, vec3 endPoint);
		Ray(vec3 startPoint, vec3 direction, float length = -1);
	};
}