#include "Octree.h"

#include "Loopie/Math/Frustum.h"
#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Scene/Entity.h"
#include "Loopie/Core/Log.h"

namespace Loopie
{
	// *** World Bounds as a constructor *** - PSS 30/11/2025
	// We should define a fixed world bounds as it's the best way for performance
	// Right now it should be something like 
	// AABB(vec3(-1000, -1000, -1000), vec3(1000, 1000, 1000));
	Octree::Octree(const AABB& rootBounds)
	{
		m_rootNode = std::make_unique<OctreeNode>(rootBounds);
	}

	// *** Steps of Insert *** - PSS 30/11/2025
	// 1. If it is within the bounds of the Octree, add it to the root node
	// 2. If you run out of Bucket Space and you are a leaf, subdivide node
	//	  into 8 child nodes and add them in the parent node's list
	// 3. Redistribute all GameObjects to proper childs based on their
	//    position in space
	// 4. If it reaches maximum depth, stop subdividing and add the object in
	//    the list of entities of the node even we are over max entities (8) capacity
	void Octree::Insert(std::shared_ptr<Entity> entity)
	{
		AABB entityAABB = GetEntityAABB(entity);
		InsertRecursively(m_rootNode.get(), entity, entityAABB, 0);
	}

	void Octree::Remove(std::shared_ptr<Entity> entity)
	{
		AABB entityAABB = GetEntityAABB(entity);
		RemoveRecursively(m_rootNode.get(), entity, entityAABB);
	}

	// This can be optimized if it's too slow
	void Octree::Update(std::shared_ptr<Entity> entity)
	{
		Remove(entity);
		Insert(entity);
	}

	void Octree::Clear()
	{
		m_rootNode.reset();
	}

	// *** How Rebuild works *** - PSS 30/11/2025
	// This allows us to rebalance the tree if it becomes too unbalanced
	// Right now it has to be called manually, but we could get the statistics
	// of the Octree and rebuild automatically given a certain statistic.
	void Octree::Rebuild()
	{
		AABB rootBounds = m_rootNode->m_aabb;
		std::vector<std::shared_ptr<Entity>> allEntities;
		CollectAllEntitiesFromNode(m_rootNode.get(), allEntities);

		Clear();

		m_rootNode = std::make_unique<OctreeNode>(rootBounds);
		m_rootNode->m_isLeaf = true;

		for (auto& entity : allEntities)
		{
			Insert(entity);
		}
	}

	// This debug draws the whole Octree. We might consider doing optimizations, 
	// like frustrum, and expand it to debug from a certain Octree downwards
	void Octree::DebugDraw(vec3 color)
	{
		DebugDrawRecursively(m_rootNode.get(), color);
	}

	void Octree::DebugPrintOctreeStatistics()
	{
		OctreeStatistics statistics = GetStatistics();
		Log::Info("==========================");
		Log::Info("Printing Octree Statistics");
		Log::Info("==========================");
		Log::Info("Total Nodes = {0}", statistics.totalNodes);
		Log::Info("Leaf Nodes = {0}", statistics.leafNodes);
		Log::Info("Internal Nodes = {0}", statistics.internalNodes);
		Log::Info("Total Entities = {0}", statistics.totalEntities);
		// The line below could be implemented with a few changes.
		//Log::Info("Total Visible Entities = {0}", statistics.totalEntities); 
		Log::Info("Max Depth = {0}", statistics.maxDepth);
		Log::Info("Min Entities Per Leaf = {0}", statistics.minEntitiesPerLeaf);
		Log::Info("Max Entities Per Leaf = {0}", statistics.maxEntitiesPerLeaf);
		Log::Info("Average Entities Per Leaf = {0}", statistics.averageEntitiesPerLeaf);
		Log::Info("Empty Leaves = {0}", statistics.emptyLeaves);
		Log::Info("Overfilled Leaves = {0}", statistics.overfilledLeaves);
	}

	OctreeStatistics Octree::GetStatistics() const
	{
		OctreeStatistics stats;
		GatherStatisticsRecursively(m_rootNode.get(), stats, 0);

		if (stats.leafNodes > 0)
		{
			stats.averageEntitiesPerLeaf = static_cast<float>(stats.totalEntities) / stats.leafNodes;
		}

		return stats;
	}

	void Octree::CollectIntersectingObjectsWithRay(vec3 rayOrigin, vec3 rayDirection,
												   std::vector<std::shared_ptr<Entity>>& entities)
	{
		CollectIntersectingObjectsWithRayRecursively(m_rootNode.get(), rayOrigin, rayDirection, entities);
	}

	void Octree::CollectIntersectingObjectsWithAABB(const AABB& queryBox,
													std::vector<std::shared_ptr<Entity>>& entities)
	{
		CollectIntersectingObjectsWithAABBRecursively(m_rootNode.get(), queryBox, entities);
	}

	void Octree::CollectIntersectingObjectsWithSphere(const vec3& center, const float& radius,
													  std::vector<std::shared_ptr<Entity>>& entities)
	{
		CollectIntersectingObjectsWithSphereRecursively(m_rootNode.get(), center, radius, entities);
	}

	// This uses Frustum
	void Octree::CollectVisibleEntitiesFrustum(const Frustum& frustum, 
											   std::vector<std::shared_ptr<Entity>>& visibleEntities)
	{
		CollectVisibleEntitiesFrustumRecursively(m_rootNode.get(), frustum, visibleEntities);
	}

	void Octree::CollectAllEntities(std::vector<std::shared_ptr<Entity>>& entities)
	{
		CollectAllEntitiesFromNode(m_rootNode.get(), entities);
	}

	AABB Octree::GetEntityAABB(const std::shared_ptr<Entity>& entity) const
	{
		auto meshRenderer = entity->GetComponent<MeshRenderer>();
		if (meshRenderer)
		{
			return meshRenderer->GetWorldAABB();
		}
		else
		{
			AABB aabb;
			aabb.Enclose(entity->GetTransform()->GetPosition());
			return aabb;
		}
	}
	
	void Octree::InsertRecursively(OctreeNode* node, std::shared_ptr<Entity> entity, const AABB& entityAABB, int depth)
	{
		if (!node)
		{
			return;
		}

		// If the node is a leaf, add entity and check for entities and depth
		// If max capacity has reached and hasn't reached max depth, it will subdivide the node
		// and redistribute all entities
		if (node->m_isLeaf)
		{
			node->m_entities.push_back(entity);

			if (node->m_entities.size() > MAX_ENTITIES_PER_NODE && depth < MAXIMUM_DEPTH)
			{
				Subdivide(node);
				RedistributeEntities(node);
			}
			return;
		}


		// If not leaf, find child/ren that interesect with the entity's AABB
		bool inserted = false;
		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			if (node->m_children[i] && node->m_children[i]->m_aabb.Intersects(entityAABB))
			{
				InsertRecursively(node->m_children[i].get(), entity, entityAABB, depth + 1);
				inserted = true;
			}
		}

		// If no child intersected, add to this node
		if (!inserted)
		{
			node->m_entities.push_back(entity);
		}
	}

	void Octree::RemoveRecursively(OctreeNode* node, std::shared_ptr<Entity> entity, const AABB& entityAABB)
	{
		if (!node)
		{
			return;
		}
		
		// Early exit - if the entities' AABB doesn't intersect with this node's AABB, skip it and its children.
		if (!entityAABB.Intersects(node->m_aabb))
		{
			return;
		}

		if (node->m_isLeaf)
		{
			auto it = std::find(node->m_entities.begin(), node->m_entities.end(), entity);
			if (it != node->m_entities.end())
			{
				node->m_entities.erase(it);
			}
			return;
		}

		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			if (node->m_children[i])
			{
				RemoveRecursively(node->m_children[i].get(), entity, entityAABB);
			}
		}
	}

	void Octree::Subdivide(OctreeNode* node)
	{
		// Subdivide node into 8 different nodes
		std::array<AABB, MAX_ENTITIES_PER_NODE> childAABBs = ComputeChildAABBs(node->m_aabb);

		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			node->m_children[i] = std::make_unique<OctreeNode>(childAABBs[i]);
			node->m_children[i]->m_parent = node;
			node->m_children[i]->m_isLeaf = true;
		}

		node->m_isLeaf = false;

	}

	void Octree::RedistributeEntities(OctreeNode* node)
	{
		// Redistribute entities into those 8 different nodes
		if (node->m_isLeaf)
		{
			return;
		}

		std::vector<std::shared_ptr<Entity>> entitiesToRedistribute = std::move(node->m_entities);

		for (auto& entity : entitiesToRedistribute)
		{
			AABB entityAABB = GetEntityAABB(entity);
			bool redistributed = false;

			for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
			{
				if (node->m_children[i] && node->m_children[i]->m_aabb.Intersects(entityAABB))
				{
					InsertRecursively(node->m_children[i].get(), entity, entityAABB, 0);
					redistributed = true;
					break;
				}
			}

			// If entity doesn't fit it any child, keep it at the parent
			if (!redistributed)
			{
				node->m_entities.push_back(entity);
			}
		}

	}

	std::array<AABB, MAX_ENTITIES_PER_NODE> Octree::ComputeChildAABBs(const AABB& parentAABB) const
	{
		std::array<AABB, MAX_ENTITIES_PER_NODE> children;
		vec3 center = parentAABB.GetCenter();
		vec3 min = parentAABB.MinPoint;
		vec3 max = parentAABB.MaxPoint;

		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			// This generates all octants by using bit-wise operations
			vec3 childMin, childMax;
			childMin.x = (i & 1) ? center.x : min.x;
			childMin.y = (i & 2) ? center.y : min.y;
			childMin.z = (i & 4) ? center.z : min.z;
			childMax.x = (i & 1) ? max.x : center.x;
			childMax.y = (i & 2) ? max.y : center.y;
			childMax.z = (i & 4) ? max.z : center.z;

			AABB aabb;
			aabb.MinPoint = childMin;
			aabb.MaxPoint = childMax;
			children[i] = aabb;
		}

		return children;
	}

	void Octree::DebugDrawRecursively(OctreeNode* node, vec3 color)
	{
		if (!node)
		{
			return;
		}

		// TODO (PSS - 27/11/25): Draw node->m_children[i]->m_aabb

		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			if (node->m_children[i])
			{
				DebugDrawRecursively(node->m_children[i].get(), color);
			}
		}
	}

	void Octree::GatherStatisticsRecursively(OctreeNode* node, OctreeStatistics& stats, int depth) const
	{
		if (!node)
		{
			return;
		}

		stats.totalNodes++;
		stats.maxDepth = std::max(stats.maxDepth, depth);

		if (node->m_isLeaf)
		{
			stats.leafNodes++;
			int entityCount = static_cast<int>(node->m_entities.size());
			stats.totalEntities += entityCount;

			if (entityCount == 0)
			{
				stats.emptyLeaves++;
			}

			if (entityCount > MAX_ENTITIES_PER_NODE && depth >= MAXIMUM_DEPTH)
			{
				stats.overfilledLeaves++;
			}

			stats.minEntitiesPerLeaf = std::min(stats.minEntitiesPerLeaf, entityCount);
			stats.maxEntitiesPerLeaf = std::max(stats.maxEntitiesPerLeaf, entityCount);
		}
		else
		{
			stats.internalNodes++;

			for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
			{
				if (node->m_children[i])
				{
					GatherStatisticsRecursively(node->m_children[i].get(), stats, depth + 1);
				}
			}
		}
	}

	// Can be made public if necessary
	void Octree::CollectAllEntitiesFromNode(OctreeNode* node, std::vector<std::shared_ptr<Entity>>& entities)
	{
		if (!node)
		{
			return;
		}

		entities.insert(entities.end(), node->m_entities.begin(), node->m_entities.end());

		// Recurse to children
		if (!node->m_isLeaf)
		{
			for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
			{
				if (node->m_children[i])
				{
					CollectAllEntitiesFromNode(node->m_children[i].get(), entities);
				}
			}
		}
	}

	void Octree::CollectIntersectingObjectsWithRayRecursively(OctreeNode* node, vec3 rayOrigin, vec3 rayDirection,
										 std::vector<std::shared_ptr<Entity>>& entities)
	{
		if (!node)
		{
			return;
		}

		// Early exit - if the ray doesn't intersect with this node's AABB, skip it and its children.
		vec3 dummyHitPoint;
		if (!node->m_aabb.IntersectsRay(rayOrigin, rayDirection, dummyHitPoint))
		{
			return;
		}

		if (node->m_isLeaf)
		{
			entities.insert(entities.end(), node->m_entities.begin(), node->m_entities.end());
			return;
		}

		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			if (node->m_children[i])
			{
				CollectIntersectingObjectsWithRayRecursively(node->m_children[i].get(), rayOrigin, rayDirection, entities);
			}
		}
	}

	void Octree::CollectIntersectingObjectsWithAABBRecursively(OctreeNode* node, const AABB& queryBox,
		std::vector<std::shared_ptr<Entity>>& entities)
	{
		if (!node)
		{
			return;
		}

		// Early exit - if the queryBox' AABB doesn't intersect with this node's AABB, skip it and its children.
		if (!queryBox.Intersects(node->m_aabb))
		{
			return;
		}

		if (node->m_isLeaf)
		{
			entities.insert(entities.end(), node->m_entities.begin(), node->m_entities.end());
			return;
		}

		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			if (node->m_children[i])
			{
				CollectIntersectingObjectsWithAABBRecursively(node->m_children[i].get(), queryBox, entities);
			}
		}
	}

	void Octree::CollectIntersectingObjectsWithSphereRecursively(OctreeNode* node, const vec3& center, const float& radius,
																 std::vector<std::shared_ptr<Entity>>& entities)
	{
		if (!node)
		{
			return;
		}

		// Early exit - if the sphere doesn't intersect with this node's AABB, skip it and its children.
		if (!node->m_aabb.IntersectsSphere(center, radius))
		{
			return;
		}

		if (node->m_isLeaf)
		{
			entities.insert(entities.end(), node->m_entities.begin(), node->m_entities.end());
			return;
		}

		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			if (node->m_children[i])
			{
				CollectIntersectingObjectsWithSphereRecursively(node->m_children[i].get(), center, radius, entities);
			}
		}
	}

	void Octree::CollectVisibleEntitiesFrustumRecursively(OctreeNode* node, const Frustum& frustum,
												   std::vector<std::shared_ptr<Entity>>& visibleEntities)
	{
		if (!node)
		{
			return;
		}

		// Early exit - if the frustum' AABB doesn't intersect with this node's AABB, skip it and its children.
		if (!frustum.Intersects(node->m_aabb))
		{
			return;
		}

		if (node->m_isLeaf)
		{
			visibleEntities.insert(visibleEntities.end(), node->m_entities.begin(), node->m_entities.end());
			return;
		}

		for (int i = 0; i < MAX_ENTITIES_PER_NODE; ++i)
		{
			if (node->m_children[i])
			{
				CollectVisibleEntitiesFrustumRecursively(node->m_children[i].get(), frustum, visibleEntities);
			}
		}
	}
}