#pragma once
#include "Loopie/Math/OctreeNode.h"

#include <memory>
#include <array>


namespace Loopie {
	constexpr int MAXIMUM_DEPTH = 3; // Can be modified as necessary

	class Entity;
	struct Frustum;

	struct OctreeStatistics
	{
		int totalNodes = 0;
		int leafNodes = 0;
		int internalNodes = 0;
		int totalEntities = 0;
		//int visibleEntities = 0;
		int maxDepth = 0;
		int minEntitiesPerLeaf = INT_MAX;
		int maxEntitiesPerLeaf = 0;
		float averageEntitiesPerLeaf = 0.0f;
		int emptyLeaves = 0;
		int overfilledLeaves = 0; // Leaves exceeding MAX_ENTITIES_PER_NODE at max depth
	};

	//template<typename T>
	class Octree
	{
		// *** Possible functions to add *** - PSS 30/11/2025
		// FindNearest (useful for AI, click selection, etc.)
		// Template - Query (return all objects of specific template type given an AABB / sphere)
		// Optimize (if we see that the performance of the Octree is bad)

	public:
		Octree(const AABB& rootBounds);
		~Octree() = default;

		void Insert(std::shared_ptr<Entity> entity);
		void Remove(std::shared_ptr<Entity> entity);
		void Update(std::shared_ptr<Entity> entity);
		void Clear();
		void Rebuild();
		void DebugDraw(vec3 color);
		void DebugPrintOctreeStatistics();
		OctreeStatistics GetStatistics() const;
		void CollectIntersectingObjectsWithRay(vec3 rayOrigin, vec3 rayDirection,
											   std::vector<std::shared_ptr<Entity>>& entities);

		void CollectIntersectingObjectsWithAABB(const AABB& queryBox,
												std::vector<std::shared_ptr<Entity>>& entities);

		void CollectIntersectingObjectsWithSphere(const vec3& center, const float& radius,
												  std::vector<std::shared_ptr<Entity>>& entities);

		void CollectVisibleEntitiesFrustum(const Frustum& frustum, 
										   std::vector<std::shared_ptr<Entity>>& visibleEntities);

		void CollectAllEntities(std::vector<std::shared_ptr<Entity>>& entities);



	private:
		AABB GetEntityAABB(const std::shared_ptr<Entity>& entity) const;
		void InsertRecursively(OctreeNode* node, std::shared_ptr<Entity> entity, const AABB& entityAABB, int depth);
		void RemoveRecursively(OctreeNode* node, std::shared_ptr<Entity> entity, const AABB& entityAABB);
		void Subdivide(OctreeNode* node);
		void RedistributeEntities(OctreeNode* node);
		std::array<AABB, MAX_ENTITIES_PER_NODE> ComputeChildAABBs(const AABB& parentAABB) const;
		void DebugDrawRecursively(OctreeNode* node, vec3 color);
		void GatherStatisticsRecursively(OctreeNode* node, OctreeStatistics& stats, int depth) const;

		void CollectAllEntitiesFromNode(OctreeNode* node, std::vector<std::shared_ptr<Entity>>& entities);

		void CollectIntersectingObjectsWithRayRecursively(OctreeNode* node, vec3 rayOrigin, vec3 rayDirection,
									 std::vector<std::shared_ptr<Entity>>& entities);

		void CollectIntersectingObjectsWithAABBRecursively(OctreeNode* node, const AABB& queryBox,
														   std::vector<std::shared_ptr<Entity>>& entities);

		void CollectIntersectingObjectsWithSphereRecursively(OctreeNode* node, const vec3& center, const float& radius,
															 std::vector<std::shared_ptr<Entity>>& entities);

		void CollectVisibleEntitiesFrustumRecursively(OctreeNode* node, const Frustum& frustum,
													  std::vector<std::shared_ptr<Entity>>& visibleEntities);

	private:
		std::unique_ptr<OctreeNode> m_rootNode;
	};
}