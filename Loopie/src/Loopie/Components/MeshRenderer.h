#pragma once
#include "Loopie/Components/Component.h"
#include "Loopie/Events/EventTypes.h"

#include "Loopie/Resources/Types/Mesh.h"
#include "Loopie/Resources/Types/Material.h"

#include <memory>

namespace Loopie {
	class MeshRenderer : public Component, public IObserver<TransformNotification> {
	public:
		DEFINE_TYPE(MeshRenderer)

		MeshRenderer();
		~MeshRenderer();
		void Init() override; //// From Component
		void OnNotify(const TransformNotification& id) override;

		void Render();
		
		std::shared_ptr<Mesh> GetMesh() { return m_mesh; }
		void SetMesh(std::shared_ptr<Mesh> mesh);

		std::shared_ptr<Material> GetMaterial();
		void SetMaterial(std::shared_ptr <Material> material);
		

		void SetBoundingBoxesDirty() { m_boundingBoxesDirty = true; }
		const AABB& GetWorldAABB() const;
		const OBB& GetWorldOBB() const;

		///TEST
		void SetDrawNormalsPerFace(bool value) { m_drawNormalsPerFace = value; }
		bool GetDrawNormalsPerFace() { return m_drawNormalsPerFace; }
		void SetDrawNormalsPerTriangle(bool value) { m_drawNormalsPerTriangle = value; }
		bool GetDrawNormalsPerTriangle() { return m_drawNormalsPerTriangle; }
		void SetDrawAABB(bool value) { m_drawAABB = value; }
		bool GetDrawAABB() { return m_drawAABB; }
		void SetDrawOBB(bool value) { m_drawOBB = value; }
		bool GetDrawOBB() { return m_drawOBB; }
		///TEST

		JsonNode Serialize(JsonNode& parent) const override;
		void Deserialize(const JsonNode& data) override;

	private:
		void RecalculateBoundingBoxes() const;

		///TEST
		vec3 GetVertexVec3Data(const MeshData& data, unsigned int vertexIndex, unsigned int offset);
		void RenderNormalsPerFace(float length, const vec4& color);
		void RenderNormalsPerTriangle(float length, const vec4& color);
		bool m_drawNormalsPerFace = false;
		bool m_drawNormalsPerTriangle = false;
		bool m_drawAABB = false;
		bool m_drawOBB = false;
		///TEST

	private:
		std::shared_ptr<Material> m_material;
		std::shared_ptr<Mesh> m_mesh;

		mutable AABB m_worldAABB = AABB();
		mutable OBB m_worldOBB = OBB();
		mutable bool m_boundingBoxesDirty = true;
	};
}