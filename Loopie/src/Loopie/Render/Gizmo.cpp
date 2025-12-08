#include "Gizmo.h"

#include "Loopie/Core/Assert.h"

#include "Loopie/Render/Shader.h"
#include "Loopie/Render/VertexArray.h"
#include "Loopie/Render/VertexBuffer.h"
#include "Loopie/Render/IndexBuffer.h"

#include <glad/glad.h>

namespace Loopie {
	
	Gizmo::GizmoData Gizmo::s_Data;

	void Gizmo::Init() {
		s_Data.LineRender.VAO = std::make_shared<VertexArray>();
		s_Data.LineRender.VBO = std::make_shared<VertexBuffer>(nullptr, (unsigned int)(s_Data.MAX_VERTICES * sizeof(LineVertex)));

		BufferLayout& layout = s_Data.LineRender.VBO->GetLayout();
		layout.AddLayoutElement(0, GLVariableType::FLOAT, 3, "a_Position");
		layout.AddLayoutElement(4, GLVariableType::FLOAT, 4, "a_Color");

		unsigned int* indices = new uint32_t[s_Data.MAX_LINES * 2];
		for (uint32_t i = 0; i < s_Data.MAX_LINES * 2; i++)
			indices[i] = i;
		s_Data.LineRender.EBO = std::make_shared<IndexBuffer>(indices, s_Data.MAX_LINES * 2);
		delete[] indices;

		s_Data.LineRender.VAO->AddBuffer(s_Data.LineRender.VBO.get(), s_Data.LineRender.EBO.get());

		s_Data.LineBufferModel = new LineVertex[s_Data.MAX_VERTICES];

		s_Data.LineRender.ShaderProg = std::make_shared<Shader>("assets/shaders/LineShader.shader");
	}

	void Gizmo::Shutdown() {
		delete s_Data.LineBufferModel;
	}

	void Gizmo::BeginGizmo()
	{
		s_Data.LineRender.ShaderProg->Bind();

		StartBatch();
		DrawGrid();
	}

	void Gizmo::EndGizmo()
	{
		unsigned int dataSize = (uint32_t)((uint8_t*)s_Data.LineBuffer - (uint8_t*)s_Data.LineBufferModel);
		s_Data.LineRender.VBO->SetData(s_Data.LineBufferModel, dataSize);

		Flush();
		StartBatch();
	}

	void Gizmo::DrawLine(const vec3& p0, const vec3& p1, const vec4& color)
	{
		if (s_Data.LineCount >= s_Data.MAX_LINES)
			NextBatch();

		s_Data.LineBuffer->Position = p0;
		s_Data.LineBuffer->Color = color;
		s_Data.LineBuffer++;

		s_Data.LineBuffer->Position = p1;
		s_Data.LineBuffer->Color = color;
		s_Data.LineBuffer++;

		s_Data.LineCount++;

	}

	void Gizmo::DrawCube(const vec3& p0, const vec3& p1, const vec4& color)
	{
		vec3 minP = { std::min(p0.x, p1.x), std::min(p0.y, p1.y), std::min(p0.z, p1.z) };
		vec3 maxP = { std::max(p0.x, p1.x), std::max(p0.y, p1.y), std::max(p0.z, p1.z) };

		vec3 v000 = { minP.x, minP.y, minP.z };
		vec3 v001 = { minP.x, minP.y, maxP.z };
		vec3 v010 = { minP.x, maxP.y, minP.z };
		vec3 v011 = { minP.x, maxP.y, maxP.z };

		vec3 v100 = { maxP.x, minP.y, minP.z };
		vec3 v101 = { maxP.x, minP.y, maxP.z };
		vec3 v110 = { maxP.x, maxP.y, minP.z };
		vec3 v111 = { maxP.x, maxP.y, maxP.z };

		DrawLine(v000, v100, color);
		DrawLine(v100, v101, color);
		DrawLine(v101, v001, color);
		DrawLine(v001, v000, color);

		DrawLine(v010, v110, color);
		DrawLine(v110, v111, color);
		DrawLine(v111, v011, color);
		DrawLine(v011, v010, color);

		DrawLine(v000, v010, color);
		DrawLine(v100, v110, color);
		DrawLine(v101, v111, color);
		DrawLine(v001, v011, color);
	}

	void Gizmo::DrawCube(const std::array<vec3, 8>& corners, const vec4& color)
	{
		Gizmo::DrawLine(corners[0], corners[1], color);
		Gizmo::DrawLine(corners[1], corners[2], color);
		Gizmo::DrawLine(corners[2], corners[3], color);
		Gizmo::DrawLine(corners[3], corners[0], color);

		Gizmo::DrawLine(corners[4], corners[5], color);
		Gizmo::DrawLine(corners[5], corners[6], color);
		Gizmo::DrawLine(corners[6], corners[7], color);
		Gizmo::DrawLine(corners[7], corners[4], color);

		Gizmo::DrawLine(corners[0], corners[4], color);
		Gizmo::DrawLine(corners[1], corners[5], color);
		Gizmo::DrawLine(corners[2], corners[6], color);
		Gizmo::DrawLine(corners[3], corners[7], color);
	}

	void Gizmo::DrawCube(const OBB& obb, const vec4& color)
	{
		DrawCube(obb.GetCorners(), color);
	}

	void Gizmo::DrawCube(const AABB& aabb, const vec4& color)
	{
		DrawCube(aabb.MinPoint, aabb.MaxPoint, color);
	}

	void Gizmo::DrawFrustum(const Frustum& frustum, const vec4& color)
	{
		DrawCube(frustum.GetCorners(), color);
	}

	void Gizmo::SetGridSize(int size)
	{
		s_Data.GridHalfSize = size / 2;
	}

	void Gizmo::SetGridSpacing(float spacing)
	{
		s_Data.GridSpacing = spacing;
	}

	void Gizmo::SetGridColor(vec4 color)
	{
		s_Data.GridColor = color;
	}

	void Gizmo::Flush() {
		if (s_Data.LineCount == 0)
			return;

		s_Data.LineRender.VAO->Bind();
		s_Data.LineRender.ShaderProg->Bind();
		glDrawElements(GL_LINES, s_Data.LineCount * 2, GL_UNSIGNED_INT, nullptr);
		s_Data.LineRender.VAO->Unbind();
	}

	void Gizmo::StartBatch()
	{
		s_Data.LineCount = 0;
		s_Data.LineBuffer = s_Data.LineBufferModel;
	}

	void Gizmo::NextBatch()
	{
		EndGizmo();
		StartBatch();
	}

	void Gizmo::DrawGrid()
	{
		if (s_Data.DrawGrid) {
			for (int i = -s_Data.GridHalfSize; i <= s_Data.GridHalfSize; i++)
			{
				if (i == 0) {
					Gizmo::DrawLine({ 0.0f, 0.0f, -s_Data.GridHalfSize * s_Data.GridSpacing }, { 0.0f, 0.0f, 0.0f }, s_Data.GridColor);
					Gizmo::DrawLine({ -s_Data.GridHalfSize * s_Data.GridSpacing, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, s_Data.GridColor);
				}
				else {
					float position = i * s_Data.GridSpacing;
					Gizmo::DrawLine({ position, 0.0f, -s_Data.GridHalfSize * s_Data.GridSpacing }, { position, 0.0f, s_Data.GridHalfSize * s_Data.GridSpacing }, s_Data.GridColor);
					Gizmo::DrawLine({ -s_Data.GridHalfSize * s_Data.GridSpacing, 0.0f, position }, { s_Data.GridHalfSize * s_Data.GridSpacing, 0.0f, position }, s_Data.GridColor);
				}	
			}

			Gizmo::DrawLine({ 0.0f, 0.0f, 0.0f }, { 0.0f,s_Data.GridHalfSize * s_Data.GridSpacing, 0.0f }, {0,1,0,1});
			Gizmo::DrawLine({ 0.0f, 0.0f, 0.0f }, { s_Data.GridHalfSize * s_Data.GridSpacing, 0.0f, 0.0f }, {1,0,0,1});
			Gizmo::DrawLine({ 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f,  s_Data.GridHalfSize * s_Data.GridSpacing }, {0,0,1,1});
		}
	}
}