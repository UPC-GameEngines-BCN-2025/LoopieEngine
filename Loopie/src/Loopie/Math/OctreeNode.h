#pragma once
#include "Loopie/Math/AABB.h"

#include <memory>
#include <array>
#include <vector>

namespace Loopie
{
	constexpr int MAX_ENTITIES_PER_NODE = 8;

	class Entity;

	class OctreeNode
	{
	friend class Octree;
	public:
		// explicit added to prevent accidental conversions
		explicit OctreeNode(std::shared_ptr<Entity> entity);
		explicit OctreeNode(const AABB& aabb);
		~OctreeNode() = default;

	private:
		AABB m_aabb;
		std::vector<std::shared_ptr<Entity>> m_entities = {};

		OctreeNode* m_parent = nullptr;
		std::array<std::unique_ptr<OctreeNode>, MAX_ENTITIES_PER_NODE> m_children = {};
		bool m_isLeaf = true;
	};
}