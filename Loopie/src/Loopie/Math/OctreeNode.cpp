#include "OctreeNode.h"

#include "Loopie/Components/MeshRenderer.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Scene/Entity.h"


namespace Loopie
{
	OctreeNode::OctreeNode(std::shared_ptr<Entity> entity)
	{
		m_entities.push_back(entity);

		auto meshRenderer = entity->GetComponent<MeshRenderer>();
		if (meshRenderer)
		{
			m_aabb = meshRenderer->GetWorldAABB();
		}
		else
		{
			m_aabb.Enclose(entity->GetTransform()->GetPosition());
		}
	}

	OctreeNode::OctreeNode(const AABB& aabb)
	{
		m_aabb = aabb;
	}
}


