#pragma once
#include "Loopie/Math/MathTypes.h"
#include "Loopie/Math/Frustum.h"
#include "Loopie/Math/AABB.h"
#include "Loopie/Math/OBB.h"

#include <memory>

namespace Loopie {

	class VertexArray;
	class VertexBuffer;
	class IndexBuffer;
	class Shader;

	class Gizmo {
		friend class Renderer;
	public:
		static void Init();
		static void Shutdown();

		static void DrawLine(const vec3& p0, const vec3& p1, const vec4& color = {1,1,1,1});
		static void DrawCube(const vec3& p0, const vec3& p1, const vec4& color = {1,1,1,1});
		static void DrawCube(const std::array<vec3, 8>& corners, const vec4& color = {1,1,1,1});
		static void DrawCube(const OBB& obb, const vec4& color = {1,1,1,1});
		static void DrawCube(const AABB& aabb, const vec4& color = {1,1,1,1});
		static void DrawFrustum(const Frustum& frustum, const vec4& color = { 1,1,1,1 });

		static void SetGridSize(int size);
		static void SetGridSpacing(float spacing);
		static void SetGridColor(vec4 color);

	private:

		static void BeginGizmo();
		static void EndGizmo();

		static void Flush();
		static void StartBatch();
		static void NextBatch();

		static void DrawGrid();

		struct LineVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
		};

		struct GizmoTypeRender {
			std::shared_ptr<VertexArray> VAO;
			std::shared_ptr<VertexBuffer> VBO;
			std::shared_ptr<IndexBuffer> EBO;

			std::shared_ptr <Shader> ShaderProg;
		};

		struct GizmoData
		{
			const unsigned int MAX_LINES = 10000;
			const unsigned int MAX_VERTICES = MAX_LINES * 2;
			const float LINE_WIDTH = 2.0f;

			/// Lines
			GizmoTypeRender LineRender;
			LineVertex* LineBufferModel = nullptr;
			LineVertex* LineBuffer = nullptr;
			unsigned int LineCount = 0;
			///

			/// Grid
			int GridHalfSize=50;
			float GridSpacing = 10;
			bool DrawGrid = true;
			vec4 GridColor = { 0.6f, 0.6f, 0.6f, 1.0f };
			///
		};

	private:
		static GizmoData s_Data;
	};
}