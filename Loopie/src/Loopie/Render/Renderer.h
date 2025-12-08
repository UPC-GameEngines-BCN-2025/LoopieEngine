#pragma once
#include "Loopie/Math/MathTypes.h"
#include "Loopie/Resources/Types/Material.h"
#include "Loopie/Resources/Types/Texture.h"
#include "Loopie/Render/VertexArray.h"
#include "Loopie/Render/UniformBuffer.h"
#include "Loopie/Components/Camera.h"

#include <filesystem>

namespace Loopie {
	class Transform;

	class Renderer {
	public:

		enum class StencilOp {
			KEEP = 0x1E00,        // GL_KEEP
			ZERO = 0x0000,        // GL_ZERO
			REPLACE = 0x1E01,     // GL_REPLACE
			INCR = 0x1E02,        // GL_INCR
			INCR_WRAP = 0x8507,   // GL_INCR_WRAP
			DECR = 0x1E03,        // GL_DECR
			DECR_WRAP = 0x8508,   // GL_DECR_WRAP
			INVERT = 0x150A       // GL_INVERT
		};

		enum class StencilFunc {
			NEVER = 0x0200,       // GL_NEVER
			LESS = 0x0201,        // GL_LESS
			LEQUAL = 0x0203,      // GL_LEQUAL
			GREATER = 0x0204,     // GL_GREATER
			GEQUAL = 0x0206,      // GL_GEQUAL
			EQUAL = 0x0202,       // GL_EQUAL
			NOTEQUAL = 0x0205,    // GL_NOTEQUAL
			ALWAYS = 0x0207       // GL_ALWAYS
		};

		enum class DepthFunc {
			NEVER = 0x0200,       // GL_NEVER
			LESS = 0x0201,        // GL_LESS
			EQUAL = 0x0202,       // GL_EQUAL
			LEQUAL = 0x0203,      // GL_LEQUAL
			GREATER = 0x0204,     // GL_GREATER
			NOTEQUAL = 0x0205,    // GL_NOTEQUAL
			GEQUAL = 0x0206,      // GL_GEQUAL
			ALWAYS = 0x0207       // GL_ALWAYS
		};

		struct RenderItem {
			std::shared_ptr<VertexArray> VAO;
			unsigned int IndexCount;

			std::shared_ptr<Material> material;
			const Transform* transform;
		};
		
		// static void Init(void* (*getProcAddress)(const char*));
		using GLLoader = void* (*)(const char*);
		static void Init(GLLoader getProcAddress);
		static void Shutdown();

		static void Clear();
		static void SetClearColor(const vec4& color);
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		static void RegisterCamera(Camera& camera);
		static void UnregisterCamera(Camera& camera);
		static const std::vector<Camera*>& GetRendererCameras() { return s_renderCameras; }

		static void BeginScene(const matrix4& viewMatrix, const matrix4& projectionMatrix, bool gizmo = true);
		static void EndScene();

		static void AddRenderItem(std::shared_ptr<VertexArray> vao, std::shared_ptr<Material> material, const Transform* transform);
		static void FlushRenderItem(std::shared_ptr<VertexArray> vao, std::shared_ptr<Material> material, const Transform* transform);
		static void FlushRenderItem(std::shared_ptr<VertexArray> vao, std::shared_ptr<Material> material, const matrix4& modelMatrix);

		static void EnableDepth();
		static void DisableDepth();

		static void EnableStencil();
		static void DisableStencil();
		static void SetStencilMask(unsigned int mask);
		static void SetStencilOp(StencilOp stencil_fail, StencilOp depth_fail, StencilOp pass);
		static void SetStencilFunc(StencilFunc cond, int ref, unsigned int mask);

	private:
		static void SetRenderUniforms(std::shared_ptr<Material> material, const Transform* transform);
		static void SetRenderUniforms(std::shared_ptr<Material> material, const matrix4& modelMatrix);
		static void FlushRenderQueue();

	public:
	private:

		static std::vector<RenderItem> s_RenderQueue;
		static std::vector<Camera*> s_renderCameras;
		static std::shared_ptr<UniformBuffer> s_matricesUniformBuffer;
	};
}