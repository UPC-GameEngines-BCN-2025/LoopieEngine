#include "MeshRenderer.h"

#include "Loopie/Render/Renderer.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Render/Gizmo.h"
#include "Loopie/Math/MathTypes.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Resources/AssetRegistry.h"
#include "Loopie/Resources/ResourceManager.h"

namespace Loopie {

	MeshRenderer::MeshRenderer() {
		
	}

	MeshRenderer::~MeshRenderer()
	{
		if (GetOwner() && GetTransform())
			GetTransform()->m_transformNotifier.RemoveObserver(this);
	}

	void MeshRenderer::Init()
	{
		RecalculateBoundingBoxes();
		GetTransform()->m_transformNotifier.AddObserver(this);
	}

	void MeshRenderer::OnNotify(const TransformNotification& id)
	{
		if (id == TransformNotification::OnChanged || id == TransformNotification::OnDirty) {
			SetBoundingBoxesDirty();
		}
	}

	void MeshRenderer::Render() {
		if (m_mesh) {
			///TEST
			if(m_drawNormalsPerFace)
				RenderNormalsPerFace(0.5f,{0,1,1,1});
			if(m_drawNormalsPerTriangle)
				RenderNormalsPerTriangle(0.5f,{1,1,0,1});

			if (m_drawAABB || m_drawOBB) {
				if(m_drawAABB)
					Gizmo::DrawCube(m_worldAABB.MinPoint, m_worldAABB.MaxPoint);
				if(m_drawOBB)
					Gizmo::DrawCube(m_worldOBB.GetCorners());
			}
			///

		}
		
	}

	void MeshRenderer::SetMesh(std::shared_ptr<Mesh> mesh)
	{
		m_mesh = mesh;
		SetBoundingBoxesDirty();
	}

	void MeshRenderer::SetMaterial(std::shared_ptr<Material> material)
	{
		m_material = material;
	}

	std::shared_ptr<Material> MeshRenderer::GetMaterial() {
		if (m_material)
			return m_material;
		return Material::GetDefault();
	}

	const AABB& MeshRenderer::GetWorldAABB() const
	{
		RecalculateBoundingBoxes(); 
		return m_worldAABB;
	}

	const OBB& MeshRenderer::GetWorldOBB() const
	{
		RecalculateBoundingBoxes();
		return m_worldOBB;
	}

	JsonNode MeshRenderer::Serialize(JsonNode& parent) const
	{
		JsonNode meshRendererObj = parent.CreateObjectField("meshrenderer");

		if (m_mesh) {
			meshRendererObj.CreateField<std::string>("mesh_uuid", m_mesh->GetUUID().Get());
			meshRendererObj.CreateField<unsigned int>("mesh_index", m_mesh->GetMeshIndex());
		}
		if (m_material)
			meshRendererObj.CreateField<std::string>("material_uuid", m_material->GetUUID().Get());

		return meshRendererObj;
	}

	void MeshRenderer::Deserialize(const JsonNode& data)
	{
		// TODO: Add deserialization for mesh renderer
		if (data.Contains("mesh_uuid"))
		{
			// This is causing an error so I have to revise it at another point
			UUID id = UUID(data.GetValue<std::string>("mesh_uuid").Result);
			unsigned int index = data.GetValue<unsigned int>("mesh_index").Result;

			Metadata* meta = AssetRegistry::GetMetadata(id);
			if (meta)
				m_mesh = ResourceManager::GetMesh(*meta, index);
		}
		if (data.Contains("material_uuid")) {
			UUID id = UUID(data.GetValue<std::string>("material_uuid").Result);
			Metadata* meta = AssetRegistry::GetMetadata(id);
			if (meta)
				m_material = ResourceManager::GetMaterial(*meta);
		}
	}

	void MeshRenderer::RecalculateBoundingBoxes() const
	{
		if (!m_boundingBoxesDirty || !m_mesh)
			return;

		m_worldOBB = m_mesh->GetData().BoundingBox.ToOBB();
		m_worldOBB.ApplyTransform(GetTransform()->GetLocalToWorldMatrix());
		m_worldAABB = m_mesh->GetData().BoundingBox.Transform(GetTransform()->GetLocalToWorldMatrix());
		
		m_boundingBoxesDirty = false;
	}

	///TEST
	vec3 MeshRenderer::GetVertexVec3Data(const MeshData& data, unsigned int vertexIndex, unsigned int offset)
	{
		vec3 vec3Data(0.0f);

		unsigned int base = vertexIndex * data.VertexElements + offset / sizeof(float);
		if (base + 2 < data.Vertices.size()) {
			vec3Data.x = data.Vertices[base + 0];
			vec3Data.y = data.Vertices[base + 1];
			vec3Data.z = data.Vertices[base + 2];
		}
		return vec3Data;
	};

	void MeshRenderer::RenderNormalsPerFace(float length, const vec4& color) {
		MeshData& data = m_mesh->m_data;
		if (data.VerticesAmount == 0 || data.IndicesAmount == 0)
			return;

		const BufferLayout& layout = m_mesh->m_vbo->GetLayout();
		const BufferElement* posElem = layout.GetElementByIndex(0);
		if (posElem->Type == GLVariableType::NONE)
			return;

		Transform* transform = GetTransform();
		matrix4 modelMatrix = transform->GetLocalToWorldMatrix();
		matrix3 normalMatrix = transpose(inverse(matrix3(modelMatrix)));

		for (unsigned int i = 0; i + 5 < data.Indices.size(); i += 6) {
			unsigned int indices[6] = {
				data.Indices[i + 0], data.Indices[i + 1], data.Indices[i + 2],
				data.Indices[i + 3], data.Indices[i + 4], data.Indices[i + 5]
			};

			bool skip = false;
			for (int j = 0; j < 6; j++)
				if (indices[j] >= data.VerticesAmount)
					skip = true;
			if (skip) continue;

			vec3 p[6];
			for (int j = 0; j < 6; j++)
				p[j] = GetVertexVec3Data(data, indices[j], posElem->Offset);

			for (int j = 0; j < 6; j++)
				p[j] = vec3(modelMatrix * vec4(p[j], 1.0f));

			vec3 n1 = normalize(cross(p[1] - p[0], p[2] - p[0]));
			vec3 n2 = normalize(cross(p[4] - p[3], p[5] - p[3]));
			vec3 faceNormal = normalize(n1 + n2);

			vec3 centroid1 = (p[0] + p[1] + p[2]) / 3.0f;
			vec3 centroid2 = (p[3] + p[4] + p[5]) / 3.0f;
			vec3 faceCentroid = (centroid1 + centroid2) * 0.5f;

			Gizmo::DrawLine(faceCentroid, faceCentroid + faceNormal * length, color);
		}
	}

	void MeshRenderer::RenderNormalsPerTriangle(float length, const vec4& color)
	{
		MeshData& data = m_mesh->m_data;
		if (data.VerticesAmount == 0 || data.IndicesAmount == 0)
			return;

		const BufferLayout& layout = m_mesh->m_vbo->GetLayout();
		const BufferElement* posElem = layout.GetElementByIndex(0); // a_Position
		if (posElem->Type == GLVariableType::NONE)
			return;

		Transform* transform = GetTransform();
		matrix4 modelMatrix = transform->GetLocalToWorldMatrix();
		matrix3 normalMatrix = transpose(inverse(matrix3(modelMatrix)));

		for (unsigned int i = 0; i + 2 < data.Indices.size(); i += 3) {
			unsigned int i0 = data.Indices[i + 0];
			unsigned int i1 = data.Indices[i + 1];
			unsigned int i2 = data.Indices[i + 2];

			if (i0 >= data.VerticesAmount || i1 >= data.VerticesAmount || i2 >= data.VerticesAmount)
				continue;

			vec3 p0 = GetVertexVec3Data(data, i0, posElem->Offset);
			vec3 p1 = GetVertexVec3Data(data, i1, posElem->Offset);
			vec3 p2 = GetVertexVec3Data(data, i2, posElem->Offset);

			p0 = vec3(modelMatrix * vec4(p0, 1.0f));
			p1 = vec3(modelMatrix * vec4(p1, 1.0f));
			p2 = vec3(modelMatrix * vec4(p2, 1.0f));

			vec3 n = normalize(cross(p1 - p0, p2 - p0));
			vec3 centroid = (p0 + p1 + p2) / 3.0f;

			Gizmo::DrawLine(centroid, centroid + n * length, color);
		}
	}

}